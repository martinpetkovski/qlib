#pragma once
#include "qlib.h"
#include <assert.h>

inline void smoke_test_all(void) {
    qinitialize_keys(0, 0);
    qget_led_code(0);
    qget_led_code_ascii(0);
    qinitialize_macros(0, 0);
    qinitialize_profiles(0, 0);
    qset_profile(0);
    qpush_profile(0);
    qpop_profile();
    qprocess_keypress(0, true);
    qtick();
    qclean();
    qset_feature_flag(0);
    qis_flag_set(0);
    qinitialize_light_color(0);
    qprocess_lights(0);
}

inline void test_qremap_value(void) {
    const int32_t low1            = 0;
    const int32_t high1           = 10;
    const int32_t low2            = 100;
    const int32_t high2           = 200;
    const int32_t value           = 5;

    const int32_t expected_result = 150;
    const int32_t result = qremap_value(low1, high1, low2, high2, value);

    assert(result == expected_result);
}