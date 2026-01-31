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
 * @file OpCodesExtension.h
 * @brief Extended operation codes for SUPER protocol.
 *
 * This file extends the base OperationsCodes.h from the Robko 01 Framework
 * Library with additional operation codes specific to this firmware.
 */

#ifndef _OPCODESEXTENSION_H
#define _OPCODESEXTENSION_H

#include "OperationsCodes.h"

/**
 * @brief Extended operation codes.
 *
 * These codes extend the base OpCodes enum from the framework library.
 * Values start from 17 to avoid conflicts with base OpCodes (1-16).
 */
namespace OpCodesExt
{
    /**
     * @brief Move robot using joint interpolation.
     *
     * Performs synchronized motion where all joints arrive at target
     * positions simultaneously using trapezoidal velocity profile.
     *
     * Payload format (12 bytes - 6 x int16_t):
     * - Bytes 0-1:  Joint 1 (Base) target position [steps]
     * - Bytes 2-3:  Joint 2 (Shoulder) target position [steps]
     * - Bytes 4-5:  Joint 3 (Elbow) target position [steps]
     * - Bytes 6-7:  Joint 4 (Left Diff) target position [steps]
     * - Bytes 8-9:  Joint 5 (Right Diff) target position [steps]
     * - Bytes 10-11: Joint 6 (Gripper) target position [steps]
     *
     * Response payload (4 bytes - uint32_t):
     * - Bytes 0-3: Motion duration in milliseconds
     */
    const uint8_t MoveInterpolated = 17U;

    /**
     * @brief Move robot using inverse kinematics.
     *
     * Performs motion to Cartesian coordinates using inverse kinematics.
     *
     * Payload format (12 bytes - 6 x int16_t):
     * - Bytes 0-1:  X position [mm * 10]
     * - Bytes 2-3:  Y position [mm * 10]
     * - Bytes 4-5:  Z position [mm * 10]
     * - Bytes 6-7:  Pitch angle [degrees * 10]
     * - Bytes 8-9:  Roll angle [degrees * 10]
     * - Bytes 10-11: Gripper position [steps]
     *
     * Response payload (4 bytes - uint32_t):
     * - Bytes 0-3: Motion duration in milliseconds
     */
    const uint8_t MoveIK = 18U;

    /**
     * @brief Get interpolator state.
     *
     * Returns current interpolator state and progress.
     *
     * No payload required.
     *
     * Response payload (5 bytes):
     * - Byte 0: InterpolatorState enum value
     * - Bytes 1-4: Progress (0-100 as float * 100)
     */
    const uint8_t GetInterpolatorState = 19U;
}

#endif // _OPCODESEXTENSION_H
