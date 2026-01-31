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

// InterpolatorConfig.h

#ifndef _INTERPOLATORCONFIG_h
#define _INTERPOLATORCONFIG_h

/**
 * @brief Number of joints in the robot arm.
 */
#define INTERP_NUM_JOINTS 6

/**
 * @brief Time step for interpolation updates (milliseconds).
 * @note 20ms provides good balance between smoothness and CPU overhead.
 */
#define INTERP_TIME_STEP_MS 20

/**
 * @brief Default maximum velocity (steps/sec) for joints.
 * @note Can be overridden per joint during initialization.
 */
#define INTERP_DEFAULT_MAX_VEL 80.0f

/**
 * @brief Default maximum acceleration (steps/sec^2) for joints.
 * @note Can be overridden per joint during initialization.
 */
#define INTERP_DEFAULT_MAX_ACCEL 60.0f

/**
 * @brief Minimum motion duration (milliseconds).
 * @note Prevents very short motions that may cause issues.
 */
#define INTERP_MIN_DURATION_MS 100

/**
 * @brief Position tolerance for completion check (steps).
 */
#define INTERP_POSITION_TOLERANCE 2

#endif // _INTERPOLATORCONFIG_h
