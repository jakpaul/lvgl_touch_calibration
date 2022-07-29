#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

#define LV_TC_START_MSG                 "Precisely touch the cursors (using a touch pen) to perform the calibration."
#define LV_TC_READY_MSG                 "Calibration completed. You can check it by moving the cursor around on the screen."
#define LV_TC_RECALIBRATE_TXT           "Recalibrate"
#define LV_TC_ACCEPT_TXT                "Accept"

#define LV_TC_SCREEN_DEFAULT_POINTS     {{80, 150}, {350, 400}, {720, 80}}



#ifdef __cplusplus
}
#endif