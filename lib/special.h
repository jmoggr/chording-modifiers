//
// exposes
//

extern SpecialKey special_keys[NUM_SPECIAL_KEYS];
uint16_t latched_layer = 0;

void tap_key(uint16_t keycode);
SpecialKey* get_special_key(uint16_t keycode);
bool is_special_keycode_active_or_pressed(uint16_t keycode);
void clear_active(void);
bool is_special_keycode_active(uint16_t keycode);
void on_special_keyup(SpecialKey * special_key);
void on_special_keydown(SpecialKey * special_key);
void reset_special_keys(void);


//
// implementation
//

void latch_layer(uint16_t layer) {
    // TODO: should different layers have priority?
    latched_layer = layer;
}

SpecialKey* get_special_key(uint16_t keycode) {
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        if (special_keys[i].keycode == keycode) {
            return &special_keys[i];
        }
    }

    return 0;
}

bool update_cooling(SpecialKey * special_key) {
    if (special_key->is_cooling) {
        special_key->is_cooling = false;
        return true;
    }

    return false;
}

bool is_key_in_combo(int combo_index, uint16_t keycode) {
    for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
        if (COMBO_KEYS[combo_index][key_index] == keycode) {
            return true;
        }
    }

    return false;
}

bool is_special_keycode_active_or_pressed(uint16_t keycode) {
    SpecialKey * special_key = get_special_key(keycode);

    if (!special_key) {
        return false;
    }

    return special_key->is_active || special_key->is_pressed;
}

void clear_active(void) {
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        special_keys[i].is_active = false;
    }
}


bool update_active(SpecialKey * special_key) {
    bool is_active = num_pressed_keys > 0 && special_key->keycode != KC_SPC;
    if (is_active) {
        special_key->is_active = true;
        return true;
    }

    for (int combo_index = 0; combo_index < NUM_COMBOS; combo_index++) {
        if (!is_key_in_combo(combo_index, special_key->keycode)) {
            continue;
        }

        bool all_other_keys_pressed = true;
        for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
            if (special_key->keycode == COMBO_KEYS[combo_index][key_index]) {
                continue;
            }

            if (!is_special_keycode_active_or_pressed(COMBO_KEYS[combo_index][key_index])) {
                all_other_keys_pressed = false;
                break;
            }
        }

        if (!all_other_keys_pressed) {
            continue;
        }

        special_key->is_active = true;
        return true;
    }

    return false;
}

bool is_special_keycode_active(uint16_t keycode) {
    SpecialKey * special_key = get_special_key(keycode);

    if (!special_key) {
        return false;
    }

    return special_key->is_active;
}

bool all_keys_in_combo_active(int combo_index) {
    for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
        if (!is_special_keycode_active(COMBO_KEYS[combo_index][key_index])) {
            return false;
        }
    }

    return true;
}


bool update_latched_layers(SpecialKey * special_key) {
    for (int combo_index = 0; combo_index < NUM_COMBOS; combo_index++) {
        if (!is_key_in_combo(combo_index, special_key->keycode)) {
            continue;
        }

        if (!all_keys_in_combo_active(combo_index)) {
            continue;
        }

        // set `is_active = false` for all keys in combo
        for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
            SpecialKey * other_special_key = get_special_key(COMBO_KEYS[combo_index][key_index]);
            if (!other_special_key) {
                // TODO: log error
                continue;
            }
            other_special_key->is_active = false;
        }

        int layer_index = COMBO_KEYS[combo_index][0] + 1;

        latch_layer(COMBO_KEYS[combo_index][layer_index]);
        return true;
    }

    return false;
}

void tap_key(uint16_t keycode) {
    register_code16(keycode);
    unregister_code16(keycode);
}

void on_special_keyup(SpecialKey * special_key) {
    special_key->is_pressed = false;

    if (special_key->is_registered) {
        unregister_code16(special_key->keycode);
        special_key->is_registered = false;
    }

    if (update_cooling(special_key)) {
        return;
    }

    if (update_active(special_key)) {
        update_latched_layers(special_key);
        return;
    }

    if (special_key->keycode == KC_SPC) {
        tap_key(KC_SPC);
    } else {
        special_key->is_latched = true;
    }
}

void reset_special_keys(void) {
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        if (special_keys[i].keycode == KC_SPC) {
            continue;
        }

        if (special_keys[i].is_pressed) {
            special_keys[i].is_cooling = true;
        }

        special_keys[i].is_latched = false;
        special_keys[i].is_active = false;
    }
}

// Check if any combos that include the special key have other keys that
// are active or pressed
bool no_other_combo_keys_active_or_pressed(SpecialKey * special_key) {
    for (int combo_index = 0; combo_index < NUM_COMBOS; combo_index++) {
        if (!is_key_in_combo(combo_index, special_key->keycode)) {
            continue;
        }

        // no other keys in the combo may be pressed or active
        for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
            if (special_key->keycode == COMBO_KEYS[combo_index][key_index]) {
                continue;
            }

            if (is_special_keycode_active_or_pressed(COMBO_KEYS[combo_index][key_index])) {
                return false;
            }
        }
    }

    return true;
}

// For all combos that special_key is part of, unregister any other registered
// keys in that combo
void unregister_other_keys_in_combo(SpecialKey * special_key) {
    for (int combo_index = 0; combo_index < NUM_COMBOS; combo_index++) {
        if (!is_key_in_combo(combo_index, special_key->keycode)) {
            continue;
        }

        for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
            if (special_key->keycode == COMBO_KEYS[combo_index][key_index]) {
                continue;
            }

            SpecialKey * other_combo_key = get_special_key(COMBO_KEYS[combo_index][key_index]);
            if (!other_combo_key) {
                continue;
            }

            if (other_combo_key->is_registered) {
                unregister_code16(COMBO_KEYS[combo_index][key_index]);
                other_combo_key->is_registered = false;
                continue;
            }
        }
    }
}


void on_special_keydown(SpecialKey * special_key) {
    special_key->is_pressed = true;

    // A special key should only be registered if no other keys in the combo
    // are active or pressed
    bool should_register = no_other_combo_keys_active_or_pressed(special_key);

    if (should_register) {
        if (special_key->is_modifier) {
            register_code16(special_key->keycode);
            special_key->is_registered = true;
        }
    } else {
        unregister_other_keys_in_combo(special_key);
    }
}
