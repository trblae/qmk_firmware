/* Copyright 2017 trblae
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
#include "single_switch.h"
#include "action_layer.h"

#ifdef RGBLIGHT_ENABLE
#define rgblight_set_blue        rgblight_sethsv (0xFF,  0xFF, 0xFF);
#define rgblight_set_white       rgblight_sethsv (0x00,  0x00, 0xFF);
#define rgblight_set_purple      rgblight_sethsv (0x10E, 0xFF, 0xFF);
#define rgblight_set_off         rgblight_sethsv (0x00,  0x00, 0x00);
#endif

enum layers {
  BASE = 0,
  LOCK
};

// two layers for single switch/key keyboard
// 1. base layer - single tap is enter, double tap is switch to layer 2
// 2. func layer - single tap win+L, double tab is switch to layer 1
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [BASE] = KEYMAP(TD(BASE)),
  [LOCK] = KEYMAP(TD(LOCK))
};

// tap dance, run this function on each tap
void base_each(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count > 2) {
    register_code(KC_ENT);
    unregister_code(KC_ENT);
  }
}
// tap dance, run this funciton when sequence is finished (timeout)
// or if interrupted, e.g., by another key press
void base_finished(qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 1:
      register_code(KC_ENT);
      break;
    case 2:
      break;
    default:
      register_code(KC_ENT);
      unregister_code(KC_ENT);
      register_code(KC_ENT);
      unregister_code(KC_ENT);
      register_code(KC_ENT);
      break;
  }
}
// tap dance, run this function when resetting (on key up)
void base_reset(qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 2:
      layer_on(LOCK);
      break;
    default:
      unregister_code(KC_ENT);
      break;
  }
}

void lock_each(qk_tap_dance_state_t *state, void *user_data) {
  if (state->count > 2) {
    register_code(KC_LGUI);
    register_code(KC_L);
    unregister_code(KC_L);
    unregister_code(KC_LGUI);
  }
}
void lock_finished(qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 1:
      register_code(KC_LGUI);
      register_code(KC_L);
    case 2:
      break;
    default:
      register_code(KC_LGUI);
      register_code(KC_L);
      unregister_code(KC_L);
      unregister_code(KC_LGUI);
      register_code(KC_LGUI);
      register_code(KC_L);
      unregister_code(KC_L);
      unregister_code(KC_LGUI);
      register_code(KC_LGUI);
      register_code(KC_L);
      break;
  }
}
void lock_reset(qk_tap_dance_state_t *state, void *user_data) {
  switch (state->count) {
    case 2:
      layer_off(LOCK);
      break;
    default:
      unregister_code(KC_L);
      unregister_code(KC_LGUI);
      break;
  }
}

qk_tap_dance_action_t tap_dance_actions[] = {
  [BASE] = ACTION_TAP_DANCE_FN_ADVANCED(base_each, base_finished, base_reset),
  [LOCK] = ACTION_TAP_DANCE_FN_ADVANCED(lock_each, lock_finished, lock_reset)
};

// Runs whenever there is a layer state change.
uint32_t layer_state_set_user(uint32_t state) {
  uint8_t layer = biton32(state);
  switch (layer) {
    case LOCK:
      #ifdef RGBLIGHT_ENABLE
      rgblight_set_white
      #endif
      break;
    case BASE:
    default:
      #ifdef RGBLIGHT_ENABLE
      rgblight_set_blue
      #endif
      break;
  }
  return state;
};
