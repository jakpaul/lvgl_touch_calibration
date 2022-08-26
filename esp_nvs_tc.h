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

#define ESP_NVS_TC_PARTITION    NVS_DEFAULT_PART_NAME       //Defaults to "nvs"
#define ESP_NVS_TC_NAMESPACE    "touch_calib"
#define ESP_NVS_TC_KEY          "coeff"


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
 * The default callback for writing the calibartion coefficients to nvs.
 * @param coeff the coefficients to save (lv_tc_coeff_t)
 */
void esp_nvs_tc_coeff_save_cb(lv_tc_coeff_t coeff);



#ifdef __cplusplus
}
#endif