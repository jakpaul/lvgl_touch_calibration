# Resistive Touch Panel Calibration System for LVGL

This repository aims to provide a simple, ready-to-embed component for performing calibration of a resistive touch panel in an [LVGL](https://lvgl.io/) project.
It can be used as an ESP-IDF component (thanks to [@hiruna](https://github.com/hiruna)) but is also compatible with other platforms.

### Features
- Corrects for **panel misalignment, scale and rotation**. This means that, as long as the touch controller is working properly, its output range does not matter; no dialing numbers in by hand for prescaling is required.
- Simple integration: Just a couple of callbacks need to be defined to interface with the calibration component. It handles all the transformation math.
- Lets the user verify the calibration by showing the touch position after a completed calibration.
- Recalibration after a timeout: The process always restarts after some given timeout if the results are not accepted. This prevents the device from no longer being controllable via the touchscreen if the user misclicks - since this would result in a faulty calibration.
- With previous versions, when the calibration was started through touch input, the press would sometimes falsely be registered as the first calibration point. A delay before start of the sequence prevents this.
- Configurable (see below). Options are available through ESP-IDF menuconfig or editing the [config file](./lv_tc_config.h).

### Possible future improvements
- Allow setting other calibration modes such as: more points for better accuracy (through averaging)

The system has been tested to work reliably with LVGL 8.3.0 on a 480×800 pixels LCD with an XPT2046 touch controller.

![Calibration screen preview](./preview.gif)

## Calibration Sequence
When the `lv_tc_screen` has been loaded and `lv_tc_screen_start(...)` is called, the user is prompted to touch the screen in three different locations, one by one. Once this is done, the system allows the user to check the calibration results (by showing a cursor at any position the user presses). The calibration is then either restarted or accepted.

The results can be written to non-volatile storage (NVS). They can then be restored after a reset, so that no recalibration is required. System-specific functions for this need to be provided. 

This repo contains some examplary functions for the ESP32 (on top of the [ESP32 NVS Library](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html)).

## Usage
Clone this this repo and place all its contents in your project.
### Firmware setup
This section documents how to add touch calibration to an LVGL project: The system provides a custom input device driver for lvgl (see below). Note that you have to **replace** your own driver if there already is one in your code.

Place this code in your LVGL initialization:
```c
//...

#include "lv_tc.h"
#include "lv_tc_screen.h"

void init() {
    //...

    static lv_indev_drv_t indevDrv;

    /*
        Initialize the calibrated touch driver.
        Sets its type to LV_INDEV_TYPE_POINTER,
        uses its user_data field. DO NOT OVERRIDE

        Also provide a read callback to interface with your touch controller!
    */
    lv_tc_indev_drv_init(&indevDrv, your_indev_read_cb);

    /*
        Register the driver.
    */
    lv_indev_drv_register(&indevDrv);

    /*
        If using NVS:
        Register a calibration coefficients save callback.
    */
    lv_tc_register_coeff_save_cb(your_nvs_coeff_save_cb);

    /*
        Create the calibration screen.
    */
    lv_obj_t *tCScreen = lv_tc_screen_create();

    /*
        Register a callback for when the calibration finishes.
        An LV_EVENT_READY event is triggered.
    */
    lv_obj_add_event_cb(tCScreen, your_tc_finish_cb, LV_EVENT_READY, NULL);

    /*
        If using NVS:
        Init NVS and check for existing calibration data.
    */
    if(your_nvs_init_and_check()) {
        /*
            Data exists: proceed with the normal application without
            showing the calibration screen
        */
        your_start_application(); /* Implement this */
    } else {
        /*
            There is no data: load the calibration screen, perform the calibration
        */
        lv_disp_load_scr(tCScreen);
        lv_tc_screen_start(tCScreen);
    }

    //...
}

//...
```

The functions above (that you have to implement yourself) have the following signatures:
```c
/*
    Your touch panel driver read callback
*/
void your_indev_read_cb(
        lv_indev_drv_t *indevDriver,
        lv_indev_data_t *indevData
    ) {
    
    indevData->state = /* Implement this */;
    indevData->point.x = /* Implement this */;
    indevData->point.y = /* Implement this */;
}

/*
    If using NVS:
    Your callback for writing the new coefficients to NVS
*/
void your_nvs_coeff_save_cb(
        lv_tc_coeff_t coeff
    ) {

    /* Implement this */
}

/*
    Your callback for when the calibration finishes
*/
void your_tc_finish_cb(
        lv_event_t *event
    ) {
    /*
        Load the application
    */
    your_start_application(); /* Implement this */
}

/*
    If using NVS:
    Your function for initializing NVS
    and checking wheter calibration data is stored
    @returns true if it exists, false if not
*/
bool your_nvs_init_and_check() {
    /*
        Initialize NVS
    */

    /* Implement this */

    /*
        Check for existing data
    */

    /* Implement this */
}
```

To use the included NVS functions for the ESP32 do:
```c
//...

#include "esp_nvs_tc.h"

void init() {
    //...

    /*
        Register the ESP32 NVS calibration coefficients save callback.
    */
    lv_tc_register_coeff_save_cb(esp_nvs_tc_coeff_save_cb);

    //...

    /*
        Init ESP32 NVS and check for existing calibration data.
    */
    if(esp_nvs_tc_coeff_init()) {
        /*
            Data exists: proceed with the normal application without
            showing the calibration screen
        */
        your_start_application(); /* Implement this */
    } else {
        /*
            There is no data: load the calibration screen, perform the calibration
        */
        lv_disp_load_scr(tCScreen);
        lv_tc_screen_start(tCScreen);
    }

    //...
}

//...
```

### Configuration
A couple of configuration options for the system are available in [lv_tc_config.h](./lv_tc_config.h) or via ESP-IDF menuconfig:
- The text on the UI. (`LV_TC_START_MSG`, `LV_TC_READY_MSG`, `LV_TC_RECALIBRATE_TXT`, `LV_TC_ACCEPT_TXT`, `LV_TC_RECALIBRATE_TIMEOUT_FORMAT`)

- The positions of the points on the screen that are to be pressed during calibration. 

    `LV_TC_SCREEN_ENABLE_AUTO_POINTS`: If this is enabled (it is by default), the points are chosen automatically based on your screen resolution. If you want to set them yourself, disable the option.

    The points can then be set with `LV_TC_SCREEN_DEFAULT_POINTS`.
    Place them according to these guidelines: All three points should
    - not be on a straight line. If the points do not form a triangle, the calibration will not work.
    - ideally cover the largest possible area on the screen. This minimizes any error in the calibration process.
    - not be too close to the edges of the screen. Here, the resistive touchscreen does not perform as accurately. This would cause the calibration results to be unreliable. For my screen setup, keeping a margin of roughly 15% worked well.

- The delay before start of the calibration in milliseconds (default is 1000). Set the option to 0 to start immediately. (`LV_TC_START_DELAY_MS`)

- The timeout for automatically restarting the calibration in seconds (default is 30). Set the option to 0 to not have the calibration time out. (`LV_TC_RECALIB_TIMEOUT_S`)

That's it.

## Background
Screens with resistive touch usually come with a slight misalignment of the display and touch layers. This means that simply reading the position of the user's touch from the touch controller does not yield the coordinates of the pixel below.

To determine the actual on-screen position the user intended to press, a transformation of the touch controller output is required. By prompting the user to touch the screen at known locations, the transformation can be calculated.

A more in-depth explanation of the procedure can be found [here](https://www.maximintegrated.com/en/design/technical-documents/app-notes/5/5296.html).

## License
This work is licensed under the MIT License. Details can be found in the [license file](https://github.com/jakpaul/lvgl_touch_calibration/blob/master/LICENSE.md).
