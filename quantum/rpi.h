/* Copyright 2025 Raspberry Pi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "compiler_support.h"
#include <stdint.h>

#include <stdbool.h>

#define RPI_PROTOCOL_VERSION ((uint32_t)0x00000001)
#define RPI_HID_COMMAND_LENGTH 32

#define RPI_COMMAND_ID 0xFC

#ifdef RGB_MATRIX_ENABLE
// Changing these values will cause the EEPROM on previous versions to be invalidated
#define RPI_RGB_SEQUENCE_MODES_MAX 7
#define RPI_RGB_SEQUENCE_MODE_CUSTOM_INDEX RPI_RGB_SEQUENCE_MODES_MAX
#define RPI_RGB_MODES_MAX (RPI_RGB_SEQUENCE_MODES_MAX + 1)
#define RPI_RGB_CUSTOM_LEDS_MAX 16*6
STATIC_ASSERT(RGB_MATRIX_LED_COUNT <= RPI_RGB_CUSTOM_LEDS_MAX, "RGB_MATRIX_LED_COUNT is greater than the EEPROM space available for custom LEDs");

#define NO_START_ANIMATION   0x00
#define START_ANIM_B_NO_FADE  0x01
#define START_ANIM_B_FADE_VAL 0x02
#define START_ANIM_W_NO_FADE  0x03
#define START_ANIM_W_FADE_SAT 0x04

#define SKIP_MODE 0xFFFF

typedef struct {
    uint8_t flags;
    uint16_t effect;
    uint8_t speed;
    bool fixed_hue;
    uint8_t startup_animation;
    uint8_t h;
    uint8_t s;
} rpi_rgb_mode_t;

#endif

enum rpi_command_id {
    id_rpi_get_version             = 0x01,
    id_rpi_reset_eeprom            = 0x02,
#ifdef RGB_MATRIX_ENABLE
    id_rpi_get_current_mode_index  = 0x03,
    id_rpi_set_current_mode_index  = 0x04,
    id_rpi_get_mode                = 0x05,
    id_rpi_set_mode                = 0x06,
    id_rpi_get_hue                 = 0x07,
    id_rpi_set_hue                 = 0x08,
    id_rpi_get_brightness          = 0x09,
    id_rpi_set_brightness          = 0x0A,
    id_rpi_get_current_direct_leds = 0x0B,
    id_rpi_get_saved_direct_leds   = 0x0C,
    id_rpi_save_direct_leds        = 0x0D,
    id_rpi_load_direct_leds        = 0x0E,
#endif
};

#ifdef RGB_MATRIX_ENABLE
void change_rpi_rgb_mode(bool increase);
void change_rpi_rgb_hue(bool increase);
void reload_rpi_rgb_mode_from_eeprom(void);
bool rpi_rgb_current_mode_is_a_preset(void);
uint8_t rpi_rgb_current_mode_startup_animation(void);

bool rpi_rgb_current_mode_is_fixed_hue(void);
void configure_rpi_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode);
#endif

void rpi_handle_cmd(uint8_t *data, uint8_t length);

void rpi_init(void);
void eeconfig_init_rpi(void);
bool rpi_eeprom_is_valid(void);
