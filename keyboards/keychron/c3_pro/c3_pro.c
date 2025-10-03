/* Copyright 2024 @ Keychron (https://www.keychron.com)
 * Copyright 2025 Thomas Eberhardt
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "c3_pro.h"

#ifdef CAPS_WORD_ENABLE
// Keymaps which have Caps Word enabled use the Caps Lock LED in
// blinking mode to indicate its status.

static bool     caps_word_active = false;
static bool     caps_word_led_on;
static uint16_t caps_word_led_timer;

#    define LED_PIN_ON_STATE 1
#    define CAPS_WORD_LED_CYCLE 300

// Caps Lock LED is updated in housekeeping_task_kb()
bool led_update_user(led_t led_state) {
    return false;
}

void caps_word_set_user(bool active) {
    caps_word_active = active;
    if (active) {
        caps_word_led_timer = timer_read() + CAPS_WORD_LED_CYCLE;
        caps_word_led_on    = true;
    }
}
#endif

#ifdef DYNAMIC_MACRO_ENABLE

#    define MACRO_LED_CYCLE 300

static bool     macro_rec_active = false;
static bool     macro_led_on;
static uint16_t macro_led_timer;

bool dynamic_macro_record_start_user(int8_t direction) {
    macro_rec_active = true;
    macro_led_timer  = timer_read() + MACRO_LED_CYCLE;
    macro_led_on     = true;
    return true;
}

bool dynamic_macro_play_user(int8_t direction) {
    return true;
}

bool dynamic_macro_record_key_user(int8_t direction, keyrecord_t *record) {
    return true;
}

bool dynamic_macro_record_end_user(int8_t direction) {
    macro_rec_active = false;
    return true;
}

#endif

void keyboard_post_init_kb(void) {
    gpio_set_pin_output_push_pull(LED_MAC_OS_PIN);
    gpio_set_pin_output_push_pull(LED_WIN_OS_PIN);
    gpio_write_pin(LED_MAC_OS_PIN, !LED_OS_PIN_ON_STATE);
    gpio_write_pin(LED_WIN_OS_PIN, !LED_OS_PIN_ON_STATE);

    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    bool     mac_led_on, win_led_on;
    uint16_t curr_timer;

    if (get_highest_layer(default_layer_state) == 0) {
        mac_led_on = true;
        win_led_on = false;
    }
    if (get_highest_layer(default_layer_state) == 2) {
        mac_led_on = false;
        win_led_on = true;
    }

#ifdef DYNAMIC_MACRO_ENABLE
    if (macro_rec_active) {
        curr_timer = timer_read();
        if (timer_expired(curr_timer, macro_led_timer)) {
            macro_led_timer = curr_timer + MACRO_LED_CYCLE;
            macro_led_on    = !macro_led_on;
        }
        if (!macro_led_on) {
            mac_led_on = false;
            win_led_on = false;
        }
    }
#endif
    gpio_write_pin(LED_MAC_OS_PIN, mac_led_on ? LED_OS_PIN_ON_STATE : !LED_OS_PIN_ON_STATE);
    gpio_write_pin(LED_WIN_OS_PIN, win_led_on ? LED_OS_PIN_ON_STATE : !LED_OS_PIN_ON_STATE);

#ifdef CAPS_WORD_ENABLE
    bool caps_lock_led_on = host_keyboard_led_state().caps_lock;

    if (caps_word_active) {
        curr_timer = timer_read();
        if (timer_expired(curr_timer, caps_word_led_timer)) {
            caps_word_led_timer = curr_timer + CAPS_WORD_LED_CYCLE;
            caps_word_led_on    = !caps_word_led_on;
        }
        caps_lock_led_on = caps_word_led_on;
    }
    gpio_write_pin(LED_CAPS_LOCK_PIN, caps_lock_led_on ? LED_PIN_ON_STATE : !LED_PIN_ON_STATE);
#endif
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (!process_record_user(keycode, record)) {
        return false;
    }
    switch (keycode) {
#ifdef RGB_MATRIX_ENABLE
        case QK_RGB_MATRIX_TOGGLE:
            if (record->event.pressed) {
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags(LED_FLAG_NONE);
                        rgb_matrix_set_color_all(0, 0, 0);
                    } break;
                    default: {
                        rgb_matrix_set_flags(LED_FLAG_ALL);
                    } break;
                }
            }
            if (!rgb_matrix_is_enabled()) {
                rgb_matrix_set_flags(LED_FLAG_ALL);
                rgb_matrix_enable();
            }
            return false;
#endif
#ifdef LED_MATRIX_ENABLE
        case QK_LED_MATRIX_TOGGLE:
            if (record->event.pressed) {
                switch (led_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        led_matrix_set_flags(LED_FLAG_NONE);
                        led_matrix_set_value_all(0);
                    } break;
                    default: {
                        led_matrix_set_flags(LED_FLAG_ALL);
                    } break;
                }
            }
            if (!led_matrix_is_enabled()) {
                led_matrix_set_flags(LED_FLAG_ALL);
                led_matrix_enable();
            }
            return false;
#endif
        case KC_OSSW:
            if (record->event.pressed) {
                // Switches default layer between `MAC_BASE` and `WIN_BASE` (0 and 2)
                if (get_highest_layer(default_layer_state) == 2 ) {
                    set_single_persistent_default_layer(0);
                } else {
                    set_single_persistent_default_layer(2);
                }
            }
            return false;
        default:
            return true;
    }
}

void suspend_power_down_kb(void) {
    gpio_write_pin(LED_WIN_OS_PIN, !LED_OS_PIN_ON_STATE);
    gpio_write_pin(LED_MAC_OS_PIN, !LED_OS_PIN_ON_STATE);
#ifdef CAPS_WORD_ENABLE
    gpio_write_pin(LED_CAPS_LOCK_PIN, !LED_PIN_ON_STATE);
#endif
    suspend_power_down_user();
}
