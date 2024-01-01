/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata_identify_data_log.h"

enum_ata_type
ata_stub_get_ata_type(const void *log) {
    (void)log;
    /*
     * Ref: https://people.freebsd.org/~imp/asiabsdcon2015/works/d2161r5-ATAATAPI_Command_Set_-_3.pdf
     *      -- Annex A
     *
     * My basic idea to get the ata type is using
     * `READ LOG EXT` command to get `IDENTIFY DEVICE data log`
     * which parsed later
     * 
     * But now it is a stub finished later
     */
    return PATA;
}
