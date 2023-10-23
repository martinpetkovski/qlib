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
    const int32_t low1  = 0;
    const int32_t high1 = 10;
    const int32_t low2  = 100;
    const int32_t high2 = 200;
    const int32_t value = 5;

    const int32_t expected_result = 150;
    const int32_t result          = qremap_value(low1, high1, low2, high2, value);

    assert(result == expected_result);
}

inline void test_initialize_qkeys(void) {
#define NUM_KEYS 104
    qkey keyboard[NUM_KEYS];

    for (uint16_t i = 0; i < NUM_KEYS; i++) {
        keyboard[i].keycode               = i + 1;
        keyboard[i].ascii                 = 'A' + i;
        keyboard[i].ledcode               = keyboard[i].ascii + 4;
        keyboard[i].base_color            = (qcolor){255, 0, 0};
        keyboard[i].current_color         = (qcolor){0, 255, 0};
        keyboard[i].effect                = 0;
        keyboard[i].last_activation_frame = 0;
        keyboard[i].duration              = 0;
    }

    qkey* keyboard_ptr = keyboard;
    qinitialize_keys(&keyboard_ptr, NUM_KEYS);

    qkey* key = qget_key_ascii('A');
    assert(key);
    assert(qget_led_code_ascii('A') == 'A' + 4);

    key = qget_key(NUM_KEYS);
    assert(key);
    assert(qget_led_code(NUM_KEYS) == key->ascii + 4);

#undef NUM_KEYS
}