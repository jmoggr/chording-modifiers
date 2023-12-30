enum custom_keycodes {
    KC_LYR1 = SAFE_RANGE,
    KC_RNGC,
    KC_RNGN,
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
        .keycode = KC_LYR1,
        .is_modifier = false,
        SPECIAL_KEY_DEFAULT
    }
};

// All keys used in combos MUST also be included in special_keys
#define NUM_COMBOS 3
uint16_t COMBO_KEYS[NUM_COMBOS][4] = {
    { 2, KC_SPC, KC_LSFT, SYMBOLS },
    { 2, KC_SPC, KC_LCTL, LAYER1 },
    { 1, KC_LYR1, LAYER2 },
};

#define KEYHELD_DELAY 250