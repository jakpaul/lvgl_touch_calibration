/*********************
 *      INCLUDES
 *********************/

#include "lv_tc.h"

#include "math.h"


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_tc_indev_drv_read_cb(lv_indev_drv_t *indevDrv, lv_indev_data_t *data);


/**********************
 *  STATIC VARIABLES
 **********************/

static lv_tc_coeff_t calibResult = {false, 0, 0, 0, 0, 0, 0};

static lv_obj_t *registeredTCScreen = NULL;
static void (*registeredInputCb)(lv_obj_t *screenObj, lv_indev_data_t *data) = NULL;
static void (*registeredSaveCb)(lv_tc_coeff_t coeff) = NULL;


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_tc_indev_drv_init(lv_indev_drv_t *indevDrv, void (*readCb)(lv_indev_drv_t *indevDrv, lv_indev_data_t *data)) {
    lv_indev_drv_init(indevDrv);
    indevDrv->type = LV_INDEV_TYPE_POINTER;
    indevDrv->read_cb = lv_tc_indev_drv_read_cb;
    indevDrv->user_data = readCb;
}

void _lv_tc_register_input_cb(lv_obj_t *screenObj, void (*inputCb)(lv_obj_t *screenObj, lv_indev_data_t *data)) {
    registeredTCScreen = screenObj;
    registeredInputCb = inputCb;
}

void lv_tc_register_coeff_save_cb(void (*saveCb)(lv_tc_coeff_t coeff)) {
    registeredSaveCb = saveCb;
}

lv_tc_coeff_t* lv_tc_get_coeff() {
    return &calibResult;
}

void lv_tc_set_coeff(lv_tc_coeff_t coeff, bool save) {
    calibResult = coeff;

    if(save) {
        lv_tc_save_coeff();
    }
}

void lv_tc_save_coeff() {
    if(registeredSaveCb) {
        registeredSaveCb(calibResult);
    }
}

void lv_tc_compute_coeff(lv_point_t *scrP, lv_point_t *tchP, bool save, lv_tc_mode_t mode) {   //The computation is explained here: https://www.maximintegrated.com/en/design/technical-documents/app-notes/5/5296.html
    const lv_tc_val_t divisor = (
          (lv_tc_val_t)tchP[0].x * ((lv_tc_val_t)tchP[2].y - (lv_tc_val_t)tchP[1].y)
        - (lv_tc_val_t)tchP[1].x * (lv_tc_val_t)tchP[2].y
        + (lv_tc_val_t)tchP[1].y * (lv_tc_val_t)tchP[2].x
        + (lv_tc_val_t)tchP[0].y * ((lv_tc_val_t)tchP[1].x - (lv_tc_val_t)tchP[2].x)
    );

    lv_tc_coeff_t result = {
        true,
        (
              (lv_tc_val_t)scrP[0].x * ((lv_tc_val_t)tchP[2].y - (lv_tc_val_t)tchP[1].y)
            - (lv_tc_val_t)scrP[1].x * (lv_tc_val_t)tchP[2].y
            + (lv_tc_val_t)scrP[2].x * (lv_tc_val_t)tchP[1].y
            + ((lv_tc_val_t)scrP[1].x - (lv_tc_val_t)scrP[2].x) * (lv_tc_val_t)tchP[0].y
        ) / divisor,
        - (
              (lv_tc_val_t)scrP[0].x * ((lv_tc_val_t)tchP[2].x - (lv_tc_val_t)tchP[1].x)
            - (lv_tc_val_t)scrP[1].x * (lv_tc_val_t)tchP[2].x
            + (lv_tc_val_t)scrP[2].x * (lv_tc_val_t)tchP[1].x
            + ((lv_tc_val_t)scrP[1].x - (lv_tc_val_t)scrP[2].x) * (lv_tc_val_t)tchP[0].x
        ) / divisor,
        (
              (lv_tc_val_t)scrP[0].x * ((lv_tc_val_t)tchP[1].y * (lv_tc_val_t)tchP[2].x - (lv_tc_val_t)tchP[1].x * (lv_tc_val_t)tchP[2].y)
            + (lv_tc_val_t)tchP[0].x * ((lv_tc_val_t)scrP[1].x * (lv_tc_val_t)tchP[2].y - (lv_tc_val_t)scrP[2].x * (lv_tc_val_t)tchP[1].y)
            + (lv_tc_val_t)tchP[0].y * ((lv_tc_val_t)scrP[2].x * (lv_tc_val_t)tchP[1].x - (lv_tc_val_t)scrP[1].x * (lv_tc_val_t)tchP[2].x)
        ) / divisor,
        (
              (lv_tc_val_t)scrP[0].y * ((lv_tc_val_t)tchP[2].y - (lv_tc_val_t)tchP[1].y)
            - (lv_tc_val_t)scrP[1].y * (lv_tc_val_t)tchP[2].y
            + (lv_tc_val_t)scrP[2].y * (lv_tc_val_t)tchP[1].y
            + ((lv_tc_val_t)scrP[1].y - (lv_tc_val_t)scrP[2].y) * (lv_tc_val_t)tchP[0].y
        ) / divisor,
        - (
              (lv_tc_val_t)scrP[0].y * ((lv_tc_val_t)tchP[2].x - (lv_tc_val_t)tchP[1].x)
            - (lv_tc_val_t)scrP[1].y * (lv_tc_val_t)tchP[2].x
            + (lv_tc_val_t)scrP[2].y * (lv_tc_val_t)tchP[1].x
            + ((lv_tc_val_t)scrP[1].y - (lv_tc_val_t)scrP[2].y) * (lv_tc_val_t)tchP[0].x
        ) / divisor,
        (
              (lv_tc_val_t)scrP[0].y * ((lv_tc_val_t)tchP[1].y * (lv_tc_val_t)tchP[2].x - (lv_tc_val_t)tchP[1].x * (lv_tc_val_t)tchP[2].y)
            + (lv_tc_val_t)tchP[0].x * ((lv_tc_val_t)scrP[1].y * (lv_tc_val_t)tchP[2].y - (lv_tc_val_t)scrP[2].y * (lv_tc_val_t)tchP[1].y)
            + (lv_tc_val_t)tchP[0].y * ((lv_tc_val_t)scrP[2].y * (lv_tc_val_t)tchP[1].x - (lv_tc_val_t)scrP[1].y * (lv_tc_val_t)tchP[2].x)
        ) / divisor
    };

    lv_tc_set_coeff(result, save);
}

lv_point_t _lv_tc_transform_point_indev(lv_indev_data_t *data) {
    if(data->state == LV_INDEV_STATE_PRESSED) {
        return lv_tc_transform_point(data->point);
    } else {
        lv_point_t point = {0, 0};
        return point;
    }
}

lv_point_t lv_tc_transform_point(lv_point_t point) {
    lv_point_t tranformedPoint = point;
    if(calibResult.isValid) {
        tranformedPoint.x = roundf((lv_tc_val_t)point.x * calibResult.a + (lv_tc_val_t)point.y * calibResult.b + calibResult.c);
        tranformedPoint.y = roundf((lv_tc_val_t)point.x * calibResult.d + (lv_tc_val_t)point.y * calibResult.e + calibResult.f);
    }
    
    return tranformedPoint;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_tc_indev_drv_read_cb(lv_indev_drv_t *indevDrv, lv_indev_data_t *data) {
    if(!indevDrv->user_data) return;

    ((void (*)(lv_indev_drv_t*, lv_indev_data_t*))indevDrv->user_data)(indevDrv, data);

    if(registeredTCScreen && registeredInputCb && registeredTCScreen == lv_scr_act()) {
        registeredInputCb(registeredTCScreen, data);
    }

    data->point = _lv_tc_transform_point_indev(data);
}