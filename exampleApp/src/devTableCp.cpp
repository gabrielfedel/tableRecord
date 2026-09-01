#include <math.h>
#include <stddef.h>
#include <string.h>

#include <string>
#include <vector>

#include "dbAccess.h"
#include "dbLink.h"
#include "dbFldTypes.h"
#include "devSup.h"
#include "recGbl.h"
#include "alarm.h"
#include "errlog.h"
#include "epicsTypes.h"
#include "epicsString.h"
#include "epicsStdlib.h"
#include "epicsExport.h"

#include "tableRecord.h"
#include "tableRecordUtil.h"

/*
 * Table Stat device support for the table record.
 *
 * Reduces input arrays into a compressed statistics table. The INP field is an
 * instrument-IO link specifying the accumulation factor N and an ordered list of
 * statistics functions, one per active output column, e.g.:
 *
 *   field(INP, "@N=8 STATS=avg,rms,min,max,sum")
 *
 * Each active output column's CxxINP links to the source array directly, e.g.:
 *
 *   field(C00INP, "TBL:SRC.C00VAL")
 *
 * The STATS list is positional: the first stat applies to C00, the second to C01,
 * and so on. The number of stats must equal the number of active output columns.
 *
 * On every read_table, each column's CxxINP array is fetched via dbGetLink and
 * reduced in fixed-size groups of N rows: output row j is the stat over input
 * rows [j*N, j*N+N). The number of output rows is floor(inputRows / N), capped
 * by this record's MAXROWS; a trailing partial group (< N rows) is dropped.
 * Supported stats: avg, min, max, rms, sum. Output columns must be DBF_DOUBLE.
 *
 * CONSTANT links are supported as CxxINP (e.g. for testing): dbGetLink handles
 * them transparently. Any link type accepted by dbGetLink may be used.
 */


/* ------------------------------------------------------------------ */
/* Device support                                                       */
/* ------------------------------------------------------------------ */


struct DevTableStatPvt {
    epicsUInt32  nAccum;   /* N: source rows per output sample (>= 1) */
    size_t       maxrows;  /* this record's MAXROWS (output publish cap) */
    std::vector<TableRecordWrapper::DataColumn> data_cols; /* our outputs */
    std::vector<epicsFloat64> inbuf;                       /* shared read buffer */
};


/*
 * Parse the INP instio string "N=<uint>" N= and STATS= is mandatory.
 */
static bool parse_inp(const char *instio, epicsUInt32 &nAccum) {
    bool haveN = false;

    std::string s(instio);
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t'))
            ++i;
        size_t j = i;
        while (j < s.size() && s[j] != ' ' && s[j] != '\t')
            ++j;
        if (j == i)
            break;
        std::string tok = s.substr(i, j - i);
        i = j;

        if (tok.compare(0, 2, "N=") == 0) {
            epicsUInt32 v = 0;
            if (epicsParseUInt32(tok.c_str() + 2, &v, 0, NULL) != 0 || v < 0)
                return false;
            nAccum = v;
            haveN = true;
        }
        /* else: unknown token, ignore */
    }
    return haveN;
}


static long stat_init_record(struct dbCommon *pcommon) {
    tableRecord *prec = (tableRecord *)pcommon;
    struct link *plnk = &prec->inp;

    /* Pass 0: validate INP, then defer to pass 1. The active data columns
       (NUMCOLS) are only known after the record validates column names, which
       happens after this pass-0 callback returns. */
    if (prec->pact != TABLEREC_DEVINIT_PASS1) {
        if (plnk->type != INST_IO || !plnk->value.instio.string
                                  || plnk->value.instio.string[0] == '\0') {
            recGblRecordError(S_db_badField, pcommon,
                "devTableStat: INP must be an instrument-IO link, "
                "e.g. field(INP, \"@N=8\")");
            return S_db_badField;
        }
        return TABLEREC_DEVINIT_PASS1;
    }

    /* Pass 1: parse INP, capture outputs, validate stat count. */
    epicsUInt32 nAccum = 0;
    if (!parse_inp(plnk->value.instio.string, nAccum)) {
        recGblRecordError(S_db_badField, pcommon,
            "devTableStat: malformed INP (expected \"@N=<n>\";"
            "both N is mandatory)");
        return S_db_badField;
    }
    DevTableStatPvt *pvt = new DevTableStatPvt();
    pvt->nAccum = nAccum;

    TableRecordWrapper rec(pcommon);
    pvt->maxrows = rec.max_data_rows();
    rec.data_cols(pvt->data_cols);


    // Don't need to do this check (the base idea is to have only 1 row)
//    if (stats.size() != pvt->data_cols.size()) {
//        recGblRecordError(S_db_badField, pcommon,
//            "devTableStat: STATS count does not match number of active columns");
//        errlogPrintf("%s devTableStat: STATS has %zu entries but record has %zu active columns\n",
//                     prec->name, stats.size(), pvt->data_cols.size());
//        delete pvt;
//        return S_db_badField;
//    }

    // 10 should be replaced by the maximum number of elements of biggest column
    pvt->inbuf.resize(pvt->maxrows * 10);
    rec.set_private(pvt);

    return 0;
}

static long stat_read_table(tableRecord *prec) {
    TableRecordWrapper rec(*prec);
    DevTableStatPvt *pvt = rec.get_private<DevTableStatPvt>();
    long status = 0;

    for (size_t i = 0; i < pvt->data_cols.size(); ++i) {
        auto &out = pvt->data_cols[i];

        if (!*out.val)
            continue;

        long nRequest = (long)pvt->inbuf.size();
        if (dbGetLink(out.inp, DBF_DOUBLE, pvt->inbuf.data(), NULL, &nRequest) != 0) {
            errlogPrintf("%s devTableStat: dbGetLink failed for column %zu\n",
                         prec->name, i);
            recGblSetSevr(prec, READ_ALARM, INVALID_ALARM);
            status = -1;
            continue;
        }
        epicsFloat64 *dst = (epicsFloat64 *)*out.val;
        //only 1 row
        dst[0] =  pvt->inbuf.data()[pvt->nAccum];

        *out.numrows = 1;
        *out.chgd    = 1;
    }

    return status;

    return 0;
}

tabledset devTableCp = {
    {5, NULL, NULL, stat_init_record, NULL},
    stat_read_table
};
epicsExportAddress(dset, devTableCp);
