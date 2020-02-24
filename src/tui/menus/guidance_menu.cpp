//
// Created by brian on 1/21/20.
//

#include "guidance_menu.h"

namespace {
using subsonic_ipt::Point;
using DirectionHandler = void (*)(SerLCD&, Point);

void print_direction_arrived(SerLCD& lcd, Point direction);

void print_direction_forward(SerLCD& lcd, Point direction);

void print_direction_backward(SerLCD& lcd, Point direction);

void print_direction_left(SerLCD& lcd, Point direction);

void print_direction_right(SerLCD& lcd, Point direction);
} // namespace


namespace subsonic_ipt {

const char* GuidanceMenu::get_menu_name() const noexcept
{
    return "GUIDE";
}

void GuidanceMenu::refresh_display(SerLCD& lcd)
{
    print_screen_title(lcd);
    lcd.setCursor(0, 1);

    const auto direction = m_navigator->compute_direction(
        m_device_state->position,
        m_device_state->facing
    );

    constexpr Angle backwards = Angle{M_PI};
    const auto travel_angle = direction.angle();
    DirectionHandler handler;

    // Whether the travel angle is within the snap tolerance of the forward direction.
    bool near_forward = ((travel_angle < m_snap_tolerance) || (travel_angle > m_snap_tolerance.conjugate()));
    // Whether the travel angle is with the snap tolerance of the backward direction.
    bool near_backward = (travel_angle > (backwards - m_snap_tolerance))
        && (travel_angle < (backwards + m_snap_tolerance));

    // When the device is at its original position, we denote the travel angle
    // as NaN. When this occurs, invoke the forward handler.
    if (direction.norm() <= m_arrival_tolerance || travel_angle.is_nan()) {
        handler = print_direction_arrived;
    } else if (near_forward) {
        handler = print_direction_forward;
    } else if (near_backward) {
        handler = print_direction_backward;
    } else if (direction.m_y > 0) {
        handler = print_direction_left;
    } else {
        handler = print_direction_right;
    }

    handler(lcd, direction);
}

void GuidanceMenu::interact(const Menu::Input& input)
{
    if (input.up) {
        m_navigator->cycle_destination(false);
    }
    if (input.down){
        m_navigator->cycle_destination(true);
    }
    if (input.enter) {
        m_navigator->overwrite_destination(m_device_state->position);
    }
}

} // namespace subsonic_ipt


namespace {
using subsonic_ipt::Point;

void print_direction_arrived(SerLCD& lcd, Point direction)
{
    lcd.print("You Have Arrived");
}

void print_direction_forward(SerLCD& lcd, Point direction)
{
    lcd.write("Go forward ");
    lcd.write(direction.norm());
    lcd.write("m");
}

void print_direction_backward(SerLCD& lcd, Point direction)
{
    lcd.write("Turn around");
}

void print_direction_left(SerLCD& lcd, Point direction)
{
    lcd.write("Turn ");
    lcd.write(static_cast<int>(direction.angle().deg()));
    lcd.write("* Left");
}

void print_direction_right(SerLCD& lcd, Point direction)
{
    lcd.write("Turn ");
    lcd.write(360 - static_cast<int>(direction.angle().deg()));
    lcd.write("* Right");
}
} // namespace
