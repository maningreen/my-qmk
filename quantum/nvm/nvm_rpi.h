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

#include <stdint.h>
#include <stdbool.h>
#include "rpi.h"

void nvm_rpi_read_magic(uint8_t *magic0, uint8_t *magic1, uint8_t *magic2);
void nvm_rpi_update_magic(uint8_t magic0, uint8_t magic1, uint8_t magic2);

#ifdef RGB_MATRIX_ENABLE
int nvm_rpi_get_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode);
int nvm_rpi_set_rgb_mode(uint8_t mode_index, rpi_rgb_mode_t *mode);

int nvm_rpi_get_rgb_mode_index(uint8_t *mode_index);
int nvm_rpi_set_rgb_mode_index(uint8_t mode_index);

int nvm_rpi_get_rgb_hue(uint8_t *hue);
int nvm_rpi_set_rgb_hue(uint8_t hue);

int nvm_rpi_get_custom_led(uint8_t led_index, uint8_t *hue, uint8_t *sat, uint8_t *val);
int nvm_rpi_set_custom_led(uint8_t led_index, uint8_t hue, uint8_t sat, uint8_t val);
#endif
