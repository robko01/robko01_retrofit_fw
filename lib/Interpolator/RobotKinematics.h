/**
 * @file RobotKinematics.h
 * @brief Inverse Kinematics module for Robko01 robot arm
 * @author Orlin Dimitrov
 * @version 1.0.0
 *
 * Geometric IK solver for 5-DOF articulated arm + gripper.
 * Converts Cartesian coordinates (X, Y, Z, Pitch, Roll) to joint angles.
 */

#ifndef ROBOT_KINEMATICS_H
#define ROBOT_KINEMATICS_H

#pragma region Headers

#include <Arduino.h>
#include <math.h>

#pragma endregion

#pragma region Robot Dimensions (mm)

/** @brief Base + torso height from ground to shoulder joint. */
#define IK_BASE_HEIGHT    200.0f

/** @brief Upper arm length (shoulder to elbow). */
#define IK_UPPER_ARM      177.8f

/** @brief Forearm length (elbow to wrist). */
#define IK_FOREARM        177.8f

/** @brief Maximum reach (sum of arm segments). */
#define IK_MAX_REACH      (IK_UPPER_ARM + IK_FOREARM)

/** @brief Minimum reach (difference of arm segments). */
#define IK_MIN_REACH      fabsf(IK_UPPER_ARM - IK_FOREARM)

#pragma endregion

#pragma region Joint Limits (radians)

/** @brief Base joint minimum angle. */
#define IK_J1_MIN  -1.5708f

/** @brief Base joint maximum angle. */
#define IK_J1_MAX   1.5708f

/** @brief Shoulder joint minimum angle. */
#define IK_J2_MIN  -0.8f

/** @brief Shoulder joint maximum angle. */
#define IK_J2_MAX   2.5f

/** @brief Elbow joint minimum angle. */
#define IK_J3_MIN  -2.5f

/** @brief Elbow joint maximum angle. */
#define IK_J3_MAX   2.5f

/** @brief Wrist pitch minimum angle. */
#define IK_J4_MIN  -3.1416f

/** @brief Wrist pitch maximum angle. */
#define IK_J4_MAX   3.1416f

/** @brief Wrist roll minimum angle. */
#define IK_J5_MIN  -1.5708f

/** @brief Wrist roll maximum angle. */
#define IK_J5_MAX   1.5708f

/** @brief Gripper minimum angle. */
#define IK_J6_MIN  -0.6f

/** @brief Gripper maximum angle. */
#define IK_J6_MAX   0.6f

#pragma endregion

#pragma region Constants

/** @brief Degrees to radians conversion factor. */
#define IK_DEG_TO_RAD  0.01745329252f

/** @brief Radians to degrees conversion factor. */
#define IK_RAD_TO_DEG  57.2957795131f

/** @brief Number of joints. */
#define IK_NUM_JOINTS  6

#pragma endregion

#pragma region IK Result Codes

/** @brief IK solution found successfully. */
#define IK_OK                 0

/** @brief Target is outside workspace. */
#define IK_ERR_UNREACHABLE    1

/** @brief Joint limit exceeded. */
#define IK_ERR_JOINT_LIMIT    2

/** @brief Invalid input parameters. */
#define IK_ERR_INVALID_INPUT  3

#pragma endregion

/**
 * @class RobotKinematics
 * @brief Inverse kinematics solver for Robko01 robot arm.
 *
 * Uses geometric approach to compute joint angles from Cartesian coordinates.
 * The arm has 5 DOF for positioning + 1 for gripper.
 */
class RobotKinematics
{
public:
    /**
     * @brief Initialize the kinematics solver.
     * @param stepsPerRad Array of 6 conversion factors (steps per radian for each joint).
     */
    void init(float stepsPerRad[IK_NUM_JOINTS]);

    /**
     * @brief Solve inverse kinematics.
     *
     * @param x Target X position in mm.
     * @param y Target Y position in mm.
     * @param z Target Z position in mm.
     * @param pitchDeg Desired end-effector pitch in degrees.
     * @param rollDeg Desired end-effector roll in degrees.
     * @param gripper Gripper position (passed through as steps).
     * @param jointSteps Output array of 6 joint positions in steps.
     * @return IK_OK if solution found, error code otherwise.
     */
    int solve(float x, float y, float z,
              float pitchDeg, float rollDeg, float gripper,
              long jointSteps[IK_NUM_JOINTS]);

    /**
     * @brief Check if a position is within the workspace.
     *
     * @param x Target X position in mm.
     * @param y Target Y position in mm.
     * @param z Target Z position in mm.
     * @return true if reachable, false otherwise.
     */
    bool isReachable(float x, float y, float z);

    /**
     * @brief Get the last computed joint angles.
     * @param angles Output array of 6 angles in radians.
     */
    void getLastAngles(float angles[IK_NUM_JOINTS]);

    /**
     * @brief Get the last error code.
     * @return Last error code from solve().
     */
    int getLastError();

private:
    /** @brief Steps per radian for each joint. */
    float stepsPerRad_[IK_NUM_JOINTS];

    /** @brief Last computed joint angles in radians. */
    float lastAngles_[IK_NUM_JOINTS];

    /** @brief Last error code. */
    int lastError_;

    /**
     * @brief Check if all joint angles are within limits.
     * @param angles Array of joint angles in radians.
     * @return true if all within limits, false otherwise.
     */
    bool checkLimits(float angles[IK_NUM_JOINTS]);

    /**
     * @brief Clamp a value to a range.
     * @param val Input value.
     * @param minVal Minimum value.
     * @param maxVal Maximum value.
     * @return Clamped value.
     */
    float clamp(float val, float minVal, float maxVal);

    /**
     * @brief Normalize angle to -PI to PI range.
     * @param angle Input angle in radians.
     * @return Normalized angle.
     */
    float normalizeAngle(float angle);
};

#endif // ROBOT_KINEMATICS_H
