/*********************
 *      INCLUDES
 *********************/

#include "lv_tc_screen.h"

#include "math.h"

#include "lv_tc_config.h"
#include "img/lv_tc_indicator_img.c"


/*********************
 *      DEFINES
 *********************/

#define MY_CLASS &lv_tc_screen_class

#define STEP_FINISH 3
#define INDICATOR_SIZE 50

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_tc_screen_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj);
//static void lv_tc_screen_event(const lv_obj_class_t *class_p, lv_event_t *event);

static void lv_tc_screen_input_cb(lv_obj_t *screenObj, lv_indev_data_t *data);
static void lv_tc_screen_process_input(lv_obj_t* screenObj, lv_point_t tchPoint);
static void lv_tc_screen_step(lv_obj_t* screenObj, uint8_t step, lv_point_t tchPoint, lv_tc_mode_t mode);
static void lv_tc_screen_set_indicator_pos(lv_obj_t* screenObj, lv_point_t point);

static void lv_tc_screen_finish(lv_obj_t *screenObj);
static void lv_tc_screen_ready(lv_obj_t *screenObj);

static void lv_tc_screen_recalibrate_btn_click_cb(lv_event_t *event);
static void lv_tc_screen_accept_btn_click_cb(lv_event_t *event);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_tc_screen_class = {
    .constructor_cb = lv_tc_screen_constructor,
    .instance_size = sizeof(lv_tc_screen_t),
    .base_class = &lv_obj_class
};


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_tc_screen_create() {
    lv_obj_t *obj = lv_obj_class_create_obj(MY_CLASS, NULL);
    lv_obj_class_init_obj(obj);
    return obj;
}

void lv_tc_screen_set_points(lv_obj_t* screenObj, lv_point_t *scrPoints) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    memcpy(tCScreenObj->scrPoints, scrPoints, sizeof(lv_point_t) * 3);
}

void lv_tc_screen_start(lv_obj_t* screenObj) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    lv_label_set_text_static(tCScreenObj->msgLabelObj, LV_TC_START_MSG);
    lv_obj_add_flag(tCScreenObj->recalibrateBtnObj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(tCScreenObj->acceptBtnObj, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_align(tCScreenObj->msgLabelObj, LV_ALIGN_CENTER, 0, -50);
    lv_obj_align_to(tCScreenObj->recalibrateBtnObj, tCScreenObj->msgLabelObj, LV_ALIGN_OUT_BOTTOM_MID, -140, 20);
    lv_obj_align_to(tCScreenObj->acceptBtnObj, tCScreenObj->msgLabelObj, LV_ALIGN_OUT_BOTTOM_MID, 140, 20);


    lv_point_t point = {0, 0};
    lv_tc_screen_step(screenObj, 0, point, TC_MODE_THREE_POINTS);
    _lv_tc_register_input_cb(screenObj, lv_tc_screen_input_cb);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tc_screen_constructor(const lv_obj_class_t *class_p, lv_obj_t *obj) {
    LV_UNUSED(class_p);
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)obj;

    tCScreenObj->inputEnabled = true;
    
    lv_point_t points[3] = LV_TC_SCREEN_DEFAULT_POINTS;
    lv_tc_screen_set_points(obj, points);

    tCScreenObj->indicatorObj = lv_img_create(obj);
    lv_img_set_src(tCScreenObj->indicatorObj, &lv_tc_indicator_img);
    lv_obj_clear_flag(tCScreenObj->indicatorObj, LV_OBJ_FLAG_CLICKABLE);


    tCScreenObj->msgLabelObj = lv_label_create(obj);
    lv_obj_set_style_text_align(tCScreenObj->msgLabelObj, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_size(tCScreenObj->msgLabelObj, lv_pct(60), LV_SIZE_CONTENT);


    tCScreenObj->recalibrateBtnObj = lv_btn_create(obj);
    lv_obj_set_size(tCScreenObj->recalibrateBtnObj, lv_pct(30), LV_SIZE_CONTENT);
    lv_obj_add_event_cb(tCScreenObj->recalibrateBtnObj, lv_tc_screen_recalibrate_btn_click_cb, LV_EVENT_CLICKED, obj);
    
    lv_obj_t *recalibrateBtnLabelObj = lv_label_create(tCScreenObj->recalibrateBtnObj);
    lv_label_set_text_static(recalibrateBtnLabelObj, LV_TC_RECALIBRATE_TXT);
    lv_obj_center(recalibrateBtnLabelObj);


    tCScreenObj->acceptBtnObj = lv_btn_create(obj);
    lv_obj_set_size(tCScreenObj->acceptBtnObj, lv_pct(30), LV_SIZE_CONTENT);
    lv_obj_add_event_cb(tCScreenObj->acceptBtnObj, lv_tc_screen_accept_btn_click_cb, LV_EVENT_CLICKED, obj);

    lv_obj_t *acceptBtnLabelObj = lv_label_create(tCScreenObj->acceptBtnObj);
    lv_label_set_text_static(acceptBtnLabelObj, LV_TC_ACCEPT_TXT);
    lv_obj_center(acceptBtnLabelObj);

    lv_obj_move_foreground(tCScreenObj->indicatorObj);
}

static void lv_tc_screen_input_cb(lv_obj_t *screenObj, lv_indev_data_t *data) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    if(tCScreenObj->inputEnabled && data->state == LV_INDEV_STATE_PRESSED) {
        lv_tc_screen_process_input(screenObj, data->point);
    }

    if(data->state == LV_INDEV_STATE_RELEASED) {
        tCScreenObj->inputEnabled = true;
    }
}

static void lv_tc_screen_process_input(lv_obj_t* screenObj, lv_point_t tchPoint) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    if(tCScreenObj->currentStep < STEP_FINISH) {
        tCScreenObj->inputEnabled = false;
        lv_tc_screen_step(screenObj, tCScreenObj->currentStep + 1, tchPoint, TC_MODE_THREE_POINTS);
    } else {
        lv_tc_screen_set_indicator_pos(screenObj, lv_tc_transform_point(tchPoint));
    }
}

static void lv_tc_screen_step(lv_obj_t* screenObj, uint8_t step, lv_point_t tchPoint, lv_tc_mode_t mode) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    tCScreenObj->currentStep = step;

    if(step > 0) {
        tCScreenObj->tchPoints[step - 1] = tchPoint;
    }
    if(step == STEP_FINISH) {
        lv_tc_compute_coeff(tCScreenObj->scrPoints, tCScreenObj->tchPoints, false, mode);
        lv_tc_screen_finish(screenObj);
        return;
    }

    lv_tc_screen_set_indicator_pos(screenObj, tCScreenObj->scrPoints[step]);
}

static void lv_tc_screen_set_indicator_pos(lv_obj_t* screenObj, lv_point_t point) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    lv_obj_set_pos(tCScreenObj->indicatorObj, point.x - INDICATOR_SIZE / 2, point.y - INDICATOR_SIZE / 2);
}

static void lv_tc_screen_finish(lv_obj_t *screenObj) {
    lv_tc_screen_t *tCScreenObj = (lv_tc_screen_t*)screenObj;

    lv_label_set_text_static(tCScreenObj->msgLabelObj, LV_TC_READY_MSG);
    lv_obj_clear_flag(tCScreenObj->recalibrateBtnObj, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(tCScreenObj->acceptBtnObj, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_align(tCScreenObj->msgLabelObj, LV_ALIGN_CENTER, 0, -50);
    lv_obj_align_to(tCScreenObj->recalibrateBtnObj, tCScreenObj->msgLabelObj, LV_ALIGN_OUT_BOTTOM_MID, -140, 20);
    lv_obj_align_to(tCScreenObj->acceptBtnObj, tCScreenObj->msgLabelObj, LV_ALIGN_OUT_BOTTOM_MID, 140, 20);
}

static void lv_tc_screen_ready(lv_obj_t *screenObj) {
    lv_tc_save_coeff();
    lv_event_send(screenObj, LV_EVENT_READY, lv_tc_get_coeff());
}


static void lv_tc_screen_recalibrate_btn_click_cb(lv_event_t *event) {
    lv_tc_screen_start(event->user_data);
}

static void lv_tc_screen_accept_btn_click_cb(lv_event_t *event) {
    lv_tc_screen_ready(event->user_data);
}
