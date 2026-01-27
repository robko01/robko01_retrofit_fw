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
 *  @brief Multi axis robot controller, for Robko01.
 *
 *  This file content a basic robot controller based on ESP32 adapted for Robko01 Io signals.
 *  The purpose of this software is to control Robko01 robot in multi axis mode Speed and Position.
 *
 *  @author Orlin Dimnitrov (orlin369)
 */

/*

+------+----------+-----------+
| Axis |   Sign   | Direction |
+------+----------+-----------+
|    0 | Positive | CCW       |
|    0 | Negative | CW        |
|    1 | Positive | Forward   |
|    1 | Negative | Backward  |
|    2 | Positive | Backward  |
|    2 | Negative | Forward   |
|    3 | Positive | Down      |
|    3 | Negative | Up        |
|    4 | Positive | CW        |
|    4 | Negative | CCW       |
|    5 | Positive | Open      |
|    5 | Negative | Close     |
+------+----------+-----------+

*/

#pragma region Headers

#include "ApplicationConfiguration.h"

#include "DebugPort.h"

#if defined(ENABLE_MOTORS)
#include <AccelStepper.h>
#include <MultiStepper.h>
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_ESTOP) || defined(ENABLE_LIMITS)
#include <Button2.h>
#endif // defined(ENABLE_ESTOP) || defined(ENABLE_LIMITS)



#if defined(ENABLE_WIFI)
#include <WiFi.h>
#include <WiFiUdp.h>
#endif // defined(ENABLE_WIFI)

#if defined(ENABLE_ETH)
#include <WiFi.h>
#include <ETH.h>
#endif // defined(ENABLE_ETH)

#if defined(ENABLE_ETH_ENC28J60)
#include <SPI.h>
#include <EthernetENC.h>
#endif // defined(ENABLE_ETH_ENC28J60)

#pragma region mDNS
#if defined(ENABLE_MDNS)
#include <ESPmDNS.h>
#endif // defined(ENABLE_MDNS)
#pragma endregion // mDNS

#if defined(ENABLE_OTA)
#include <ArduinoOTA.h>
#endif // defined(ENABLE_OTA)

#if defined(ENABLE_WG)
#include <WireGuard-ESP32.h>
#endif // defined(ENABLE_WG)

#if defined(ENABLE_SUPER)
#include "SUPER.h"
#include "OperationsCodes.h"
#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
#include <CommandParser.h>
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_MOTORS) || defined(ENABLE_SUPER) || defined(ENABLE_TCM_COMMANDS)
#include "JointPosition.h"
#include "JointPositionUnion.h"
#endif // defined(ENABLE_MOTORS) || defined(ENABLE_SUPER) || defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_WDT) || defined(ENABLE_PS4)
#include "FxTimer.h"
#endif // defined(ENABLE_WDT) || defined(ENABLE_PS4)

#if defined(ENABLE_STATUS_LCD)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "Wire.h"
#include <LiquidCrystal_I2C.h>
#endif // defined(ENABLE_STATUS_LCD)

#if defined(ENABLE_SPI_IO)
#include <SPI.h>
#endif // defined(ENABLE_SPI_IO)

#if defined(ENABLE_PS4)
#include <PS4Controller.h>
#endif // defined(ENABLE_PS4)

#pragma endregion // Headers

#pragma region Types
#if defined(ENABLE_TCM_COMMANDS)
/**
 * @brief Command parser type.
 *
 */
typedef CommandParser<CMDS_COUNT, ARGS_COUNT, CMD_NAME_LENGTH, ARGS_LENGTH> CommandParser_t;
#endif            // define(ENABLE_TCM_COMMANDS)
#pragma endregion // Types

#pragma region Enums

enum OperationModes : uint8_t
{
  NONE = 0U,
  Positioning,
  Speed,
};

#pragma endregion // Enums

#pragma region Prototypes

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties();

#if defined(ENABLE_MOTORS_IO)
/**
 * @brief Initialize the pins of the board.
 *
 */
void init_motors_pins();
#endif // defined(ENABLE_MOTORS_IO)

#if defined(ENABLE_MOTORS)
/**
 * @brief Initialize the stepper drivers.
 *
 */
void init_drivers();

/**
 * @brief Unable disable drivers.
 *
 */
void enable_drivers(bool state);

/**
 * @brief Update the stepper drivers.
 *
 */
void update_drivers();
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMITS)
/**
 * @brief Initialize the limit switches.
 *
 */
void init_limits();

/**
 * @brief Update limit switches input state.
 *
 */
void update_limits();

/**
 * @brief Find the limits of the axises.
 *
 */
void find_limits();

#endif // defined(ENABLE_LIMITS)

#if defined(ENABLE_ESTOP)
/**
 * @brief Initialize the E-Stop switch input.
 *
 */
void init_estop();

/**
 * @brief Update E-Stop switch input state.
 *
 */
void update_estop();
#endif // defined(ENABLE_ESTOP)

#if defined(ENABLE_WIFI)
/**
 * @brief Initialize the WiFi communication.
 *
 */
void init_wifi();

/**
 * @brief Update WiFi connection process.
 * 
 */
void update_wifi();
#endif // defined(ENABLE_WIFI)

#pragma region mDNS
#if defined(ENABLE_MDNS)
/**
 * @brief Set the up mDNS service.
 * 
 */
void init_mdns();

/**
 * @brief Update mDNS service.
 * 
 */
void update_mdns();
#endif // defined(ENABLE_MDNS)
#pragma endregion // mDNS

#if defined(ENABLE_NTP)
/**
 * @brief Initialize the NTP.
 *
 */
void init_ntp();
#endif // defined(ENABLE_NTP)

#if defined(ENABLE_WG)
/**
 * @brief Initialize the WireGuard channel.
 *
 */
void init_wg();
#endif // defined(ENABLE_WG)

#if defined(ENABLE_OTA)
/**
 * @brief Initialize the over the air updates.
 *
 */
void init_ota();
#endif // defined(ENABLE_OTA)

#if defined(ENABLE_SUPER)
/**
 * @brief Initialize the SUPER.
 *
 */
void init_super();

/**
 * @brief Update SUPER.
 *
 */
void update_super();

/**
 * @brief Callback handler function.
 *
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t *payload);
#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
/**
 * @brief Initialise the serial commands.
 *
 */
void init_tcm_commands();

/**
 * @brief Update the serial commands.
 *
 */
void update_tcm_commands();

/**
 * @brief 218 / 228 Set Command F.3 (FREE)
 *
 * @param args
 * @param response
 */
void cmd_free(CommandParser_t::Argument *args, char *response);

/**
 * @brief 220 / 228 Close Command F.6 (@CLOSE)
 *
 * @param args
 * @param response
 */
void cmd_close(CommandParser_t::Argument *args, char *response);

/**
 * @brief 220 / 228 Read Command F.6 (@READ)
 *
 * @param args
 * @param response
 */
void cmd_read(CommandParser_t::Argument *args, char *response);

/**
 * @brief 221 / 228 Reset Command F.6 (@RESET)
 *
 * @param args
 * @param response
 */
void cmd_reset(CommandParser_t::Argument *args, char *response);

/**
 * @brief 221 / 228 Set Command F.6 (@SET)
 *
 * @param args
 * @param response
 */
void cmd_set(CommandParser_t::Argument *args, char *response);

/**
 * @brief 221 / 228 Set Command F.6 (@STEP)
 *
 * @param args
 * @param response
 */
void cmd_step(CommandParser_t::Argument *args, char *response);
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_WDT)
/**
 * @brief Initialise the WDT.
 *
 */
void init_wdt();

/**
 * @brief Update WDT.
 *
 */
void update_wdt();

/**
 * @brief Feed the WDT.
 *
 */
void feed_wdt();

/**
 * @brief WDT expiration check function.
 *
 * @return true
 * @return false
 */
bool wdt_expired();
#endif // defined(ENABLE_WDT)

#if defined(ENABLE_STATUS_LCD)
bool is_i2c_dev_connected(uint8_t address, unsigned long timeout);

void init_lcd();

void check_lcd();

void draw_lcd();

void display_text_animation();

void task_lcd(void *parameter);
#endif // defined(ENABLE_STATUS_LCD)

// ENABLE_SHMR features removed

#if defined(ENABLE_PS4)
/**
 * @brief Initialize PS4 host controller.
 * 
 */
void init_ps4();

/**
 * @brief Update PS4 process.
 * 
 */
void update_ps4();
#endif // defined(ENABLE_PS4)

#pragma endregion // Prototypes

#pragma region Variables

#if defined(ENABLE_MOTORS) || defined(ENABLE_SUPER)
/**
 * @brief Motors enabled flag.
 *
 */
bool MotorsEnabled_g;

/**
 * @brief Motors state bits.
 *
 */
uint8_t MotorState_g;

/**
 * @brief Motors operation mode.
 *
 */
uint8_t OperationMode_g;

/**
 * @brief Safety stop flag.
 *
 */
int SafetyStopFlag_g;

/**
 * @brief Store position flag.
 *
 */
bool StorePosition_g;

/**
 * @brief Move joint in absolute mode.
 *
 */
JointPositionUnion MoveAbsolute_g;

/**
 * @brief Move joint in relative mode.
 *
 */
JointPositionUnion MoveRelative_g;

/**
 * @brief Move joint in speed mode.
 *
 */
JointPositionUnion MoveSpeed_g;

/**
 * @brief Curent joint positions.
 *
 */
JointPositionUnion CurrentPositions_g;

#endif // defined(ENABLE_MOTORS) || defined(ENABLE_SUPER)

#if defined(ENABLE_MOTORS)
/**
 * @brief TCP server for robot operation service.
 *
 */
AccelStepper stepper1;

/**
 * @brief Stepper driver instance for .
 *
 */
AccelStepper stepper2;

/**
 * @brief Stepper driver instance for .
 *
 */
AccelStepper stepper3;

/**
 * @brief Stepper driver instance for .
 *
 */
AccelStepper stepper4;

/**
 * @brief Stepper driver instance for .
 *
 */
AccelStepper stepper5;

/**
 * @brief Stepper driver instance for .
 *
 */
AccelStepper stepper6;

#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMITS)
/**
 * @brief Limit switch for M1.
 *
 */
Button2 M1LimitSwitch_g;

/**
 * @brief Limit switch for M2.
 *
 */
Button2 M2LimitSwitch_g;

/**
 * @brief Limit switch for M3.
 *
 */
Button2 M3LimitSwitch_g;

/**
 * @brief Limit switch for M6.
 *
 */
Button2 M6LimitSwitch_g;
#endif // defined(ENABLE_LIMITS)

#if defined(ENABLE_ESTOP)
/**
 * @brief Limit switch for M1.
 *
 */
Button2 EStopSwitch_g;
#endif // defined(ENABLE_ESTOP)

#if defined(ENABLE_ESTOP) || defined(ENABLE_LIMITS)
/**
 * @brief Limit switches states.
 *
 */
uint8_t InputsState_g;
#endif // defined(ENABLE_ESTOP) || defined(ENABLE_LIMITS)



#if defined(ENABLE_WIFI)

/**
 * @brief SSID
 *
 */
const char *SSID_g = WIFI_SSID;

/**
 * @brief Password
 *
 */
const char *PASS_g = WIFI_PASS;

#endif // defined(ENABLE_WIFI)

#if defined(ENABLE_WG)

/**
 * @brief [Interface] Address
 *
 */
IPAddress LocalIP_g;

/**
 * @brief WireGuard client.
 *
 */
static WireGuard WireGuardClient_g;

#endif // defined(ENABLE_WG)

#if defined(ENABLE_SUPER)

#if defined(SUPER_TCP)
/**
 * @brief TCP server for robot operation service.
 *
 */
WiFiServer TCPServer_g(SUPER_SERVICE_PORT);
#endif

#if defined(SUPER_UDP)
/**
 * @brief TCP server for robot operation service.
 *
 */
WiFiUDP UDPServer_g;
#endif

#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
/**
 * @brief Command parser instance.
 *
 */
CommandParser_t CommandParser_g;

/**
 * @brief Line to enter command.
 *
 */
char CommandLine_g[LINE_LENGTH];

/**
 * @brief Response line.
 *
 */
char Response_g[CommandParser_t::MAX_RESPONSE_SIZE];

/**
 * @brief 
 * 
 */
double MotorsSpeed_g;
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_WDT)
/**
 * @brief Watch dog timer.
 *
 */
FxTimer *WatchDogTimer_g;

/**
 * @brief Watch dog counter.
 *
 */
int WatchDogCounter_g;
#endif // defined(ENABLE_WDT)

#if defined(ENABLE_STATUS_LCD)
/**
 * @brief LCD driver instance.
 * 
 */
LiquidCrystal_I2C LCD_g(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

/**
 * @brief Tracks the LCD connection state.
 * 
 */
bool LCDConnState_g = false;

/**
 * @brief Display data state.
 * 
 */
int DisplayDataState_g = 0;

/**
 * @brief Update progress.
 * 
 */
unsigned int UpdateProgress_g;

/**
 * @brief Update total.
 * 
 */
unsigned int UpdateTotal_g;

/**
 * @brief Update message.
 * 
 */
char LCDFirstLine_g[LCD_COLUMNS];

/**
 * @brief Update message.
 * 
 */
char LCDSecondLine_g[LCD_COLUMNS];


#endif // defined(ENABLE_STATUS_LCD)

// SHMR global declarations removed

#if defined(ENABLE_PS4)
/**
 * @brief PS4 update timer.
 *
 */
FxTimer *PS4UpdateTimer_g;

/**
 * @brief PS4 time to update flag.
 * 
 */
bool PS4TimeToUpdate_g;

#if defined(ENABLE_SLEEP_MODE)
/**
 * @brief PS4 sleep counter.
 * 
 */
uint32_t PS4SleepCounter_g;
#endif // defined(ENABLE_SLEEP_MODE)
#endif // defined(ENABLE_PS4)

#pragma endregion // Variables

/**
 * @brief Setup the peripheral hardware and variables.
 *
 */
void setup()
{
  //
  setup_debug_port(DBG_OUTPUT_PORT_BAUDRATE);

#if defined(ENABLE_STATUS_LCD)
  // Init the display.
  init_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  //
  show_device_properties();

#if defined(ENABLE_MOTORS_IO)
  init_motors_pins();
#endif // defined(ENABLE_MOTORS_IO)

#if defined(ENABLE_MOTORS)
  init_drivers();
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMITS)
  init_limits();
  // find_limits();
#endif // defined(ENABLE_LIMITS)

#if defined(ENABLE_ESTOP)
  init_estop();
#endif // defined(ENABLE_ESTOP)



#if defined(ENABLE_WIFI)
  init_wifi();
#endif // defined(ENABLE_WIFI)

#if defined(ENABLE_MDNS)
  init_mdns();
#endif // defined(ENABLE_MDNS)

#if defined(ENABLE_NTP)
  init_ntp();
#endif // defined(ENABLE_NTP)

#if defined(ENABLE_WG)
  init_wg();
#endif // defined(ENABLE_WG)

#if defined(ENABLE_OTA)
  init_ota();
#endif // defined(ENABLE_OTA)

#if defined(ENABLE_SUPER)
  init_super();
#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
  init_tcm_commands();
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_WDT)
  init_wdt();
#endif // defined(ENABLE_WDT)

#if defined(ENABLE_SPI_IO)
  SPI.begin(); // Begins the SPI communication

  SPI.setClockDivider(SPI_CLOCK_DIV8); // Sets clock for SPI communication at 8 (16/8=2Mhz)

  digitalWrite(SS, HIGH); // Setting SlaveSelect as HIGH (So master does not connect with slave)
#endif // defined(ENABLE_SPI_IO)

// SHMR initialization removed

#if defined(ENABLE_PS4)
  init_ps4();
#endif // defined(ENABLE_PS4)

#if defined(ENABLE_STATUS_LCD)
  // Create the LCD task. 
  xTaskCreate(task_lcd, "task_lcd", 2048, NULL, 2, NULL);
#endif // defined(ENABLE_STATUS_LCD)
}

/**
 * @brief Main loop of the program.
 *
 */
void loop()
{

#if defined(ENABLE_ESTOP)
  update_estop();
#endif // defined(ENABLE_ESTOP)

#if defined(ENABLE_LIMITS)
  update_limits();
#endif // defined(ENABLE_LIMITS)

#if defined(ENABLE_WDT)
  update_wdt();
#endif // defined(ENABLE_WDT)

#if defined(ENABLE_WIFI)
  // update_wifi();
#endif // defined(ENABLE_WIFI)

#if defined(ENABLE_SUPER)
  update_super();
#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
  update_tcm_commands();
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_PS4)
  update_ps4();
#endif // defined(ENABLE_PS4)

#if defined(ENABLE_OTA)
  ArduinoOTA.handle();
#endif // defined(ENABLE_OTA)

#if defined(ENABLE_MOTORS)
  if (SafetyStopFlag_g == LOW)
  {
    // Robko01.update();
    // MotorState_g = Robko01.get_motor_state();
    update_drivers();
  }

  if (MotorState_g == 0 && StorePosition_g)
  {
    StorePosition_g = false;
    // TODO: Save position.
  }
  else
  {
    StorePosition_g = true;
  }
#endif // defined(ENABLE_MOTORS)

// SHMR runtime runner removed

#if defined(ENABLE_STATUS_LCD)
#endif // defined(ENABLE_STATUS_LCD)
}

#pragma region Functions

/** @brief Printout in the debug console flash state.
 *  @return Void.
 */
void show_device_properties()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ESP8266)
  // ESP8266
  DEBUGLOG("Flash chip size: %u\r\n", ESP.getFlashChipRealSize());
#endif

  DEBUGLOG("Sketch size: %u\r\n", ESP.getSketchSize());
  DEBUGLOG("Free flash space: %u\r\n", ESP.getFreeSketchSpace());
  DEBUGLOG("Free heap: %d\r\n", ESP.getFreeHeap());
  DEBUGLOG("SDK version: %s\r\n", ESP.getSdkVersion());
  DEBUGLOG("Build name: %s\r\n", BUILD_NAME);
  DEBUGLOG("Build version: %s\r\n", BUILD_VERSION);
#if defined(ENABLE_WIFI)
  DEBUGLOG("MAC address: %s\r\n", WiFi.macAddress().c_str());
#endif // defined(ENABLE_WIFI)
  DEBUGLOG("\r\n");
}

#if defined(ENABLE_MOTORS_IO)
/**
 * @brief Initialize the pins of the board.
 *
 */
void init_motors_pins()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  // Set the pin direction.
  pinMode(PIN_ENABLE, OUTPUT);
  // Disable all drivers.
  digitalWrite(PIN_ENABLE, HIGH);

  //
  pinMode(PIN_DIR_1, OUTPUT);
  //
  digitalWrite(PIN_DIR_1, HIGH);
  //
  pinMode(PIN_STP_1, OUTPUT);
  //
  digitalWrite(PIN_STP_1, HIGH);

  //
  pinMode(PIN_DIR_2, OUTPUT);
  //
  digitalWrite(PIN_DIR_2, HIGH);
  //
  pinMode(PIN_STP_2, OUTPUT);
  //
  digitalWrite(PIN_STP_2, HIGH);

  //
  pinMode(PIN_DIR_3, OUTPUT);
  //
  digitalWrite(PIN_DIR_3, HIGH);
  //
  pinMode(PIN_STP_3, OUTPUT);
  //
  digitalWrite(PIN_STP_3, HIGH);

  //
  pinMode(PIN_DIR_4, OUTPUT);
  //
  digitalWrite(PIN_DIR_4, HIGH);
  //
  pinMode(PIN_STP_4, OUTPUT);
  //
  digitalWrite(PIN_STP_4, HIGH);

  //
  pinMode(PIN_DIR_5, OUTPUT);
  //
  digitalWrite(PIN_DIR_5, HIGH);
  //
  pinMode(PIN_STP_5, OUTPUT);
  //
  digitalWrite(PIN_STP_5, HIGH);

  //
  pinMode(PIN_DIR_6, OUTPUT);
  //
  digitalWrite(PIN_DIR_6, HIGH);
  //
  pinMode(PIN_STP_6, OUTPUT);
  //
  digitalWrite(PIN_STP_6, HIGH);
}
#endif // defined(ENABLE_MOTORS_IO)

#if defined(ENABLE_MOTORS)
/**
 * @brief Initialize the stepper drivers.
 *
 */
void init_drivers()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  // Toggle the safety because there is noe E-Stop at this time.
  SafetyStopFlag_g = LOW;

  // Store position flag initialization.
  StorePosition_g = false;

  // Init the motors state.
  MotorState_g = 0;

  // Init the steppers operation mode.
  OperationMode_g = OperationModes::NONE;

  stepper1 = AccelStepper(AccelStepper::DRIVER, PIN_STP_1, PIN_DIR_1);
  stepper2 = AccelStepper(AccelStepper::DRIVER, PIN_STP_2, PIN_DIR_2);
  stepper3 = AccelStepper(AccelStepper::DRIVER, PIN_STP_3, PIN_DIR_3);
  stepper4 = AccelStepper(AccelStepper::DRIVER, PIN_STP_4, PIN_DIR_4);
  stepper5 = AccelStepper(AccelStepper::DRIVER, PIN_STP_5, PIN_DIR_5);
  stepper6 = AccelStepper(AccelStepper::DRIVER, PIN_STP_6, PIN_DIR_6);

  stepper1.setAcceleration(M1_ACCEL);
  stepper2.setAcceleration(M2_ACCEL);
  stepper3.setAcceleration(M3_ACCEL);
  stepper4.setAcceleration(M4_ACCEL);
  stepper5.setAcceleration(M5_ACCEL);
  stepper6.setAcceleration(M6_ACCEL);

  stepper1.setMaxSpeed(M1_MAX_SPEED);
  stepper2.setMaxSpeed(M2_MAX_SPEED);
  stepper3.setMaxSpeed(M3_MAX_SPEED);
  stepper4.setMaxSpeed(M4_MAX_SPEED);
  stepper5.setMaxSpeed(M5_MAX_SPEED);
  stepper6.setMaxSpeed(M6_MAX_SPEED);

  // Set directions.
  stepper3.setPinsInverted(true, false, false);
  stepper5.setPinsInverted(true, false, false);
  stepper6.setPinsInverted(true, false, false);

// SHMR stepper assignments removed
}

/**
 * @brief Unable disable drivers.
 *
 */
void enable_drivers(bool state)
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  if (state)
  {
#if defined(ENABLE_MOTORS_IO)
    // Enable stepper drivers.
    digitalWrite(PIN_ENABLE, LOW);
#endif // #if defined(ENABLE_MOTORS_IO)
#if defined(ENABLE_MOTORS)
    stepper1.enableOutputs();
    stepper2.enableOutputs();
    stepper3.enableOutputs();
    stepper4.enableOutputs();
    stepper5.enableOutputs();
    stepper6.enableOutputs();
#endif // defined(ENABLE_MOTORS)
  }
  else
  {
#if defined(ENABLE_MOTORS_IO)
    // Disable stepper drivers.
    digitalWrite(PIN_ENABLE, HIGH);
#endif // #if defined(ENABLE_MOTORS_IO)

#if defined(ENABLE_MOTORS)
    stepper1.disableOutputs();
    stepper2.disableOutputs();
    stepper3.disableOutputs();
    stepper4.disableOutputs();
    stepper5.disableOutputs();
    stepper6.disableOutputs();

    stepper1.setSpeed(0);
    stepper2.setSpeed(0);
    stepper3.setSpeed(0);
    stepper4.setSpeed(0);
    stepper5.setSpeed(0);
    stepper6.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
  }

  MotorsEnabled_g = state;
}

/**
 * @brief Update the stepper drivers.
 *
 */
void update_drivers()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

  static bool state = false;
  if (OperationMode_g == OperationModes::Positioning)
  {
    state = stepper1.run();
    bitWrite(MotorState_g, 0, state);
    state = stepper2.run();
    bitWrite(MotorState_g, 1, state);
    state = stepper3.run();
    bitWrite(MotorState_g, 2, state);
    state = stepper4.run();
    bitWrite(MotorState_g, 3, state);
    state = stepper5.run();
    bitWrite(MotorState_g, 4, state);
    state = stepper6.run();
    bitWrite(MotorState_g, 5, state);
    // DEBUGLOG("%d, %d, %d, %d, %d, %d\r\n",
    //   stepper1.currentPosition(),
    //   stepper2.currentPosition(),
    //   stepper3.currentPosition(),
    //   stepper4.currentPosition(),
    //   stepper5.currentPosition(),
    //   stepper6.currentPosition());
  }
  else if (OperationMode_g == OperationModes::Speed)
  {
    state = stepper1.runSpeed();
    bitWrite(MotorState_g, 0, state);
    state = stepper2.runSpeed();
    bitWrite(MotorState_g, 1, state);
    state = stepper3.runSpeed();
    bitWrite(MotorState_g, 2, state);
    state = stepper4.runSpeed();
    bitWrite(MotorState_g, 3, state);
    state = stepper5.runSpeed();
    bitWrite(MotorState_g, 4, state);
    state = stepper6.runSpeed();
    bitWrite(MotorState_g, 5, state);
    // DEBUGLOG("MotorState_g: %d\r\n", MotorState_g);
  }
}
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMITS)
/**
 * @brief Initialize the limit switches.
 *
 */
void init_limits()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  //
  pinMode(M1_LIMIT, INPUT_PULLUP);
  //
  pinMode(M2_LIMIT, INPUT_PULLUP);
  //
  pinMode(M3_LIMIT, INPUT_PULLUP);
  //
  pinMode(M6_LIMIT, INPUT_PULLUP);

  M1LimitSwitch_g.begin(M1_LIMIT);
  M1LimitSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
  M2LimitSwitch_g.begin(M2_LIMIT);
  M2LimitSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
  M3LimitSwitch_g.begin(M3_LIMIT);
  M3LimitSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
  M6LimitSwitch_g.begin(M6_LIMIT);
  M6LimitSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
}

/**
 * @brief Update limit switches input state.
 *
 */
void update_limits()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

  M1LimitSwitch_g.loop();
  M2LimitSwitch_g.loop();
  M3LimitSwitch_g.loop();
  M6LimitSwitch_g.loop();

  bitWrite(InputsState_g, 4, M1LimitSwitch_g.isPressed());
  bitWrite(InputsState_g, 5, M2LimitSwitch_g.isPressed());
  bitWrite(InputsState_g, 6, M3LimitSwitch_g.isPressed());
  bitWrite(InputsState_g, 7, M6LimitSwitch_g.isPressed());
}

/**
 * @brief Find the limits of the axises.
 *
 */
void find_limits()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableLimits_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

#if defined(ENABLE_LIMIT_1) || \
    defined(ENABLE_LIMIT_2) || \
    defined(ENABLE_LIMIT_3) || \
    defined(ENABLE_LIMIT_6)
  unsigned long t0 = 0;
  unsigned long t1 = 0;
  unsigned long dt = 0;
  bool timeout_flag = false;
#endif

#if defined(ENABLE_MOTORS)
  enable_drivers(true);
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMIT_1)

#if defined(ENABLE_MOTORS)
  // Set stepper direction and speed.
  stepper1.setSpeed(-FAST_FORWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // Begin time.
  t0 = millis();

  // This is in case the switch is already hit.
  M1LimitSwitch_g.loop();
  while (!M1LimitSwitch_g.isPressed())
  {
    // Read the limit switch state.
    M1LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper1.runSpeed();
#endif // defined(ENABLE_MOTORS)

    // Update the clock for the timeout flag.
    t1 = millis();
    dt = t1 - t0;
    if (dt >= M2_TIMEOUT_MS)
    {
      timeout_flag = true;
      break;
    }
  }

  // Do something with the flag.
  if (timeout_flag)
  {
    DEBUGLOG("Overdue time for reaching position on axis Base\r\n");
#if defined(ENABLE_MOTORS)
    enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    return;
  }

#if defined(ENABLE_MOTORS)
  // Flip stepper direction and speed.
  stepper1.setSpeed(SLOW_BACKWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // This is in case the switch is already hit.
  M1LimitSwitch_g.loop();
  while (M1LimitSwitch_g.isPressed() == 1)
  {
    // Read the limit switch state.
    M1LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper1.runSpeed();
#endif // defined(ENABLE_MOTORS)
  }

#if defined(ENABLE_MOTORS)
  // Clear software way the homed position.
  stepper1.setCurrentPosition(0);
#endif // defined(ENABLE_MOTORS)
#endif // defined(ENABLE_LIMIT_1)

#if defined(ENABLE_LIMIT_2)
#if defined(ENABLE_MOTORS)
  // Set stepper direction and speed.
  stepper2.setSpeed(FAST_FORWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // Begin time.
  t0 = millis();

  // This is in case the switch is already hit.
  M2LimitSwitch_g.loop();
  while (!M2LimitSwitch_g.isPressed())
  {
    // Read the limit switch state.
    M2LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper2.runSpeed();
#endif // defined(ENABLE_MOTORS)

    // Update the clock for the timeout flag.
    t1 = millis();
    dt = t1 - t0;
    if (dt >= M2_TIMEOUT_MS)
    {
      timeout_flag = true;
      break;
    }
  }

  // Do something with the flag.
  if (timeout_flag)
  {
    DEBUGLOG("Overdue time for reaching position on axis Shoulder\r\n");
#if defined(ENABLE_MOTORS)
    enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    return;
  }

#if defined(ENABLE_MOTORS)
  // Flip stepper direction and speed.
  stepper2.setSpeed(-SLOW_BACKWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // This is in case the switch is already hit.
  M2LimitSwitch_g.loop();
  while (M2LimitSwitch_g.isPressed() == 1)
  {
    // Read the limit switch state.
    M2LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper2.runSpeed();
#endif // defined(ENABLE_MOTORS)
  }

#if defined(ENABLE_MOTORS)
  // Clear software way the homed position.
  stepper2.setCurrentPosition(0);
#endif // defined(ENABLE_MOTORS)
#endif // defined(ENABLE_LIMIT_2)

#if defined(ENABLE_LIMIT_3)
#if defined(ENABLE_MOTORS)
  // Set stepper direction and speed.
  stepper3.setSpeed(FAST_FORWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // This is in case the switch is already hit.
  M3LimitSwitch_g.loop();
  while (M3LimitSwitch_g.isPressed())
  {
    // Read the limit switch state.
    M3LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper3.runSpeed();
#endif // defined(ENABLE_MOTORS)
  }

#if defined(ENABLE_MOTORS)
  // Flip stepper direction and speed.
  stepper3.setSpeed(-SLOW_BACKWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // Begin time.
  t0 = millis();

  // This is in case the switch is already hit.
  M3LimitSwitch_g.loop();
  while (!M3LimitSwitch_g.isPressed() == 1)
  {
    // Read the limit switch state.
    M3LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper3.runSpeed();
#endif // defined(ENABLE_MOTORS)

    // Update the clock for the timeout flag.
    t1 = millis();
    dt = t1 - t0;
    if (dt >= M3_TIMEOUT_MS)
    {
      timeout_flag = true;
      break;
    }
  }

  // Do something with the flag.
  if (timeout_flag)
  {
    DEBUGLOG("Overdue time for reaching position on axis Elbow\r\n");
#if defined(ENABLE_MOTORS)
    enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    return;
  }

#if defined(ENABLE_MOTORS)
  // Clear software way the homed position.
  stepper3.setCurrentPosition(0);
#endif // defined(ENABLE_MOTORS)
#endif // defined(ENABLE_LIMIT_2)

#if defined(ENABLE_LIMIT_6)
#if defined(ENABLE_MOTORS)
  // Set stepper direction and speed.
  stepper6.setSpeed(-FAST_FORWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // Begin time.
  t0 = millis();

  // This is in case the switch is already hit.
  M6LimitSwitch_g.loop();
  while (M6LimitSwitch_g.isPressed())
  {
    // Read the limit switch state.
    M6LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper6.runSpeed();
#endif // defined(ENABLE_MOTORS)

    // Update the clock for the timeout flag.
    t1 = millis();
    dt = t1 - t0;
    if (dt >= M6_TIMEOUT_MS)
    {
      timeout_flag = true;
      break;
    }
  }

  // Do something with the flag.
  if (timeout_flag)
  {
    DEBUGLOG("Overdue time for reaching position on axis Gripper\r\n");
#if defined(ENABLE_MOTORS)
    enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    return;
  }

#if defined(ENABLE_MOTORS)
  // Flip stepper direction and speed.
  stepper6.setSpeed(SLOW_BACKWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // This is in case the switch is already hit.
  M6LimitSwitch_g.loop();
  while (!M6LimitSwitch_g.isPressed() == 1)
  {
    // Read the limit switch state.
    M6LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper6.runSpeed();
#endif // defined(ENABLE_MOTORS)
  }
#if defined(ENABLE_MOTORS)
  // Clear software way the homed position.
  stepper6.setCurrentPosition(0);

  // Move to standard opening.
  stepper6.setSpeed(40);
  stepper6.runToNewPosition(300);
#endif // defined(ENABLE_MOTORS)
#endif // defined(ENABLE_LIMIT_6)

#if defined(ENABLE_MOTORS)
  enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
}
#endif // defined(ENABLE_LIMITS)

#if defined(ENABLE_ESTOP)
/**
 * @brief Initialize the E-Stop switch input.
 *
 */
void init_estop()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableEStop_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)
  //
  // pinMode(E_STOP, INPUT_PULLUP);

  EStopSwitch_g.begin(E_STOP, INPUT_PULLUP);
  EStopSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
  EStopSwitch_g.setPressedHandler([](Button2& btn) {
#if defined(ENABLE_LIMITS)
    bitWrite(InputsState_g, 3, EStopSwitch_g.isPressed());
    DEBUGLOG("E-STOP Released!\r\n");
#endif
  });
    EStopSwitch_g.setReleasedHandler([](Button2& btn) {
#if defined(ENABLE_LIMITS)
    bitWrite(InputsState_g, 3, EStopSwitch_g.isPressed());
    DEBUGLOG("E-STOP Pressed!\r\n");
#endif
  });
}

/**
 * @brief Update E-Stop switch input state.
 *
 */
void update_estop()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableEStop_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)
  EStopSwitch_g.loop();
}
#endif // defined(ENABLE_ESTOP)

#if defined(ENABLE_WIFI)
/**
 * @brief Initialize the WiFi communication.
 *
 */
void init_wifi()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWifiInterface_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_g, PASS_g);
  WiFi.setHostname(WIFI_HOST_NAME);
  DEBUGLOG("Connecting:  %s\r\n", SSID_g);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DEBUGLOG(".");
  }
  DEBUGLOG("\r\n");
  DEBUGLOG("Connected:   %s\r\n", SSID_g);
  DEBUGLOG("RSSI:        %d\r\n", WiFi.RSSI());
  DEBUGLOG("IP Address:  %s\r\n", WiFi.localIP().toString().c_str());
  DEBUGLOG("Gateway:     %s\r\n", WiFi.gatewayIP().toString().c_str());
  DEBUGLOG("DNS:         %s\r\n", WiFi.dnsIP().toString().c_str());
}

/**
 * @brief Update WiFi connection process.
 * 
 */
void update_wifi()
{
  static unsigned long CurrMillisL;
  static unsigned long PrevMillisL;
  static bool RestartServices = false;
  CurrMillisL = millis();
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (CurrMillisL - PrevMillisL >= WIFI_RECONNECT_TIME_MS))
  {
    DEBUGLOG("Reconnecting to WiFi... %d\r\n", millis());
    init_wifi();
    // WiFi.reconnect();
    // Restart services.
    RestartServices = true;
    // Update last time.
    PrevMillisL = CurrMillisL;
  }

  if (RestartServices == true)
  {
#if defined(ENABLE_NTP)
    // init_ntp();
#endif // defined(ENABLE_NTP)
#if defined(ENABLE_WG)
    // init_wg();
#endif // defined(ENABLE_WG)
#if defined(ENABLE_OTA)
    // init_ota();
#endif // defined(ENABLE_OTA)
#if defined(ENABLE_SUPER)
    // init_super();
#endif // defined(ENABLE_SUPER)
    // Services wer restated.
    RestartServices = false;
  }
}
#endif // defined(ENABLE_WIFI)

#if defined(ENABLE_MDNS)
/**
 * @brief Set the up mDNS service.
 * 
 */
void init_mdns()
{
#if defined(SHOW_FUNC_NAME)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES
  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp32.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp32")) {
    DEBUGLOG("Error setting up MDNS responder!\r\n");
    while (1) {
      delay(1000);
    }
  }
  DEBUGLOG("mDNS responder started\r\n");

  // Add service to MDNS-SD
  // MDNS.addService("http", "tcp", 80);
}

/**
 * @brief Update mDNS service.
 * 
 */
void update_mdns()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

}
#endif // defined(ENABLE_MDNS)

#if defined(ENABLE_NTP)
/**
 * @brief Initialize the NTP.
 *
 */
void init_ntp()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableNTP_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  DEBUGLOG("Adjusting system time...\r\n");
  // Config time.
  configTime(NTP_TIMEZONE * 3600, 0, NTP_1, NTP_2, NTP_3);
  DEBUGLOG("System time adjusted...\r\n");
}
#endif // defined(ENABLE_NTP)

#if defined(ENABLE_WG)
/**
 * @brief Initialize the WireGuard channel.
 *
 */
void init_wg()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWG_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  DEBUGLOG("Initializing WireGuard...\r\n");
  if (!LocalIP_g.fromString(WG_LOCAL_IP))
  {
    DEBUGLOG("Invalid Wire Guard local IP address %s\r\n", WG_LOCAL_IP);
    for (;;)
    {
    }
  }
  WireGuardClient_g.begin(
      LocalIP_g,
      WG_PRIVATE_KEY,
      WG_ENDPOINT,
      WG_PUBLIC_KEY,
      WG_PORT);
  DEBUGLOG("WireGuard client started.\r\n");
}
#endif // defined(ENABLE_WG)

#if defined(ENABLE_OTA)
/**
 * @brief Initialize the over the air updates.
 *
 */
void init_ota()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableOTA_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  // Port defaults to 3232
  ArduinoOTA.setPort(OTA_PORT);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(OTA_HOST_NAME);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash(OTA_PASS_HASH);

  ArduinoOTA
      .onStart([]()
      {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
          type = "sketch";
        }
        else
        {  // U_SPIFFS
          type = "filesystem";
        }
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        DEBUGLOG("Start updating %s\r\n", type);
#if defined(ENABLE_STATUS_LCD)
        DisplayDataState_g = 1;
#endif // defined(ENABLE_STATUS_LCD)
      })
      .onEnd([]()
      {
        DEBUGLOG("End\r\n");
#if defined(ENABLE_STATUS_LCD)
        DisplayDataState_g = 2;
#endif // defined(ENABLE_STATUS_LCD)
      })
      .onProgress([](unsigned int progress, unsigned int total)
      {
        DEBUGLOG("Progress: %u%%\r\n", (progress / (total / 100)));
#if defined(ENABLE_STATUS_LCD)
        UpdateProgress_g = progress;
        UpdateTotal_g = total;
        DisplayDataState_g = 3;
#endif // defined(ENABLE_STATUS_LCD)
      })
      .onError([](ota_error_t error)
      {
        DEBUGLOG("Error[%u]: \r\n", error);
        if (error == OTA_AUTH_ERROR)
        {
          DEBUGLOG("Auth Failed\r\n");
        }
        else if (error == OTA_BEGIN_ERROR)
        {
          DEBUGLOG("Begin Failed\r\n");
        }
        else if (error == OTA_CONNECT_ERROR)
        {
          DEBUGLOG("Connect Failed\r\n");
        }
        else if (error == OTA_RECEIVE_ERROR)
        {
          DEBUGLOG("Receive Failed\r\n");
        }
        else if (error == OTA_END_ERROR)
        {
          DEBUGLOG("End Failed\r\n");
        }
      });

  ArduinoOTA.begin();
}
#endif // defined(ENABLE_OTA)

#if defined(ENABLE_SUPER)
/**
 * @brief Initialize the communication.
 *
 */
void init_super()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableSUPER_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  // Initialize the SUPER protocol parser.
  SUPER.setCbRequest(cbRequestHandler);

#if defined(SUPER_TCP)
  // Start the server.
  TCPServer_g.begin();
#endif
#if defined(SUPER_UDP)
  // Start the server.
  UDPServer_g.begin(SUPER_SERVICE_PORT);
  SUPER.init(UDPServer_g);
#endif
}

/**
 * @brief Update communication.
 *
 */
void update_super()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableSUPER_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(SUPER_TCP)
  static uint8_t StateL = 0;

  static WiFiClient ClientL;

  if (StateL == 0)
  {
    // Listen for incoming clients.
    ClientL = TCPServer_g.accept();
    if (ClientL)
    {
      SUPER.init(ClientL);
      StateL = 1;
      DEBUGLOG("Connected: %s\r\n", ClientL.remoteIP().toString().c_str());
#if defined(ENABLE_MOTORS_IO)
      enable_drivers(true);
#endif // ENABLE_MOTORS_IO
#if defined(ENABLE_WDT)
      feed_wdt();
#endif // ENABLE_WDT
    }
  }
  if (StateL == 1)
  {
    if (ClientL.connected())
    {
      SUPER.update();
#if defined(ENABLE_WDT)
      if (wdt_expired())
      {
        StateL = 2;
        DEBUGLOG("WDT EXPIRED...\r\n");
      }
#endif // ENABLE_WDT
    }
    else
    {
      StateL = 2;
    }
  }
  if (StateL == 2)
  {
    ClientL.stop();
    StateL = 0;
    DEBUGLOG("Disconnected: %s\r\n", ClientL.remoteIP().toString().c_str());
#if defined(ENABLE_MOTORS_IO)
    enable_drivers(false);
#endif // ENABLE_MOTORS_IO
  }
#endif // SUPER_TCP

#if defined(SUPER_UDP)
  if (UDPServer_g.parsePacket())
  {
#if defined(ENABLE_WDT)
    feed_wdt();
#endif // ENABLE_WDT
    UDPServer_g.beginPacket(UDPServer_g.remoteIP(), UDPServer_g.remotePort());
    SUPER.update();
    UDPServer_g.endPacket();
    UDPServer_g.flush();
  }
#if defined(ENABLE_WDT)
  if (wdt_expired())
  {
#if defined(ENABLE_MOTORS)
    if (MotorsEnabled_g == true)
    {
      DEBUGLOG("WDT EXPIRED...\r\n");
      enable_drivers(false);
    }
#endif // ENABLE_MOTORS
  }
  else
  {
#if defined(ENABLE_MOTORS)
    if (MotorsEnabled_g == false)
    {
      DEBUGLOG("WDT RESET BY NEW UDP PACKAGE...\r\n");
      enable_drivers(true);
    }
#endif // ENABLE_MOTORS
  }
#endif // ENABLE_WDT
#endif // SUPER_UDP
}

/**
 * @brief Callback handler function.
 *
 * @param opcode Operation code of the call.
 * @param size Size of the payload.
 * @param payload Payload data.
 */
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t *payload)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableSUPER_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  if (opcode == OpCodes::Ping)
  {
#if defined(ENABLE_WDT)
    feed_wdt();
#endif // defined(ENABLE_WDT)
    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  }
  else if (opcode == OpCodes::Stop)
  {
#if defined(ENABLE_MOTORS)
    // Robko01.stop_motors();
    stepper1.stop();
    stepper2.stop();
    stepper3.stop();
    stepper4.stop();
    stepper5.stop();
    stepper6.stop();
#endif // SHOW_FUNC_NAMES
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::Disable)
  {
#if defined(ENABLE_MOTORS)
    // Robko01.disable_motors();
    enable_drivers(false);
#endif // SHOW_FUNC_NAMES
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::Enable)
  {
#if defined(ENABLE_MOTORS)
    // Robko01.enable_motors();
    enable_drivers(true);
#endif // SHOW_FUNC_NAMES
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::Clear)
  {
#if defined(ENABLE_MOTORS)
    // Robko01.clear_motors();
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);
    stepper3.setCurrentPosition(0);
    stepper4.setCurrentPosition(0);
    stepper5.setCurrentPosition(0);
    stepper6.setCurrentPosition(0);
#endif // SHOW_FUNC_NAMES
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::MoveRelative)
  {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false)
    {
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
      return;
    }
    // If it is move, do not execute the command.
    if (MotorState_g != 0)
    {
      uint8_t m_payloadResponse[1] = {MotorState_g};
      SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);
      return;
    }

    // TODO: Move to function.
    size_t DataLengthL = sizeof(JointPosition_t);
    for (uint8_t index = 0; index < DataLengthL; index++)
    {
      MoveRelative_g.Buffer[index] = payload[index];
    }
#if defined(ENABLE_MOTORS)
    // Set motion data.
    // Robko01.move_relative(MoveRelative_g.Value);
    OperationMode_g = OperationModes::Positioning;

    stepper1.setSpeed(MoveRelative_g.Value.BaseSpeed);
    stepper1.move(MoveRelative_g.Value.BasePos);
    stepper2.setSpeed(MoveRelative_g.Value.ShoulderSpeed);
    stepper2.move(MoveRelative_g.Value.ShoulderPos);
    stepper3.setSpeed(MoveRelative_g.Value.ElbowSpeed);
    stepper3.move(MoveRelative_g.Value.ElbowPos);
    stepper4.setSpeed(MoveRelative_g.Value.LeftDiffSpeed);
    stepper4.move(MoveRelative_g.Value.LeftDiffPos);
    stepper5.setSpeed(MoveRelative_g.Value.RightDiffSpeed);
    stepper5.move(MoveRelative_g.Value.RightDiffPos);
    stepper6.setSpeed(MoveRelative_g.Value.GripperSpeed);
    stepper6.move(MoveRelative_g.Value.GripperPos);
#endif // SHOW_FUNC_NAMES
    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::MoveAbsolute)
  {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false)
    {
      // Respond with error.
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);

      // Exit
      return;
    }
    // If it is move, do not execute the command.
    if (MotorState_g != 0)
    {
      // Respond with busy.
      uint8_t m_payloadResponse[1];
      m_payloadResponse[0] = MotorState_g;
      SUPER.send_raw_response(opcode, StatusCodes::Busy, m_payloadResponse, 1);

      // Exit
      return;
    }

    // Extract motion data.
    size_t DataLengthL = sizeof(JointPosition_t);
    for (uint8_t index = 0; index < DataLengthL; index++)
    {
      MoveAbsolute_g.Buffer[index] = payload[index];
    }
#if defined(ENABLE_MOTORS)
    // Set motion data.
    // Robko01.move_absolute(MoveAbsolute_g.Value);
    OperationMode_g = OperationModes::Positioning;
    if (stepper1.currentPosition() != MoveAbsolute_g.Value.BasePos)
    {
      stepper1.setSpeed(MoveAbsolute_g.Value.BaseSpeed);
      stepper1.moveTo(MoveAbsolute_g.Value.BasePos);  
    }
    if (stepper2.currentPosition() != MoveAbsolute_g.Value.ShoulderPos)
    {
      stepper2.setSpeed(MoveAbsolute_g.Value.ShoulderSpeed);
      stepper2.moveTo(MoveAbsolute_g.Value.ShoulderPos); 
    }
    if (stepper3.currentPosition() != MoveAbsolute_g.Value.ElbowPos)
    {
      stepper3.setSpeed(MoveAbsolute_g.Value.ElbowSpeed);
      stepper3.moveTo(MoveAbsolute_g.Value.ElbowPos);
    }
    if (stepper4.currentPosition() != MoveAbsolute_g.Value.LeftDiffPos)
    {
      stepper4.setSpeed(MoveAbsolute_g.Value.LeftDiffSpeed);
      stepper4.moveTo(MoveAbsolute_g.Value.LeftDiffPos);
    }
    if (stepper5.currentPosition() != MoveAbsolute_g.Value.RightDiffPos)
    {
      stepper5.setSpeed(MoveAbsolute_g.Value.RightDiffSpeed);
      stepper5.moveTo(MoveAbsolute_g.Value.RightDiffPos);
    }
    if (stepper6.currentPosition() != MoveAbsolute_g.Value.GripperPos)
    {
      stepper6.setSpeed(MoveAbsolute_g.Value.GripperSpeed);
      stepper6.moveTo(MoveAbsolute_g.Value.GripperPos);
    }
#endif // SHOW_FUNC_NAMES
    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::DO)
  {
#if defined(ENABLE_SPI_IO)
    // Set port A.
    // Robko01.set_port_a(payload[0]);

    static byte MasterSendL, MasterReceiveL;

    MasterSendL = payload[0];

    // Send the mastersend value to slave also receives value from slave.
    MasterReceiveL = SPI.transfer(MasterSendL);
#endif // defined(ENABLE_SPI_IO)
    static uint8_t dos = 0;

    dos = payload[0];

    DEBUGLOG("DOs: %d\r\n", dos);

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, 1);
  }
  else if (opcode == OpCodes::DI)
  {
    static uint8_t m_payloadResponse[1];

#if defined(ENABLE_LIMITS)
    m_payloadResponse[0] = InputsState_g; // Robko01.get_port_a();
#endif // ENABLE_LIMITS

#if defined(ENABLE_WDT)
    feed_wdt();
#endif // ENABLE_WDT

    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
  }
  else if (opcode == OpCodes::IsMoving)
  {
    uint8_t m_payloadResponse[1];
    m_payloadResponse[0] = MotorState_g;
    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, m_payloadResponse, 1);
  }
  else if (opcode == OpCodes::CurrentPosition)
  {
#if defined(ENABLE_MOTORS)
    // CurrentPositions_g.Value = Robko01.get_position();
    CurrentPositions_g.Value.BasePos = (int16_t)stepper1.currentPosition();
    CurrentPositions_g.Value.BaseSpeed = (int16_t)stepper1.speed();
    CurrentPositions_g.Value.ShoulderPos = (int16_t)stepper2.currentPosition();
    CurrentPositions_g.Value.ShoulderSpeed = (int16_t)stepper2.speed();
    CurrentPositions_g.Value.ElbowPos = (int16_t)stepper3.currentPosition();
    CurrentPositions_g.Value.ElbowSpeed = (int16_t)stepper3.speed();
    CurrentPositions_g.Value.LeftDiffPos = (int16_t)stepper4.currentPosition();
    CurrentPositions_g.Value.LeftDiffSpeed = (int16_t)stepper4.speed();
    CurrentPositions_g.Value.RightDiffPos = (int16_t)stepper5.currentPosition();
    CurrentPositions_g.Value.RightDiffSpeed = (int16_t)stepper5.speed();
    CurrentPositions_g.Value.GripperPos = (int16_t)stepper6.currentPosition();
    CurrentPositions_g.Value.GripperSpeed = (int16_t)stepper6.speed();
#endif // SHOW_FUNC_NAMES
#if defined(ENABLE_WDT)
    feed_wdt();
#endif // ENABLE_WDT
    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, CurrentPositions_g.Buffer, sizeof(JointPosition_t));
  }
  else if (opcode == OpCodes::MoveSpeed)
  {
    // If it is not enabled, do not execute.
    if (MotorsEnabled_g == false)
    {
      SUPER.send_raw_response(opcode, StatusCodes::Error, NULL, 0);
      return;
    }

    for (uint8_t index = 0; index < size; index++)
    {
      MoveSpeed_g.Buffer[index] = payload[index];
    }
#if defined(ENABLE_MOTORS)
    // Set motion data.
    // Robko01.move_speed(MoveSpeed_g.Value);
    OperationMode_g = OperationModes::Speed;

    stepper1.setSpeed(MoveSpeed_g.Value.BaseSpeed);
    stepper2.setSpeed(MoveSpeed_g.Value.ShoulderSpeed);
    stepper3.setSpeed(MoveSpeed_g.Value.ElbowSpeed);
    stepper4.setSpeed(MoveSpeed_g.Value.LeftDiffSpeed);
    stepper5.setSpeed(MoveSpeed_g.Value.RightDiffSpeed);
    stepper6.setSpeed(MoveSpeed_g.Value.GripperSpeed);
#endif // defined(ENABLE_MOTORS)
    // Respond with success.
    SUPER.send_raw_response(opcode, StatusCodes::Ok, NULL, 0);
  }
  else if (opcode == OpCodes::SetRobotID)
  {
    // TODO: Write to I2C EEPROM.
    // for (uint8_t index = 0; index < DataLengthL; index++)
    //{
    //	Motion.Buffer[index] = m_payloadRequest[index];
    //}

    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  }
  else if (opcode == OpCodes::GetRobotID)
  {
    // TODO: Read from I2C EEPROM.
    // for (uint8_t index = 0; index < DataLengthL; index++)
    //{
    //	m_payloadRequest[index] = Motion.Buffer[index];
    //}

    SUPER.send_raw_response(opcode, StatusCodes::Ok, payload, size - 1);
  }
// SHMR SUPER opcodes removed
  else
  {
    DEBUGLOG("Unknown operation code: %d\r\n", opcode);
  }
}
#endif // defined(ENABLE_SUPER)

#if defined(ENABLE_TCM_COMMANDS)
/**
 * @brief Initialise the serial commands.
 *
 */
void init_tcm_commands()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  strcpy(KEY_CRLF,"\r\n");
  strcpy(KEY_LFCR,"\n\r");
  strcpy(KEY_LF,"\n");
  strcpy(KEY_CR,"\r");

  CommandParser_g.registerCommand(CMD_FREE, NO_ARGS, &cmd_free);
  CommandParser_g.registerCommand(CMD_CLOSE, NO_ARGS, &cmd_close);
  CommandParser_g.registerCommand(CMD_READ, NO_ARGS, &cmd_read);
  CommandParser_g.registerCommand(CMD_RESET, NO_ARGS, &cmd_reset);
  CommandParser_g.registerCommand(CMD_SET, SET_ARGS, &cmd_set);
  CommandParser_g.registerCommand(CMD_STEP, STEP_ARGS, &cmd_step);
}

/**
 * @brief Update the serial commands.
 *
 */
void update_tcm_commands()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  // DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  static char RecvCharL;
  static size_t CharIndexL;
  static size_t LineLengthL;

  if (Serial.available())
  {
    // Read line.
    // LineLengthL = Serial.readBytesUntil(KEY_RETURN, CommandLine_g, LINE_LENGTH-1);

    RecvCharL = Serial.read(); // Read the incoming character

    // Add character to the command line.
    CommandLine_g[CharIndexL++] = RecvCharL;

#if defined(ENABLE_ECHO)
    // Echo the received character
    Serial.print(RecvCharL);
#endif // defined(ENABLE_ECHO)

    if((RecvCharL==KEY_CR[0]) || (RecvCharL==KEY_LF[0]))
    {
      LineLengthL = CharIndexL;
      // DEBUGLOG("LineLengthL %d; CharIndexL %d\r\n", LineLengthL, CharIndexL);
    }

    // for (int index = 0; index < CharIndexL; index++)
    // {
    //   Serial.print(CommandLine_g[index]);
    // }

    // If left key is pressed.
    if (strcmp(CommandLine_g, KEY_LEFT) == 0)
    {
      // DEBUGLOG("\r");
      CharIndexL--;
    }

    // If backspace key pressed.
    if (RecvCharL == KEY_BACKSPACE[0])
    {
      DEBUGLOG("\b");
      CharIndexL--;
      return;
    }

    // Add the character to the buffer if there's space
    if (CharIndexL >= LINE_LENGTH)
    {
      DEBUGLOG("\r\nMaximum line length excide! \r\n");
      // 
      RecvCharL = '\0';

      // Clear the line.
      for (size_t index = 0; index < LINE_LENGTH; index++)
      {
        CommandLine_g[index] = '\0';
      }

      // Clear buffer on overflow.
      CharIndexL = 0;

      // Clear line length.
      LineLengthL = 0;
    }

    // 
    // DEBUGLOG("LineLengthL %d; CharIndexL %d\r\n", LineLengthL, CharIndexL);

    if (LineLengthL == CharIndexL)
    {
      // 2025.01.09 -> Fix the dialect problems due to comand parser disabletie to resolve arguments delimiter specific symbol.
      for (size_t index = 0; index < LineLengthL; index++)
      {
        if (CommandLine_g[index] == ',')
        {
          CommandLine_g[index] = ' ';
        }
        if (CommandLine_g[index] == '\r')
        {
          CommandLine_g[index] = '\0';
        }
        if (CommandLine_g[index] == '\n')
        {
          CommandLine_g[index] = '\0';
        }
      }

      // Terminate string.
      // Minus one comes from <CR> char at the end of the line.
      CommandLine_g[LineLengthL - 1] = '\0';

      // Parse the command.
      CommandParser_g.processCommand(CommandLine_g, Response_g);

      // Clear the line.
      for (size_t index = 0; index < LINE_LENGTH; index++)
      {
        CommandLine_g[index] = '\0';
      }

      // Clear buffer.
      CharIndexL = 0;

      // Clear line length.
      LineLengthL = 0;

      // 
      DEBUGLOG("\r\nResponse: %s\r\n", Response_g);
    }
  }

#if defined(ENABLE_WDT)
  if (wdt_expired())
  {
#if defined(ENABLE_MOTORS)
    if (MotorsEnabled_g == true)
    {
      DEBUGLOG("WDT EXPIRED...\r\n");
      enable_drivers(false);
    }
#endif // ENABLE_MOTORS
  }
  else
  {
#if defined(ENABLE_MOTORS)
    if (MotorsEnabled_g == false)
    {
      DEBUGLOG("WDT RESET BY NEW TCM PACKAGE...\r\n");
      enable_drivers(true);
    }
#endif // ENABLE_MOTORS
  }
#endif // ENABLE_WDT
}

/**
 * @brief 218 / 228 Set Command F.3 (FREE)
 *
 * @param args
 * @param response
 */
void cmd_free(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_MOTORS)
  enable_drivers(false);
#endif // defined(ENABLE_MOTORS)

  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\nOK\r\n");
}

/**
 * @brief 220 / 228 Close Command F.6 (@CLOSE)
 *
 * @param args
 * @param response
 */
void cmd_close(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_LIMIT_1) || \
    defined(ENABLE_LIMIT_2) || \
    defined(ENABLE_LIMIT_3) || \
    defined(ENABLE_LIMIT_6)
  unsigned long t0 = 0;
  unsigned long t1 = 0;
  unsigned long dt = 0;
  bool timeout_flag = false;
#endif

#if defined(ENABLE_MOTORS)
  enable_drivers(true);
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMIT_6)
#if defined(ENABLE_MOTORS)
  // Set stepper direction and speed.
  stepper6.setSpeed(-FAST_FORWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // Begin time.
  t0 = millis();

  // This is in case the switch is already hit.
  M6LimitSwitch_g.loop();
  while (M6LimitSwitch_g.isPressed())
  {
    // Read the limit switch state.
    M6LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper6.runSpeed();
#endif // defined(ENABLE_MOTORS)

    // Update the clock for the timeout flag.
    t1 = millis();
    dt = t1 - t0;
    if (dt >= M6_TIMEOUT_MS)
    {
      timeout_flag = true;
      break;
    }
  }

  // Do something with the flag.
  if (timeout_flag)
  {
    DEBUGLOG("Overdue time for reaching position on axis Gripper\r\n");
#if defined(ENABLE_MOTORS)
    enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    return;
  }

#if defined(ENABLE_MOTORS)
  // Flip stepper direction and speed.
  stepper6.setSpeed(SLOW_BACKWARD_SPS);
#endif // defined(ENABLE_MOTORS)

  // This is in case the switch is already hit.
  M6LimitSwitch_g.loop();
  while (!M6LimitSwitch_g.isPressed() == 1)
  {
    // Read the limit switch state.
    M6LimitSwitch_g.loop();

#if defined(ENABLE_MOTORS)
    // Run the stepper motor.
    stepper6.runSpeed();
#endif // defined(ENABLE_MOTORS)
  }
#endif // defined(ENABLE_LIMIT_6)

  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\nOK\r\n");
}

/**
 * @brief 220 / 228 Read Command F.6 (@READ)
 *
 * @param args
 * @param response
 */
void cmd_read(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  static int16_t CurPos1 = 0;
  static int16_t CurPos2 = 0;
  static int16_t CurPos3 = 0;
  static int16_t CurPos4 = 0;
  static int16_t CurPos5 = 0;
  static int16_t CurPos6 = 0;
  static uint16_t LimitSwitchesStateL = 0;

#if defined(ENABLE_MOTORS)
  CurPos1 = (int16_t)stepper1.currentPosition(),
  CurPos2 = (int16_t)stepper2.currentPosition(),
  CurPos3 = (int16_t)stepper3.currentPosition(),
  CurPos4 = (int16_t)stepper4.currentPosition(),
  CurPos5 = (int16_t)stepper5.currentPosition(),
  CurPos6 = (int16_t)stepper6.currentPosition(),
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_LIMITS)
  LimitSwitchesStateL = InputsState_g;
#endif // ENABLE_LIMITS

  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\n%d, %d, %d, %d, %d, %d, %d\r\n",
           CurPos1,
           CurPos2,
           CurPos3,
           CurPos4,
           CurPos5,
           CurPos6,
           LimitSwitchesStateL);
}

/**
 * @brief 221 / 228 Reset Command F.6 (@RESET)
 *
 * @param args
 * @param response
 */
void cmd_reset(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_MOTORS)
  // Clear software way the homed position.
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  stepper3.setCurrentPosition(0);
  stepper4.setCurrentPosition(0);
  stepper5.setCurrentPosition(0);
  stepper6.setCurrentPosition(0);
#endif // defined(ENABLE_MOTORS)

  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\nOK\r\n");
}

/**
 * @brief 221 / 228 Set Command F.6 (@SET)
 *
 * @param args
 * @param response
 */
void cmd_set(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  // DEBUGLOG("Set the robot\r\n");
  MotorsSpeed_g = args[0].asDouble;
  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\nOK\r\n");
}

/**
 * @brief 221 / 228 Set Command F.6 (@STEP)
 *
 * @param args
 * @param response
 */
void cmd_step(CommandParser_t::Argument *args, char *response)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif // SHOW_FUNC_NAMES

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableTCM_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  // DEBUGLOG("@STEP ");
  // DEBUGLOG("D: %d ", (int32_t)args[0].asDouble);
  // DEBUGLOG("J1: %d ", (int32_t)args[1].asDouble);
  // DEBUGLOG("J2: %d ", (int32_t)args[2].asDouble);
  // DEBUGLOG("J3: %d ", (int32_t)args[3].asDouble);
  // DEBUGLOG("J4: %d ", (int32_t)args[4].asDouble);
  // DEBUGLOG("J5: %d ", (int32_t)args[5].asDouble);
  // DEBUGLOG("J6: %d ", (int32_t)args[6].asDouble);
  MotorsSpeed_g = args[0].asDouble;

#if defined(ENABLE_MOTORS)
  stepper1.setSpeed(MotorsSpeed_g);
  stepper2.setSpeed(MotorsSpeed_g);
  stepper3.setSpeed(MotorsSpeed_g);
  stepper4.setSpeed(MotorsSpeed_g);
  stepper5.setSpeed(MotorsSpeed_g);
  stepper6.setSpeed(MotorsSpeed_g);

  stepper1.moveTo(args[1].asDouble);
  stepper2.moveTo(args[2].asDouble);
  stepper3.moveTo(args[3].asDouble);
  stepper4.moveTo(args[4].asDouble);
  stepper5.moveTo(args[5].asDouble);
  stepper6.moveTo(args[6].asDouble);

  OperationMode_g = OperationModes::Positioning;

  enable_drivers(true);
#endif // defined(ENABLE_MOTORS)

#if defined(ENABLE_WDT)
    feed_wdt();
#endif // ENABLE_WDT

  snprintf(response,
           CommandParser_t::MAX_RESPONSE_SIZE,
           "\r\nOK\r\n");
  // DEBUGLOG("DOs %d\r\n", (int32_t)args[7].asDouble);
}
#endif // defined(ENABLE_TCM_COMMANDS)

#if defined(ENABLE_WDT)
/**
 * @brief Initialise the WDT.
 *
 */
void init_wdt()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWDT_g)
{
  // Print cancel execution message.
  DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

#if defined(ENABLE_STATUS_LCD)
  sprintf(LCDFirstLine_g, __FUNCTION__);
  draw_lcd();
#endif // defined(ENABLE_STATUS_LCD)

  // Setup the WDT.
  WatchDogTimer_g = new FxTimer();
  WatchDogTimer_g->setExpirationTime(WDT_UPDATE_INTERVAL);
  WatchDogTimer_g->updateLastTime();

  // Feed the watchdog  timer.
  WatchDogCounter_g = WDT_TIMEOUT;
}

/**
 * @brief Update WDT.
 *
 */
void update_wdt()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWDT_g)
{
  // Print cancel execution message.
  // DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  WatchDogTimer_g->update();
  if (WatchDogTimer_g->expired())
  {
    WatchDogTimer_g->updateLastTime();
    WatchDogTimer_g->clear();
    if (WatchDogCounter_g > 0)
    {
      WatchDogCounter_g--;
    }
  }
}

/**
 * @brief Feed the WDT.
 *
 */
void feed_wdt()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWDT_g)
{
  // Print cancel execution message.
  // DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  WatchDogCounter_g = WDT_TIMEOUT;
}

/**
 * @brief WDT expiration check function.
 *
 * @return true
 * @return false
 */
bool wdt_expired()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

#if defined(ENABLE_FEATURES_FLAGS)
// If the flag is false.
if (!EnableWDT_g)
{
  // Print cancel execution message.
  // DEBUGLOG("Cancel execution: %s\r\n", __PRETTY_FUNCTION__);
  // Exit from the function.
  return;
}
#endif // defined(ENABLE_FEATURES_FLAGS)

  return (bool)(WatchDogCounter_g <= 0);
}
#endif // defined(ENABLE_WDT)

#if defined(ENABLE_STATUS_LCD)
bool is_i2c_dev_connected(uint8_t address, unsigned long timeout)
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif
    static unsigned long startTime = 0;
    
    startTime = millis();
    while (millis() - startTime < timeout)
    {
        Wire.beginTransmission(address);
        uint8_t error = Wire.endTransmission();
        if (error == 0)
        {
            return true; // Device is connected
        }
    }
    return false; // Timeout or no response
}

void init_lcd()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif
    Wire.begin();
    LCD_g.begin(LCD_COLUMNS, LCD_ROWS);

    // Check if the LCD is connected
    LCDConnState_g = is_i2c_dev_connected(LCD_ADDRESS, I2C_TIMEOUT_MS);

    if (LCDConnState_g)
    {
        DEBUGLOG("LCD connected. Starting animations...\r\n");
        LCD_g.backlight();
    }
    else
    {
        DEBUGLOG("LCD not detected. Please check the connection.\r\n");
    }
}

void check_lcd()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

    // Check the connection status of the LCD
    bool currentState = is_i2c_dev_connected(LCD_ADDRESS, I2C_TIMEOUT_MS);
    if (currentState != LCDConnState_g)
    {
        LCDConnState_g = currentState;
        if (LCDConnState_g)
        {
            DEBUGLOG("LCD reconnected.\r\n");
            LCD_g.begin(LCD_COLUMNS, LCD_ROWS);
            LCD_g.backlight();
        }
        else
        {
            DEBUGLOG("LCD disconnected!\r\n");
        }
    }
}

void draw_lcd()
{
  LCD_g.clear();
  LCD_g.setCursor(0, 0);
  LCD_g.print(LCDFirstLine_g);
  LCD_g.setCursor(0, 1);
  LCD_g.print(LCDSecondLine_g);
}

void display_text_animation()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif
    LCD_g.clear();

    // Slide-in animation
    String message = "BIG KYP";
    for (int index = 0; index <= LCD_COLUMNS - message.length(); index++)
    {
        LCD_g.setCursor(index, 0);
        LCD_g.print(message);
        vTaskDelay(500);
        LCD_g.clear();
    }

    // Blink animation
    for (int index = 0; index < 3; index++)
    {
        LCD_g.setCursor(0, 0);
        LCD_g.print(message);
        vTaskDelay(700);
        LCD_g.clear();
        vTaskDelay(700);
    }

    // Static display
    LCD_g.setCursor(0, 0);
    LCD_g.print(message);
}

void task_lcd(void *parameter)
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif

  for(;;)
  {
    // Update the display.
    check_lcd();
    if (LCDConnState_g)
    {
      if (DisplayDataState_g == 0)
      {
        // Perform animations if LCD is connected
        display_text_animation();
      }
      // Start
      else if (DisplayDataState_g == 1)
      {
        sprintf(LCDFirstLine_g, "Start update...");
        draw_lcd();
        vTaskDelay(500);
      }
      // End
      else if (DisplayDataState_g == 2)
      {
        sprintf(LCDFirstLine_g, "End update...");
        draw_lcd();
        vTaskDelay(5000);
        DisplayDataState_g == 0;
      }
      // Start
      else if (DisplayDataState_g == 3)
      {
        sprintf(LCDFirstLine_g, "Progress: %u%%", (UpdateProgress_g / (UpdateTotal_g / 100)));
        draw_lcd();
        vTaskDelay(1000);
      }
      else if (DisplayDataState_g == 4)
      {
        draw_lcd();
        DisplayDataState_g = 0;
        vTaskDelay(1000);
      }
      else
      {
        // nothing to change
      }
    }
  }
}

#endif // defined(ENABLE_STATUS_LCD)

// SHMR implementations removed

#if defined(ENABLE_PS4)

// #error PS4_MAC
#if "E8:61:7E:40:63:18" = PS4_MAC
#error "Unsupported board"
#endif
/**
 * @brief Initialize PS4 host controller.
 * 
 */
void init_ps4()
{
#if defined(SHOW_FUNC_NAMES)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif
  DEBUGLOG("PS4_MAC: %s\r\n", PS4_MAC);
  if (!PS4.begin(PS4_MAC))
  {
    DEBUGLOG("Can not run PS4 control module.\r\n");
    for(;;){}
  }

  // Setup the WDT.
  PS4UpdateTimer_g = new FxTimer();
  PS4UpdateTimer_g->setExpirationTime(PS4_UPDATE_TIMER_TIME_MS);
  PS4UpdateTimer_g->updateLastTime();

  // Clear the flag.
  PS4TimeToUpdate_g = false;
#if defined(ENABLE_SLEEP_MODE)
  // Feed the timer.
  PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif
#if defined(ENABLE_MOTORS)
  // Set operation mode.
  OperationMode_g = OperationModes::Speed;
#endif // defined(ENABLE_MOTORS)

  DEBUGLOG("PS4 Ready...\r\n");
}

/**
 * @brief Update PS4 process.
 * 
 */
void update_ps4()
{
#if defined(SHOW_FUNC_NAMES_S)
  DEBUGLOG("\r\n");
  DEBUGLOG(__PRETTY_FUNCTION__);
  DEBUGLOG("\r\n");
#endif
  static long BattLevelL;
  static long BaseSpeedL;
  static long ShoulderSpeedL;
  static long ElbowSpeedL;
  static long PL, RL, LDL, RDL;
  static long GripperSpeedL;

  // Update the PS4 timer.
  PS4UpdateTimer_g->update();
  if (PS4UpdateTimer_g->expired())
  {
    PS4UpdateTimer_g->updateLastTime();
    PS4UpdateTimer_g->clear();
    // Set the flag.
    if (PS4TimeToUpdate_g == false)
    {
      PS4TimeToUpdate_g = true;
    }

#if defined(ENABLE_SLEEP_MODE)
    if (PS4SleepCounter_g <= 0)
    {
#if defined(ENABLE_MOTORS)
      // DEBUGLOG("Sleep time has come.\r\n");
      if (MotorsEnabled_g)
      {
        enable_drivers(false);
      }
#endif // defined(ENABLE_MOTORS)
    }
    else
    {
      // DEBUGLOG("Coming... %d\r\n", PS4SleepCounter_g);
      PS4SleepCounter_g -= 1;
    }
#endif // defined(ENABLE_SLEEP_MODE)
  }

  // Below has all accessible outputs from the controller
  if (PS4.isConnected() && PS4TimeToUpdate_g)
  {
    // Clear the flag.
    if (PS4TimeToUpdate_g == true)
    {
      PS4TimeToUpdate_g = false;
    }

#if defined(ENABLE_PS4_DEBUG)
    if (PS4.Right()) Serial.println("Right Button");
    if (PS4.Down()) Serial.println("Down Button");
    if (PS4.Up()) Serial.println("Up Button");
    if (PS4.Left()) Serial.println("Left Button");

    if (PS4.Square()) Serial.println("Square Button");
    if (PS4.Cross()) Serial.println("Cross Button");
    if (PS4.Circle()) Serial.println("Circle Button");
    if (PS4.Triangle()) Serial.println("Triangle Button");

    if (PS4.UpRight()) Serial.println("Up Right");
    if (PS4.DownRight()) Serial.println("Down Right");
    if (PS4.UpLeft()) Serial.println("Up Left");
    if (PS4.DownLeft()) Serial.println("Down Left");

    if (PS4.L1()) Serial.println("L1 Button");
    if (PS4.R1()) Serial.println("R1 Button");

    if (PS4.Share()) Serial.println("Share Button");
    if (PS4.Options()) Serial.println("Options Button");
    if (PS4.L3()) Serial.println("L3 Button");
    if (PS4.R3()) Serial.println("R3 Button");

    if (PS4.PSButton()) Serial.println("PS Button");
    if (PS4.Touchpad()) Serial.println("Touch Pad Button");

    if (PS4.L2()) {
      DEBUGLOG("L2 button at %d\n", PS4.L2Value());
    }
    if (PS4.R2()) {
      DEBUGLOG("R2 button at %d\n", PS4.R2Value());
    }
#endif // defined(ENABLE_PS4_DEBUG)

    // Disable drives.
    if (PS4.Cross())
    {
#if defined(ENABLE_MOTORS)
      enable_drivers(false);
#endif // defined(ENABLE_MOTORS)
    }
    
    // Enable drives.
    if (PS4.Circle())
    {
#if defined(ENABLE_MOTORS)
      enable_drivers(true);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
      PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
    }

#if defined(ENABLE_MOTORS)
    // If the drives are disable do not ask for coordinates.
    if (!MotorsEnabled_g)
    {
      return;
    }
#endif // defined(ENABLE_MOTORS)

    // Base
    if (PS4.LStickX()) {
      BaseSpeedL = map(PS4.LStickX(), X_MIN, X_MAX, PRC_MAX, PRC_MIN);
#if defined(ENABLE_MOTORS)
      BaseSpeedL = constrain(BaseSpeedL, -M1_MAX_SPEED, M1_MAX_SPEED);
#endif // defined(ENABLE_MOTORS)
      if ((BaseSpeedL > DEAD_SPACE_LEFT_X) || (BaseSpeedL < -DEAD_SPACE_LEFT_X))
      {
        DEBUGLOG("Left Stick X at %d\n", BaseSpeedL);
#if defined(ENABLE_MOTORS)
        stepper1.setSpeed(BaseSpeedL);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
        PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
      }
      else
      {
#if defined(ENABLE_MOTORS)
        stepper1.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
      }
    }
    else
    {
#if defined(ENABLE_MOTORS)
      stepper1.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
    }

    // Shoulder
    if (PS4.LStickY()) {
      ShoulderSpeedL = map(PS4.LStickY(), Y_MIN, Y_MAX, PRC_MIN, PRC_MAX);
#if defined(ENABLE_MOTORS)
      ShoulderSpeedL = constrain(ShoulderSpeedL, -M2_MAX_SPEED, M2_MAX_SPEED);
#endif // defined(ENABLE_MOTORS)
      if ((ShoulderSpeedL > DEAD_SPACE_LEFT_Y) || (ShoulderSpeedL < -DEAD_SPACE_LEFT_Y))
      {
        DEBUGLOG("Left Stick Y at %d\n", ShoulderSpeedL);
#if defined(ENABLE_MOTORS)
        stepper2.setSpeed(ShoulderSpeedL);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
        PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
      }
      else
      {
#if defined(ENABLE_MOTORS)
        stepper2.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
      }
    }
    else
    {
#if defined(ENABLE_MOTORS)
      stepper2.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
    }

    // Differential
    if (PS4.R1())
    {
      // R
      RL = map(PS4.RStickX(), X_MIN, X_MAX, PRC_MIN, PRC_MAX);
#if defined(ENABLE_MOTORS)
      RL = constrain(RL, -M3_MAX_SPEED, M3_MAX_SPEED);
#endif // defined(ENABLE_MOTORS)
      // P
      PL = map(PS4.RStickY(), Y_MIN, Y_MAX, PRC_MIN, PRC_MAX);
#if defined(ENABLE_MOTORS)
      PL = constrain(PL, -M4_MAX_SPEED, M4_MAX_SPEED);
#endif // defined(ENABLE_MOTORS)
      // DEBUGLOG("Stick X: %d; Y: %d\r\n", RL, PL);

      // Mix throttle and direction
      LDL = PL + RL;
      RDL = PL - RL;

      if (LDL > -DEAD_SPACE_LEFT_Y && LDL < DEAD_SPACE_LEFT_Y)
      {
        LDL = 0;
      }

      if (RDL > -DEAD_SPACE_LEFT_X && RDL < DEAD_SPACE_LEFT_X)
      {
        RDL = 0;
      }

      DEBUGLOG("Differential L: %d; R: %d\r\n", LDL, -RDL);

#if defined(ENABLE_MOTORS)
      // Stop Elbow and Gripper if DF axis is running.
      stepper4.setSpeed(LDL);
      stepper5.setSpeed(-RDL);
      stepper3.setSpeed(0);
      stepper6.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
      PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
    }
    // Elbow
    else
    {
      ElbowSpeedL = map(PS4.RStickY(), Y_MIN, Y_MAX, PRC_MAX, PRC_MIN);
#if defined(ENABLE_MOTORS)
      ElbowSpeedL = constrain(ElbowSpeedL, -M6_MAX_SPEED, M6_MAX_SPEED);
#endif // defined(ENABLE_MOTORS)
      if ((ElbowSpeedL > DEAD_SPACE_RIGHT_Y) || (ElbowSpeedL < -DEAD_SPACE_RIGHT_Y))
      {
        DEBUGLOG("Right Stick Y at %d\n", PS4.RStickY());
#if defined(ENABLE_MOTORS)
        stepper3.setSpeed(-ElbowSpeedL);
        stepper6.setSpeed(ElbowSpeedL);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
        PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
      }
      else
      {
#if defined(ENABLE_MOTORS)
        stepper3.setSpeed(0);
        stepper6.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
      }
#if defined(ENABLE_MOTORS)
      stepper4.setSpeed(0);
      stepper5.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
    }

    // Gripper
    if (PS4.L2()) {
      GripperSpeedL = map(PS4.L2Value(), 0, 255, 0, PRC_MAX);
      if (PS4.L1())
      {
        GripperSpeedL *= -1;
      }
      DEBUGLOG("L2 at %d\n", GripperSpeedL);
      
      if ((GripperSpeedL > DEAD_SPACE_LEFT_Y) || (GripperSpeedL < -DEAD_SPACE_LEFT_Y))
      {
#if defined(ENABLE_MOTORS)
        GripperSpeedL = constrain(GripperSpeedL, -M6_MAX_SPEED, M6_MAX_SPEED);
        stepper6.setSpeed(GripperSpeedL);
#endif // defined(ENABLE_MOTORS)
#if defined(ENABLE_SLEEP_MODE)
        PS4SleepCounter_g = PS4_SLEEP_COUNT;
#endif // defined(ENABLE_SLEEP_MODE)
      }
      else
      {
#if defined(ENABLE_MOTORS)
        stepper6.setSpeed(0);
#endif // defined(ENABLE_MOTORS)
      }
    }

    if (PS4.Charging()) Serial.println("The controller is charging");
    if (PS4.Audio()) Serial.println("The controller has headphones attached");
    if (PS4.Mic()) Serial.println("The controller has a mic attached");

    // BattLevelL = constrain(PS4.Battery(), BATT_MIN_LEVEL, BATT_MAX_LEVEL);
    // BattLevelL = map(BattLevelL, BATT_MIN_LEVEL, BATT_MAX_LEVEL, 0, PRC_MAX);
    // DEBUGLOG("Battery Level : %d\n", PS4.Battery());
  }
}
#endif // defined(ENABLE_PS4)
#pragma endregion // Functions
