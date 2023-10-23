/*
 * MIT License
 *
 * Copyright (c) 2022-2023 Martin Petkovski
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * qlib.h
 *
 * QLIB is a single-header library which contains common utility functions
 * for custom keyboard firmwares.
 *
 */

#pragma once

#pragma warning(disable : 5045) // comment this out if you want to use Spectre mitigation

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//---------
//  MATHS
//---------
inline int32_t qremap_value(const int32_t low1, const int32_t high1, const int32_t low2, const int32_t high2, const int32_t value) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

inline uint32_t quremap_value(const uint32_t low1, const uint32_t high1, const uint32_t low2, const uint32_t high2, const uint32_t value) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

inline uint8_t qlerp(const uint8_t a, const uint8_t b, const float f) {
    return a + (uint8_t)(f * (float)(b - a));
}

inline float qclamp(const float n, const float lower, const float upper) {
    return (float)fmax(lower, fmin(n, upper));
}

inline float qcircular_ease_out(float p) {
    p = p * p;
    p = qclamp(p, 0.0f, 1.0f);
    return -(p * (p - 2));
}

//------------
//  TYPEDEFS
//------------

typedef void (*qmacro_fn)(void);

enum qlight_effect_types { LIGHT_UP, FADE_IN, FADE_OUT, FADE_IN_OUT };

typedef struct qcolor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} qcolor;

#pragma pack(push, 1)
typedef struct qkey {
    uint16_t keycode;
    uint16_t ascii;
    uint16_t ledcode;
    qcolor   base_color;
    qcolor   current_color;
    uint8_t  effect;
    uint64_t last_activation_frame;
    uint64_t duration;
} qkey;

typedef struct qmacro {
    uint16_t  macro_id;
    uint16_t  keycode;
    qmacro_fn macro_fn;
    qcolor    color;
    bool      active;
} qmacro;

typedef struct qprofile {
    uint16_t  profile_id;
    uint16_t* macro_ids;
    uint16_t  num_macros;
    qcolor    color;

} qprofile;
#pragma pack(pop)

uint64_t  qframe                 = 0;
qkey*     qkeys                  = 0x00;
uint16_t  qnum_keys              = 0;
qmacro*   qmacros                = 0x00;
uint16_t  qnum_macros            = 0;
qprofile* qprofiles              = 0x00;
uint16_t  qnum_profiles          = 0;
uint16_t  qfeature_flags         = 0;
qcolor    qlight_color           = {.r = 255, .g = 255, .b = 255};
uint16_t* qprofiles_stack        = 0;
uint8_t   qprofiles_stack_cursor = 0;

//--------
//  KEYS
//--------

inline void qinitialize_keys(qkey** keys, const uint16_t num_keys) {
    if (!keys) return;
    qkeys     = *keys;
    qnum_keys = num_keys;
}

inline qkey* qget_key(const uint16_t keycode) {
    for (uint16_t i = 0; i < qnum_keys; i++) {
        if (qkeys[i].keycode == keycode) {
            return &qkeys[i];
        }
    }

    return 0x00;
}

inline qkey* qget_key_ascii(const uint16_t ascii_code) {
    for (uint16_t i = 0; i < qnum_keys; i++) {
        if (qkeys[i].ascii == ascii_code) {
            return &qkeys[i];
        }
    }
    return 0x00;
}

inline uint16_t qget_led_code(const uint16_t keycode) {
    const qkey* key = qget_key(keycode);
    if (!key) return 0;
    return key->ledcode;
}

inline uint16_t qget_led_code_ascii(const uint16_t ascii_code) {
    const qkey* key = qget_key_ascii(ascii_code);
    if (!key) return 0;
    return key->ledcode;
}

//----------
//  MACROS
//----------

inline void qinitialize_macros(qmacro** macros, const uint16_t num_macros) {
    if (!macros) return;
    qmacros     = *macros;
    qnum_macros = num_macros;
}

inline qmacro*qget_macro(const uint16_t id) {
    for (uint16_t i = 0; i < qnum_macros; i++) {
        if (qmacros[i].macro_id == id) {
            return &qmacros[i];
        }
    }

    return 0x00;
}

inline void qdeactivate_macros(void) {
    for (uint16_t i = 0; i < qnum_macros; i++) {
        qmacros[i].active = false;
    }
}

//------------
//  PROFILES
//------------

inline void qinitialize_profiles(qprofile** profiles, const uint16_t num_profiles) {
    if (!profiles) return;
    qprofiles       = *profiles;
    qnum_profiles   = num_profiles;
    qprofiles_stack = (uint16_t*)malloc(256 * sizeof(uint16_t));
    memset(qprofiles_stack, 0, sizeof(uint16_t));
}

inline qprofile* qget_profile(const uint16_t id) {
    for (uint16_t i = 0; i < qnum_profiles; i++) {
        if (qprofiles[i].profile_id == id) {
            return &qprofiles[i];
        }
    }

    return 0x00;
}

inline void qset_profile(const uint16_t id) {
    if (qnum_profiles == 0) return;

    for (uint16_t i = 0; i < qnum_profiles; i++) {
        if (qprofiles[i].profile_id == id) {
            qprofiles_stack[0]     = qprofiles[i].profile_id;
            qprofiles_stack_cursor = 0;

            qdeactivate_macros();

            for (uint16_t j = 0; j < qprofiles[i].num_macros; j++) {
                qmacro* current_macro = qget_macro(qprofiles[i].macro_ids[j]);

                if (!current_macro) continue;

                current_macro->active = true;
            }
        }
    }
}

inline void qgenerate_profile_from_stack(void) {
    qdeactivate_macros();

    for (uint16_t i = 0; i < qprofiles_stack_cursor; i++) {
        const qprofile* profile = qget_profile(qprofiles_stack[i]);
        if (!profile) continue;

        for (uint16_t j = 0; j < profile->num_macros; j++) {
            qmacro* current_macro = qget_macro(profile->macro_ids[j]);
            if (!current_macro) continue;

            current_macro->active = true;
        }
    }
}

inline void qpush_profile(const uint16_t id) {
    if (qnum_profiles == 0) return;

    for (uint16_t i = 0; i < qnum_profiles; i++) {
        if (qprofiles[i].profile_id == id) {
            qprofiles_stack[qprofiles_stack_cursor] = qprofiles[i].profile_id;
            qprofiles_stack_cursor++;
        }
    }

    qgenerate_profile_from_stack();
}

inline void qpop_profile(void) {
    if (qnum_profiles == 0) return;

    qprofiles_stack_cursor--;
    qgenerate_profile_from_stack();
}

//-----------
//  PROCESS
//-----------

inline void qprocess_keypress(const uint16_t keycode, const bool pressed) {
    if (!pressed) return;

    for (uint16_t i = 0; i < qnum_macros; i++) {
        const qmacro* current_macro = &qmacros[i];
        if (keycode == current_macro->macro_id) {
            current_macro->macro_fn();
        }
    }

    qkey* current_key = qget_key(keycode);
    if (current_key) {
        current_key->last_activation_frame = qframe;
    }
}

//---------
//  UTILS
//---------

inline void qtick(void) {
    qframe++;
}

inline void qclean(void) {
    free(qprofiles_stack);
}

//------------
//  FEATURES
//------------

inline void qset_feature_flag(const uint16_t flag) {
    qfeature_flags |= flag;
}

inline bool qis_flag_set(const uint16_t flag) {
    return qfeature_flags & flag;
}

//----------
//  LIGHTS
//----------

inline void qinitialize_light_color(const qcolor* color) {
    if (!color) return;
    for (uint16_t i = 0; i < qnum_keys; i++) {
        qkeys[i].base_color = (*color);
    }

    for (uint16_t i = 0; i < qnum_macros; i++) {
        qkey* key = qget_key(qmacros[i].keycode);
        if (!key) continue;
        key->base_color = qmacros[i].color;
    }
}

inline void qprocess_lights(void (*light_fn)(uint16_t, uint8_t, uint8_t, uint8_t)) {
    for (uint16_t i = 0; i < qnum_keys; i++) {
        qkey* current_key = qget_key(i);
        if (!current_key) continue;

        const uint64_t end_time = current_key->last_activation_frame + current_key->duration;
        if (end_time > qframe) {
            int64_t mapped_value = 0;
            if (current_key->effect == FADE_OUT) {
                mapped_value = qremap_value((uint32_t)current_key->last_activation_frame, (uint32_t)end_time, (uint32_t)0, (uint32_t)256, (uint32_t)qframe);
            } else if (current_key->effect == FADE_IN) {
                mapped_value = qremap_value((uint32_t)current_key->last_activation_frame, (uint32_t)end_time, (uint32_t)256, (uint32_t)0, (uint32_t)qframe);
            }

            float normalized_mapped_value = (float)((float)mapped_value / 255.0f);
            normalized_mapped_value       = qcircular_ease_out(normalized_mapped_value);

            current_key->current_color.r = qlerp(current_key->current_color.r, current_key->base_color.r, normalized_mapped_value);
            current_key->current_color.g = qlerp(current_key->current_color.g, current_key->base_color.g, normalized_mapped_value);
            current_key->current_color.b = qlerp(current_key->current_color.b, current_key->base_color.b, normalized_mapped_value);
        }
    }

    for (uint16_t i = 0; i < qnum_keys; i++) {
        light_fn(qkeys[i].ledcode, qkeys[i].current_color.r, qkeys[i].current_color.g, qkeys[i].current_color.b);
    }
}
