enum custom_keycodes {
    KC_LYR1 = SAFE_RANGE,
    KC_LYR2,
    KC_LYR3,
    KC_LYR4,
    KC_RNGC,
    KC_RNGN,
    KC_TEST,
    KC_TEST2,
    KC_GMNG,
    KC_TEST3,
    KC_TEST4,
};

enum layers {
    BASE,  // default layer
    SYMBOLS,
    LAYER1,
    LAYER2,
    LAYER3,
    LAYER4,
};

#define SPECIAL_KEY_DEFAULT .is_pressed = false, \
                            .is_active = false, \
                            .is_cooling = false, \
                            .is_latched = false, \
                            .is_registered = false,

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

#define KEYHELD_DELAY 250