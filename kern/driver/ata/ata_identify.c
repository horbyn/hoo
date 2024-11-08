/**************************************************************************
 *                                                                        *
 *                     Copyright (C)    horbyn, 2024                      *
 *                              (horbyn@outlook.com)                      *
 *                                                                        *
 **************************************************************************/
#include "ata_identify.h"
#include "kern/panic.h"
#include "user/lib.h"

/**
 * @brief parse ata string
 * 
 * @param buff a buffer containing the ata string
 * @param bufflen size
 * @return
 */
static const char *
ata_print_string(const void *buff, uint32_t bufflen) {
    if (bufflen % 2 != 0)    panic("ata_print_string(): bad buffer size");
    if (buff == null)    panic("ata_print_string(): null pointer");

    static char __ata_string_buff[SIZE_ATA_STRING_BUFF];
    bzero(__ata_string_buff, sizeof(__ata_string_buff));
    const char *p = buff;
    for (uint32_t i = 0; i < bufflen; i += 2) {
        __ata_string_buff[i] = p[i + 1];
        __ata_string_buff[i + 1] = p[i];
    }
    return __ata_string_buff;
}

/**
 * @brief get ata device type
 * 
 * @param buff ata identify data
 * @return 
 */
ata_t
ata_get_device_type(const ataid_t *buff) {
    if (buff == null)    panic("ata_get_device_type(): null pointer");
    if (buff->word0_ == 0)    return ATA_TYPE_DEVICE_ATA;
    else    return ATA_TYPE_DEVICE_UNKNOWN;
}

/**
 * @brief get ata sector number
 * 
 * @param buff ata identify data
 * @return total sectors
 */
uint32_t
ata_get_sectors(const ataid_t *buff) {
    if (buff == null)    panic("ata_get_sectors(): parameter invalid");
    return *((uint32_t *)buff->word60_61_);
}

/**
 * @brief get ata serial number
 * 
 * @param buff ata identify data
 * @param serial the result buffer
 */
void
ata_get_serial_number(const ataid_t *buff, ataser_t *serial) {
    if (buff == null || serial == null)
        panic("ata_get_serial_number(): parameter invalid");

    memmove(serial, ata_print_string(&(buff->word10_19_),
        sizeof(ataser_t)), sizeof(ataser_t));

    // there are invalid data when
    // occurs two or more consecutive spaces
    for (uint32_t i = 0; i < sizeof(ataser_t); i++) {
        if (*((uint8_t *)serial + i) == 0x20
            && *((uint8_t *)serial + i + 1) == 0x20) {
            *((uint8_t *)serial + i) = 0;
            break;
        }
    }
}

/**
 * @brief get the ata model number
 * 
 * @param buff the ata identify data
 * @param model the result buffer
 */
void
ata_get_model_number(const ataid_t *buff, atamod_t *model) {
    if (buff == null || model == null)
        panic("ata_get_model_number(): parameter invalid");

    memmove(model, ata_print_string(&(buff->word27_46_),
        sizeof(atamod_t)), sizeof(atamod_t));

    // there are invalid data when
    // occurs two or more consecutive spaces
    for (uint32_t i = 0; i < sizeof(atamod_t); i++) {
        if (*((uint8_t *)model + i) == 0x20
            && *((uint8_t *)model + i + 1) == 0x20) {
            *((uint8_t *)model + i) = 0;
            break;
        }
    }
}
