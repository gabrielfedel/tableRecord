/* SPDX-License-Identifier: BSD-3-Clause
 * See file: COPYRIGHT
 * Author: Bruno Martins
 */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include "dbAccess.h"
#include "dbFldTypes.h"
#include "dbLink.h"
#include "devSup.h"
#include "cantProceed.h"
#include "epicsExport.h"
#include "epicsString.h"
#include "tablecalcRecord.h"

static long soft_init_record(struct dbCommon *prec)
{
    return 0;
}

static long soft_read_table(tablecalcRecord *prec)
{
    return 0;
}

tablecalcdset devTablecalcSoft = {
    {5, NULL, NULL, soft_init_record, NULL},
    soft_read_table
};
epicsExportAddress(dset, devTablecalcSoft);
