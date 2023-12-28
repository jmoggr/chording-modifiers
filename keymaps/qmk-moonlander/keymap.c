/* Copyright 2020 ZSA Technology Labs, Inc <@zsa>
 * Copyright 2020 Jack Humbert <jack.humb@gmail.com>
 * Copyright 2020 Christopher Courtney <drashna@live.com> (@drashna)
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

#include QMK_KEYBOARD_H
#include "version.h"

uint16_t get_layer_key(void);
bool is_all_keys_pressed(uint16_t keycode[]);
void tap_key(uint16_t keycode);
bool my_is_key_pressed(uint16_t keycode);
void on_keyup(uint16_t keycode);
void on_keydown(uint16_t keycode, keyrecord_t * record);
void press_key(uint16_t keycode, keyrecord_t* record);
bool handle_custom_keys(uint16_t keycode, keyrecord_t *record);
void start_keyup_cooldown(void);
void cancel_keyup_cooldown(void);
void start_keyheld_delay(void);
void cancel_keyheld_delay(void);
void start_keyheld(uint16_t keycode);
void cancel_keyheld(void);
uint16_t get_combo_key(void);
uint16_t get_keycode(void);
uint16_t get_normal_key(void);
void on_keyheld_delay_timeout(void);

bool is_qwerty_active = true;
bool is_keyup_cooldown = false;
bool is_keyheld = false;
uint16_t keyup_cooldown_timer = 0;
uint16_t keyheld_delay_timer = 0;
uint16_t keyheld_keycode = 0;

enum layers {
    QWERTY,
    BASE,  // default layer
    LAYER1,
    LAYER2,
    LAYER3,
    LAYER4,
};

enum custom_keycodes {
    KC_LYR1 = ML_SAFE_RANGE,
    KC_LYR2,
    KC_LYR3,
    KC_LYR4,
    TGL_QW,
};

#define NUM_COMBOS 15
#define NUM_COMBO_KEYS 24
uint16_t MY_COMBOS[NUM_COMBOS][NUM_COMBO_KEYS] = {
    { KC_ESC, KC_S, KC_T, 0},
    { KC_ENT, KC_H, KC_T, 0},
    { KC_TAB, KC_S, KC_H, 0},
    { KC_UNDS, KC_N, KC_E, 0},
    { KC_DOT, KC_E, KC_O, 0},
    { S(KC_ENT), KC_LYR1, KC_H, KC_T, 0},
    { S(KC_1), KC_LYR3, KC_LYR2, KC_N, 0},
    { S(KC_2), KC_LYR3, KC_LYR2, KC_E, 0},
    { S(KC_3), KC_LYR3, KC_LYR2, KC_O, 0},
    { S(KC_4), KC_LYR3, KC_LYR2, KC_L, 0},
    { S(KC_5), KC_LYR3, KC_LYR2, KC_B, 0},
    { S(KC_6), KC_LYR3, KC_LYR2, KC_J, 0},
    { S(KC_7), KC_LYR3, KC_LYR2, KC_F, 0},
    { S(KC_8), KC_LYR3, KC_LYR2, KC_U, 0},
    { S(KC_9), KC_LYR3, KC_LYR2, KC_P, 0},
};

const uint16_t PASSTHROUGH_KEYS[] = { RGB_M_P, RESET, RGB_M_R, KC_RALT, KC_RGUI, KC_LALT, KC_RCTL, KC_LCTL, KC_LSFT, 0 };


void start_keyup_cooldown() {
    keyup_cooldown_timer = timer_read();
    is_keyup_cooldown = true;
}

void cancel_keyup_cooldown() {
    keyup_cooldown_timer = 0;
    is_keyup_cooldown = false;
}

void start_keyheld_delay() {
    keyheld_delay_timer = timer_read();
}

void cancel_keyheld_delay() {
    keyheld_delay_timer = 0;
}

void start_keyheld(uint16_t keycode) {
    register_code16(keycode);
    keyheld_keycode = keycode;
    is_keyheld = true;
}

void cancel_keyheld() {
    unregister_code16(keyheld_keycode);
    keyheld_keycode = 0;
    is_keyheld = false;
}


struct PressedKey {
    uint16_t keycode;
    uint8_t row;
    uint8_t col;
};

#define MAX_PRESSED_KEYS 26
struct PressedKey pressed_keys[MAX_PRESSED_KEYS];

void press_key(uint16_t keycode, keyrecord_t* record) {
    if (num_pressed_keys == MAX_PRESSED_KEYS) {
        return;
    }

    pressed_keys[num_pressed_keys].keycode = keycode;
    pressed_keys[num_pressed_keys].row = record->event.key.row;
    pressed_keys[num_pressed_keys].col = record->event.key.col;

    num_pressed_keys += 1;
}

void release_key(uint16_t keycode) {
    int index = -1;
    for (int i = 0; i < num_pressed_keys; i++) {
        if (pressed_keys[i].keycode == keycode) {
            index = i;
            break;
        }
    }

    if (index < 0) {
        return;
    }

    for(int i = index; i < num_pressed_keys - 1; i++) {
        pressed_keys[i].keycode = pressed_keys[i + 1].keycode;
        pressed_keys[i].row = pressed_keys[i + 1].row;
        pressed_keys[i].col = pressed_keys[i + 1].col;
    }

    num_pressed_keys -= 1;
}


bool is_pathrough_key(uint16_t keycode) {
    for (uint16_t index = 0;; ++index) {
        if (keycode == PASSTHROUGH_KEYS[index]) return true;
        if (!PASSTHROUGH_KEYS[index]) break;
    }

    return false;
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [QWERTY] = LAYOUT_moonlander(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_LEFT,           KC_RGHT, KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        KC_M,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_DOWN,           KC_UP,   KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSLS,
        KC_I, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_NO,             KC_NO,   KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_L, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                                KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
        KC_GRV,  KC_NO,   KC_NO,   KC_BSPC, KC_SPC,           TGL_QW,            KC_NO,            KC_UNDS, MO(LAYER3),   KC_LBRC, KC_RBRC, KC_NO,
                                            KC_LSFT, KC_LCTL, KC_ENT,  KC_TAB, OSM(MOD_RALT),  OSM(MOD_RSFT)
    ),
    
    [BASE] = LAYOUT_moonlander(
        KC_ESC, KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,         KC_F7,   KC_F8,    KC_F9,    KC_F10,    KC_F11,    KC_F12,    RESET,
        KC_NO,  KC_Q,    KC_D,    KC_R,    KC_W,    KC_NO,   KC_PGUP,         KC__VOLUP, KC__VOLUP,   KC_F,    KC_U,    KC_P,    KC_Z,    KC_NO,
        KC_G,   KC_A,    KC_S,    KC_H,    KC_T,    KC_LSHIFT,   KC_PGDOWN,          KC__VOLDOWN,   KC__MUTE,   KC_N,    KC_E,    KC_O,    KC_I,    KC_Y,
        KC_NO,  KC_V,    KC_X,    KC_M,    KC_C,    KC_NO,                            KC_NO,   KC_L,    KC_B,    KC_J,    KC_K,    RGB_M_P,
        KC_NO,  KC_NO,   KC_NO,   KC_LALT,   KC_LYR3,          TGL_QW,         KC_NO,           KC_LYR4, KC_NO,  KC_NO,   KC_NO,   RGB_M_R,
                                           KC_LYR1, KC_LCTL, KC_LSFT,        KC_ENT,  KC_RGUI, KC_LYR2
    ),

    [LAYER1] = LAYOUT_moonlander(
        _______, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), _______,       _______, S(KC_6), S(KC_7), S(KC_8), S(KC_9), S(KC_0), _______,
        _______, S(KC_Q), S(KC_D), S(KC_R), S(KC_W), _______, _______,       _______, _______, KC_RPRN, KC_RCBR, KC_RABK, KC_RBRC, _______,
        S(KC_G), S(KC_A), S(KC_S), S(KC_H), S(KC_T), _______, _______,       _______, _______, KC_SCLN, KC_COMM, KC_COLN, KC_DQUO, KC_NO,
        _______, S(KC_V), S(KC_X), S(KC_M), S(KC_C), _______,                         _______, KC_DLR,  KC_AMPR, KC_PLUS, KC_QUES, _______,
        _______, _______, _______, _______, _______,          _______,       _______,          _______,  _______, _______, _______, _______,
                                            _______, _______, _______,       _______, _______, _______
    ),

    [LAYER2] = LAYOUT_moonlander(
        _______, _______, _______, _______, _______, _______, _______,       _______, _______, _______, _______, _______, _______, _______,
        _______, KC_LBRC, KC_LABK, KC_LCBR, KC_LPRN, _______, _______,       _______, _______, S(KC_F), S(KC_U), S(KC_P), S(KC_Z), _______,
        KC_NO,   KC_MINS, KC_QUOT, KC_EQL,  KC_SLSH, _______, _______,       _______, _______, S(KC_N), S(KC_E), S(KC_O), S(KC_I), S(KC_Y),
        _______, KC_GRV,  KC_PIPE, KC_EXLM, KC_ASTR, _______,                         _______, S(KC_L), S(KC_B), S(KC_J), S(KC_K), _______,
        _______, _______, _______, _______, KC_TAB,           _______,       _______,          KC_NO,   _______, _______, _______, _______,
                                            _______, _______, _______,       _______, _______, _______
    ),

    [LAYER3] = LAYOUT_moonlander(
        _______, _______, _______, _______, _______, _______, _______,       _______, _______, _______, _______, _______, _______, _______,
        _______, KC_NO,   KC_9,   KC_8,   KC_7,      _______, _______,       _______, _______, KC_NO,   KC_AT,   KC_HASH, KC_NO,   _______,
        KC_NO,   KC_0,    KC_3,   KC_2,   KC_1,      _______, _______,       _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,
        _______, KC_NO,   KC_6,   KC_5,   KC_4,      _______,                         _______, KC_PERC, KC_BSLS, KC_CIRC, KC_TILD, _______,
        _______, _______, _______, _______, _______,          _______,       _______,          _______, _______, _______, _______, _______,
                                            _______, _______, _______,       _______, _______, _______
    ),
        [LAYER4] = LAYOUT_moonlander(
        _______, _______, _______, _______, _______, _______, _______,       _______, _______, _______, _______, _______, _______, _______,
        _______, KC_NO,   KC_4,   KC_5,   KC_6,      _______, _______,       _______, _______, KC_NO,   KC_AT,   KC_HASH, KC_NO,   _______,
        KC_NO,   KC_0,    KC_1,   KC_2,   KC_3,      _______, _______,       _______, _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,
        _______, KC_NO,   KC_7,   KC_8,   KC_9,      _______,                         _______, KC_PERC, KC_BSLS, KC_CIRC, KC_TILD, _______,
        _______, _______, _______, _______, _______,          _______,       _______,          _______, _______, _______, _______, _______,
                                            _______, _______, _______,       _______, _______, _______
    ),

           
/*
    [SYMB] = LAYOUT_moonlander(
        VRSN,    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,           _______, KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        _______, KC_EXLM, KC_AT,   KC_LCBR, KC_RCBR, KC_PIPE, _______,           _______, KC_UP,   KC_7,    KC_8,    KC_9,    KC_ASTR, KC_F12,
        _______, KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_GRV,  _______,           _______, KC_DOWN, KC_4,    KC_5,    KC_6,    KC_PLUS, _______,
        _______, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_TILD,                             KC_AMPR, KC_1,    KC_2,    KC_3,    KC_BSLS, _______,
        EEP_RST, _______, _______, _______, _______,          RGB_VAI,           RGB_TOG,          _______, KC_DOT,  KC_0,    KC_EQL,  _______,
                                            RGB_HUD, RGB_VAD, RGB_HUI, TOGGLE_LAYER_COLOR,_______, _______
    ),

    [MDIA] = LAYOUT_moonlander(
        LED_LEVEL,_______,_______, _______, _______, _______, _______,           _______, _______, _______, _______, _______, _______, RESET,
        _______, _______, _______, KC_MS_U, _______, _______, _______,           _______, _______, _______, _______, _______, _______, _______,
        _______, _______, KC_MS_L, KC_MS_D, KC_MS_R, _______, _______,           _______, _______, _______, _______, _______, _______, KC_MPLY,
        _______, _______, _______, _______, _______, _______,                             _______, _______, KC_MPRV, KC_MNXT, _______, _______,
        _______, _______, _______, KC_BTN1, KC_BTN2,         _______,            _______,          KC_VOLU, KC_VOLD, KC_MUTE, _______, _______,
                                            _______, _______, _______,           _______, _______, _______
    ),
    */
};



uint16_t get_keycode() {
    uint16_t keycode = get_combo_key();
    if (keycode != 0) {
        return keycode;
    }

    keycode = get_normal_key();
    if (keycode != 0) {
        return keycode;
    }

    keycode = get_layer_key();
    if (keycode != 0) {
        return keycode;
    }

    return 0;
}

uint16_t get_layer_key() {
    for (int i = num_pressed_keys - 1; i >= 0; i--) {
        switch (pressed_keys[i].keycode) {
            case KC_LYR1:
                return KC_SPC;
            case KC_LYR2:
                return KC_BSPC;
        }
    }

    return 0;
}


uint16_t get_normal_key() {
    int key_row = -1;
    int key_col = -1;

    bool is_layer1_pressed = false;
    bool is_layer2_pressed = false;
    bool is_layer3_pressed = false;
    bool is_layer4_pressed = false;

    for (int i = num_pressed_keys - 1; i >= 0; i--) {
        switch (pressed_keys[i].keycode) {
            case KC_LYR1:
                is_layer1_pressed = true;
                break;
            case KC_LYR2:
                is_layer2_pressed = true;
                break;
            case KC_LYR3:
                is_layer3_pressed = true;
                break;
            case KC_LYR4:
                is_layer4_pressed = true;
                break;
            default:
                key_row = pressed_keys[i].row;
                key_col = pressed_keys[i].col;
                break;
        }
    }

    if (key_row == -1 || key_col == -1) {
        return 0;
    }

    int active_layer = BASE;

    if (is_layer1_pressed) {
        active_layer = LAYER1;
    } else if (is_layer2_pressed) {
        active_layer = LAYER2;
    } else if (is_layer3_pressed) {
        active_layer = LAYER3;
    } else if (is_layer4_pressed) {
        active_layer = LAYER4;
    }

    return keymaps[active_layer][key_row][key_col];
}


uint16_t get_combo_key() {
    if (num_pressed_keys <= 1) {
        return 0;
    }

    for (int combo_index = 0; combo_index < NUM_COMBOS; combo_index++) {
        if (is_all_keys_pressed(MY_COMBOS[combo_index] + 1)) {
            return MY_COMBOS[combo_index][0];
        }
    }

    return 0;
}

void tap_key(uint16_t keycode) {
    register_code16(keycode);
    unregister_code16(keycode);
}


bool is_all_keys_pressed(uint16_t keycodes[]) {
    for (uint16_t i = 0;; ++i) {
        if (!keycodes[i]) return true;
        if (!my_is_key_pressed(keycodes[i])) return false;
    }
}

bool my_is_key_pressed(uint16_t keycode) {
    for (int i = 0; i < num_pressed_keys; i++) {
        if (pressed_keys[i].keycode == keycode) {
            return true;
        }
    }

    return false;
}

void on_keyup(uint16_t keycode) {
    if (is_keyheld) {
        cancel_keyheld();
        start_keyup_cooldown();
        start_keyheld_delay();
        release_key(keycode);

        return;
    }

    if (is_keyup_cooldown) {
        release_key(keycode);
        return;
    }

    uint16_t tapped_keycode = get_keycode();
    if (tapped_keycode == 0) {
        release_key(keycode);
        start_keyheld_delay();
        return;
    }

    tap_key(tapped_keycode);
    start_keyup_cooldown();
    start_keyheld_delay();
    release_key(keycode);
}


bool handle_custom_keys(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case TGL_QW:
            if (record->event.pressed) {
                if (is_qwerty_active) {
                    layer_move(BASE);
                    cancel_keyheld();
                    cancel_keyheld_delay();
                    cancel_keyup_cooldown();
                    num_pressed_keys = 0;
                } else {
                    layer_move(QWERTY);
                }

                is_qwerty_active = !is_qwerty_active;
            }

            return true;
    }

    return false;
}


void on_keydown(uint16_t keycode, keyrecord_t *record) {
    if (is_keyheld) {
        cancel_keyheld();
    }

    start_keyheld_delay();
    cancel_keyup_cooldown();
    
    press_key(keycode, record);
}


void on_keyheld_delay_timeout() {
    uint16_t keycode = get_keycode();
    if (keycode == 0) {
        return;
    }

    start_keyheld(keycode);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (handle_custom_keys(keycode, record)) return false;

    if (is_qwerty_active) {
        return true;
    }
    if (is_pathrough_key(keycode)) return true;

    if (record->event.pressed) {
        on_keydown(keycode, record);
    } else {
        on_keyup(keycode);
    }

    
    return false;
}

void matrix_scan_user() {
    if (keyheld_delay_timer && timer_elapsed(keyheld_delay_timer) > KEYHELD_DELAY) {
        on_keyheld_delay_timeout();
        cancel_keyheld_delay();
    }

    if (keyup_cooldown_timer && timer_elapsed(keyup_cooldown_timer) > KEYUP_COOLDOWN_DELAY) {
        cancel_keyup_cooldown();
    }
}



// void keyboard_post_init_user(void) {
//   // Customise these values to desired behaviour
//   debug_enable=true;
//   debug_matrix=true;
//   //debug_keyboard=true;
//   //debug_mouse=true;
// }

// keyrecord_t record {
//   keyevent_t event {
//     keypos_t key {
//       uint8_t col
//       uint8_t row
//     }
//     bool     pressed
//     uint16_t time
//   }
// }