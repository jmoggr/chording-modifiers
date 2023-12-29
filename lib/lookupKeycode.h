//
// exposes
//

uint16_t lookup_keycode(uint8_t row, uint8_t col);


//
// implementation
//

uint16_t apply_modifiers(uint16_t keycode) {
    uint16_t modified_keycode = keycode;
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        if (!special_keys[i].is_active && !special_keys[i].is_latched) {
            continue;
        }

        switch (special_keys[i].keycode) {
            case KC_LSFT:
                modified_keycode = S(modified_keycode);
                break;
            case KC_LCTL:
                modified_keycode = C(modified_keycode);
                break;
            case KC_LALT:
                modified_keycode = A(modified_keycode);
                break;
            case KC_RGUI:
                modified_keycode = G(modified_keycode);
                break;
            default:
                break;
        }
    }

    return modified_keycode;
}

int get_combo_layer_if_active(int combo_index) {
    bool all_keys_active_or_pressed = true;
    for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
        if (!is_special_keycode_active_or_pressed(COMBO_KEYS[combo_index][key_index])) {
            all_keys_active_or_pressed = false;
            break;
        }
    }

    if (!all_keys_active_or_pressed) {
        return 0;
    }

    for (int key_index = 1; key_index <= COMBO_KEYS[combo_index][0]; key_index++) {
        SpecialKey * special_key = get_special_key(COMBO_KEYS[combo_index][key_index]);
        if (!special_key) {
            // TODO: log error
            continue;
        }

        special_key->is_cooling = special_key->is_pressed;
        special_key->is_active = false;
    }

    int layer_index = COMBO_KEYS[combo_index][0] + 1;
    return COMBO_KEYS[combo_index][layer_index];
}

int get_active_layer(void) {
    for (int i = 0; i < NUM_COMBOS; i++) {
        int active_layer = get_combo_layer_if_active(i);
        if (active_layer) {
            return active_layer;
        }
    }

    int active_layer = BASE;

    if (latched_layer) {
        active_layer = latched_layer;
        latched_layer = 0;
    }

    return active_layer;
}

uint16_t lookup_keycode(uint8_t row, uint8_t col) {
    int layer = get_active_layer();

    uint16_t keycode = pgm_read_word(&keymaps[layer][row][col]);
    keycode = apply_modifiers(keycode);

    return keycode;
}