// sudo make splitkb/kyria/rev1:jmoggr:flash
//
// https://www.reddit.com/r/ErgoMechKeyboards/comments/r7y42o/keymap_tinkering/

#include QMK_KEYBOARD_H
#include "version.h"
#include "lib/lib.h"

// Key events are only sent when a normal key is released

// active means that the key has just been released and that can be used when selecting the active layer.
// It is reset when a normal key is released
// It is only set when another key is pressed, if no other key is pressed that the key has a connection (normal keys, another key in the combo) to, then the key becomes latched

// cooling means that a pressed key has already been "used" in another keyup event and nothing should be done when it's released


// Pressed -> +pressed -> Released -> -pressed, +active -> Used in Normal Keyup -> -active
// Pressed -> +pressed -> Used in Normal Keyup -> +cooling -> Released -> -pressed, + -cooling

void keyboard_post_init_user(void) {
    #ifdef CONSOLE_ENABLE
        debug_enable = true;
    #endif
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
        KC_ESC,  KC_Z,    KC_D,    KC_R,    KC_W,    KC_BSPC,                                         KC_UP,   KC_F,    KC_U,    KC_P,    KC_Q,   KC_NO,
        KC_G,    KC_A,    KC_S,    KC_H,    KC_T,    KC_RSFT,                                         KC_DOWN, KC_N,    KC_E,    KC_O,    KC_I,   KC_Y,
        KC_NO,   KC_V,    KC_X,    KC_M,    KC_C,    KC_NO,   KC_NO,    KC_NO,      KC_NO,   KC_NO,   KC_NO,   KC_L,    KC_B,    KC_J,    KC_K,   KC_NO,
                                   KC_NO,   KC_NO,   KC_LCTL, KC_SPC,   KC_RGUI,    KC_NO,   KC_LSFT, KC_RALT, KC_NO,   KC_NO
    ),

    // KC_SPC + KC_LSFT
    [SYMBOLS] = LAYOUT(
        _______, KC_LABK, KC_LBRC, KC_LCBR, KC_LPRN, KC_NO,                                       _______, KC_RPRN, KC_RCBR, KC_RBRC, KC_RABK, _______,
        KC_EXLM, KC_DQUO, KC_QUOT, KC_COMM, KC_DOT,  KC_NO,                                       _______, KC_BSPC, KC_ENT,  KC_TAB,  KC_MINS, KC_QUES,
        _______, KC_DLR,  KC_SLSH, KC_EQL,  KC_SCLN, KC_NO,   _______, _______, _______, _______, _______, KC_COLN, KC_UNDS, KC_BSLS, KC_PLUS, _______,
                                   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    // KC_LCTL + KC_SPC
    [LAYER1] = LAYOUT(
        _______, KC_DOT,  KC_4,    KC_5,    KC_6,    KC_TAB,                                      _______, KC_RNGN, KC_RNGC, KC_NO,   KC_NO,    _______,
        KC_NO,   KC_0,    KC_1,    KC_2,    KC_3,    KC_ENT,                                      _______, KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT, KC_RIGHT,
        _______, KC_MINS, KC_7,    KC_8,    KC_9,    KC_NO,   _______, _______, _______, _______, _______, KC_NO,   KC_NO,   KC_NO,   KC_NO,    _______,
                                   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),

    // KC_LSFT + KC_RALT
    [LAYER2] = LAYOUT(
        _______, KC_DOT,  KC_4,    KC_5,    KC_6,    KC_NO,                                       _______, KC_AT,   KC_HASH, KC_GRAVE,   KC_NO,   _______,
        KC_NO,   KC_0,    KC_1,    KC_2,    KC_3,    KC_NO,                                       _______, C(KC_BSPC),   KC_PERC, KC_ASTR, KC_TILD, KC_NO,
        _______, KC_MINS, KC_7,    KC_8,    KC_9,    KC_NO,   _______, _______, _______, _______, _______, KC_AMPR, KC_PIPE, KC_CIRC, KC_NO,   _______,
                                   _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    ),
};

bool oled_task_user(void) {
    return false;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        on_keydown(keycode, record->event.key.row, record->event.key.col);
    } else {
        on_keyup(keycode, record->event.key.row, record->event.key.col);
    }
    
    return false;
}

void matrix_scan_user() {
    on_matrix_scan();
}