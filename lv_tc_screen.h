#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl.h"

#include "lv_tc.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t screenObj;
    lv_obj_t *indicatorObj;
    lv_obj_t *msgLabelObj;
    lv_obj_t *recalibrateBtnObj;
    lv_obj_t *acceptBtnObj;
    bool inputEnabled;
    lv_point_t scrPoints[3];
    lv_point_t tchPoints[3];
    uint8_t currentStep;
} lv_tc_screen_t;

extern const lv_obj_class_t lv_tc_screen_class;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a touch calibration screen
 * @returns a pointer to the newly created screen (lv_obj_t*)
 */
lv_obj_t* lv_tc_screen_create();

/**
 * Set the points on screen to perform the calibration with
 * @param scrPoints a pointer to the first element of an array holding the three new points
 */
void lv_tc_screen_set_points(lv_obj_t* screenObj, lv_point_t *scrPoints);

/**
 * Start the calibration process
 * @param screenObj a pointer to the calibration screen (lv_obj_t*)
 */
void lv_tc_screen_start(lv_obj_t* screenObj);




#ifdef __cplusplus
}
#endif