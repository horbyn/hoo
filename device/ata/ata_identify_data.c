/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#include "ata_identify_data.h"

static char __ata_string_buff[SIZE_ATA_STRING_BUFF];

/**
 * @brief parse ata string
 * 
 * @param buff a buffer containing the ata string
 * @param bufflen size
 * @return
 */
const char *
ata_print_string(const void *buff, size_t bufflen) {
    if (bufflen % 2 != 0)
        panic("ata_print_string(): bad buffer size\n");

    const char *p = buff;
    for (size_t i = 0; i < bufflen; i += 2) {
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
enum_ata_type_device
ata_get_device_type(const ata_identify_data_t *buff) {
    if (buff->word0_ata_device_ == 0)    return ATA_TYPE_DEVICE_ATA;
    else    return ATA_TYPE_DEVICE_UNKNOWN;
}

/**
 * @brief get ata serial number
 * 
 * @param buff ata identify data
 * @param serial the result buffer
 */
void
ata_get_serial_number(const ata_identify_data_t *buff,
ata_serial_number_t *serial) {
    if (buff == null || serial == null)
        panic("ata_get_serial_number(): parameter invalid");

    memmove(serial, ata_print_string(&(buff->word10_19_),
        sizeof(ata_serial_number_t)), sizeof(ata_serial_number_t));

    // there are invalid data when
    // occurs two or more consecutive spaces
    for (size_t i = 0; i < sizeof(ata_serial_number_t); i++) {
        if (*((uint8_t *)serial + i) == 0x20
            && *((uint8_t *)serial + i + 1) == 0x20) {
            *((uint8_t *)serial + i) = 0;
            break;
        }
    }
}

/**
 * @brief if the ata serial number buffer empties?
 * 
 * @param buff the ata identify buffer
 * @return true empty
 * @return false non-empty
 */
bool
ata_isempty_serial_number(const ata_serial_number_t *buff) {
    static ata_serial_number_t null_sn;
    bzero(&null_sn, sizeof(ata_serial_number_t));
    return memcmp(buff, &null_sn, sizeof(ata_serial_number_t));
}

/**
 * @brief get the ata model number
 * 
 * @param buff the ata identify data
 * @param model the result buffer
 */
void
ata_get_model_number(const ata_identify_data_t *buff,
ata_model_number_t *model) {
    if (buff == null || model == null)
        panic("ata_get_model_number(): parameter invalid");

    memmove(model, ata_print_string(&(buff->word27_46_),
        sizeof(ata_model_number_t)), sizeof(ata_model_number_t));

    // there are invalid data when
    // occurs two or more consecutive spaces
    for (size_t i = 0; i < sizeof(ata_model_number_t); i++) {
        if (*((uint8_t *)model + i) == 0x20
            && *((uint8_t *)model + i + 1) == 0x20) {
            *((uint8_t *)model + i) = 0;
            break;
        }
    }
}

/**
 * @brief get the ata model number
 * 
 * @param buff the identify data buffer
 * @return true empty
 * @return false non-empty
 */
bool
ata_isempty_model_number(const ata_model_number_t *buff) {
    static ata_model_number_t null_mn;
    bzero(&null_mn, sizeof(ata_model_number_t));
    return memcmp(buff, &null_mn, sizeof(ata_model_number_t));
}

void
ata_print_extended_number_of_sectors(const ata_extended_number_of_sectors_t *buff) {
    uint32_t low = buff->extended_number_of_sectors_[0],
        high = buff->extended_number_of_sectors_[1];

    if (low != 0 && high != 0) {
        kprintf("Extended number of user addressable sectors: 0x%x%x; ",
            low, high);
    }
}

uint32_t
ata_get_sectors(const ata_identify_data_t *buff) {
    if (buff == null)    panic("ata_get_sectors(): parameter invalid");

    return *((uint32_t *)buff->word60_61_total_addressable_sectors_lba28_);
}

/**
 * @brief parse ata identify data
 * 
 * @param buff    ata identify data
 * @param bufflen data size
 * @param ata_type ata type
 */
void
ata_identify_data_parse(const void *buff, size_t bufflen, enum_ata_type ata_type) {
    if (buff == null || bufflen != sizeof(ata_identify_data_t))
        panic("ata_identify_data_parse(): invalid data\n");

    const ata_identify_data_t *p = buff;
    char tmp[256];
    bzero(tmp, sizeof(tmp));

    /* funny parse! */

    if (ata_get_device_type(p) == ATA_TYPE_DEVICE_ATA)
        kprintf("ATA Device; ");

    if (p->word2_specific_configuration_) {
        kprintf("Specific configuration: ");
        if (p->word2_specific_configuration_ == 0x37c8)    kprintf("SET FEATURES/incomplete; ");
        else if (p->word2_specific_configuration_ == 0x738c)    kprintf("SET FEATURES/complete; ");
        else if (p->word2_specific_configuration_ == 0x8c73)    kprintf("incomplete; ");
        else if (p->word2_specific_configuration_ == 0xc837)    kprintf("complete; ");
        else    kprintf("unknown: 0x%x; ", p->word2_specific_configuration_);
    }

    ata_get_serial_number(p, (ata_serial_number_t *)tmp);
    if (!ata_isempty_serial_number((ata_serial_number_t *)tmp))
        kprintf("Serial number: %s; ", tmp);

    kprintf("Firmware revision: %s; ",
        ata_print_string(&(p->word23_26_), sizeof(ata_firmware_revision_t)));

    ata_get_model_number(p, (ata_model_number_t *)tmp);
    if (!ata_isempty_model_number((ata_model_number_t *)tmp))
        kprintf("Model number: %s; ", tmp);

    if (p->word47_maximum_sectors_per_drq_block_)
        kprintf("Maxinum sectors per DRQ data block: %d; ", p->word47_maximum_sectors_per_drq_block_);

    if (p->word48_trusted_computing_feature_)
        kprintf("Trusted computing: supported; ");

    if (p->word49_alignment_error_reporting_ != 0x3) {
        if (p->word49_alignment_error_reporting_ == 0)
            kprintf("Long phhysical sector alignment error reporting: disabled; ");
        else if (p->word49_alignment_error_reporting_ == 1)
            kprintf("Long phhysical sector alignment error reporting: enabled; ");
        else    kprintf("Alignment error occurs: device reports command aborted; ");
    }
    if (p->word49_dma_supported_) {
        if (ata_type == PATA)    kprintf("DMA: supported; ");
    }
    if (p->word49_lba_supported_)    kprintf("LBA: supported; ");
    if (p->word49_iordy_display_supported_) {
        if (ata_type == PATA)    kprintf("Disabling of IORDY via SET FEATURES command: supported; ");
    }
    if (p->word49_iordy_supported_) {
        if (ata_type == PATA)    kprintf("IORDY signal: supported; ");
    }
    if (p->word49_standby_timer_periods_)    kprintf("Standby timer: vendor; ");
    else    kprintf("Standby timer: Standard; ");

    if (p->word50_standby_timer_mini_)    kprintf("Minimum standby timer: no; ");
    else    kprintf("Minimum standby timer: vendor; ");

    bool flag_validate_words64__70 = false;
    if (ata_type == PATA) {
        if (p->word53_set_to_validate_word64_70_)    flag_validate_words64__70 = true;
    }
    bool flag_validate_word88 = false;
    if (ata_type == PATA) {
        if (p->word53_set_to_validate_word88_)    flag_validate_word88 = true;
    }
    if (p->word53_free_fall_sensitivity_ != 0)
        kprintf("FREE-FALL SENSITIVITY: supported; ");

    if (p->word59_set_to_validate_multirw_)
        kprintf("Number of logical sectors of per DRQ data block: %d; ",
            p->word59_sectors_per_drq_data_block_);
    if (p->word59_sanitize_antifreeze_lock_supported_)
        kprintf("Sanitize antifreeze lock: supported; ");
    if (p->word59_acs3_commands_allowed_by_sanitize_)
        kprintf("ACS3 commands allowed by sanitize: supported; ");
    if (p->word59_sanitize_supported_)
        kprintf("Sanitize: supported; ");
    if (p->word59_crypto_scramble_supported_)
        kprintf("Cryptographic scramble: supported; ");
    if (p->word59_overwrite_supported_)
        kprintf("Overwrite: supported; ");
    if (p->word59_block_erase_supported_)
        kprintf("Block erase: supported; ");

    if (p->word60_61_total_addressable_sectors_lba28_ != 0)
        kprintf("Total sectors of LBA28: %d; ", ata_get_sectors(p));

    if (ata_type == PATA) {
        if (p->word63_multiword_dma_mode0_supported_)
            kprintf("Multiword DMA mode 0: supported; ");
        if (p->word63_multiword_dma_mode1_supported_)
            kprintf("Multiword DMA mode 1: supported; ");
        if (p->word63_multiword_dma_mode2_supported_)
            kprintf("Multiword DMA mode 2: supported; ");
    }
    if (p->word63_multiword_dma_mode0_enabled_)
        kprintf("Multiword DMA mode 0: enabled; ");
    if (p->word63_multiword_dma_mode1_enabled_)
        kprintf("Multiword DMA mode 1: enabled; ");
    if (p->word63_multiword_dma_mode2_enabled_)
        kprintf("Multiword DMA mode 2: enabled; ");

    bool flag_validate_words230__233 = false;
    if (flag_validate_words64__70) {

        if (ata_type == PATA) {
            if (p->word64_pio_mode3_is_supported_)
                kprintf("PIO mode 3: supported; ");
            if (p->word64_pio_mode4_is_supported_)
                kprintf("PIO mode 4: supported; ");
        }

        if (ata_type == PATA) {
            if (p->word65_min_multiword_cycle_time_)
                kprintf("Min multiword cycle time: %d; ",
                    p->word65_min_multiword_cycle_time_);
        }
        if (ata_type == PATA) {
            if (p->word66_recommended_multiword_cycle_time_)
                kprintf("Recommended multiword cycle time: %d; ",
                    p->word66_recommended_multiword_cycle_time_);
        }
        if (ata_type == PATA) {
            if (p->word67_min_pio_transfer_time_without_iordy_)
                kprintf("Min PIO transfer time without IORDY: %d; ",
                    p->word67_min_pio_transfer_time_without_iordy_);
        }
        if (ata_type == PATA) {
            if (p->word68_min_pio_transfer_time_with_iordy_)
                kprintf("Min PIO transfer time with IORDY: %d; ",
                    p->word68_min_pio_transfer_time_with_iordy_);
        }

        if (p->word69_non_volatile_write_cache_)
            kprintf("Non-volatile write cache: %d; ",
                p->word69_non_volatile_write_cache_);
        flag_validate_words230__233 =
            (p->word69_set_to_validate_word230_233_) ? true : false;
        if (p->word69_encrypt_all_supported_)    kprintf("Encrypt all: supported; ");
        if (p->word69_rzat_supported_)    kprintf("RZAT: supported; ");
        if (p->word69_28bit_supported_)    kprintf("28-bit: supported; ");
        if (p->word69_download_microcode_dma_supported_)
            kprintf("Download microcode DMA: supported; ");
        if (p->word69_write_buffer_dma_supported_)
            kprintf("Write buffer DMA: supported; ");
        if (p->word69_read_buffer_dma_supported_)
            kprintf("Read buffer DMA: supported; ");
        if (p->word69_lps_misalignment_reporting_supported_)
            kprintf("LPS misalignment reporting: supported; ");
        if (p->word69_drat_supported_)
            kprintf("DRAT: supported; ");

    }

    if (p->word75_ncq_feature_supported_) {
        kprintf("Maximum queue depth: %d; ", p->word75_maximum_queue_depth_ + 1);
    }

    bool flag_validate_words77__79 = false;
    if (!(*((uint16_t *)(p) + 76) == 0 || *((uint16_t *)(p) + 76) == 0xffff))
        flag_validate_words77__79 = true;
    if (flag_validate_words77__79 && ata_type == SATA) {
        if (p->word76_sata_gen1_signaling_speed_supported_)
            kprintf("SATA Gen1 signaling speed: supported; ");
        if (p->word76_sata_gen2_signaling_speed_supported_)
            kprintf("SATA Gen2 signaling speed: supported; ");
        if (p->word76_sata_gen3_signaling_speed_supported_)
            kprintf("SATA Gen3 signaling speed: supported; ");
        if (p->word76_ncq_feature_set_supported_)
            kprintf("NCQ feature set: supported; ");
        if (p->word76_receipt_of_host_initiated_power_management_requests_supported_)
            kprintf("Receipt of host-initiated power management requests: supported; ");
        if (p->word76_sata_phy_event_counters_log_supported_)
            kprintf("SATA PHY event counters log: supported; ");
        if (p->word76_unload_while_ncq_commands_are_outstanding_supported_)
            kprintf("Unload while NCQ commands are outstanding: supported; ");
        if (p->word76_ncq_priority_information_supported_)
            kprintf("NCQ priority information: supported; ");
        if (p->word76_host_automatic_partial_to_slumber_transtions_supported_)
            kprintf("Host automatic partial to slumber transitions: supported; ");
        if (p->word76_device_automatic_partial_to_slumber_transitions_supported_)
            kprintf("Device automatic partial to slumber transitions: supported; ");
        if (p->word76_read_log_dma_ext_as_equivalent_to_read_log_ext_supported_)
            kprintf("Read LOG DMA EXT as equivalent to READ LOG EXT: supported; ");
    }

    if (flag_validate_words77__79) {
        if (ata_type == SATA && (*((uint16_t *)p + 77) != 0)) {
            if (p->word77_current_serial_ata_signal_speed_)
                kprintf("Current SATA signal speed: %d; ",
                    p->word77_current_serial_ata_signal_speed_);
            if (p->word77_ncq_streaming_supported_)
                kprintf("NCQ streaming: supported; ");
            if (p->word77_ncq_queue_management_command_supported_)
                kprintf("NCQ queue management command: supported; ");
            if (p->word77_send_and_receive_queued_commands_supported_)
                kprintf("Send/Receive queued commands: supported; ");
        }

        if (p->word78_non_zero_buffer_offsets_supported_)
            kprintf("Non-zero buffer offsets: supported; ");
        if (p->word78_dma_setup_auto_activation_supported_)
            kprintf("DMA setup auto-activation: supported; ");
        if (p->word78_device_initiated_power_management_supported_)
            kprintf("Device initiated power management: supported; ");
        if (p->word78_in_order_data_delivery_supported_)
            kprintf("In-order data delivery: supported; ");
        if (p->word78_hardware_feature_control_supported_)
            kprintf("Hardware feature control: supported; ");
        if (p->word78_software_settings_preservation_supported_)
            kprintf("Software settings preservation: supported; ");
        if (p->word78_ncq_autosense_supported_)
            kprintf("NCQ autosense: supported; ");

        if (p->word79_non_zero_buffer_offsets_enabled_)
            kprintf("Non-zero buffer offsets: enabled; ");
        if (p->word79_dma_setup_auto_activation_enabled_)
            kprintf("DMA setup auto-activation: enabled; ");
        if (p->word79_device_initiated_power_management_enabled_)
            kprintf("Device initiated power management: enabled; ");
        if (p->word79_in_order_data_delivery_enabled_)
            kprintf("In-order data delivery: enabled; ");
        if (p->word79_hardware_feature_control_enabled_)
            kprintf("Hardware feature control: enabled; ");
        if (p->word79_software_settings_preservation_enabled_)
            kprintf("Software settings preservation: enabled; ");
        if (p->word79_automatic_partial_to_slumber_transitions_enabled_)
            kprintf("Antomatic partial to slumber transitions: enabled; ");

    } // end #77-79

    if (!(*((uint16_t *)p + 80) == 0 || *((uint16_t *)p + 80) == 0xffff))
        if (p->word80_major_version_number_)
            kprintf("Major version number: %d; ", p->word80_major_version_number_);

    if (p->word81_minor_version_number_ != 0 || p->word81_minor_version_number_ != 0xffff) {
        kprintf("Minor version number: ");
        switch (p->word81_minor_version_number_) {
        case 0x13: kprintf("ATA/ATAPI-5 T13 1321D version 3; "); break;
        case 0x15: kprintf("ATA/ATAPI-5 T13 1321D version 1; "); break;
        case 0x16: kprintf("ATA/ATAPI-5 published, ANSI INCITS 340-2000; "); break;
        case 0x18: kprintf("ATA/ATAPI-6 T13 1410D version 0; "); break;
        case 0x19: kprintf("ATA/ATAPI-6 T13 1410D version 3a; "); break;
        case 0x1a: kprintf("ATA/ATAPI-7 T13 1532D version 1; "); break;
        case 0x1b: kprintf("ATA/ATAPI-6 T13 1410D version 2; "); break;
        case 0x1c: kprintf("ATA/ATAPI-6 T13 1410D version 1; "); break;
        case 0x1d: kprintf("ATA/ATAPI-7 published, ANSI INCITS 397-2005; "); break;
        case 0x1e: kprintf("ATA/ATAPI-7 T13 1532D version 0; "); break;
        case 0x1f: kprintf("ACS-3 Revision 3b; "); break;
        case 0x21: kprintf("ATA/ATAPI-7 T13 1532D version 4a; "); break;
        case 0x22: kprintf("ATA/ATAPI-6 published, ANSI INCITS 361-2002; "); break;
        case 0x27: kprintf("ATA8-ACS vision 3c; "); break;
        case 0x28: kprintf("ATA8-ACS vision 6; "); break;
        case 0x29: kprintf("ATA8-ACS vision 4; "); break;
        case 0x31: kprintf("ACS-2 Revision 2; "); break;
        case 0x33: kprintf("ATA8-ACS vision 3e; "); break;
        case 0x39: kprintf("ATA8-ACS vision 4c; "); break;
        case 0x42: kprintf("ATA8-ACS vision 3f; "); break;
        case 0x52: kprintf("ATA8-ACS vision 3b; "); break;
        case 0x6d: kprintf("ACS-3 Revision 5; "); break;
        case 0x82: kprintf("ACS-2 published, ANSI INCITS 482-2012; "); break;
        case 0x107: kprintf("ATA8-ACS vision 2d; "); break;
        case 0x110: kprintf("ACS-2 Revision 3; "); break;
        case 0x11b: kprintf("ACS-3 Revision 4; "); break;
        default: kprintf("Reserved; "); break;
        }
    }

    bool flag_validate_words82__83 =
        (p->word83_setup_to_validate_word82_83_ == 1
        && p->word83_clear_to_validate_word82_83_ == 0) ? true : false;
    if (flag_validate_words82__83) {
        if (p->word82_smart_)
            kprintf("Smart feature: supported; ");
        if (p->word82_security_supported_)
            kprintf("Security: supported; ");
        if (p->word82_volatile_write_cache_supported_)
            kprintf("Volatile write cache: supported; ");
        if (p->word82_read_look_ahead_supported_)
            kprintf("Read look-ahead: supported; ");
        if (p->word82_write_buffer_supported_)
            kprintf("Write buffer: supported; ");
        if (p->word82_read_buffer_supported_)
            kprintf("Read buffer: supported; ");
        if (p->word82_nop_supported_)
            kprintf("Nop command: supported; ");

        if (p->word83_download_microcode_supported_)
            kprintf("Download microcode: supported; ");
        if (p->word83_apm_supported_)    kprintf("APM: supported; ");
        if (p->word83_puis_supported_)    kprintf("Puis: supported; ");
        if (p->word83_spin_up_supported_)    kprintf("Spin-up: supported; ");
        if (p->word83_48bit_supported_)    kprintf("48bit: supported; ");
        if (p->word83_flush_cache_ext_supported_)
            kprintf("Flush cache ext: supported; ");
    }

    bool flag_validate_word84 =
        (p->word84_set_to_validate_word84_ == 1
        && p->word84_clear_to_validate_word84_ == 0) ? true : false;
    if (flag_validate_word84) {
        if (p->word84_smart_error_logging_supported_)
            kprintf("Smart error logging: supported; ");
        if (p->word84_smart_self_test_supported_)
            kprintf("Smart self test: supported; ");
        if (p->word84_streaming_supported_)    kprintf("Streaming: supported; ");
        if (p->word84_gpl_supported_)    kprintf("GPL: supported; ");
        if (p->word84_write_fua_ext_supported_)
            kprintf("Write FUA ext: supported; ");
        if (p->word84_unload_supported_)    kprintf("Unload: supported; ");
    }

    bool flag_validate_words85__87 =
        (p->word87_set_to_validate_word85_87_ == 1
        && p->word87_clear_to_validate_word85_87_ == 0) ? true : false;
    if (flag_validate_words85__87) {
        if (p->word85_smart_enabled_)    kprintf("Smart feature: enabled; ");
        if (p->word85_security_enabled_)    kprintf("Security feature: enabled; ");
        if (p->word85_volatile_write_cache_enabled_)
            kprintf("Volatile write cache: enabled; ");
        if (p->word85_read_look_ahead_enabled_)
            kprintf("Read look-ahead: enabled; ");
        if (p->word85_write_buffer_supported_)    kprintf("Write buffer: supported; ");
        if (p->word85_read_buffer_supported_)    kprintf("Read buffer: supported; ");
        if (p->word85_nop_buffer_supported_)    kprintf("NOP buffer: supported; ");

        if (p->word86_download_microcode_supported_)
            kprintf("Download microcode: supported; ");
        if (p->word86_apm_enabled_)    kprintf("APM: enabled; ");
        if (p->word86_puis_enabled_)    kprintf("PUIS: enabled; ");
        if (p->word86_spin_up_supported_)    kprintf("Spin-up: supported; ");
        if (p->word86_48bit_supported_)    kprintf("48-bit: supported; ");
        if (p->word86_flush_cache_ext_supported_)
            kprintf("Flush cache extended: supported; ");

        if (p->word87_smart_error_logging_supported_)
            kprintf("Smart error logging: supported; ");
        if (p->word87_smart_self_test_supported_)
            kprintf("Smart self test: supported; ");
        if (p->word87_gpl_supported_)    kprintf("GPL: supported; ");
        if (p->word87_write_fua_ext_supported_)
            kprintf("Write FUA extended: supported; ");
        if (p->word87_unload_supported_)    kprintf("Unload: supported; ");
    }

    if (flag_validate_word88) {
        if (ata_type == PATA) {
            if (p->word88_pata_udma_mode0_supported_)    kprintf("PATA UDMA Mode0; ");
            if (p->word88_pata_udma_mode1_supported_)    kprintf("PATA UDMA Mode1; ");
            if (p->word88_pata_udma_mode2_supported_)    kprintf("PATA UDMA Mode2; ");
            if (p->word88_pata_udma_mode3_supported_)    kprintf("PATA UDMA Mode3; ");
            if (p->word88_pata_udma_mode4_supported_)    kprintf("PATA UDMA Mode4; ");
            if (p->word88_pata_udma_mode5_supported_)    kprintf("PATA UDMA Mode5; ");
            if (p->word88_pata_udma_mode6_supported_)    kprintf("PATA UDMA Mode6; ");
        }
        if (p->word88_udma_mode0_enabled_)    kprintf("UDMA Mode0; ");
        if (p->word88_udma_mode1_enabled_)    kprintf("UDMA Mode1; ");
        if (p->word88_udma_mode2_enabled_)    kprintf("UDMA Mode2; ");
        if (p->word88_udma_mode3_enabled_)    kprintf("UDMA Mode3; ");
        if (p->word88_udma_mode4_enabled_)    kprintf("UDMA Mode4; ");
        if (p->word88_udma_mode5_enabled_)    kprintf("UDMA Mode5; ");
        if (p->word88_udma_mode6_enabled_)    kprintf("UDMA Mode6; ");

    }

    if (p->word89_normal_security_erase_time_)
        kprintf("Normal security erase time: 0x%x; ",
            p->word89_normal_security_erase_time_);

    if (p->word90_enhanced_security_erase_time_)
        kprintf("Enhanced security erase time: 0x%x; ",
            p->word90_enhanced_security_erase_time_);

    if (p->word91_apm_level_)    kprintf("Apm level: %d; ", p->word91_apm_level_);

    if (p->word92_master_password_identifier_)
        kprintf("Master password identifier: 0x%x; ",
            p->word92_master_password_identifier_);

    if (ata_type == PATA) {
        bool flag_validate_word93 =
            (p->word93_pata_set_to_validate_word93_ == 1
            && p->word93_pata_clear_to_validate_word93_ == 0) ? true : false;
        if (flag_validate_word93) {
            if (p->word93_d0_device_number_detect_)
                kprintf("D0 device number detect: 0x%x; ",
                    p->word93_d0_device_number_detect_);
            if (p->word93_d0_diagnostic_)
                kprintf("D0 diagnostic: 0x%x; ", p->word93_d0_diagnostic_);
            if (p->word93_d0_pdiag_)
                kprintf("D0 pdiag: 0x%x; ", p->word93_d0_pdiag_);
            if (p->word93_d0_dasp_)
                kprintf("D0 dasp: 0x%x; ", p->word93_d0_dasp_);
            if (p->word93_d0_d1_selection_)
                kprintf("D0 d1 selection: 0x%x; ", p->word93_d0_d1_selection_);
            if (p->word93_d1_device_number_detect_)
                kprintf("D1 device number detect: 0x%x; ",
                    p->word93_d1_device_number_detect_);
            if (p->word93_d1_pdiag_)
                kprintf("D1 pdiag: 0x%x; ", p->word93_d1_pdiag_);
            if (p->word93_cblid_)
                kprintf("CBLID: 0x%x; ", p->word93_cblid_);
        }
    }

    if (p->word95_stream_minimum_request_size_)
        kprintf("Stream minimum request size: 0x%x; ", p->word95_stream_minimum_request_size_);

    if (p->word96_dma_sector_time_)
        kprintf("DMA sector time: 0x%x; ", p->word96_dma_sector_time_);

    if (p->word97_stream_access_latency_)
        kprintf("Stream access latency: 0x%x; ", p->word97_stream_access_latency_);

    if (p->word98_99_stream_granularity_)
        kprintf("Stream granularity: 0x%x; ", p->word98_99_stream_granularity_);

    uint32_t words100__103_low = *((uint32_t *)p + 100);
    uint32_t words100__103_high = *((uint32_t *)p + 101);
    kprintf("Max LBA: ");
    if (words100__103_high != 0)    kprintf("%x", words100__103_high);
    else    kprintf("0");
    if (words100__103_low != 0)    kprintf("%x", words100__103_low - 1);
    else    kprintf("0; ");

    if (p->word104_sector_time_)
        kprintf("Streaming transfer time: 0x%x; ", p->word104_sector_time_);

    if (p->word105_maximum_number_512b_blocks_ != 0)
        kprintf("Maximum number of 512-byte blocks: 0x%x; ",
            p->word105_maximum_number_512b_blocks_);

    bool flag_validate_word106 =
        (p->word106_set_to_validate_word106_ == 1
        && p->word106_clear_to_validate_word106_ == 0) ? true : false;
    if (flag_validate_word106) {
        if (p->word106_logical_to_physical_sector_relationship_)
            kprintf("Logical to physical sector relationship: 0x%x; ",
                p->word106_logical_to_physical_sector_relationship_);
        if (p->word106_logical_sector_size_supported_)
            kprintf("Logical sector size: supported; ",
                p->word106_logical_sector_size_supported_);
        if (p->word106_logical_to_physical_secctor_relationship_supported_)
            kprintf("Logical to physical sector relationship: supported; ",
                p->word106_logical_to_physical_secctor_relationship_supported_);
    }

    if (p->word107_inter_seek_delay_)
        kprintf("Inter-seek delay: 0x%x; ", p->word107_inter_seek_delay_);

    kprintf("World wide name: ");
    for (size_t i = 0; i < 4; ++i) {
        if (*((uint16_t *)p + 108 + i) != 0)
            kprintf("%x ", *((uint16_t *)p + 108 + i));
    }
    kprintf("; ");

    uint32_t logical_sector_size = *((uint32_t *)p->word117_118_logical_sector_size_);
    if (logical_sector_size != 0)
        kprintf("Logical sector size: 0x%x; ", logical_sector_size);

    bool flag_validate_word119 =
        (p->word86_set_to_validate_word119_120_ == 1
        && p->word119_set_to_validate_word119_ == 1
        && p->word119_clear_to_validate_word119_ == 0) ? true : false;
    if (flag_validate_word119) {
        if (p->word119_wrv_supported_)    kprintf("Wrv: supported; ");
        if (p->word119_write_uncorrectable_supported_)
            kprintf("Write uncorrectable: supported; ");
        if (p->word119_gpl_dma_supported_)    kprintf("GPL DMA: supported; ");
        if (p->word119_dm_mode3_supported_)    kprintf("DM mode 3: supported; ");
        if (p->word119_free_fall_supported_)    kprintf("Free fall: supported; ");
        if (p->word119_sense_data_supported_)    kprintf("Sense data: supported; ");
        if (p->word119_epc_supported_)    kprintf("EPC: supported; ");
        if (p->word119_amax_addr_supported_)    kprintf("Amax addr: supported; ");
        if (p->word119_dsn_supported_)    kprintf("DSN: supported; ");
    }

    bool flag_validate_word120 =
        (p->word86_set_to_validate_word119_120_ == 1
        && p->word120_set_to_validate_word120_ == 1
        && p->word120_clear_to_validate_word120_ == 0) ? true : false;
    if (flag_validate_word120) {
        if (p->word120_wrv_enabled_)    kprintf("Wrv: enabled; ");
        if (p->word120_write_uncorrectable_supported_)
            kprintf("Write uncorrectable: supported; ");
        if (p->word120_gpl_dma_supported_)    kprintf("GPL DMA: supported; ");
        if (p->word120_dm_mode3_supported_)    kprintf("DM mode 3: supported; ");
        if (p->word120_free_fall_supported_)    kprintf("Free fall: supported; ");
        if (p->word120_sense_data_enabled_)    kprintf("Sense data: enabled; ");
        if (p->word120_epc_enabled_)    kprintf("EPC: enabled; ");
        if (p->word120_dsn_enabled_)    kprintf("DSN: enabled; ");
    }

    if (p->word128_security_supported_)    kprintf("Security: supported; ");
    if (p->word128_security_enabled_)    kprintf("Security: enabled; ");
    if (p->word128_security_locked_)    kprintf("Security: locked; ");
    if (p->word128_security_frozen_)    kprintf("Security: frozen; ");
    if (p->word128_security_count_expired_)    kprintf("Security count: expired; ");
    if (p->word128_enhanced_security_erase_supported_)
        kprintf("Enhanced security erase: supported; ");
    if (p->word128_master_password_capability_)
        kprintf("Master password capability: 0x%x; ", 
            p->word128_master_password_capability_);

    kprintf("Vendor specific: %s; ",
        ata_print_string(&p->word129_159_, sizeof(ata_vendor_specific_t)));

    if (p->word168_nominal_form_factor_)
        kprintf("Nominal form factor: 0x%x; ", p->word168_nominal_form_factor_);

    if (p->word169_trim_supported_)    kprintf("Trim: supported; ");

    kprintf("Additional product identifier: %s; ",
        ata_print_string(&p->word170_173_, sizeof(ata_extra_product_id_t)));

    bool flag_validate_words176__205 =
        (p->word87_set_to_validate_word176_205_ == 1) ? true : false;
    if (flag_validate_words176__205) {
        kprintf("Current media serial number: %s; ",
            ata_print_string(&p->word176_205_, sizeof(ata_current_media_serial_number_t)));
    }

    if (p->word206_set_to_support_sct_command_transport_)
        kprintf("SCT command transport: supported; ");
    if (p->word206_set_to_support_sct_write_same_)
        kprintf("SCT write same: supported; ");
    if (p->word206_set_to_support_sct_error_recovery_control_)
        kprintf("SCT error recovery control: supported; ");
    if (p->word206_set_to_support_sct_feature_control_)
        kprintf("SCT feature control: supported; ");
    if (p->word206_set_to_support_sct_data_tables_)
        kprintf("SCT data tables: supported; ");
    if (p->word206_vendor_specific_)
        kprintf("Vendor specific: 0x%x; ", p->word206_vendor_specific_);

    if (p->word106_logical_to_physical_secctor_relationship_supported_) {
        if (p->word209_logical_sector_offset_)
            kprintf("Logical sector offset: 0x%x; ", p->word209_logical_sector_offset_);
    }

    uint32_t wrv_mode3_count = *((uint32_t *)p->word210_211_wrv_mode3_count_);
    if (wrv_mode3_count != 0)
        kprintf("WRV mode3 count: 0x%x; ", wrv_mode3_count);

    uint32_t wrv_mode2_count = *((uint32_t *)p->word212_213_wrv_mode2_count_);
    if (wrv_mode2_count != 0)
        kprintf("WRV mode2 count: 0x%x; ", wrv_mode2_count);

    if (p->word217_nominal_media_rotation_rate_)
        kprintf("Nominal media rotation rate: 0x%x; ",
            p->word217_nominal_media_rotation_rate_);

    if (p->word220_wrv_mode_)
        kprintf("WRV mode: 0x%x; ", p->word220_wrv_mode_);

    if (*((uint16_t *)p + 222) != 0 || *((uint16_t *)p + 222) != 0xffff) {
        if (p->word222_ata_transport_major_version_)
            kprintf("ATA transport major version: 0x%x; ",
                p->word222_ata_transport_major_version_);
        if (p->word222_transport_type_)
            kprintf("Transport type: 0x%x; ", p->word222_transport_type_);
    }

    if (p->word223_standard_version_ != 0 || p->word223_standard_version_ != 0xffff) {
        if (p->word223_standard_version_ == 0x21)
            kprintf("Minor standard version: ATA8-AST T13 Project D1697 version 0b");
        if (p->word223_standard_version_ == 0x51)
            kprintf("Minor standard version: ATA-AST T13 Project D1697 version 1");
    }

    if (flag_validate_words230__233) {
            ata_print_extended_number_of_sectors(&p->word230_233_);
    }

    if (p->word234_dm_minimum_transfer_size_)
        kprintf("Minimum transfer size: 0x%x; ", p->word234_dm_minimum_transfer_size_);

    if (p->word235_dm_maximum_transfer_size_)
        kprintf
        ("Maximum transfer size: 0x%x; ", p->word235_dm_maximum_transfer_size_);

    if (p->word255_checksum_validity_indicator_ == 0xa5)
        kprintf("Data structure checksum: 0x%x; ", p->word255_data_structure_checksum_);

    kprintf("\n");
}
