//
// Created by brian on 1/21/20.
//

#include "guidance_menu.h"

namespace subsonic_ipt {

const char* GuidanceMenu::get_menu_name() const noexcept
{
    return "GUIDE";
}

void GuidanceMenu::refresh_display(SerLCD& lcd)
{
    print_screen_title(lcd);

    const auto direction = m_navigator->compute_direction(
        m_device_state->position,
        m_device_state->facing
    );

    constexpr Angle backwards = Angle{M_PI};
    const auto travel_angle = direction.angle();

    // Whether the travel angle is within the snap tolerance of the forward direction.
    bool near_forward = ((travel_angle < m_snap_tolerance) || (travel_angle > m_snap_tolerance.conjugate()));
    // Whether the travel angle is with the snap tolerance of the backward direction.
    bool near_backward = (travel_angle > (backwards - m_snap_tolerance))
        && (travel_angle < (backwards + m_snap_tolerance));

    lcd.setCursor(0, 3);
    // When the device is at its original position, we denote the travel angle
    // as NaN. When this occurs, invoke the forward handler.
    if (direction.norm() <= m_arrival_tolerance || travel_angle.is_nan()) {
        lcd.print("You Have Arrived");
    } else if (near_forward) {
        lcd.write("Go forward ");
        lcd.write(direction.norm());
        lcd.write("m");
    } else if (near_backward) {
        lcd.write("Turn around");
    } else if (direction.m_y > 0) {
        lcd.write("Turn ");
        lcd.write(static_cast<int>(direction.angle().deg()));
        lcd.write("* Left");
    } else {
        lcd.write("Turn ");
        lcd.write(360 - static_cast<int>(direction.angle().deg()));
        lcd.write("* Right");
    }
}

void GuidanceMenu::interact(const Menu::Input& input)
{
    if (input.up) {
        m_navigator->cycle_destination(false);
    }
    if (input.down) {
        m_navigator->cycle_destination(true);
    }
    if (input.enter) {
        m_navigator->overwrite_destination(m_device_state->position);
    }
}

} // namespace subsonic_ipt

