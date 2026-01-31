/**
 * @file RobotKinematics.cpp
 * @brief Inverse Kinematics implementation for Robko01 robot arm
 * @author Orlin Dimitrov
 * @version 1.0.0
 */

#include "RobotKinematics.h"

#pragma region Public Methods

void RobotKinematics::init(float stepsPerRad[IK_NUM_JOINTS])
{
    for (int IndexL = 0; IndexL < IK_NUM_JOINTS; IndexL++)
    {
        stepsPerRad_[IndexL] = stepsPerRad[IndexL];
        lastAngles_[IndexL] = 0.0f;
    }
    lastError_ = IK_OK;
}

int RobotKinematics::solve(float x, float y, float z,
                            float pitchDeg, float rollDeg, float gripper,
                            long jointSteps[IK_NUM_JOINTS])
{
    // Convert pitch and roll from degrees to radians.
    float PitchRadL = pitchDeg * IK_DEG_TO_RAD;
    float RollRadL = rollDeg * IK_DEG_TO_RAD;

    // Step 1: Calculate base angle (rotation around Z-axis).
    // Handle the case when x and y are both zero (straight up).
    float Theta1L = 0.0f;
    if (fabsf(x) > 0.001f || fabsf(y) > 0.001f)
    {
        Theta1L = atan2f(y, x);
    }

    // Step 2: Calculate horizontal distance and relative height.
    float rL = sqrtf(x * x + y * y);  // Horizontal distance from base axis.
    float zRelL = z - IK_BASE_HEIGHT;  // Height relative to shoulder.

    // Step 3: Calculate distance from shoulder to wrist center.
    float dL = sqrtf(rL * rL + zRelL * zRelL);

    // Step 4: Check if target is reachable.
    if (dL > IK_MAX_REACH || dL < IK_MIN_REACH)
    {
        lastError_ = IK_ERR_UNREACHABLE;
        return IK_ERR_UNREACHABLE;
    }

    // Step 5: Calculate elbow angle using law of cosines.
    // cos(theta3) = (L2^2 + L3^2 - d^2) / (2 * L2 * L3)
    float CosTheta3L = (IK_UPPER_ARM * IK_UPPER_ARM + IK_FOREARM * IK_FOREARM - dL * dL) /
                       (2.0f * IK_UPPER_ARM * IK_FOREARM);

    // Clamp to valid range for acos.
    CosTheta3L = clamp(CosTheta3L, -1.0f, 1.0f);

    // Elbow angle (positive = elbow up configuration).
    // Note: The elbow angle is measured from the straight configuration.
    float Theta3L = acosf(CosTheta3L);

    // Convert to joint coordinate (elbow angle relative to forearm inline with upper arm).
    // When elbow is straight, joint angle = PI. When bent, it's less.
    Theta3L = M_PI - Theta3L;

    // Step 6: Calculate shoulder angle.
    // Alpha is the angle from horizontal to the line connecting shoulder to wrist.
    float AlphaL = atan2f(zRelL, rL);

    // Beta is the angle between upper arm and line to wrist (law of cosines).
    float CosBetaL = (IK_UPPER_ARM * IK_UPPER_ARM + dL * dL - IK_FOREARM * IK_FOREARM) /
                     (2.0f * IK_UPPER_ARM * dL);
    CosBetaL = clamp(CosBetaL, -1.0f, 1.0f);
    float BetaL = acosf(CosBetaL);

    // Shoulder angle (elbow-up solution: alpha + beta).
    float Theta2L = AlphaL + BetaL;

    // Step 7: Calculate wrist pitch.
    // The total arm angle in the vertical plane is Theta2 + (PI - Theta3).
    // Wrist pitch compensates to achieve desired end-effector pitch.
    float ArmAngleL = Theta2L + (M_PI - Theta3L);
    float Theta4L = PitchRadL - ArmAngleL + M_PI_2;

    // Step 8: Wrist roll is passed through directly.
    float Theta5L = RollRadL;

    // Step 9: Gripper is passed through as steps (no conversion).
    float Theta6L = 0.0f;  // Gripper angle not used for IK.

    // Store computed angles.
    lastAngles_[0] = Theta1L;
    lastAngles_[1] = Theta2L;
    lastAngles_[2] = Theta3L;
    lastAngles_[3] = Theta4L;
    lastAngles_[4] = Theta5L;
    lastAngles_[5] = Theta6L;

    // Step 10: Check joint limits.
    if (!checkLimits(lastAngles_))
    {
        lastError_ = IK_ERR_JOINT_LIMIT;
        return IK_ERR_JOINT_LIMIT;
    }

    // Step 11: Convert angles to steps.
    jointSteps[0] = (long)(Theta1L * stepsPerRad_[0]);
    jointSteps[1] = (long)(Theta2L * stepsPerRad_[1]);
    jointSteps[2] = (long)(Theta3L * stepsPerRad_[2]);
    jointSteps[3] = (long)(Theta4L * stepsPerRad_[3]);
    jointSteps[4] = (long)(Theta5L * stepsPerRad_[4]);
    jointSteps[5] = (long)(gripper);  // Gripper passed through directly as steps.

    lastError_ = IK_OK;
    return IK_OK;
}

bool RobotKinematics::isReachable(float x, float y, float z)
{
    float rL = sqrtf(x * x + y * y);
    float zRelL = z - IK_BASE_HEIGHT;
    float dL = sqrtf(rL * rL + zRelL * zRelL);

    return (dL <= IK_MAX_REACH && dL >= IK_MIN_REACH);
}

void RobotKinematics::getLastAngles(float angles[IK_NUM_JOINTS])
{
    for (int IndexL = 0; IndexL < IK_NUM_JOINTS; IndexL++)
    {
        angles[IndexL] = lastAngles_[IndexL];
    }
}

int RobotKinematics::getLastError()
{
    return lastError_;
}

#pragma endregion

#pragma region Private Methods

bool RobotKinematics::checkLimits(float angles[IK_NUM_JOINTS])
{
    // Check each joint against its limits.
    if (angles[0] < IK_J1_MIN || angles[0] > IK_J1_MAX) return false;
    if (angles[1] < IK_J2_MIN || angles[1] > IK_J2_MAX) return false;
    if (angles[2] < IK_J3_MIN || angles[2] > IK_J3_MAX) return false;
    if (angles[3] < IK_J4_MIN || angles[3] > IK_J4_MAX) return false;
    if (angles[4] < IK_J5_MIN || angles[4] > IK_J5_MAX) return false;
    // Gripper (angles[5]) is not checked as it's passed through directly.

    return true;
}

float RobotKinematics::clamp(float val, float minVal, float maxVal)
{
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

float RobotKinematics::normalizeAngle(float angle)
{
    while (angle > M_PI) angle -= 2.0f * M_PI;
    while (angle < -M_PI) angle += 2.0f * M_PI;
    return angle;
}

#pragma endregion
