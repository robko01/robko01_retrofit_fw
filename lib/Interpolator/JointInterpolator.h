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

// JointInterpolator.h

#ifndef _JOINTINTERPOLATOR_h
#define _JOINTINTERPOLATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "InterpolatorConfig.h"

/**
 * @brief Interpolator state machine states.
 */
enum InterpolatorState : uint8_t
{
    INTERP_IDLE = 0,        ///< No motion, ready for new command
    INTERP_PLANNING,        ///< Calculating motion profile
    INTERP_ACCELERATING,    ///< In acceleration phase
    INTERP_CRUISING,        ///< At constant velocity
    INTERP_DECELERATING,    ///< In deceleration phase
    INTERP_DONE,            ///< Motion complete
    INTERP_ERROR            ///< Error state
};

/**
 * @brief Motion profile data for a single joint.
 */
struct JointProfile
{
    long startPos;          ///< Starting position (steps)
    long targetPos;         ///< Target position (steps)
    long distance;          ///< Total distance to travel (signed)
    float velocityScale;    ///< Velocity scaling factor relative to dominant joint
};

/**
 * @brief Joint Interpolator class for synchronized multi-joint motion.
 *
 * Implements trapezoidal velocity profile with time-synchronized
 * joint motion where all joints arrive at target simultaneously.
 *
 * The dominant joint (requiring longest travel time) determines
 * the motion duration. Other joints scale their velocities
 * proportionally to arrive at the same time.
 */
class JointInterpolator
{
public:
    /**
     * @brief Constructor - initializes with default parameters.
     */
    JointInterpolator();

    /**
     * @brief Initialize the interpolator with per-joint parameters.
     * @param maxVelocities Array of max velocities per joint (steps/sec)
     * @param maxAccelerations Array of max accelerations per joint (steps/sec^2)
     */
    void init(const float* maxVelocities, const float* maxAccelerations);

    /**
     * @brief Set default parameters for all joints.
     * @param maxVel Default max velocity (steps/sec)
     * @param maxAccel Default max acceleration (steps/sec^2)
     */
    void setDefaultParams(float maxVel, float maxAccel);

    /**
     * @brief Start a new interpolated motion.
     * @param currentPositions Current joint positions (steps), array of INTERP_NUM_JOINTS
     * @param targetPositions Target joint positions (steps), array of INTERP_NUM_JOINTS
     * @return true if motion planning succeeded
     */
    bool startMotion(const long* currentPositions, const long* targetPositions);

    /**
     * @brief Update the interpolator - call from main loop.
     * @param interpolatedPositions Output array for computed positions (INTERP_NUM_JOINTS)
     * @return true if new positions are available
     */
    bool update(long* interpolatedPositions);

    /**
     * @brief Stop the current motion immediately (marks as complete).
     */
    void stop();

    /**
     * @brief Abort motion and return to idle state.
     */
    void abort();

    /**
     * @brief Check if motion is complete.
     * @return true if in IDLE or DONE state
     */
    bool isComplete() const;

    /**
     * @brief Check if interpolator is currently executing motion.
     * @return true if motion is in progress (ACCEL, CRUISE, or DECEL)
     */
    bool isActive() const;

    /**
     * @brief Get current state machine state.
     * @return Current InterpolatorState
     */
    InterpolatorState getState() const;

    /**
     * @brief Get motion progress as percentage.
     * @return Progress from 0.0 to 1.0
     */
    float getProgress() const;

    /**
     * @brief Get planned motion duration.
     * @return Duration in milliseconds
     */
    unsigned long getDuration() const;

    /**
     * @brief Get the index of the dominant joint.
     * @return Joint index (0-5) or -1 if no motion
     */
    int getDominantJoint() const;

private:
    /// Current state machine state
    InterpolatorState m_state;

    /// Motion profiles for each joint
    JointProfile m_joints[INTERP_NUM_JOINTS];

    /// Maximum velocity per joint (steps/sec)
    float m_maxVelocity[INTERP_NUM_JOINTS];

    /// Maximum acceleration per joint (steps/sec^2)
    float m_maxAcceleration[INTERP_NUM_JOINTS];

    /// Motion start timestamp (millis)
    unsigned long m_motionStartTime;

    /// Last update timestamp (millis)
    unsigned long m_lastUpdateTime;

    /// Total motion duration (ms)
    unsigned long m_totalDuration;

    /// Acceleration phase duration (ms)
    unsigned long m_accelDuration;

    /// Cruise phase duration (ms)
    unsigned long m_cruiseDuration;

    /// Deceleration phase duration (ms)
    unsigned long m_decelDuration;

    /// Peak velocity of the motion profile (steps/sec)
    float m_maxProfileVelocity;

    /// Acceleration value for the profile (steps/sec^2)
    float m_profileAcceleration;

    /// Index of joint determining motion duration
    int m_dominantJoint;

    /// Current time since motion start (seconds)
    float m_currentTime;

    /**
     * @brief Plan the motion profile based on joint distances.
     * @return true if planning succeeded
     */
    bool planMotion();

    /**
     * @brief Calculate position along trapezoidal profile.
     * @param t Time since motion start (seconds)
     * @param jointIndex Joint index (0-5)
     * @return Position at time t (steps)
     */
    long calculatePosition(float t, int jointIndex);

    /**
     * @brief Find the joint that requires the longest travel time.
     * @return Index of dominant joint, or -1 if no motion needed
     */
    int findDominantJoint();

    /**
     * @brief Calculate minimum time to travel a distance.
     * @param distance Distance to travel (steps, absolute)
     * @param maxVel Maximum velocity (steps/sec)
     * @param maxAccel Maximum acceleration (steps/sec^2)
     * @return Minimum time in seconds
     */
    float calculateMinTime(long distance, float maxVel, float maxAccel);
};

#endif // _JOINTINTERPOLATOR_h
