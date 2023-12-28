// sudo make splitkb/kyria/rev1:jmoggr:flash
//
// https://www.reddit.com/r/ErgoMechKeyboards/comments/r7y42o/keymap_tinkering/

#include QMK_KEYBOARD_H
#include "version.h"
#include "print.h"
#include "lib/shared.h"
#include "lib/combos.h"


typedef struct PressedKey PressedKey;

void on_keyup(uint16_t keycode, keyrecord_t * record);
void on_keydown(uint16_t keycode, keyrecord_t * record);
void press_key(uint16_t keycode, keyrecord_t* record);

void start_keyheld_delay(void);
uint8_t jsf8(void);
void cancel_keyheld_delay(void);
void start_keyheld(uint16_t keycode);
void cancel_keyheld(void);

uint16_t get_keycode_for_layer(uint8_t key_row, uint8_t key_col, int active_layer);
void on_keyheld_delay_timeout(void);

bool is_keyheld = false;
uint16_t keyup_cooldown_timer = 0;
uint16_t keyheld_delay_timer = 0;
uint16_t keyheld_keycode = 0;

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


#define KEYHELD_DELAY 250


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
    is_keyheld = false;
    unregister_code16(keyheld_keycode);
    keyheld_keycode = 0;
}

void on_keyheld_delay_timeout() {
    clear_active();

    if (num_pressed_keys == 0) {
        cancel_keyheld();
    }

    uint16_t keycode = 0;
    if (num_pressed_keys == 0) {
        bool is_only_space_pressed = false;
        for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
            if (special_keys[i].keycode == KC_SPC) {
                if (special_keys[i].is_pressed) {
                    is_only_space_pressed = true;
                } else {
                    break;
                }
            } else {
                if (special_keys[i].is_pressed) {
                    is_only_space_pressed = false;
                    break;
                }
            }
        }

        if (is_only_space_pressed) {
            keycode = KC_SPC;
        }
    } else {
        PressedKey* pressed_key = &pressed_keys[num_pressed_keys - 1];
        int layer = get_active_layer();
        keycode = get_keycode_for_layer(pressed_key->row, pressed_key->col, layer);
    }

    if (!keycode) {
        cancel_keyheld();
        return;
    }

    start_keyheld(keycode);
}

void press_key(uint16_t keycode, keyrecord_t* record) {
    if (num_pressed_keys == MAX_PRESSED_KEYS) {
        return;
    }

    pressed_keys[num_pressed_keys].keycode = keycode;
    pressed_keys[num_pressed_keys].row = record->event.key.row;
    pressed_keys[num_pressed_keys].col = record->event.key.col;
    pressed_keys[num_pressed_keys].keyup_timer = 0;

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

uint16_t get_keycode_for_layer(uint8_t key_row, uint8_t key_col, int active_layer) {
    return pgm_read_word(&keymaps[active_layer][key_row][key_col]);
}

void on_keyup(uint16_t keycode, keyrecord_t *record) {
    SpecialKey* special_key = get_special_key(keycode);
    if (special_key) {
        on_special_keyup(special_key);
    } else {
        release_key(keycode);
    }

    if (is_keyheld) {
        cancel_keyheld();
        start_keyheld_delay();
        return;
    }

    if (special_key) {
        start_keyheld_delay();
        return;
    }

    int layer = get_active_layer();
    uint16_t tapped_keycode = get_keycode_for_layer(record->event.key.row, record->event.key.col, layer);
    tapped_keycode = apply_modifiers(tapped_keycode);
    
    if (tapped_keycode != 0) {
        tap_key(tapped_keycode);
    }

    reset_special_keys();
}


void on_keydown(uint16_t keycode, keyrecord_t *record) {
    last_row = record->event.key.row;
    last_col = record->event.key.col;

    if (is_keyheld) {
        cancel_keyheld();
    }

    SpecialKey* special_key = get_special_key(keycode);
    if (special_key) {
        on_special_keydown(special_key);
    } else {
        clear_active();
        press_key(keycode, record);
    }

    start_keyheld_delay();
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        on_keydown(keycode, record);
    } else {
        on_keyup(keycode, record);
    }
    
    return false;
}

void matrix_scan_user() {
    if (keyheld_delay_timer && timer_elapsed(keyheld_delay_timer) > KEYHELD_DELAY) {
        cancel_keyheld_delay();
        on_keyheld_delay_timeout();
    }
}