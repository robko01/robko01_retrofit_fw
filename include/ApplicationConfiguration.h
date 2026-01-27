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


#ifndef _APPLICATIONCONFIGURATION_h
#define _APPLICATIONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif // defined(ARDUINO) && ARDUINO >= 100

#pragma region Options

// TODO: All the features can be made a CPU_FLASH flags.
// After reset the feature can be activated, with no recompilation.
// This feature does not require RAM.

// #define ENABLE_MOTORS_IO

// #define ENABLE_MOTORS

// #define ENABLE_LIMITS

// #define ENABLE_ESTOP

// #define ENABLE_FEATURES_FLAGS

// #define ENABLE_WIFI

// #define ENABLE_MDNS

// #define ENABLE_NTP

// #define ENABLE_WG

// #define ENABLE_OTA

// #define ENABLE_SUPER

// #define ENABLE_TCM_COMMANDS 1

// #define ENABLE_WDT

// #define ENABLE_STATUS_LCD

// #define ENABLE_SPI_IO

// #define ENABLE_SHMR

#define SHOW_FUNC_NAMES

#pragma endregion // Options



#pragma region IO Pins Definitions
#if defined(ENABLE_MOTORS_IO)

/**
 * @brief Motors enables pin.
 *
 */
#define PIN_ENABLE 2

/**
 * @brief Motor 1 direction pin.
 *
 */
#define PIN_DIR_1 4

/**
 * @brief Motor 1 steps pin.
 *
 */
#define PIN_STP_1 0

/**
 * @brief Motor 2 direction pin.
 *
 */
#define PIN_DIR_2 17

/**
 * @brief Motor 2 steps pin.
 *
 */

#define PIN_STP_2 16

/**
 * @brief Motor 3 direction pin.
 *
 */
#define PIN_DIR_3 12

/**
 * @brief Motor 3 steps pin.
 *
 */

#define PIN_STP_3 13

/**
 * @brief Motor 4 direction pin.
 *
 */
#define PIN_DIR_4 27

/**
 * @brief Motor 4 steps pin.
 *
 */

#define PIN_STP_4 14

/**
 * @brief Motor 5 direction pin.
 *
 */
#define PIN_DIR_5 25

/**
 * @brief Motor 5 steps pin.
 *
 */
#define PIN_STP_5 26

/**
 * @brief Motor 6 direction pin.
 *
 */
#define PIN_DIR_6 32

/**
 * @brief Motor 6 steps pin.
 *
 */
#define PIN_STP_6 33

#endif // defined(ENABLE_MOTORS_IO)
#if defined(ENABLE_LIMITS)

/**
 * @brief Motort 1 limit switch.
 *
 */
#define M1_LIMIT 34

/**
 * @brief Motort 2 limit switch.
 *
 */
#define M2_LIMIT 39

/**
 * @brief Motort 3 limit switch.
 *
 */
#define M3_LIMIT 36

/**
 * @brief Motort 6 limit switch.
 *
 */
#define M6_LIMIT 35
#endif // defined(ENABLE_LIMITS)
#if defined(ENABLE_ESTOP)
/**
 * @brief E-Stop pin definition.
 * 
 */
#define E_STOP 15
#endif // defined(ENABLE_ESTOP)
#pragma endregion

#pragma region Motors Parameters
#if defined(ENABLE_MOTORS)

#if !defined(DEFAULT_MAX_SPEED) 
#define DEFAULT_MAX_SPEED 100
#endif

#if !defined(DEFAULT_ACCEL) 
#define DEFAULT_ACCEL 75
#endif

#if !defined(M1_MAX_SPEED) 
#define M1_MAX_SPEED DEFAULT_MAX_SPEED
#endif

#if !defined(M2_MAX_SPEED) 
#define M2_MAX_SPEED DEFAULT_MAX_SPEED
#endif

#if !defined(M3_MAX_SPEED) 
#define M3_MAX_SPEED DEFAULT_MAX_SPEED
#endif

#if !defined(M4_MAX_SPEED) 
#define M4_MAX_SPEED DEFAULT_MAX_SPEED
#endif

#if !defined(M5_MAX_SPEED) 
#define M5_MAX_SPEED DEFAULT_MAX_SPEED
#endif

#if !defined(M6_MAX_SPEED) 
#define M6_MAX_SPEED DEFAULT_MAX_SPEED
#endif


#if !defined(M1_ACCEL) 
#define M1_ACCEL DEFAULT_ACCEL
#endif

#if !defined(M2_ACCEL) 
#define M2_ACCEL DEFAULT_ACCEL
#endif

#if !defined(M3_ACCEL) 
#define M3_ACCEL DEFAULT_ACCEL
#endif

#if !defined(M4_ACCEL) 
#define M4_ACCEL DEFAULT_ACCEL
#endif

#if !defined(M5_ACCEL) 
#define M5_ACCEL DEFAULT_ACCEL
#endif

#if !defined(M6_ACCEL) 
#define M6_ACCEL DEFAULT_ACCEL
#endif

#endif			  // defined(ENABLE_MOTORS)
#pragma endregion // Motors Parameters

#pragma region Limit Switches
#if defined(ENABLE_LIMITS) || defined(ENABLE_ESTOP)
/**
 * @brief Debounce time for limit switches inputs. [ms]
 *
 */
#define DEBOUNCE_TIME_MS 100
#endif

#if defined(ENABLE_LIMITS)
/**
 * @brief Fast forward steps per second to find the limit switch.
 * 
 */
#define FAST_FORWARD_SPS 50

/**
 * @brief Slow backward steps per second to release the limit switch.
 * 
 */
#define SLOW_BACKWARD_SPS 20

/**
 * @brief Enable find limit 1 flag.
 *
 */
#define ENABLE_LIMIT_1 1

/**
 * @brief Enable find limit 2 flag.
 *
 */
#define ENABLE_LIMIT_2 1

/**
 * @brief Enable find limit 3 flag.
 *
 */
#define ENABLE_LIMIT_3 1

/**
 * @brief Enable find limit 6 flag.
 *
 */
#define ENABLE_LIMIT_6 1

/**
 * @brief M1 timeout in miliseconds.
 *
 */
#define M1_TIMEOUT_MS 17000

/**
 * @brief M2 timeout in miliseconds.
 *
 */
#define M2_TIMEOUT_MS 17000

/**
 * @brief M3 timeout in miliseconds.
 *
 */
#define M3_TIMEOUT_MS 17000

/**
 * @brief M6 timeout in miliseconds.
 *
 */
#define M6_TIMEOUT_MS 17000

#endif			  // defined(ENABLE_LIMITS)
#pragma endregion // ENABLE_LIMITS

// SHMR configuration removed
#endif

/**
 * @brief MFeed NTP server.
 *
 */
#if !defined(NTP_1)
#define NTP_1 "ntp.jst.mfeed.ad.jp"
#endif

/**
 * @brief NICT NTP server.
 *
 */
#if !defined(NTP_2)
#define NTP_2 "ntp.nict.jp"
#endif

/**
 * @brief Google NTP server.
 *
 */
#if !defined(NTP_3)
#define NTP_3 "time.google.com"
#endif

#endif // ENABLE_NTP
#pragma endregion // NTP Client

#pragma region Wireguard
#if defined(ENABLE_WG)

/**
 * @brief [Peer] Endpoint port.
 *
 */
#if !defined(WG_PORT)
#define WG_PORT 13231
#endif

/**
 * @brief [Endpoint] IP address of the server.
 *
 */
#if !defined(WG_ENDPOINT)
#define WG_ENDPOINT "127.0.0.1"
#endif

/**
 * @brief [PrivateKey] Private key.
 *
 */
#if !defined(WG_PRIVATE_KEY)
#define WG_PRIVATE_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#endif

/**
 * @brief [PublicKey] Public key.
 *
 */
#if !defined(WG_PUBLIC_KEY)
#define WG_PUBLIC_KEY "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#endif

/**
 * @brief [Address] Local IP address.
 *
 */
#if !defined(WG_LOCAL_IP)
#define WG_LOCAL_IP "192.168.100.2"
#endif

#endif			  // defined(ENABLE_WG)
#pragma endregion // Wireguard

#pragma region mDNS
#pragma endregion // mDNS

#pragma region OTA
#if defined(ENABLE_OTA)

#if !defined(OTA_PORT)
#define OTA_PORT 3232
#endif

#if !defined(OTA_HOST_NAME)
#define OTA_HOST_NAME "ROBKO01"
#endif

#if !defined(OTA_PASS_HASH)
#define OTA_PASS_HASH "21232f297a57a5a743894a0e4a801fc3"
#endif

#endif // defined(ENABLE_OTA)
#pragma endregion

#pragma region SUPER
#if defined(ENABLE_SUPER)

#if !defined(SUPER_SERVICE_PORT)
#define SUPER_SERVICE_PORT 10182
#endif

// #define SUPER_TCP

#define SUPER_UDP

//#define SUPER_SERIAL

#endif // defined(ENABLE_SUPER)
#pragma endregion // SUPER

#pragma region TCM Commands
#if defined(ENABLE_TCM_COMMANDS)

#define DBG_OUTPUT_PORT_BAUDRATE 9600

#define ENABLE_ECHO

char KEY_CRLF[4];
char KEY_LFCR[4];
char KEY_LF[2];
char KEY_CR[2];

/**
 * @brief Backspace key code.
 * 
 */
const char KEY_BACKSPACE[1] = {0x7F};

/**
 * @brief Left key code.
 * 
 */
const char KEY_LEFT[3] = {0x1B, 0x5B, 0x44};


#if !defined(LINE_LENGTH)
/**
 * @brief Maximum line length.
 * 
 */
#define LINE_LENGTH 128
#endif // !defined(LINE_LENGTH)


#if !defined(CMDS_COUNT)
/**
 * @brief Number of commands.
 * 
 */
#define CMDS_COUNT 10
#endif // !defined(CMDS_COUNT)

/**
 * @brief Maximum number of arguments per command.
 * 
 */
#define ARGS_COUNT 9

/**
 * @brief Maximum ASCII bytes per name percommand.
 * 
 */
#define CMD_NAME_LENGTH 10

/**
 * @brief Maximum ASCII bytes per argument.
 * 
 */
#define ARGS_LENGTH 5

/**
 * @brief 
 * 
 */
#define CMD_CLOSE "@CLOSE"

/**
 * @brief 
 * 
 */
#define CMD_READ "@READ"

/**
 * @brief 
 * 
 */
#define CMD_RESET "@RESET"

/**
 * @brief 
 * 
 */
#define CMD_SET "@SET"

/**
 * @brief 
 * 
 */
#define CMD_STEP "@STEP"

/**
 * @brief 
 * 
 */
#define CMD_FREE "FREE"

/**
 * @brief 
 * 
 */
#define CMD_HOME "HOME"


/**
 * @brief 
 * 
 */
#define NO_ARGS ""

/**
 * @brief 
 * 
 */
#define SET_ARGS "d"

/**
 * @brief 
 * 
 */
#define STEP_ARGS "dddddddd"
#endif // defined(ENABLE_TCM_COMMANDS)
#pragma endregion // TCM Commands

#pragma region Watchdog Timer
#if defined(ENABLE_WDT)

#if !defined(WDT_UPDATE_INTERVAL)
#define WDT_UPDATE_INTERVAL 1
#endif

#if !defined(WDT_TIMEOUT)
#define WDT_TIMEOUT 3000
#endif

#endif			  // defined(ENABLE_WDT)
#pragma endregion // Watchdog Timer

#pragma region Status LCD
#if defined(ENABLE_STATUS_LCD)

/*
The driver is for PCF8574
Address space is the following:

A0 A1 A2
:  :  : - 0x27
|  :  : - 0x26
:  |  : - 0x25
|  |  : - 0x24
:  :  | - 0x23
|  :  | - 0x22
:  |  | - 0x21
|  |  | - 0x20

*/

#if !defined(LCD_ADDRESS)
/**
 * @brief Default LCD module I2C address.
 *
 */
#define LCD_ADDRESS 0x20
#endif // #if !defined(LCD_ADDRESS)

#if !defined(LCD_COLUMNS)
/**
 * @brief LCD width
 * 
 */
#define LCD_COLUMNS 16
#endif // #if !defined(LCD_COLUMNS)

#if !defined(LCD_ROWS)
/**
 * @brief LCD height
 * 
 */
#define LCD_ROWS 2
#endif // #if !defined(LCD_ROWS)

#if !defined(I2C_TIMEOUT_MS)
/**
 * @brief Default I2C device timeout.
 *
 */
#define I2C_TIMEOUT_MS 1000UL
#endif // #if !defined(I2C_TIMEOUT_MS)
#endif // defined(ENABLE_STATUS_LCD)
#pragma endregion

// SHMR configuration removed

#pragma region PS4
#if defined(ENABLE_PS4)

#if defined(ENABLE_PS4_DEBUG)
#define ENABLE_PS4_DEBUG
#endif

// #if defined(ENABLE_SLEEP_MODE)
#define ENABLE_SLEEP_MODE
// #endif

/**
 * @brief Host MAC.
 *
 */
#if !defined(PS4_MAC)
#define PS4_MAC "XX:XX:XX:XX:XX:XX"
#endif

#if !defined(BATT_MIN_LEVEL)
#define BATT_MIN_LEVEL 1
#endif

#if !defined(BATT_MAX_LEVEL)
#define BATT_MAX_LEVEL 8
#endif

/**
 * @brief Dead space in percentage..
 *
 */
#if !defined(X_MIN)
#define X_MIN -128
#endif

#if !defined(X_MAX)
#define X_MAX 127
#endif

#if !defined(Y_MIN)
#define Y_MIN -128
#endif

#if !defined(Y_MAX)
#define Y_MAX 127
#endif

/**
 * @brief Dead space in percentage..
 *
 */
#if !defined(DEAD_SPACE_LEFT_X)
#define DEAD_SPACE_LEFT_X 10
#endif

/**
 * @brief Dead space in percentage..
 *
 */
#if !defined(DEAD_SPACE_LEFT_Y)
#define DEAD_SPACE_LEFT_Y 10
#endif

/**
 * @brief Dead space in percentage..
 *
 */
#if !defined(DEAD_SPACE_RIGHT_X)
#define DEAD_SPACE_RIGHT_X 10
#endif

/**
 * @brief Dead space in percentage..
 *
 */
#if !defined(DEAD_SPACE_RIGHT_Y)
#define DEAD_SPACE_RIGHT_Y 10
#endif

#if !defined(PRC_MIN)
#define PRC_MIN -100
#endif

#if !defined(PRC_ZERO)
#define PRC_ZERO 0
#endif

#if !defined(PRC_MAX)
#define PRC_MAX 100
#endif

#if !defined(PS4_UPDATE_TIMER_TIME_MS)
#define PS4_UPDATE_TIMER_TIME_MS 100
#endif

#if defined(ENABLE_SLEEP_MODE)
#if !defined(PS4_SLEEP_COUNT)
/**
 * @brief 10 cycles pred second; each cycle is 100ms.
 * @note 100 cycles is 10 seconds.
 */
#define PS4_SLEEP_COUNT 100 
#endif
#endif // defined(ENABLE_SLEEP_MODE)

#endif // defined(ENABLE_PS4)
#pragma endregion // PS4

#endif // _APPLICATIONCONFIGURATION_h
