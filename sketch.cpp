/**
 * sketch.cpp - Arduino sketch entry point
 *
 * Copyright (c) 2019 Brian Schubert
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
#include "src/led_array.h"
#include "src/inputs/buttons.h"
#include "src/inputs/mpu.h"
#include "src/pin.h"
#include "src/state.h"
#include "src/tui/menu_manager.h"
#include "src/tui/menus/guidance_menu.h"
#include "src/tui/menus/destination_menu.h"
#include "src/tui/menus/unit_menu.h"

// When defined, a message will be printed to the Serial output whenever
// a button press registered.
#define SUBSONIC_DEBUG_SERIAL_BUTTONS

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
constexpr int REFRESH_PERIOD_MILLI = 300;

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
    {45, 0.5},
    {90, 1},
};


/******************************************************************************\
 * Internal definitions
 *
 * Sketch declarations placed in anonymous namespace to given the
 * symbols internal linkage.
\******************************************************************************/
namespace {

/**
 * Wrapper modeling the array of LEDs being used by this sketch.
 */
LEDArray<sizeof(LED_PINS)> g_led_array(LED_PINS);

/**
 * Global structure to track the positional data associated with the device.
 */
Navigator g_nav{};

SerLCD g_lcd{};

IPTState g_device_state{
    .position = {0},
    .facing = {0},
    .localized_unit = LengthUnit::Meters,
    .device_motion = {},
};

GuidanceMenu g_guidance_menu(
    &g_device_state,
    &g_nav,
    Angle::from_degrees(10.0),
    ARRIVAL_THRESHOLD
);

DestinationMenu g_destination_menu(
    &g_device_state,
    &g_nav
);

UnitMenu g_unit_menu(
    &g_device_state
);

Menu* const g_menus[]{
    &g_guidance_menu,
    &g_destination_menu,
    &g_unit_menu,
};

MenuManager g_menu_manager{g_menus, sizeof(g_menus) / sizeof(Menu*)};

/**
 * Wrapper around the SparkFun LCD api for displaying guidance directions.
 */
//DirectionScreen g_screen(
//    LiquidCrystal(
//        LCD_PINS.rs,
//        LCD_PINS.enable,
//        LCD_PINS.data[0],
//        LCD_PINS.data[1],
//        LCD_PINS.data[2],
//        LCD_PINS.data[3]
//    ),
//    Angle::from_degrees(10.0),
//    ARRIVAL_THRESHOLD,
//    print_screen_title,
//    print_direction_arrived,
//    print_direction_forward,
//    print_direction_left,
//    print_direction_right,
//    print_direction_backward
//);

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

    // Configure whether the LED array should use PWM when illuminating
    // LEDS
    g_led_array.set_pwm(true);

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
    g_lcd.print(' ');

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
        .up = button_closed_once(ButtonUp),
        .down = button_closed_once(ButtonDown),
        .left = button_closed_once(ButtonLeft),
        .right = button_closed_once(ButtonRight),
        .enter = button_closed_once(ButtonEnter),
    };

    g_menu_manager.interact(input);

    const auto time = millis();
    // Check if sufficient time has passed since the last display update.
    if (time - g_last_display_update >= REFRESH_PERIOD_MILLI) {
        g_last_display_update = time;
#ifdef SUBSONIC_DEBUG_SERIAL_POSITION
//        Serial.print("Now at (");
//        Serial.print(g_nav.current_pos().m_x);
//        Serial.print(',');
//        Serial.print(g_nav.current_pos().m_y);
//        Serial.println(")");
#endif
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
        // Illuminate a number of LEDs proportional to the device's distance
        // from the target destination.
        g_led_array.activate_led_percent(1 - (direction_dist / g_max_distance));
    }
}

void update_position(const DeviceMotion& device_motion)
{
    // Copy new motion measurements into device state storage.
    g_device_state.device_motion = device_motion;
    auto current_time = micros();
    auto time_delta = static_cast<double>(current_time - g_last_position_update_u);
    time_delta /= 1e6;             // convert microseconds to seconds

    // Yaw is reported as a clockwise rotation, so we flip its sign
    // to change to the counterclockwise rotation used by the navigation
    // logic.
    auto yaw_angle = Angle{-device_motion.yaw};
    yaw_angle.normalize();
    g_device_state.facing = yaw_angle;
    const auto displacement = time_delta * pitch_to_vel(Angle{device_motion.pitch}) * Point::unit_from_angle(yaw_angle);
    g_device_state.position = g_device_state.position + displacement;

    // Recompute current time to account for time lost to arithmetic
    g_last_position_update_u = micros();

}

double pitch_to_vel(Angle pitch)
{
    for (const auto pair : PITCH_VEL_MAPPING) {
        if (pitch.deg() < pair[0]) {
            return pair[1];
        }
    }
    constexpr size_t last_pos = (sizeof(PITCH_VEL_MAPPING) / sizeof(PITCH_VEL_MAPPING[0])) - 1;
    return PITCH_VEL_MAPPING[last_pos][1];
}

} // namespace
