
// RNG functions

// https://filterpaper.github.io/prng.html

#define INITIAL_SEED 0xf1

static uint8_t jsf8_seed = INITIAL_SEED;

#define rot8(x,k) (((x) << (k))|((x) >> (8 - (k))))
uint8_t jsf8(void) {
	static uint8_t b = 0xee, c = 0xee, d = 0xee;

	uint8_t e = jsf8_seed - rot8(b, 1);
	jsf8_seed = b ^ rot8(c, 4);
	b = c + d;
	c = d + e;
	return d = e + jsf8_seed;
}

// Old display code

oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_180; }

bool oled_task_user(void) {
    char str[20] = { 0 };

    sprintf(str, "%03u\n", test);
    oled_write(str, false);

    if (is_gaming_mode) {
        oled_write_P(PSTR("Gaming Mode\n"), false);
        return false;
    }

    oled_write_P(PSTR("Shift"), false);
    if (is_shift_active) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_sft_latched) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_shift_cooling) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_shift_pressed) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }
    oled_write_P(PSTR("\n"), false);

    oled_write_P(PSTR("Layer"), false);
    if (is_layer_active) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_lyr4_latched) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_layer_cooling) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }

    if (is_layer_pressed) {
        oled_write_P(PSTR(" T "), false);
    } else {
        oled_write_P(PSTR(" F "), false);
    }
    oled_write_P(PSTR("\n"), false);

    sprintf(str, "%hhu, %hhu\n", last_row, last_col);
    oled_write(str, false);

    sprintf(str, "%i\n", last_layer);
    oled_write(str, false);

    sprintf(str, "%#06x\n", KC_TEST3);
    oled_write(str, false);

    return false;
}