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

// JointInterpolator.cpp

#include "JointInterpolator.h"
#include <math.h>

JointInterpolator::JointInterpolator()
    : m_state(INTERP_IDLE)
    , m_motionStartTime(0)
    , m_lastUpdateTime(0)
    , m_totalDuration(0)
    , m_accelDuration(0)
    , m_cruiseDuration(0)
    , m_decelDuration(0)
    , m_maxProfileVelocity(0)
    , m_profileAcceleration(0)
    , m_dominantJoint(-1)
    , m_currentTime(0)
{
    setDefaultParams(INTERP_DEFAULT_MAX_VEL, INTERP_DEFAULT_MAX_ACCEL);

    // Initialize joint profiles
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        m_joints[i].startPos = 0;
        m_joints[i].targetPos = 0;
        m_joints[i].distance = 0;
        m_joints[i].velocityScale = 0.0f;
    }
}

void JointInterpolator::setDefaultParams(float maxVel, float maxAccel)
{
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        m_maxVelocity[i] = maxVel;
        m_maxAcceleration[i] = maxAccel;
    }
}

void JointInterpolator::init(const float* maxVelocities, const float* maxAccelerations)
{
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        m_maxVelocity[i] = maxVelocities[i];
        m_maxAcceleration[i] = maxAccelerations[i];
    }
    m_state = INTERP_IDLE;
}

bool JointInterpolator::startMotion(const long* currentPositions, const long* targetPositions)
{
    // Check if already in motion
    if (m_state != INTERP_IDLE && m_state != INTERP_DONE && m_state != INTERP_ERROR)
    {
        return false;
    }

    // Store positions and calculate distances
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        m_joints[i].startPos = currentPositions[i];
        m_joints[i].targetPos = targetPositions[i];
        m_joints[i].distance = targetPositions[i] - currentPositions[i];
    }

    // Plan the motion profile
    m_state = INTERP_PLANNING;
    if (!planMotion())
    {
        m_state = INTERP_ERROR;
        return false;
    }

    // Check for zero motion
    if (m_totalDuration == 0)
    {
        m_state = INTERP_DONE;
        return true;
    }

    // Start motion
    m_motionStartTime = millis();
    m_lastUpdateTime = m_motionStartTime;
    m_currentTime = 0.0f;
    m_state = INTERP_ACCELERATING;

    return true;
}

bool JointInterpolator::planMotion()
{
    // Find dominant joint (longest travel time)
    m_dominantJoint = findDominantJoint();

    if (m_dominantJoint < 0)
    {
        // No motion needed - all joints already at target
        m_totalDuration = 0;
        m_accelDuration = 0;
        m_cruiseDuration = 0;
        m_decelDuration = 0;
        return true;
    }

    // Get dominant joint parameters
    long domDistance = abs(m_joints[m_dominantJoint].distance);
    float domMaxVel = m_maxVelocity[m_dominantJoint];
    float domMaxAccel = m_maxAcceleration[m_dominantJoint];

    // Calculate trapezoidal profile for dominant joint
    // Time to accelerate to max velocity: t_accel = v_max / a
    float t_accel = domMaxVel / domMaxAccel;

    // Distance during acceleration: d_accel = 0.5 * a * t^2
    float d_accel = 0.5f * domMaxAccel * t_accel * t_accel;

    // Check if we can reach max velocity (trapezoidal vs triangular profile)
    if (2 * d_accel >= domDistance)
    {
        // Triangular profile - never reach max velocity
        // d = 0.5 * a * t^2 for each half, so d_total = a * t^2
        // t = sqrt(d / a)
        t_accel = sqrt((float)domDistance / domMaxAccel);
        m_accelDuration = (unsigned long)(t_accel * 1000.0f);
        m_cruiseDuration = 0;
        m_decelDuration = m_accelDuration;
        m_maxProfileVelocity = domMaxAccel * t_accel;
    }
    else
    {
        // Trapezoidal profile - reach and maintain max velocity
        float d_cruise = domDistance - 2.0f * d_accel;
        float t_cruise = d_cruise / domMaxVel;

        m_accelDuration = (unsigned long)(t_accel * 1000.0f);
        m_cruiseDuration = (unsigned long)(t_cruise * 1000.0f);
        m_decelDuration = m_accelDuration;
        m_maxProfileVelocity = domMaxVel;
    }

    // Calculate total duration with minimum check
    m_totalDuration = m_accelDuration + m_cruiseDuration + m_decelDuration;
    if (m_totalDuration < INTERP_MIN_DURATION_MS)
    {
        m_totalDuration = INTERP_MIN_DURATION_MS;
    }

    m_profileAcceleration = domMaxAccel;

    // Calculate velocity scaling for all joints
    // Each joint moves proportionally to the dominant joint
    long domDist = m_joints[m_dominantJoint].distance;
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        if (domDist != 0)
        {
            m_joints[i].velocityScale = (float)m_joints[i].distance / (float)domDist;
        }
        else
        {
            m_joints[i].velocityScale = 0.0f;
        }
    }

    return true;
}

int JointInterpolator::findDominantJoint()
{
    int dominant = -1;
    float maxTime = 0.0f;

    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        // Skip joints with no motion
        if (m_joints[i].distance == 0)
        {
            continue;
        }

        // Calculate minimum time for this joint
        float time = calculateMinTime(
            abs(m_joints[i].distance),
            m_maxVelocity[i],
            m_maxAcceleration[i]
        );

        if (time > maxTime)
        {
            maxTime = time;
            dominant = i;
        }
    }

    return dominant;
}

float JointInterpolator::calculateMinTime(long distance, float maxVel, float maxAccel)
{
    if (distance == 0)
    {
        return 0.0f;
    }

    // Time to accelerate to max velocity
    float t_accel = maxVel / maxAccel;

    // Distance during acceleration
    float d_accel = 0.5f * maxAccel * t_accel * t_accel;

    if (2 * d_accel >= distance)
    {
        // Triangular profile - time = 2 * sqrt(d / a)
        return 2.0f * sqrt((float)distance / maxAccel);
    }
    else
    {
        // Trapezoidal profile
        float d_cruise = distance - 2.0f * d_accel;
        float t_cruise = d_cruise / maxVel;
        return 2.0f * t_accel + t_cruise;
    }
}

bool JointInterpolator::update(long* interpolatedPositions)
{
    // Check for inactive states
    if (m_state == INTERP_IDLE || m_state == INTERP_ERROR)
    {
        return false;
    }

    // If already done, return final positions
    if (m_state == INTERP_DONE)
    {
        for (int i = 0; i < INTERP_NUM_JOINTS; i++)
        {
            interpolatedPositions[i] = m_joints[i].targetPos;
        }
        return true;
    }

    // Calculate elapsed time
    unsigned long now = millis();
    unsigned long elapsed = now - m_motionStartTime;
    m_currentTime = elapsed / 1000.0f;

    // Update state based on elapsed time
    if (elapsed < m_accelDuration)
    {
        m_state = INTERP_ACCELERATING;
    }
    else if (elapsed < m_accelDuration + m_cruiseDuration)
    {
        m_state = INTERP_CRUISING;
    }
    else if (elapsed < m_totalDuration)
    {
        m_state = INTERP_DECELERATING;
    }
    else
    {
        // Motion complete - return final positions
        m_state = INTERP_DONE;
        for (int i = 0; i < INTERP_NUM_JOINTS; i++)
        {
            interpolatedPositions[i] = m_joints[i].targetPos;
        }
        return true;
    }

    // Calculate interpolated positions for all joints
    for (int i = 0; i < INTERP_NUM_JOINTS; i++)
    {
        interpolatedPositions[i] = calculatePosition(m_currentTime, i);
    }

    m_lastUpdateTime = now;
    return true;
}

long JointInterpolator::calculatePosition(float t, int jointIndex)
{
    // Convert durations to seconds
    float t_accel = m_accelDuration / 1000.0f;
    float t_cruise = m_cruiseDuration / 1000.0f;
    float t_total = m_totalDuration / 1000.0f;

    // Prevent division by zero
    if (t_total <= 0.0f || t_accel <= 0.0f)
    {
        return m_joints[jointIndex].targetPos;
    }

    // Calculate normalized position (0 to 1) along trajectory
    // This is the "S" parameter representing progress
    float s;

    if (t <= 0.0f)
    {
        s = 0.0f;
    }
    else if (t >= t_total)
    {
        s = 1.0f;
    }
    else if (t <= t_accel)
    {
        // Acceleration phase: s = 0.5 * (t/t_accel)^2 * (t_accel/t_total)
        float ratio = t / t_accel;
        s = 0.5f * ratio * ratio * (t_accel / t_total);
    }
    else if (t <= t_accel + t_cruise)
    {
        // Cruise phase: linear interpolation
        float dt = t - t_accel;
        // Position at end of accel phase
        float s_accel = 0.5f * t_accel / t_total;
        // Add linear cruise portion
        s = s_accel + (dt / t_total);
    }
    else
    {
        // Deceleration phase
        float dt = t - t_accel - t_cruise;
        float t_decel = m_decelDuration / 1000.0f;
        float remaining = t_decel - dt;

        if (remaining <= 0.0f)
        {
            s = 1.0f;
        }
        else
        {
            // s = 1 - 0.5 * (remaining/t_decel)^2 * (t_decel/t_total)
            float ratio = remaining / t_decel;
            s = 1.0f - 0.5f * ratio * ratio * (t_decel / t_total);
        }
    }

    // Clamp s to valid range
    if (s < 0.0f) s = 0.0f;
    if (s > 1.0f) s = 1.0f;

    // Apply progress to this joint's motion
    return m_joints[jointIndex].startPos +
           (long)(s * (float)m_joints[jointIndex].distance);
}

void JointInterpolator::stop()
{
    m_state = INTERP_DONE;
}

void JointInterpolator::abort()
{
    m_state = INTERP_IDLE;
}

bool JointInterpolator::isComplete() const
{
    return (m_state == INTERP_IDLE || m_state == INTERP_DONE);
}

bool JointInterpolator::isActive() const
{
    return (m_state == INTERP_ACCELERATING ||
            m_state == INTERP_CRUISING ||
            m_state == INTERP_DECELERATING);
}

InterpolatorState JointInterpolator::getState() const
{
    return m_state;
}

float JointInterpolator::getProgress() const
{
    if (m_state == INTERP_IDLE)
    {
        return 0.0f;
    }
    if (m_state == INTERP_DONE)
    {
        return 1.0f;
    }
    if (m_totalDuration == 0)
    {
        return 1.0f;
    }

    unsigned long elapsed = millis() - m_motionStartTime;
    float progress = (float)elapsed / (float)m_totalDuration;

    return (progress > 1.0f) ? 1.0f : progress;
}

unsigned long JointInterpolator::getDuration() const
{
    return m_totalDuration;
}

int JointInterpolator::getDominantJoint() const
{
    return m_dominantJoint;
}
