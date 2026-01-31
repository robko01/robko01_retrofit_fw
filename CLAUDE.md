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
│   └── Interpolator/     # Multi-joint interpolation library
├── claude_tests/         # Claude Code test scripts (PowerShell)
├── platformio.ini        # Main PlatformIO configuration
├── platformio_*.ini      # Environment-specific configurations
└── .pio/                 # Build output (generated)
```

## Claude Tests Folder

The `claude_tests/` folder contains test scripts created by Claude Code for hardware testing.

### Available Test Scripts

| Script | Description | Usage |
|--------|-------------|-------|
| `test_interpolator.ps1` | Tests @MOVEJ commands on robot | `powershell -ExecutionPolicy Bypass -File claude_tests/test_interpolator.ps1` |

### Adding New Tests

When creating hardware test scripts:
1. Place them in `claude_tests/` folder
2. Use PowerShell for serial communication on Windows
3. Document the script in this table
4. Use COM7 at 9600 baud for robot communication

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

4. **Test the feature before merging:**
   - For software-only changes: Run build and verify functionality
   - For hardware-dependent changes: **ASK the user to test manually**
   - Never merge untested code into dev

5. **ASK before merging to dev:**
   - Always ask the user for approval before merging feature into dev
   - Example: "Feature is ready and committed. May I merge to dev and main?"

6. **Merge feature branch to dev (with --no-ff to preserve branch history):**
   ```bash
   git checkout dev
   git merge feature/<short-description> --no-ff -m "Merge feature/<short-description> into dev"
   ```

7. **Merge dev to main (with --no-ff to preserve branch history):**
   ```bash
   git checkout main
   git merge dev --no-ff -m "Merge dev into main"
   ```

8. **Push both branches and clean up:**
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

## Version and Build Number Management

### Version File Location

The firmware version is centralized in `include/Version.h`:

```cpp
#define FW_VERSION_MAJOR 1
#define FW_VERSION_MINOR 0
#define FW_BUILD_NUMBER 58
```

### Version Format

`MAJOR.MINOR.BUILD` (e.g., `1.0.58`)

- **MAJOR**: Increment for breaking changes or major new features
- **MINOR**: Increment for new features or significant improvements
- **BUILD**: Increment on each merge to main branch

### When to Update Version Numbers

| Change Type | Update |
|-------------|--------|
| Merge to main | Increment `FW_BUILD_NUMBER` |
| New feature | Increment `FW_VERSION_MINOR`, reset `FW_BUILD_NUMBER` to 0 |
| Breaking change | Increment `FW_VERSION_MAJOR`, reset MINOR and BUILD to 0 |
| Bug fix | Increment `FW_BUILD_NUMBER` |
| Documentation only | No version change |

### Build Number Update Workflow

**IMPORTANT:** When merging to main, always update the build number:

1. **Before merging to main**, edit `include/Version.h`:
   ```cpp
   // Increment FW_BUILD_NUMBER by 1
   #define FW_BUILD_NUMBER 59  // was 58
   ```

2. **Update CHANGELOG.md** with the new version and changes:
   ```markdown
   ## [1.0.59] - YYYY-MM-DD

   ### Added
   - Description of new features

   ### Changed
   - Description of changes

   ### Fixed
   - Description of bug fixes
   ```

3. **Commit the version update** as part of the feature branch before merging

### Files to Update on Version Change

| File | What to Update |
|------|----------------|
| `include/Version.h` | `FW_BUILD_NUMBER` (always), `FW_VERSION_MINOR`/`FW_VERSION_MAJOR` (when applicable) |
| `CHANGELOG.md` | Add new version section with changes |

### Using Version in Code

Include the version header and use the macros:

```cpp
#include "Version.h"

// Print version
Serial.printf("Firmware version: %s\n", FW_FULL_VERSION);
// Output: "Firmware version: v1.0.58"

// Access individual components
Serial.printf("Build: %d\n", FW_BUILD_NUMBER);
```

### Changelog Format

Follow [Keep a Changelog](https://keepachangelog.com/) format:

```markdown
## [MAJOR.MINOR.BUILD] - YYYY-MM-DD

### Added
- New features

### Changed
- Changes to existing functionality

### Deprecated
- Features to be removed in future

### Removed
- Removed features

### Fixed
- Bug fixes

### Security
- Security-related changes
```
