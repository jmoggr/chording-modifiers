//
// exposes
//

void on_keydown(uint16_t keycode, uint8_t row, uint8_t col);
void on_keyup(uint16_t keycode, uint8_t row, uint8_t col);
void on_keyheld_delay_timeout(void);


//
// implementation
//

bool is_keyheld = false;
uint16_t keyup_cooldown_timer = 0;
uint16_t keyheld_delay_timer = 0;
uint16_t keyheld_keycode = 0;

void start_keyheld_delay(void) {
    keyheld_delay_timer = timer_read();
}

void cancel_keyheld_delay(void) {
    keyheld_delay_timer = 0;
}

void start_keyheld(uint16_t keycode) {
    register_code16(keycode);
    keyheld_keycode = keycode;
    is_keyheld = true;
}

void cancel_keyheld(void) {
    is_keyheld = false;
    unregister_code16(keyheld_keycode);
    keyheld_keycode = 0;
}

void on_keyheld_delay_timeout(void) {
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
        keycode = lookup_keycode(pressed_key->row, pressed_key->col);
    }

    if (!keycode) {
        cancel_keyheld();
        return;
    }

    start_keyheld(keycode);
}

void on_keyup(uint16_t keycode, uint8_t row, uint8_t col) {
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
    
    uint16_t new_keycode = lookup_keycode(row, col);
    if (new_keycode != 0) {
        tap_key(new_keycode);
    }

    reset_special_keys();
}


void on_keydown(uint16_t keycode, uint8_t row, uint8_t col) {
    if (is_keyheld) {
        cancel_keyheld();
    }

    SpecialKey* special_key = get_special_key(keycode);
    if (special_key) {
        on_special_keydown(special_key);
    } else {
        clear_active();
        press_key(keycode, row, col);
    }

    start_keyheld_delay();
}

void on_matrix_scan(void) {
    if (keyheld_delay_timer && timer_elapsed(keyheld_delay_timer) > KEYHELD_DELAY) {
        cancel_keyheld_delay();
        on_keyheld_delay_timeout();
    }
}