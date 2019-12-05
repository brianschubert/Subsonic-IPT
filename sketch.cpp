/**
 * sketch.cpp - Arduino sketch entry point
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "src/point.h"
#include "src/navigator.h"
#include "src/led_array.h"
#include "src/direction_screen.h"
#include "src/buttons.h"
#include "src/mpu.h"
#include "src/pin.h"

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
    uint8_t x = 16;
    uint8_t y = 2;
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
//    {65, 2.0},
//    {90, 3.0},
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

/**
 * Prints a title line to the given LCD screen.
 */
void print_screen_title(LiquidCrystal& lcd);

/**
 * Prints a "you have arrived" direction to the given LCD screen.
 */
void print_direction_arrived(LiquidCrystal& lcd, Point direction);

/**
 * Prints a "move-forward" direction to the given LCD screen.
 */
void print_direction_forward(LiquidCrystal& lcd, Point direction);

/**
 * Prints a "turn-left" direction to the given LCD screen.
 */
void print_direction_left(LiquidCrystal& lcd, Point direction);

/**
 * Prints a "turn-right" direction to the given LCD screen.
 */
void print_direction_right(LiquidCrystal& lcd, Point direction);

/**
 * Prints a "turn around" direction to the given LCD screen.
 */
void print_direction_backward(LiquidCrystal& lcd, Point direction);

/**
 * Wrapper around the SparkFun LCD api for displaying guidance directions.
 */
DirectionScreen g_screen(
    LiquidCrystal(
        LCD_PINS.rs,
        LCD_PINS.enable,
        LCD_PINS.data[0],
        LCD_PINS.data[1],
        LCD_PINS.data[2],
        LCD_PINS.data[3]
    ),
    Angle::from_degrees(10.0),
    ARRIVAL_THRESHOLD,
    print_screen_title,
    print_direction_arrived,
    print_direction_forward,
    print_direction_left,
    print_direction_right,
    print_direction_backward
);

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
 * Whether a button is currently being pressed.
 *
 * Used to prevent multiple loop cycles from acting on the same button
 * depression.
 */
bool g_button_pressed{false};

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

    // Initialize the LCD library for a 16x2 character display.
    g_screen.lcd().begin(LCD_DIMENSIONS.x, LCD_DIMENSIONS.y);
    // Clear the LCD screen.
    g_screen.lcd().clear();

    // Set the pin modes of the button pins.
    for (auto button : BUTTON_PINS) {
        pinMode(button, INPUT_PULLUP);
    }

    // Set the pin modes of the LED pins.
    for (auto led : LED_PINS) {
        pinMode(led, OUTPUT);
    }

    // Display interactive welcome sequence
    g_screen.lcd().print("Subsonic IPT");
    g_screen.lcd().setCursor(0, 1);
    g_screen.lcd().print(' ');

    for (int i = 0; i < (LCD_DIMENSIONS.x / 2) - 2; ++i) {
        g_screen.lcd().print("* ");
        delay(250);
    }

    Serial.println("Waiting for use input to calibrate...");
    g_screen.lcd().clear();
    g_screen.lcd().setCursor(0, 0);
    g_screen.lcd().print("Press any button");
    g_screen.lcd().setCursor(0, 1);
    g_screen.lcd().print("for calibration");
    while (!button_any_tap_once()) refresh_buttons(); // Wait for any button to be pressed

    g_screen.lcd().clear();
    g_screen.lcd().setCursor(0, 0);
    g_screen.lcd().print("Initializing MPU");
    g_screen.lcd().setCursor(0, 1);
    g_screen.lcd().print("Calibrating...");

    Serial.println("Beginning MPU setup...");
    auto mpu_status = setup_mpu(I2C_CLOCK_RATE);
    if (mpu_status != 0) {
        g_screen.lcd().clear();
        g_screen.lcd().setCursor(0, 0);
        g_screen.lcd().print("FAILED TO START");
        g_screen.lcd().setCursor(0, 1);
        g_screen.lcd().print("MPU - [STOPPING]");
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

    if (button_closed_once(ButtonSet)) {
#ifdef SUBSONIC_DEBUG_SERIAL_BUTTONS
        Serial.println("Setting new destination.");
#endif
        g_nav.overwrite_destination(g_nav.current_pos());
    }

    if (button_closed_once(ButtonCycle)) {
#ifdef SUBSONIC_DEBUG_SERIAL_BUTTONS
        Serial.println("Cycling destinations.");
#endif
        g_nav.cycle_destination();
    }

    const auto time = millis();
    // Check if sufficient time has passed since the last display update.
    if (time - g_last_display_update >= REFRESH_PERIOD_MILLI) {
        g_last_display_update = time;
#ifdef SUBSONIC_DEBUG_SERIAL_POSITION
        Serial.print("Now at (");
        Serial.print(g_nav.current_pos().m_x);
        Serial.print(',');
        Serial.print(g_nav.current_pos().m_y);
        Serial.println(")");
#endif

        // Compute the guidance direction that should be displayed to the user.
        Point user_direction = g_nav.compute_direction();
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
        // Print the guidance direction to the LCD screen.
        g_screen.print_direction(user_direction);
    }
}

void print_screen_title(LiquidCrystal& lcd)
{
    lcd.print("To #");
    lcd.print(g_nav.current_destination_index());
    lcd.print("(");
    const auto destination = g_nav.current_destination();
    lcd.print(static_cast<int>(destination.m_x));
    lcd.print(",");
    lcd.print(static_cast<int>(destination.m_y));
    lcd.print(")");
}

void print_direction_arrived(LiquidCrystal& lcd, Point direction)
{
    lcd.print("You Have Arrived");
}

void print_direction_forward(LiquidCrystal& lcd, Point direction)
{
    lcd.print("Go forward ");
    lcd.print(direction.norm());
    lcd.print("m");
}

void print_direction_left(LiquidCrystal& lcd, Point direction)
{
    lcd.print("Turn ");
    lcd.print(static_cast<int>(direction.angle().deg()));
    lcd.print("* Left");
}

void print_direction_right(LiquidCrystal& lcd, Point direction)
{
    lcd.print("Turn ");
    lcd.print(360 - static_cast<int>(direction.angle().deg()));
    lcd.print("* Right");
}

void print_direction_backward(LiquidCrystal& lcd, Point direction)
{
    lcd.print("Turn around");
}

void update_position(const DeviceMotion& device_motion)
{
    auto current_time = micros();
    auto time_delta = static_cast<double>(current_time - g_last_position_update_u);
    time_delta /= 1e6;             // convert microseconds to seconds
//    constexpr uint32_t MAX_ACCEL = 16384;
//
//    // Compute acceleration * time
//    Point device_accel{
//        EXPECTED_GRAVITY * world_accel.x * time_delta / MAX_ACCEL,
//        EXPECTED_GRAVITY * world_accel.y * time_delta / MAX_ACCEL,
//    };
//
//    // Temporarily filter out very very small accelerations to
//    // reduce noise in velocity
//    if (abs(world_accel.x) < 20) {
//        device_accel.m_x = 0;
//    }
//    if (abs(world_accel.y) < 20) {
//        device_accel.m_y = 0;
//    }
//
//    // Update v = v_0 + at
//    g_nav.apply_acceleration(device_accel);
//    // Update displacement = 0.5 * at^2 + vt
//    g_nav.move(
//        time_delta * (0.5 * device_accel + g_nav.current_vel())
//    );

    // Yaw is reported as a clockwise rotation, so we flip its sign
    // to change to the counterclockwise rotation used by the navigation
    // logic.
    auto yaw_angle = Angle{-device_motion.yaw};
    yaw_angle.normalize();
    g_nav.set_facing(yaw_angle);
    g_nav.apply_displacement(
        time_delta * pitch_to_vel(Angle{device_motion.pitch}) * Point::unit_from_angle(yaw_angle)
    );

    // Recompute current time to account for time long to arithmetic
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
