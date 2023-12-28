

typedef struct SpecialKey {
    uint16_t keycode;
    bool is_pressed;
    bool is_active;
    bool is_cooling;
    bool is_latched;
    bool is_registered;
    bool is_modifier;
} SpecialKey;

SpecialKey* get_special_key(uint16_t keycode);
void on_special_keyup(SpecialKey * special_key);
void update_pressed(SpecialKey * special_key);
bool update_cooling(SpecialKey * special_key);
bool update_active(SpecialKey * special_key);
bool should_become_active(SpecialKey * special_key);
uint16_t get_other_key_in_combo(int combo_index, uint16_t keycode);
bool is_special_keycode_active(uint16_t keycode);
bool is_special_keycode_pressed(uint16_t keycode);
bool update_latched_layers(SpecialKey * special_key);
void test_header(void);
void reset_special_keys(void);
int get_active_layer(void);
void clear_active(void);
uint16_t apply_modifiers(uint16_t keycode);

uint16_t latched_layer = 0;

void latch_layer(uint16_t layer) {
    // TODO: should different layers have priority?
    latched_layer = layer;
}

#define NUM_SPECIAL_KEYS 5
SpecialKey special_keys[NUM_SPECIAL_KEYS] = {
    { 
        .keycode = KC_SPC,
        .is_modifier = false,

        .is_pressed = false,
        .is_active = false,
        .is_cooling = false,
        .is_latched = false,
        .is_registered = false,
    },
    { 
        .keycode = KC_LSFT,
        .is_modifier = true,

        .is_pressed = false,
        .is_active = false,
        .is_cooling = false,
        .is_latched = false,
        .is_registered = false,
    },
    {   .keycode = KC_LCTL,
        .is_modifier = true,

        .is_pressed = false,
        .is_active = false,
        .is_cooling = false,
        .is_latched = false,
        .is_registered = false,
    },
    {   
        .keycode = KC_RGUI,
        .is_modifier = true,

        .is_pressed = false,
        .is_active = false,
        .is_cooling = false,
        .is_latched = false,
        .is_registered = false,
    },
        {   
        .keycode = KC_RALT,
        .is_modifier = true,

        .is_pressed = false,
        .is_active = false,
        .is_cooling = false,
        .is_latched = false,
        .is_registered = false,
    }
};

#define NUM_COMBOS 3
uint16_t COMBO_KEYS[NUM_COMBOS][4] = {
    { 2, KC_SPC, KC_LSFT, SYMBOLS },
    { 2, KC_SPC, KC_LCTL, LAYER1 },
    { 2, KC_LSFT, KC_RALT, LAYER2 },
};

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


bool update_active(SpecialKey * special_key) {
    bool is_active = is_normal_pressed() && special_key->keycode != KC_SPC;
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

bool is_special_keycode_pressed(uint16_t keycode) {
    SpecialKey * special_key = get_special_key(keycode);

    if (!special_key) {
        return false;
    }

    return special_key->is_pressed;
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

void reset_special_keys() {
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        if (special_keys[i].is_pressed) {
            special_keys[i].is_cooling = true;
        }

        special_keys[i].is_latched = false;
        special_keys[i].is_active = false;
    }
}

void clear_active() {
    for (int i = 0; i < NUM_SPECIAL_KEYS; i++) {
        special_keys[i].is_active = false;
    }
}

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

int get_active_layer() {
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