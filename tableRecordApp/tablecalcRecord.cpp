/* SPDX-License-Identifier: BSD-3-Clause
 * See file: COPYRIGHT
 * Author: Gabriel Fedel
 */
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <regex>
#include <unordered_set>

#include "dbDefs.h"
#include "dbAccess.h"
#include "dbEvent.h"
#include "dbFldTypes.h"
#include "dbLink.h"
#include "dbScan.h"
#include "devSup.h"
#include "errMdef.h"
#include "errlog.h"
#include "recSup.h"
#include "recGbl.h"
#include "cantProceed.h"
#include "tableRecord.h"

#define GEN_SIZE_OFFSET
#include "tablecalcRecord.h"
#undef GEN_SIZE_OFFSET
#include "epicsExport.h"

#define report       NULL
#define initialize   NULL
static long init_record(struct dbCommon *, int);
static long process(struct dbCommon *);
static long special(DBADDR *, int);
#define get_value    NULL
static long cvt_dbaddr(DBADDR *);
static long get_array_info(DBADDR *, long *, long *);
static long put_array_info(DBADDR *, long);
static long get_units(DBADDR *, char *);
static long get_precision(const DBADDR *, long *);
#define get_enum_str    NULL
#define get_enum_strs   NULL
#define put_enum_str    NULL
#define get_graphic_double NULL
#define get_control_double NULL
#define get_alarm_double   NULL

rset tablecalcRSET = {
    RSETNUMBER,
    report, initialize, init_record, process, special, get_value,
    cvt_dbaddr, get_array_info, put_array_info, get_units, get_precision,
    get_enum_str, get_enum_strs, put_enum_str,
    get_graphic_double, get_control_double, get_alarm_double
};
epicsExportAddress(rset, tablecalcRSET);


static long init_record(struct dbCommon *pcommon, int pass)
{
    return 0;
}

static long process(struct dbCommon *pcommon)
{
    return 0;
}

static long cvt_dbaddr(DBADDR *paddr)
{
    return 0;
}

static long get_array_info(DBADDR *paddr, long *no_elements, long *offset)
{
    return 0;
}

static long put_array_info(DBADDR *paddr, long nNew)
{
    return 0;
}

static long special(DBADDR *paddr, int after)
{
    return 0;
}

static long get_units(DBADDR *paddr, char *units)
{
    return 0;
}

static long get_precision(const DBADDR *paddr, long *precision)
{
    return 0;
}
