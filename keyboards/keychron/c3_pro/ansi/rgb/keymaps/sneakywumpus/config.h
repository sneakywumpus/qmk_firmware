/* Copyright 2025 Thomas Eberhardt
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

#pragma once

// Enable the QK_MAKE keycode
#define ENABLE_COMPILE_KEYCODE

// Disable dynamic macro nesting (potentially dangerous)
#define DYNAMIC_MACRO_NO_NESTING

// Enable Caps Word with left+right shift
#define BOTH_SHIFTS_TURNS_ON_CAPS_WORD

// Tame mouse keys
#define MOUSEKEY_DELAY 300
#define MOUSEKEY_INTERVAL 20
#define MOUSEKEY_MOVE_DELTA 5
#define MOUSEKEY_MAX_SPEED 7
#define MOUSEKEY_TIME_TO_MAX 60

// Fine tuning for Control/Caps Lock MT
#define HOLD_ON_OTHER_KEY_PRESS_PER_KEY
