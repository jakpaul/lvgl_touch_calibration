#ifdef ESP_PLATFORM
/*********************
 *      INCLUDES
 *********************/

#include "esp_nvs_tc.h"


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool esp_nvs_tc_coeff_init() {
    ESP_ERROR_CHECK(nvs_flash_init_partition(ESP_NVS_TC_PARTITION));

    nvs_handle_t nVSHandle;
    lv_tc_coeff_t coeff = {false, 0, 0, 0, 0, 0, 0};
    size_t length = sizeof(coeff);

    //NVS must be opened in NVS_READWRITE mode to create the namespace if it does not exist already
    ESP_ERROR_CHECK(nvs_open_from_partition(ESP_NVS_TC_PARTITION, ESP_NVS_TC_NAMESPACE, NVS_READWRITE, &nVSHandle));

    esp_err_t err = nvs_get_blob(nVSHandle, ESP_NVS_TC_KEY, &coeff, &length);
    nvs_close(nVSHandle);

    if(err == ESP_ERR_NVS_NOT_FOUND || !coeff.isValid) {
        return false;
    } else {
        ESP_ERROR_CHECK(err);
        lv_tc_set_coeff(coeff, false);
        return true;
    }
}

bool esp_nvs_tc_coeff_erase() {
    ESP_ERROR_CHECK(nvs_flash_init_partition(ESP_NVS_TC_PARTITION));

    nvs_handle_t nVSHandle;
    ESP_ERROR_CHECK(nvs_open_from_partition(ESP_NVS_TC_PARTITION, ESP_NVS_TC_NAMESPACE, NVS_READWRITE, &nVSHandle));
    esp_err_t err = nvs_erase_key(nVSHandle, ESP_NVS_TC_KEY);
    nvs_close(nVSHandle);

    if(err == ESP_OK) {
        return true;
    }

    return true;
}

void esp_nvs_tc_coeff_save_cb(lv_tc_coeff_t coeff) {
    nvs_handle_t nVSHandle;

    ESP_ERROR_CHECK(nvs_open_from_partition(ESP_NVS_TC_PARTITION, ESP_NVS_TC_NAMESPACE, NVS_READWRITE, &nVSHandle));

    ESP_ERROR_CHECK(nvs_set_blob(nVSHandle, ESP_NVS_TC_KEY, &coeff, sizeof(coeff)));
    ESP_ERROR_CHECK(nvs_commit(nVSHandle));
    nvs_close(nVSHandle);
}


#endif