/**
 * sketch.cpp - Arduino sketch entry point
 */

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "src/point.h"
#include "src/navigator.h"
#include "src/led_array.h"
#include "src/direction_screen.h"

// When defined, debugging information will be printed to
// the serial output.
#define SUBSONIC_DEBUG_SERIAL

// When defined, this sketch will receive simulated movement data
// from a serial input instead of processing input from an accelerometer.
#define ACCEL_SIMULATION_MODE

using namespace subsonic_ipt;

// Sketch declarations placed in anonymous namespace to given the
// symbols internal linkage.
namespace {

/**
 * The period of time elapsed between updates for
 * the LCD and LED array.
 */
constexpr int REFRESH_PERIOD_MILLI = 500;

/**
 * The maximum expected size for an incoming movement message.
 */
constexpr size_t MAX_SIM_MESSAGE_LENGTH = 30;

/**
 * Whenever the device is with this distance of a target, it is considered
 * to have "arrived".
 */
constexpr double ARRIVAL_THRESHOLD = 0.5;

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
    LiquidCrystal(13, 12, 11, 10, 9, 8),
    Angle::from_degrees(10.0),
    print_screen_title,
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
double g_last_display_update;

/**
 * Whether a button is currently being pressed.
 *
 * Used to prevent multiple loop cycles from acting on the same button
 * depression.
 */
bool g_button_pressed{false};

/**
 * Returns the current state of the device's buttons.
 */
ButtonAction check_buttons();

[[nodiscard]]
/**
 * Writes the estimated displacements of the device to the provided
 * structures.
 *
 * Returns true if new data was written.
 */
bool read_deltas(Point&, Angle&);

} // namespace

/**
 * Arduino initialization.
 */
void setup()
{
    // Open a serial connection on port 9600
    Serial.begin(9600);
    // Configure whether the LED array should use PWM when illuminating
    // LEDS
    g_led_array.set_pwm(false);

    // Initialize the LCD library for a 16x2 character display.
    g_screen.lcd().begin(16, 2);
    // Clear the LCD screen.
    g_screen.lcd().clear();

    // Set the pin modes of the button pins.
    for (auto button : BUTTON_PINS) {
        pinMode(button, INPUT_PULLUP);
    }
}

/**
 * Arduino loop entry point.
 */
void loop()
{
    const auto time = millis();
    Point position_delta{};
    Angle facing_delta{};

    // Read the next estimated displacement for the device.
    if (read_deltas(position_delta, facing_delta)) {
        g_nav.update_position(position_delta);
        g_nav.update_facing(facing_delta);
    }

#ifdef SUBSONIC_DEBUG_SERIAL
    if (position_delta.norm() != 0) {
        Serial.print("Read position delta: (");
        Serial.print(position_delta.m_x);
        Serial.print(',');
        Serial.print(position_delta.m_y);
        Serial.println(')');
    }
    if (facing_delta.m_rad != 0) {
        Serial.print("Read angle delta: ");
        Serial.println(facing_delta.deg());
    }
    if (position_delta.norm() != 0 || facing_delta.m_rad != 0) {
        Serial.print("Now at (");
        Serial.print(g_nav.current_pos().m_x);
        Serial.print(',');
        Serial.print(g_nav.current_pos().m_y);
        Serial.print(") facing ");
        Serial.println(g_nav.current_facing().deg());
    }
#endif

    // Check if a button is depressed
    auto button_pressed = check_buttons();
    switch (button_pressed) {
        case ButtonAction::Set: {
            g_nav.overwrite_destination(g_nav.current_pos());
#ifdef SUBSONIC_DEBUG_SERIAL
            Serial.print("Overriding destination #");
            Serial.println(g_nav.current_destination_index());
#endif
            break;
        }
        case ButtonAction::Cycle: {
#ifdef SUBSONIC_DEBUG_SERIAL
            Serial.println("Cycling destinations.");
#endif
            g_nav.cycle_destination();
            break;
        }
        case ButtonAction::None:break;
    }

    // Check if sufficient time has passed since the last display update.
    if (time - g_last_display_update >= REFRESH_PERIOD_MILLI) {
        g_last_display_update = time;

        // Compute the guidance direction that should be displayed to the user.
        Point user_direction = g_nav.compute_direction();
        auto direction_dist = user_direction.norm();

        // Check if the device has reached a new maximum distance from
        // a target.
        if (direction_dist > g_max_distance) {
            g_max_distance = direction_dist;
#ifdef SUBSONIC_DEBUG_SERIAL
            Serial.print("Setting new max distance to ");
            Serial.println(direction_dist);
#endif
        }

#ifdef SUBSONIC_DEBUG_SERIAL
//        Serial.print("Illuminating ");
//        Serial.print(direction_dist / g_max_distance);
//        Serial.println(" percent of LEDs");
#endif
        // Illuminate a number of LEDs proportional to the device's distance
        // from the target destination.
        g_led_array.activate_led_percent(direction_dist / g_max_distance);
        // Print the guidance direction to the LCD screen.
        g_screen.print_direction(user_direction);
    }
}

namespace {

ButtonAction check_buttons()
{
    // Check each button bin for a low voltage.
    for (auto button : BUTTON_PINS) {
        if (digitalRead(button) == LOW) {
            if (g_button_pressed) {
                return ButtonAction::None;
            }
            g_button_pressed = true;
            return static_cast<ButtonAction>(button);
        }
        g_button_pressed = false;
    }
    g_button_pressed = false;
    return ButtonAction::None;
}

bool read_deltas(Point& pos_delta, Angle& facing_dela)
{
#ifdef ACCEL_SIMULATION_MODE
    // Allocate stack space for the message string.
    char instr_buffer[MAX_SIM_MESSAGE_LENGTH + 1];
    if (Serial.available()) {
        // Attempt to read a command from the serial input.
        auto bytes_read = Serial.readBytesUntil(';', instr_buffer, MAX_SIM_MESSAGE_LENGTH);
        instr_buffer[bytes_read] = 0; // Add trailing null for c-style string

        if (bytes_read == 0) {
            return false;
        }

        // The current scanning position while parsing whitespace-separated
        // floating-point values from the command
        char* finger = instr_buffer;
        // The three floating-point parts of the recieved command.
        double parts[3];

        // Parse the components of the command.
        // We assume that the command will contain EXACTLY three
        // floating-point vaues.
        for (auto i = 0; *finger; ++i) {
            parts[i] = strtod(finger, &finger);
        }

        pos_delta = Point{parts[0], parts[1]};
        facing_dela = Angle::from_degrees(parts[2]);
        return true;
    }
    return false;
#else
    // TODO Read accelerometer
    return false;
#endif
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

} // namespace
