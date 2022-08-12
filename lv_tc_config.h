#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
    This is the configuration file for the touch calibration system.
    Change the defines below to match your application
*/

#define LV_TC_START_MSG                         "Precisely touch the cursors (using a touch pen) to perform the calibration."
#define LV_TC_READY_MSG                         "Calibration completed. You can check it by moving the cursor around on the screen."
#define LV_TC_RECALIBRATE_TXT                   "Recalibrate"
#define LV_TC_ACCEPT_TXT                        "Accept"

//The format of the timeout string on the recalibration button.
//Appends to LV_TC_RECALIBRATE_TXT if LV_TC_RECALIB_TIMEOUT_S is set greater than 0
#define LV_TC_RECALIBRATE_TIMEOUT_FORMAT        " (%d)"


//Set to 1 to make the system choose the points for thecalibration automatically
//based on your screen resolution
#define LV_TC_SCREEN_ENABLE_AUTO_POINTS         1

//The default points (will be overridden if LV_TC_SCREEN_ENABLE_AUTO_POINTS is enabled)
#define LV_TC_SCREEN_DEFAULT_POINTS             {{80, 150}, {350, 400}, {720, 80}}


//Make the system restart the calibration automatically after a given timeout
//if it is not accepted by the user. This makes sure that a faulty calibration can
//always be restarted - even when it is impossible to press the 'recalibrate' button
//Set to 0 to disable
#define LV_TC_RECALIB_TIMEOUT_S                 30

#ifdef __cplusplus
}
#endif