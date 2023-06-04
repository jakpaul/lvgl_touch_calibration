if(ESP_PLATFORM)
    file(GLOB SOURCES *.c)
    set(LV_TC_INCLUDE_DIRS . img)
    list(APPEND SOURCES "img/lv_tc_indicator_img.c")
    idf_component_register(SRCS ${SOURCES}
            INCLUDE_DIRS ${LV_TC_INCLUDE_DIRS}
            REQUIRES lvgl nvs_flash)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC)
else()
    message(FATAL_ERROR "LVGL Touch Calibration: ESP_PLATFORM is not defined. Try reinstalling ESP-IDF.")
endif ()