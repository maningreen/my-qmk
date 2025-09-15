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

#include "nvm_eeprom_eeconfig_internal.h"
#include "via.h"
#include "nvm_eeprom_via_internal.h"
#include "rpi.h"

#ifdef RGB_MATRIX_ENABLE
#ifndef RPI_EEPROM_MODES_SIZE
#    define RPI_EEPROM_MODES_SIZE RPI_RGB_MODES_MAX * sizeof(rpi_rgb_mode_t)
#endif

#ifndef RPI_EEPROM_CUSTOM_LEDS_SIZE
#    define RPI_EEPROM_CUSTOM_LEDS_SIZE RPI_RGB_CUSTOM_LEDS_MAX * 3
#endif
#endif

#ifdef VIA_ENABLE
#    define RPI_EEPROM_START (VIA_EEPROM_CONFIG_END)
#else
#    define RPI_EEPROM_START (EECONFIG_SIZE)
#endif

#ifndef RPI_EEPROM_MAGIC_ADDR
#    define RPI_EEPROM_MAGIC_ADDR (RPI_EEPROM_START)
#endif

#ifdef RGB_MATRIX_ENABLE
#define RPI_EEPROM_MODE_INDEX_ADDR (RPI_EEPROM_MAGIC_ADDR + 3)
#define RPI_EEPROM_HUE_ADDR (RPI_EEPROM_MODE_INDEX_ADDR + 1)
#define RPI_EEPROM_MODES_ADDR (RPI_EEPROM_HUE_ADDR + 1)

#define RPI_EEPROM_CUSTOM_LEDS_ADDR (RPI_EEPROM_MODES_ADDR + RPI_EEPROM_MODES_SIZE)

#define RPI_EEPROM_END (RPI_EEPROM_CUSTOM_LEDS_ADDR + RPI_EEPROM_CUSTOM_LEDS_SIZE)

#else

#define RPI_EEPROM_END (RPI_EEPROM_MAGIC_ADDR + 3)

#endif
