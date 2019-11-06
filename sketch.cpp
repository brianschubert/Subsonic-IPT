#include <Arduino.h>
#include <LiquidCrystal.h>

#include "src/point.h"
#include "src/navigator.h"
#include "src/led_array.h"
#include "src/direction_screen.h"

// When defined, this sketch will receive simulated movement data
// from a serial input instead of processing input from an accelerometer.
#define ACCEL_SIMULATION_MODE

using namespace subsonic_ipt;

// Sketch declarations placed in anonymous namespace to given the
// symbols internal linkage.
namespace {

/// The period of time elapsed between updates for
/// the LCD and LED array.
constexpr int REFRESH_PERIOD_MILLI = 200;

/// The maximum expected size for an incoming movement message.
constexpr size_t MAX_SIM_MESSAGE_LENGTH = 30;

LEDArray<sizeof(LED_PINS)> g_led_array(LED_PINS);

Navigator g_nav{};

void print_screen_title(LiquidCrystal& lcd);

void print_direction_forward(LiquidCrystal& lcd, Point direction);

void print_direction_left(LiquidCrystal& lcd, Point direction);

void print_direction_right(LiquidCrystal& lcd, Point direction);

void print_direction_backward(LiquidCrystal& lcd, Point direction);

DirectionScreen g_screen(
    LiquidCrystal(13, 12, 11, 10, 9, 8),
    Angle::from_degrees(10.0),
    print_screen_title,
    print_direction_forward,
    print_direction_left,
    print_direction_right,
    print_direction_backward
);

double g_max_distance{0};

double g_last_display_update;

ButtonAction check_buttons();

bool read_deltas(Point&, Angle&);

void update_display(size_t current_destination, Point direction);

} // namespace

void setup()
{
    Serial.begin(9600);
    g_led_array.set_pwm(false);

    // Initialize the LCD library for a 16x2 character display.
    g_screen.lcd().begin(16, 2);
    // Clear the LCD screen.
    g_screen.lcd().clear();
}

void loop()
{
    const auto time = millis();
    Point position_delta{};
    Angle facing_delta{};

    read_deltas(position_delta, facing_delta);
    g_nav.update_position(position_delta);
    g_nav.update_facing(facing_delta);

    if (time - g_last_display_update >= REFRESH_PERIOD_MILLI) {
        g_last_display_update = time;
        auto button_pressed = check_buttons();
        switch (button_pressed) {
            case ButtonAction::Set: {
                g_nav.overwrite_destination(g_nav.current_pos());
                break;
            }
            case ButtonAction::Cycle: {
                g_nav.cycle_destination();
                break;
            }
            case ButtonAction::None: break;
        }

        Point user_direction = g_nav.compute_direction();
        auto direction_dist = user_direction.norm();

        if (direction_dist > g_max_distance) {
            g_max_distance = direction_dist;
        }

        g_led_array.activate_led_percent(direction_dist / g_max_distance);
        g_screen.print_direction(user_direction);
    }
}

namespace {

ButtonAction check_buttons()
{
    for (auto button : BUTTON_PINS) {
        if (digitalRead(button)) {
            return static_cast<ButtonAction>(button);
        }
    }
    return ButtonAction::None;
}

bool read_deltas(Point& pos_delta, Angle& facing_dela)
{
#ifdef ACCEL_SIMULATION_MODE
    char instr_buffer[MAX_SIM_MESSAGE_LENGTH + 1];
    if (Serial.available()) {
        auto bytes_read = Serial.readBytesUntil(';', instr_buffer, MAX_SIM_MESSAGE_LENGTH);
        instr_buffer[bytes_read] = 0;

        if (bytes_read == 0) {
            return false;
        }

        char* finger = instr_buffer;
        double parts[3];

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
    lcd.print(static_cast<int>(direction.angle().deg()) - 180);
    lcd.print("* Right");
}

void print_direction_backward(LiquidCrystal& lcd, Point direction)
{
    lcd.print("Turn around");
}

} // namespace
