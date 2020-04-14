//
// Created by brian on 1/21/20.
//

#include "guidance_menu.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

namespace {
/**
 * The columns on the screen allocated for displaying the localized
 * distance magnitude, plus one for a non-printed trailing null.
 *
 * If changed, be sure to update the format strings `format_distance` below.
 */
constexpr size_t DIST_WIDTH{5};

/**
 * Formats the given localized distance measurement into a representation
 * composed of precisely `DIST_WIDTH-1` characters.
 *
 * Currently, all values are aggressively ceiled to simplify formatting logic.
 */
void format_distance(char (& dist_buff)[DIST_WIDTH], double localized_dist) noexcept
{
    if (localized_dist >= 1 && localized_dist < 1000) {
        snprintf(dist_buff, DIST_WIDTH, "%4d", localized_dist);
    } else {
        auto order = static_cast<int>(log10(localized_dist));
        if (order > 99) {
            strcpy(dist_buff, "+INF");
        } else if (order > 9) {
            auto div = pow(10, order);
            snprintf(
                dist_buff,
                DIST_WIDTH,
                "%1de%2d",
                static_cast<int>(ceil(localized_dist / div)),
                static_cast<int>(order)
            );
        } else if (order > 0) {
            auto div = pow(10, order - 1);
            snprintf(
                dist_buff,
                DIST_WIDTH,
                "%2de%1d",
                static_cast<int>(ceil(localized_dist / div)),
                static_cast<int>(order - 1)
            );
        } else if (order > -9) {
            auto div = pow(10, -order + 1);
            snprintf(
                dist_buff,
                DIST_WIDTH,
                "%1de-%1d",
                static_cast<int>(ceil(localized_dist * div)),
                static_cast<int>(-order + 1)
            );
        } else {
            snprintf(dist_buff, DIST_WIDTH, "   %1d", 0);
        }
    }
}
}

namespace subsonic_ipt {

const char* GuidanceMenu::get_menu_name() const noexcept
{
    return "GUID";
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
        lcd.print("Go forward ");

        const auto dist_meters = direction.norm();
        char dist_buff[DIST_WIDTH];
        format_distance(
            dist_buff,
            abs(meters_to_unit(dist_meters, m_device_state->localized_unit))
        );
        const char* symbol = unit_symbol(m_device_state->localized_unit);

        lcd.setCursor(20 - DIST_WIDTH - strlen(symbol), 2);
        lcd.print(dist_buff);
        lcd.print(symbol);
    } else if (near_backward) {
        lcd.print("Turn around");
    } else if (direction.m_y > 0) {
        lcd.print("Turn ");
        lcd.print(static_cast<int>(direction.angle().deg()));
        lcd.print("* Left");
    } else {
        lcd.print("Turn ");
        lcd.print(360 - static_cast<int>(direction.angle().deg()));
        lcd.print("* Right");
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

