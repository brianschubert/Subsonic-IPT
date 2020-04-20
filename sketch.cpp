/**
 * sketch.cpp - Arduino sketch entry point
 *
 * Copyright (c) 2019, 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#define UBRRH // For code completion with HardwareSerial
#include <Arduino.h>
#include <SerLCD.h>
#include <Wire.h>

#include "src/point.h"
#include "src/navigator.h"
#include "src/inputs/buttons.h"
#include "src/inputs/mpu.h"
#include "src/pin.h"
#include "src/state.h"
#include "src/tui/menu_manager.h"
#include "src/tui/menus/guidance_menu.h"
#include "src/tui/menus/destination_menu.h"
#include "src/tui/menus/unit_menu.h"
#include "src/tui/menus/debug_menu.h"
#include "src/tui/menus/brightness_menu.h"

// When defined, a message will be printed to the Serial output whenever
// a button press registered.
//#define SUBSONIC_DEBUG_SERIAL_BUTTONS

// When defined, the devices current position will be printed periodically
// to the Serial output.
#define SUBSONIC_DEBUG_SERIAL_POSITION

// When defined, a message will be printed to the Serial output whenever
// the devices maximum distance from a waypoint is updated.
#define SUBSONIC_DEBUG_SERIAL_MAX_DIST

// When defined, a message will be printed to the Serial output each time
// the LED array is illuminated
//#define SUBSONIC_DEBUG_SERIAL_LEDS

using namespace subsonic_ipt;

/**
 * The period of time elapsed between updates for
 * the LCD and LED array.
 */
constexpr int REFRESH_PERIOD_MILLI = 100;

/**
 * Whenever the device is with this distance of a target, it is considered
 * to have "arrived".
 */
constexpr double ARRIVAL_THRESHOLD = 0.5;

/**
 * The clock rate used for I2C communication with the MPU.
 */
constexpr uint32_t I2C_CLOCK_RATE = 400000;

/**
 * The port used by `Serial` for printing debugging information.
 */
constexpr uint16_t SERIAL_PORT = 9600;

/**
 * The dimensions of the LCD screen used by this sketch.
 */
constexpr struct {
    uint8_t x = 20;
    uint8_t y = 4;
} LCD_DIMENSIONS;

//constexpr double EXPECTED_GRAVITY = 9.81;

/**
 * An angle-to-velocity mapping for simulating device movement
 * based on it gyroscopic orientation.
 */
constexpr double PITCH_VEL_MAPPING[][2] = {
    {10, 0},
    {45, 1.5},
    {90, 2},
};


/******************************************************************************\
 * Internal definitions
 *
 * Sketch declarations placed in anonymous namespace to given the
 * symbols internal linkage.
\******************************************************************************/
namespace {

/**
 * Global structure to track the positional data associated with the device.
 */
Navigator g_nav{};

SerLCD g_lcd{};

IPTState g_device_state{};

GuidanceMenu g_guidance_menu(
    &g_device_state,
    &g_nav,
    Angle::from_degrees(10.0),
    ARRIVAL_THRESHOLD,
    500
);

DestinationMenu g_destination_menu(&g_device_state, &g_nav);

UnitMenu g_unit_menu(&g_device_state);

DebugMenu g_debug_menu(&g_device_state, 500);

BrightnessMenu g_brightness_menu{};

Menu* const g_menus[]{
    &g_guidance_menu,
    &g_destination_menu,
    &g_unit_menu,
    &g_debug_menu,
    &g_brightness_menu,
};

MenuManager<5> g_menu_manager{g_menus};

/**
 * The maximum distance that this device has been from a target destination.
 *
 * Initialized to 1e-9 to prevent division by zero when used to compute
 * percentages.
 */
double g_max_distance{1e-9};

/**
 * The time in milliseconds since device startup when the device's display
 * was last updated.
 */
unsigned long g_last_display_update{0};

/**
 * The time in microseconds since device startup when the device's position
 * was last updated.
 */
unsigned long g_last_position_update_u{0};

/**
 * Callback function that is run repeatedly while the MPU is waiting
 * for new data.
 */
void subsonic_loop();

/**
 * Callback function to update the device's position each time a motion
 * packet is delivered from the MPU
 */
void update_position(const DeviceMotion& device_motion);

/**
 * Returns the horizonal velocity associated with the specified pitched.
 */
double pitch_to_vel(Angle pitch);

} // namespace


/******************************************************************************\
 * Public definitions
\******************************************************************************/

/**
 * Arduino initialization.
 */
void setup()
{
    // Open a serial connection on port 9600
    Serial.begin(SERIAL_PORT);
    Serial.println("Starting setup routine...");

    Wire.begin();
    Wire.setClock(I2C_CLOCK_RATE);
    // Initialize the LCD library
    g_lcd.begin(Wire);
    g_lcd.setContrast(5);
    g_lcd.clear();

    // Set the pin modes of the button pins.
    for (auto button : BUTTON_PINS) {
        pinMode(button, INPUT_PULLUP);
    }

    // Set the pin modes of the LED pins.
    for (auto led : LED_PINS) {
        pinMode(led, OUTPUT);
    }

    // Display interactive welcome sequence
    g_lcd.print("Subsonic IPT");
    g_lcd.setCursor(0, 1);
    g_lcd.noCursor();
    g_lcd.setBacklight(0x808080);

    for (int i = 0; i < (LCD_DIMENSIONS.x / 2) - 2; ++i) {
        g_lcd.print("* ");
        delay(250);
    }

    Serial.println("Waiting for use input to calibrate...");
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print("Press any button");
    g_lcd.setCursor(0, 1);
    g_lcd.print("for calibration");
    while (!button_any_tap_once()) { refresh_buttons(); } // Wait for any button to be pressed

    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print("Initializing MPU");
    g_lcd.setCursor(0, 1);
    g_lcd.print("Calibrating...");

    Serial.println("Beginning MPU setup...");
    auto mpu_status = setup_mpu();
    if (mpu_status != 0) {
        g_lcd.clear();
        g_lcd.setCursor(0, 0);
        g_lcd.print("FAILED TO START");
        g_lcd.setCursor(0, 1);
        g_lcd.print("MPU - [STOPPING]");
        while (true) { /* loop forever */ }
    }
    Serial.println("Setup successful.");
}

/**
 * Arduino loop entry point.
 */
void loop()
{
    run_mpu_loop(subsonic_loop, update_position);
}

/******************************************************************************\
 * Internal implementations
\******************************************************************************/
namespace {

void subsonic_loop()
{
    // Update the state of the buttons/switches
    refresh_buttons();

    const Menu::Input input{
        button_closed_once(ButtonLeft),
        button_closed_once(ButtonRight),
        button_closed_once(ButtonUp),
        button_closed_once(ButtonDown),
        button_closed_once(ButtonEnter),
    };

    g_menu_manager.interact(input);

    const auto time = millis();
    // Check if sufficient time has passed since the last display update.
    if (time - g_last_display_update >= REFRESH_PERIOD_MILLI) {
        g_last_display_update = time;

        g_menu_manager.refresh_display(g_lcd);

        // Compute the guidance direction that should be displayed to the user.
        Point user_direction = g_nav.compute_direction(
            g_device_state.position,
            g_device_state.facing
        );
        auto direction_dist = user_direction.norm();

        // Check if the device has reached a new maximum distance from
        // a target.
        if (direction_dist > g_max_distance) {
            g_max_distance = direction_dist;
#ifdef SUBSONIC_DEBUG_SERIAL_MAX_DIST
            Serial.print("Setting new max distance to ");
            Serial.println(direction_dist);
#endif
        }

#ifdef SUBSONIC_DEBUG_SERIAL_LEDS
        Serial.print("Illuminating ");
        Serial.print(direction_dist / g_max_distance);
        Serial.println(" percent of LEDs");
#endif
        // Temporary arbitrary waypoint colors.
        switch (g_nav.current_destination_index()) {
            case 0: {
                analogWrite(LED_PINS[0], 30);
                analogWrite(LED_PINS[1], 127);
                analogWrite(LED_PINS[2], 30);
                break;
            }
            case 1: {
                analogWrite(LED_PINS[0], 255);
                analogWrite(LED_PINS[1], 127);
                analogWrite(LED_PINS[2], 0);
                break;
            }
            case 2: {
                analogWrite(LED_PINS[0], 0);
                analogWrite(LED_PINS[1], 255);
                analogWrite(LED_PINS[2], 255);
                break;
            }
            case 3: {
                analogWrite(LED_PINS[0], 100);
                analogWrite(LED_PINS[1], 0);
                analogWrite(LED_PINS[2], 0);
                break;
            }
        }
//        g_led_array.activate_led_percent(1 - (direction_dist / g_max_distance));
    }
}

void update_position(const DeviceMotion& device_motion)
{
    // Member pointer to the member of DeviceMotion that contains the "true
    // pitch" of the device.
    //
    // Update this pointer if the MPU is mounted in an orientation different
    // from the expected orientation of the device.
    //
    // e.g. if the MPU is rotated 90 degrees, the "true pitch" will be the
    // roll.
    constexpr static float DeviceMotion::*true_pitch = &DeviceMotion::roll;

    // Copy new motion measurements into device state storage.
    g_device_state.device_motion = device_motion;
    g_device_state.device_motion.yaw = device_motion.yaw;
    g_device_state.device_motion.pitch = device_motion.pitch;
    g_device_state.device_motion.roll = device_motion.roll;
    auto current_time = micros();
    auto time_delta = static_cast<double>(current_time - g_last_position_update_u);
    time_delta /= 1e6;             // convert microseconds to seconds

    // Yaw is reported as a clockwise rotation, so we flip its sign
    // to change to the counterclockwise rotation used by the navigation
    // logic.
    auto yaw_angle = Angle{-device_motion.yaw};
    yaw_angle.normalize();
    g_device_state.facing = yaw_angle;
    const auto displacement =
        time_delta * pitch_to_vel(Angle{device_motion.*true_pitch}) * Point::unit_from_angle(yaw_angle);
    g_device_state.position = g_device_state.position + displacement;

    // Recompute current time to account for time lost to arithmetic
    g_last_position_update_u = micros();
    Serial.print("From (");
    Serial.print(g_device_state.position.m_x);
    Serial.print(',');
    Serial.print(g_device_state.position.m_y);
    Serial.print(")@");
    Serial.println(g_device_state.facing.deg());
}

double pitch_to_vel(Angle pitch)
{
    for (const auto pair : PITCH_VEL_MAPPING) {
        if (pitch.deg() < pair[0]) {
            return pair[1];
        }
    }
    constexpr size_t last_pos = (sizeof(PITCH_VEL_MAPPING) / sizeof(PITCH_VEL_MAPPING[0])) - 1;
    return 1; //PITCH_VEL_MAPPING[last_pos][1];
}

} // namespace
