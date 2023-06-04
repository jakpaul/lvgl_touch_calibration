#pragma once

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/

#include "stdbool.h"
#include "lv_tc.h"

#ifdef ESP_PLATFORM
    #include "nvs_flash.h"
    #include "nvs.h"
#endif


/*********************
 *      DEFINES
 *********************/

#if defined CONFIG_ESP_NVS_TC_PARTITION
#define ESP_NVS_TC_PARTITION CONFIG_ESP_NVS_TC_PARTITION
#else
#define ESP_NVS_TC_PARTITION    NVS_DEFAULT_PART_NAME       //Defaults to "nvs"
#endif

#if defined CONFIG_ESP_NVS_TC_NAMESPACE
#define ESP_NVS_TC_NAMESPACE CONFIG_ESP_NVS_TC_NAMESPACE
#else
#define ESP_NVS_TC_NAMESPACE    "touch_calib"
#endif

#if defined CONFIG_ESP_NVS_TC_KEY
#define ESP_NVS_TC_KEY CONFIG_ESP_NVS_TC_KEY
#else
#define ESP_NVS_TC_KEY          "coeff"
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the ESP's non-volatile storage for storing the calibration coefficients.
 * Also restores existing coefficients.
 * @returns true if coefficients were restored, false if none exist yet
 */
bool esp_nvs_tc_coeff_init();

/**
 * Initialize the ESP's non-volatile storage and delete the stored calibration coefficients.
 * @returns true if coefficients were deleted from the NVS, else false
 */
bool esp_nvs_tc_coeff_erase();

/**
 * The default callback for writing the calibartion coefficients to nvs.
 * @param coeff the coefficients to save (lv_tc_coeff_t)
 */
void esp_nvs_tc_coeff_save_cb(lv_tc_coeff_t coeff);



#ifdef __cplusplus
}
#endif