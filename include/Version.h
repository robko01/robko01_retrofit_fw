/*

    Robko 01 - ESP32 Control Software

    Copyright (C) [2025] [Orlin Dimitrov]

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/**
 * @file Version.h
 * @brief Centralized firmware version and build number definitions.
 *
 * This file contains the firmware version information that should be
 * updated when releasing new versions. The build number should be
 * incremented on each significant change.
 *
 * Version Format: MAJOR.MINOR.BUILD
 * - MAJOR: Incremented for breaking changes or major features
 * - MINOR: Incremented for new features or significant improvements
 * - BUILD: Incremented for each release/commit to main
 */

#ifndef _VERSION_H
#define _VERSION_H

#pragma region Version Information

/**
 * @brief Major version number.
 * Increment for breaking changes or major new features.
 */
#define FW_VERSION_MAJOR 1

/**
 * @brief Minor version number.
 * Increment for new features or significant improvements.
 */
#define FW_VERSION_MINOR 0

/**
 * @brief Build number.
 * Increment on each merge to main branch.
 * Current build: 59
 */
#define FW_BUILD_NUMBER 59

/**
 * @brief Version string macro.
 * Combines major, minor, and build into a string format.
 */
#define FW_VERSION_STR_HELPER(maj, min, bld) #maj "." #min "." #bld
#define FW_VERSION_STR_EXPAND(maj, min, bld) FW_VERSION_STR_HELPER(maj, min, bld)
#define FW_VERSION_STRING FW_VERSION_STR_EXPAND(FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_BUILD_NUMBER)

/**
 * @brief Full version with prefix for display.
 */
#define FW_FULL_VERSION "v" FW_VERSION_STRING

#pragma endregion // Version Information

#endif // _VERSION_H
