/************************************
 *                                  *
 *  Copyright (C)    horbyn, 2023   *
 *        (hoRbyn4zZ@outlook.com)   *
 *                                  *
 ************************************/
#pragma once
#ifndef __DEVICE_ATA_ATA_IDENTIFY_DATA_H__
#define __DEVICE_ATA_ATA_IDENTIFY_DATA_H__

#include "ata_identify_data_log.h"
#include "ata_stuff.h"
#include "kern/types.h"
#include "kern/debug.h"
#include "kern/lib/lib.h"

#define SIZE_ATA_STRING_BUFF 1024

typedef struct ata_serial_number {
    uint16_t serial_number_[10];
} __attribute__((packed)) ata_serial_number_t;              // serial number

typedef struct ata_firmware_revision {
    uint16_t firmware_revision_[4];
} __attribute__((packed)) ata_firmware_revision_t;          // firmware revision

typedef struct ata_model_number {
    uint16_t model_number_[20];
} __attribute__((packed)) ata_model_number_t;               // model number

typedef struct ata_vendor_specific {
    uint16_t vendor_specific_[31];                          // vendor specific
} __attribute__((packed)) ata_vendor_specific_t;

typedef struct ata_extra_product_identifier {
    uint16_t extra_product_id_[4];                          // additional product identifier
} __attribute__((packed)) ata_extra_product_id_t;

typedef struct ata_current_media_serial_number {
    uint16_t current_media_serial_number_[30];              // current media serial number
} __attribute__((packed)) ata_current_media_serial_number_t;

typedef struct ata_extended_number_of_sectors {
    uint32_t extended_number_of_sectors_[2];                // extended number of sectors
} __attribute__((packed)) ata_extended_number_of_sectors_t;

typedef struct ata_identify_data {
    uint16_t :15;
    uint16_t word0_ata_device_ :1;                          // general configuration

    uint16_t :16;
    uint16_t word2_specific_configuration_;                 // specific configuration
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    ata_serial_number_t word10_19_;                         // serial number
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    ata_firmware_revision_t word23_26_;                     // firmware revision
    ata_model_number_t word27_46_;                          // model number

    uint16_t word47_maximum_sectors_per_drq_block_:8;       // maximum number of logical sectors per drq data block
    uint16_t :8;

    uint16_t word48_trusted_computing_feature_:1;           // trusted computing feature set options
    uint16_t :15;

    uint16_t word49_alignment_error_reporting_:2;           // capabilities
    uint16_t :6;                                            // capabilities
    uint16_t word49_dma_supported_:1;                       // capabilities
    uint16_t word49_lba_supported_:1;                       // capabilities
    uint16_t word49_iordy_display_supported_:1;             // capabilities
    uint16_t word49_iordy_supported_:1;                     // capabilities
    uint16_t :1;                                            // capabilities
    uint16_t word49_standby_timer_periods_:1;               // capabilities
    uint16_t :2;                                            // capabilities

    uint16_t word50_standby_timer_mini_:1;                  // capabilities
    uint16_t :15;                                           // capabilities

    uint16_t :16;
    uint16_t :16;

    uint16_t :1;
    uint16_t word53_set_to_validate_word64_70_:1;
    uint16_t word53_set_to_validate_word88_:1;
    uint16_t :5;
    uint16_t word53_free_fall_sensitivity_:8;

    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;

    uint16_t word59_sectors_per_drq_data_block_:8;
    uint16_t word59_set_to_validate_multirw_:1;
    uint16_t :1;
    uint16_t word59_sanitize_antifreeze_lock_supported_:1;
    uint16_t word59_acs3_commands_allowed_by_sanitize_:1;
    uint16_t word59_sanitize_supported_:1;
    uint16_t word59_crypto_scramble_supported_:1;
    uint16_t word59_overwrite_supported_:1;
    uint16_t word59_block_erase_supported_:1;

    uint16_t word60_61_total_addressable_sectors_lba28_[2];
    uint16_t :16;

    uint16_t word63_multiword_dma_mode0_supported_:1;
    uint16_t word63_multiword_dma_mode1_supported_:1;
    uint16_t word63_multiword_dma_mode2_supported_:1;
    uint16_t :5;
    uint16_t word63_multiword_dma_mode0_enabled_:1;
    uint16_t word63_multiword_dma_mode1_enabled_:1;
    uint16_t word63_multiword_dma_mode2_enabled_:1;
    uint16_t :5;

    uint16_t word64_pio_mode3_is_supported_:1;
    uint16_t word64_pio_mode4_is_supported_:1;
    uint16_t :14;

    uint16_t word65_min_multiword_cycle_time_;
    uint16_t word66_recommended_multiword_cycle_time_;
    uint16_t word67_min_pio_transfer_time_without_iordy_;
    uint16_t word68_min_pio_transfer_time_with_iordy_;

    uint16_t :2;                                            // additional supported
    uint16_t word69_non_volatile_write_cache_:1;            // additional supported
    uint16_t word69_set_to_validate_word230_233_:1;         // additional supported
    uint16_t word69_encrypt_all_supported_:1;               // additional supported
    uint16_t word69_rzat_supported_:1;                      // additional supported
    uint16_t word69_28bit_supported_:1;                     // additional supported
    uint16_t :1;                                            // additional supported
    uint16_t word69_download_microcode_dma_supported_:1;    // additional supported
    uint16_t :1;                                            // additional supported
    uint16_t word69_write_buffer_dma_supported_:1;          // additional supported
    uint16_t word69_read_buffer_dma_supported_:1;           // additional supported
    uint16_t :1;                                            // additional supported
    uint16_t word69_lps_misalignment_reporting_supported_:1;// additional supported
    uint16_t word69_drat_supported_:1;                      // additional supported
    uint16_t :1;                                            // additional supported

    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;

    uint16_t word75_maximum_queue_depth_:5;                 // queue depth
    uint16_t :1;                                            // queue depth
    uint16_t word75_ncq_feature_supported_:1;               // queue depth
    uint16_t :9;                                            // queue depth

    uint16_t :1;                                            // serial ata capabilities
    uint16_t word76_sata_gen1_signaling_speed_supported_:1;
    uint16_t word76_sata_gen2_signaling_speed_supported_:1;
    uint16_t word76_sata_gen3_signaling_speed_supported_:1;
    uint16_t :4;
    uint16_t word76_ncq_feature_set_supported_:1;
    uint16_t word76_receipt_of_host_initiated_power_management_requests_supported_:1;
    uint16_t word76_sata_phy_event_counters_log_supported_:1;
    uint16_t word76_unload_while_ncq_commands_are_outstanding_supported_:1;
    uint16_t word76_ncq_priority_information_supported_:1;
    uint16_t word76_host_automatic_partial_to_slumber_transtions_supported_:1;
    uint16_t word76_device_automatic_partial_to_slumber_transitions_supported_:1;
    uint16_t word76_read_log_dma_ext_as_equivalent_to_read_log_ext_supported_:1;

    uint16_t :1;                                            // serial ata additional capabilities
    uint16_t word77_current_serial_ata_signal_speed_:3;     // serial ata additional capabilities
    uint16_t word77_ncq_streaming_supported_:1;             // serial ata additional capabilities
    uint16_t word77_ncq_queue_management_command_supported_:1;// serial ata additional capabilities
    uint16_t word77_send_and_receive_queued_commands_supported_:1;// serial ata additional capabilities
    uint16_t :9;                                            // serial ata additional capabilities

    uint16_t :1;                                            // serial ata features supported
    uint16_t word78_non_zero_buffer_offsets_supported_:1;   // serial ata features supported
    uint16_t word78_dma_setup_auto_activation_supported_:1; // serial ata features supported
    uint16_t word78_device_initiated_power_management_supported_:1;// serial ata features supported
    uint16_t word78_in_order_data_delivery_supported_:1;    // serial ata features supported
    uint16_t word78_hardware_feature_control_supported_:1;  // serial ata features supported
    uint16_t word78_software_settings_preservation_supported_:1;// serial ata features supported
    uint16_t word78_ncq_autosense_supported_:1;             // serial ata features supported
    uint16_t :8;                                            // serial ata features supported

    uint16_t :1;                                            // serial ata features enabled
    uint16_t word79_non_zero_buffer_offsets_enabled_:1;     // serial ata features enabled
    uint16_t word79_dma_setup_auto_activation_enabled_:1;   // serial ata features enabled
    uint16_t word79_device_initiated_power_management_enabled_:1;// serial ata features enabled
    uint16_t word79_in_order_data_delivery_enabled_:1;      // serial ata features enabled
    uint16_t word79_hardware_feature_control_enabled_:1;    // serial ata features enabled
    uint16_t word79_software_settings_preservation_enabled_:1;// serial ata features enabled
    uint16_t word79_automatic_partial_to_slumber_transitions_enabled_:1;// serial ata features enabled
    uint16_t :8;                                            // serial ata features enabled

    uint16_t :5;                                            // major version number
    uint16_t word80_major_version_number_:6;                // major version number
    uint16_t :5;                                            // major version number

    uint16_t word81_minor_version_number_;                  // minor version number

    uint16_t word82_smart_:1;                               // commands and feature sets supported
    uint16_t word82_security_supported_:1;                  // commands and feature sets supported
    uint16_t :3;                                            // commands and feature sets supported
    uint16_t word82_volatile_write_cache_supported_:1;      // commands and feature sets supported
    uint16_t word82_read_look_ahead_supported_:1;           // commands and feature sets supported
    uint16_t :5;                                            // commands and feature sets supported
    uint16_t word82_write_buffer_supported_:1;              // commands and feature sets supported
    uint16_t word82_read_buffer_supported_:1;               // commands and feature sets supported
    uint16_t word82_nop_supported_:1;                       // commands and feature sets supported
    uint16_t :1;                                            // commands and feature sets supported

    uint16_t word83_download_microcode_supported_:1;        // commands and feature sets supported
    uint16_t :2;                                            // commands and feature sets supported
    uint16_t word83_apm_supported_:1;                       // commands and feature sets supported
    uint16_t :1;                                            // commands and feature sets supported
    uint16_t word83_puis_supported_:1;                      // commands and feature sets supported
    uint16_t word83_spin_up_supported_:1;                   // commands and feature sets supported
    uint16_t :3;                                            // commands and feature sets supported
    uint16_t word83_48bit_supported_:1;                     // commands and feature sets supported
    uint16_t :2;                                            // commands and feature sets supported
    uint16_t word83_flush_cache_ext_supported_:1;           // commands and feature sets supported
    uint16_t word83_setup_to_validate_word82_83_:1;         // commands and feature sets supported
    uint16_t word83_clear_to_validate_word82_83_:1;         // commands and feature sets supported

    uint16_t word84_smart_error_logging_supported_:1;       // commands and feature sets supported
    uint16_t word84_smart_self_test_supported_:1;           // commands and feature sets supported
    uint16_t :2;                                            // commands and feature sets supported
    uint16_t word84_streaming_supported_:1;                 // commands and feature sets supported
    uint16_t word84_gpl_supported_:1;                       // commands and feature sets supported
    uint16_t word84_write_fua_ext_supported_:1;             // commands and feature sets supported
    uint16_t :6;                                            // commands and feature sets supported
    uint16_t word84_unload_supported_:1;                    // commands and feature sets supported
    uint16_t word84_set_to_validate_word84_:1;              // commands and feature sets supported
    uint16_t word84_clear_to_validate_word84_:1;            // commands and feature sets supported

    uint16_t word85_smart_enabled_:1;                       // commands and feature sets supported or enabled
    uint16_t word85_security_enabled_:1;                    // commands and feature sets supported or enabled
    uint16_t :3;                                            // commands and feature sets supported or enabled
    uint16_t word85_volatile_write_cache_enabled_:1;        // commands and feature sets supported or enabled
    uint16_t word85_read_look_ahead_enabled_:1;             // commands and feature sets supported or enabled
    uint16_t :5;                                            // commands and feature sets supported or enabled
    uint16_t word85_write_buffer_supported_:1;              // commands and feature sets supported or enabled
    uint16_t word85_read_buffer_supported_:1;               // commands and feature sets supported or enabled
    uint16_t word85_nop_buffer_supported_:1;                // commands and feature sets supported or enabled
    uint16_t :1;                                            // commands and feature sets supported or enabled

    uint16_t word86_download_microcode_supported_:1;        // commands and feature sets supported or enabled
    uint16_t :2;                                            // commands and feature sets supported or enabled
    uint16_t word86_apm_enabled_:1;                         // commands and feature sets supported or enabled
    uint16_t :1;                                            // commands and feature sets supported or enabled
    uint16_t word86_puis_enabled_:1;                        // commands and feature sets supported or enabled
    uint16_t word86_spin_up_supported_:1;                   // commands and feature sets supported or enabled
    uint16_t :3;                                            // commands and feature sets supported or enabled
    uint16_t word86_48bit_supported_:1;                     // commands and feature sets supported or enabled
    uint16_t :2;                                            // commands and feature sets supported or enabled
    uint16_t word86_flush_cache_ext_supported_:1;           // commands and feature sets supported or enabled
    uint16_t :1;                                            // commands and feature sets supported or enabled
    uint16_t word86_set_to_validate_word119_120_:1;         // commands and feature sets supported or enabled

    uint16_t word87_smart_error_logging_supported_:1;       // commands and feature sets supported or enabled
    uint16_t word87_smart_self_test_supported_:1;           // commands and feature sets supported or enabled
    uint16_t word87_set_to_validate_word176_205_:1;         // commands and feature sets supported or enabled
    uint16_t :2;                                            // commands and feature sets supported or enabled
    uint16_t word87_gpl_supported_:1;                       // commands and feature sets supported or enabled
    uint16_t word87_write_fua_ext_supported_:1;             // commands and feature sets supported or enabled
    uint16_t :6;                                            // commands and feature sets supported or enabled
    uint16_t word87_unload_supported_:1;                    // commands and feature sets supported or enabled
    uint16_t word87_set_to_validate_word85_87_:1;           // commands and feature sets supported or enabled
    uint16_t word87_clear_to_validate_word85_87_:1;         // commands and feature sets supported or enabled

    uint16_t word88_pata_udma_mode0_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode1_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode2_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode3_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode4_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode5_supported_:1;           // ultra dma mode
    uint16_t word88_pata_udma_mode6_supported_:1;           // ultra dma mode
    uint16_t :1;                                            // ultra dma mode
    uint16_t word88_udma_mode0_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode1_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode2_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode3_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode4_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode5_enabled_:1;                  // ultra dma mode
    uint16_t word88_udma_mode6_enabled_:1;                  // ultra dma mode
    uint16_t :1;                                            // ultra dma mode

    uint16_t word89_normal_security_erase_time_;
    uint16_t word90_enhanced_security_erase_time_;

    uint16_t word91_apm_level_:8;                           // current advanced power management level value
    uint16_t :8;                                            // current advanced power management level value

    uint16_t word92_master_password_identifier_;            // master password identifier

    uint16_t :1;                                            // hardware reset results
    uint16_t word93_d0_device_number_detect_:2;             // hardware reset results
    uint16_t word93_d0_diagnostic_:1;                       // hardware reset results
    uint16_t word93_d0_pdiag_:1;                            // hardware reset results
    uint16_t word93_d0_dasp_:1;                             // hardware reset results
    uint16_t word93_d0_d1_selection_:1;                     // hardware reset results
    uint16_t :2;                                            // hardware reset results
    uint16_t word93_d1_device_number_detect_:2;             // hardware reset results
    uint16_t word93_d1_pdiag_:1;                            // hardware reset results
    uint16_t :1;                                            // hardware reset results
    uint16_t word93_cblid_:1;                               // hardware reset results
    uint16_t word93_pata_set_to_validate_word93_:1;         // hardware reset results
    uint16_t word93_pata_clear_to_validate_word93_:1;       // hardware reset results

    uint16_t :16;
    uint16_t word95_stream_minimum_request_size_;           // stream minimum request size
    uint16_t word96_dma_sector_time_;                       // streaming transfer time--dma
    uint16_t word97_stream_access_latency_;                 // streaming access latency--dma and pio
    uint16_t word98_99_stream_granularity_[2];              // streaming performance granularity
    uint16_t word100_103_number_of_sectors_[4];             // number of user addressable logical sectors
    uint16_t word104_sector_time_;                          // streaming transfer time--pio
    uint16_t word105_maximum_number_512b_blocks_;           // the maximum number of 512-byte blocks of
                                                            // LBA range entries per data set management command

    uint16_t word106_logical_to_physical_sector_relationship_:4;// physical sector size/logical sector size
    uint16_t :8;                                            // physical sector size/logical sector size
    uint16_t word106_logical_sector_size_supported_:1;      // physical sector size/logical sector size
    uint16_t word106_logical_to_physical_secctor_relationship_supported_:1;// physical sector size/logical sector size
    uint16_t word106_set_to_validate_word106_:1;            // physical sector size/logical sector size
    uint16_t word106_clear_to_validate_word106_:1;          // physical sector size/logical sector size

    uint16_t word107_inter_seek_delay_;                     // time delay between seeks in microseconds during
                                                            // iso/iec 7779 standard acoustic testing
    uint16_t word108_111_world_wide_name_[4];               // world wide name
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t word117_118_logical_sector_size_[2];           // logical sector size

    uint16_t :1;                                            // commands and feature sets supported
    uint16_t word119_wrv_supported_:1;                      // commands and feature sets supported
    uint16_t word119_write_uncorrectable_supported_:1;      // commands and feature sets supported
    uint16_t word119_gpl_dma_supported_:1;                  // commands and feature sets supported
    uint16_t word119_dm_mode3_supported_:1;                 // commands and feature sets supported
    uint16_t word119_free_fall_supported_:1;                // commands and feature sets supported
    uint16_t word119_sense_data_supported_:1;               // commands and feature sets supported
    uint16_t word119_epc_supported_:1;                      // commands and feature sets supported
    uint16_t word119_amax_addr_supported_:1;                // commands and feature sets supported
    uint16_t word119_dsn_supported_:1;                      // commands and feature sets supported
    uint16_t :4;                                            // commands and feature sets supported
    uint16_t word119_set_to_validate_word119_:1;            // commands and feature sets supported
    uint16_t word119_clear_to_validate_word119_:1;          // commands and feature sets supported

    uint16_t :1;                                            // commands and feature sets supported or enabled
    uint16_t word120_wrv_enabled_:1;                        // commands and feature sets supported or enabled
    uint16_t word120_write_uncorrectable_supported_:1;      // commands and feature sets supported or enabled
    uint16_t word120_gpl_dma_supported_:1;                  // commands and feature sets supported or enabled
    uint16_t word120_dm_mode3_supported_:1;                 // commands and feature sets supported or enabled
    uint16_t word120_free_fall_supported_:1;                // commands and feature sets supported or enabled
    uint16_t word120_sense_data_enabled_:1;                 // commands and feature sets supported or enabled
    uint16_t word120_epc_enabled_:1;                        // commands and feature sets supported or enabled
    uint16_t :1;                                            // commands and feature sets supported or enabled
    uint16_t word120_dsn_enabled_:1;                        // commands and feature sets supported or enabled
    uint16_t :4;                                            // commands and feature sets supported or enabled
    uint16_t word120_set_to_validate_word120_:1;            // commands and feature sets supported or enabled
    uint16_t word120_clear_to_validate_word120_:1;          // commands and feature sets supported or enabled

    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;

    uint16_t word128_security_supported_:1;                 // security status
    uint16_t word128_security_enabled_:1;                   // security status
    uint16_t word128_security_locked_:1;                    // security status
    uint16_t word128_security_frozen_:1;                    // security status
    uint16_t word128_security_count_expired_:1;             // security status
    uint16_t word128_enhanced_security_erase_supported_:1;  // security status
    uint16_t :2;                                            // security status
    uint16_t word128_master_password_capability_:1;         // security status
    uint16_t :7;                                            // security status

    ata_vendor_specific_t word129_159_;                     // vendor specific
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;

    uint16_t word168_nominal_form_factor_:4;                // device nominal form factor
    uint16_t :12;                                           // device nominal form factor

    uint16_t word169_trim_supported_:1;                     // data set management support
    uint16_t :15;                                           // data set management support

    ata_extra_product_id_t word170_173_;                    // additional product identifier
    uint16_t :16;
    uint16_t :16;
    ata_current_media_serial_number_t word176_205_;         // current media serial number

    uint16_t word206_set_to_support_sct_command_transport_:1;// sct command transport
    uint16_t :1;                                            // sct command transport
    uint16_t word206_set_to_support_sct_write_same_:1;      // sct command transport
    uint16_t word206_set_to_support_sct_error_recovery_control_:1;// sct command transport
    uint16_t word206_set_to_support_sct_feature_control_:1; // sct command transport
    uint16_t word206_set_to_support_sct_data_tables_:1;     // sct command transport
    uint16_t :6;                                            // sct command transport
    uint16_t word206_vendor_specific_:4;                    // sct command transport

    uint16_t :16;
    uint16_t :16;

    uint16_t word209_logical_sector_offset_:14;             // alignment of logical blocks within a physical block
    uint16_t :2;                                            // alignment of logical blocks within a physical block

    uint16_t word210_211_wrv_mode3_count_[2];               // write-read-verify sector mode 3 count
    uint16_t word212_213_wrv_mode2_count_[2];               // write-read-verify sector mode 2 count
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t word217_nominal_media_rotation_rate_;          // nominal media rotation rate
    uint16_t :16;
    uint16_t :16;

    uint16_t word220_wrv_mode_:8;
    uint16_t :8;

    uint16_t :16;

    uint16_t word222_ata_transport_major_version_:12;       // transport major version number
    uint16_t word222_transport_type_:4;                     // transport major version number

    uint16_t word223_standard_version_;                     // transport_minor_version_number_
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    ata_extended_number_of_sectors_t word230_233_;          // extended number of user addressable sectors
    uint16_t word234_dm_minimum_transfer_size_;             // minimum number of 512-byte data blocks per download
                                                            // microcode mode 3 operation
    uint16_t word235_dm_maximum_transfer_size_;             // maximum number of 512-byte data blocks per download
                                                            // microcode mode 3 operation
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;
    uint16_t :16;

    uint16_t word255_checksum_validity_indicator_:8;        // integrity word
    uint16_t word255_data_structure_checksum_:8;            // integrity word

} __attribute__((packed)) ata_identify_data_t;

const char *ata_print_string(const void *, size_t);
enum_ata_type_device ata_get_device_type(const ata_identify_data_t *);
void ata_get_serial_number(const ata_identify_data_t *, ata_serial_number_t *);
bool ata_isempty_serial_number(const ata_serial_number_t *);
void ata_get_model_number(const ata_identify_data_t *, ata_model_number_t *);
bool ata_isempty_model_number(const ata_model_number_t *);
void ata_print_extended_number_of_sectors(const ata_extended_number_of_sectors_t *);
uint32_t ata_get_sectors(const ata_identify_data_t *);
void ata_identify_data_parse(const void *, size_t, enum_ata_type);

#endif
