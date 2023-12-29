enum custom_keycodes {
    KC_LYR1 = SAFE_RANGE,
    KC_RNGC,
    KC_RNGN
};

enum layers {
    BASE,
    SYMBOLS,
    LAYER1,
    LAYER2,
};

#define NUM_SPECIAL_KEYS 5
SpecialKey special_keys[NUM_SPECIAL_KEYS] = {
    { 
        .keycode = KC_SPC,
        .is_modifier = false,
        SPECIAL_KEY_DEFAULT
    },
    {
        .keycode = KC_LSFT,
        .is_modifier = true,
        SPECIAL_KEY_DEFAULT
    },
    {   .keycode = KC_LCTL,
        .is_modifier = true,
        SPECIAL_KEY_DEFAULT
    },
    {   
        .keycode = KC_RGUI,
        .is_modifier = true,
        SPECIAL_KEY_DEFAULT
    },
        {   
        .keycode = KC_RALT,
        .is_modifier = true,
        SPECIAL_KEY_DEFAULT
    }
};

#define NUM_COMBOS 3
uint16_t COMBO_KEYS[NUM_COMBOS][4] = {
    { 2, KC_SPC, KC_LSFT, SYMBOLS },
    { 2, KC_SPC, KC_LCTL, LAYER1 },
    { 2, KC_LSFT, KC_RALT, LAYER2 },
};

#define KEYHELD_DELAY 250