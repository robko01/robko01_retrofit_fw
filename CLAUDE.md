# CLAUDE.md - Project Guidelines

This file provides guidance for Claude Code when working with this codebase.

## Project Overview

Robko 01 Retrofit Firmware - ESP32-based multi-axis robot controller using PlatformIO and Arduino framework.

## Build & Upload Commands

```bash
# Build default environment
pio run

# Build specific environment
pio run -e serial_tcm

# Upload firmware
pio run -t upload

# Upload to specific environment
pio run -e serial_tcm -t upload

# Monitor serial output
pio device monitor

# Build and monitor
pio run -t upload && pio device monitor

# Clean build
pio run -t clean
```

## Project Structure

```
├── src/                  # Source files
│   └── main.cpp          # Main application
├── include/              # Header files
│   └── ApplicationConfiguration.h  # Feature flags and configuration
├── lib/                  # Project-specific libraries
├── platformio.ini        # Main PlatformIO configuration
├── platformio_*.ini      # Environment-specific configurations
└── .pio/                 # Build output (generated)
```

## C/C++ Naming Conventions

### Macros and Constants
Use `SCREAMING_SNAKE_CASE`:
```cpp
#define ENABLE_MOTORS
#define PIN_DIR_1 4
#define DEFAULT_MAX_SPEED 100
#define WIFI_RECONNECT_TIME_MS 5000
```

### Functions
Use `snake_case`:
```cpp
void init_motors_pins();
void update_drivers();
void show_device_properties();
void enable_drivers(bool state);
```

### Global Variables
Use `PascalCase` with `_g` suffix:
```cpp
bool MotorsEnabled_g;
uint8_t MotorState_g;
int SafetyStopFlag_g;
FxTimer *WatchDogTimer_g;
```

### Local Variables
Use `camelCase`:
```cpp
int motorSpeed;
bool isConnected;
uint8_t currentIndex;
```

### Type Definitions
Use `PascalCase` with `_t` suffix:
```cpp
typedef CommandParser<...> CommandParser_t;
```

### Classes and Structs
Use `PascalCase`:
```cpp
class JointPositionUnion;
struct JointPosition;
```

### Enums
Use `PascalCase` for enum name, `PascalCase` or `SCREAMING_SNAKE_CASE` for values:
```cpp
enum OperationModes : uint8_t
{
    NONE = 0U,
    Positioning,
    Speed,
};
```

### Pin Definitions
Use `PIN_` prefix with `SCREAMING_SNAKE_CASE`:
```cpp
#define PIN_ENABLE 2
#define PIN_DIR_1 4
#define PIN_STP_1 0
```

### Feature Flags
Use `ENABLE_` prefix:
```cpp
#define ENABLE_MOTORS
#define ENABLE_WIFI
#define ENABLE_OTA
```

## Code Organization

### Use #pragma region for Code Sections
```cpp
#pragma region Headers
#include "ApplicationConfiguration.h"
#pragma endregion // Headers

#pragma region Prototypes
void init_drivers();
#pragma endregion // Prototypes

#pragma region Variables
bool MotorsEnabled_g;
#pragma endregion // Variables
```

### Documentation Style (Doxygen)
```cpp
/**
 * @brief Initialize the stepper drivers.
 *
 * @param state Enable state for drivers.
 * @return void
 */
void enable_drivers(bool state);
```

## Feature Flag Pattern

Use conditional compilation for optional features:
```cpp
#if defined(ENABLE_WIFI)
void init_wifi();
void update_wifi();
#endif // defined(ENABLE_WIFI)
```

Always close `#if` with descriptive comment: `#endif // defined(FEATURE_NAME)`

## Arduino/ESP32 Best Practices

### Setup and Loop Pattern
```cpp
void setup() {
    Serial.begin(115200);
    init_motors_pins();
    init_drivers();
}

void loop() {
    update_drivers();
    update_limits();
}
```

### GPIO Configuration
```cpp
pinMode(PIN_ENABLE, OUTPUT);
pinMode(M1_LIMIT, INPUT_PULLUP);
digitalWrite(PIN_ENABLE, HIGH);
```

### Serial Debug Output
Use conditional debug macros:
```cpp
#if defined(SHOW_FUNC_NAMES)
    Serial.println(__func__);
#endif
```

### FreeRTOS Tasks (ESP32)
```cpp
void task_lcd(void *parameter) {
    for (;;) {
        draw_lcd();
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

xTaskCreate(task_lcd, "LCD_Task", 4096, NULL, 1, NULL);
```

## PlatformIO Best Practices

### Environment Configuration
- Use separate `.ini` files for different configurations
- Reference them via `extra_configs` in main `platformio.ini`
- Set feature flags via `build_flags`:

```ini
[env:serial_tcm]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
build_flags =
    -D ENABLE_MOTORS=1
    -D ENABLE_TCM_COMMANDS=1
```

### Library Dependencies
Prefer remote libraries in `lib_deps`:
```ini
lib_deps =
    https://github.com/waspinator/AccelStepper
    https://github.com/LennartHennigs/Button2/
```

### Monitor Configuration
```ini
monitor_speed = 115200
```

## Memory Considerations (ESP32)

- Use `PROGMEM` for large constant arrays
- Prefer `const char*` over `String` for static strings
- Use `F()` macro for Serial.print literals: `Serial.println(F("Message"));`
- Be mindful of stack size in FreeRTOS tasks

## Common Patterns in This Codebase

### Init/Update Pattern
Each feature module follows init/update pattern:
```cpp
void init_feature();   // Called once in setup()
void update_feature(); // Called repeatedly in loop()
```

### Button Debouncing
Use Button2 library with debounce time:
```cpp
#define DEBOUNCE_TIME_MS 100
Button2 M1LimitSwitch_g;
M1LimitSwitch_g.setDebounceTime(DEBOUNCE_TIME_MS);
```

### Callback Handlers
```cpp
void cbRequestHandler(uint8_t opcode, uint8_t size, uint8_t *payload);
```

## Testing

```bash
# Run unit tests
pio test

# Run tests for specific environment
pio test -e native
```

## Troubleshooting

- If upload fails, check COM port in Device Manager
- Hold BOOT button on ESP32 during upload if needed
- Use `pio device list` to find connected devices
- Check `monitor_speed` matches Serial.begin() baud rate

## Git Workflow

### Branching Strategy
- **main**: Production-ready code, only receives merges from dev
- **dev**: Development branch, created from main, where integration happens
- **feature branches**: Created from dev for each new feature or change

### Branch Naming
- Feature branches: `feature/<short-description>` (e.g., `feature/add-dimmer-support`)
- Bug fixes: `fix/<short-description>` (e.g., `fix/mac-validation`)

### Commit Workflow (Step by Step)

1. **Checkout dev branch:**
   ```bash
   git checkout dev
   ```

2. **Create feature branch from dev:**
   ```bash
   git checkout -b feature/<short-description>
   ```

3. **Stage and commit changes with descriptive message:**
   ```bash
   git add <file>
   git commit -m "$(cat <<'EOF'
   Short summary of changes

   - Detailed bullet point 1
   - Detailed bullet point 2
   - Detailed bullet point 3

   Co-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>
   EOF
   )"
   ```

4. **Merge feature branch to dev (with --no-ff to preserve branch history):**
   ```bash
   git checkout dev
   git merge feature/<short-description> --no-ff -m "Merge feature/<short-description> into dev"
   ```

5. **Merge dev to main (with --no-ff to preserve branch history):**
   ```bash
   git checkout main
   git merge dev --no-ff -m "Merge dev into main"
   ```

6. **Push both branches and clean up:**
   ```bash
   git push origin main
   git push origin dev
   git branch -d feature/<short-description>
   ```

### Important: Always Use --no-ff

Always use `--no-ff` (no fast-forward) when merging to create merge commits. This preserves the branch topology and makes the history visible in GitLens:

```
*   Merge dev into main
|\
| *   Merge feature/xyz into dev
| |\
| | * Actual commit message
| |/
```

### Commit Message Format

```
Short summary (imperative mood, max 50 chars)

- Bullet point describing change 1
- Bullet point describing change 2
- Bullet point describing change 3

Co-Authored-By: Claude Opus 4.5 <noreply@anthropic.com>
```

**Examples of good commit messages:**
- `Add WDT integration for motor safety`
- `Fix serial communication timeout handling`
- `Enable limits and estop for TCM profile`
