/**
 * direction_screen.cpp - Implementation for the direction screen interface.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "direction_screen.h"

namespace subsonic_ipt {

void DirectionScreen::print_direction(Point direction)
{
    m_lcd.clear();
    m_lcd.setCursor(0, 0);
    m_title_line(m_lcd);
    m_lcd.setCursor(0, 1);

    constexpr Angle backwards = Angle{M_PI};
    auto travel_angle = direction.angle();
    DirectionHandler handler;

    // Whether the travel angle is within the snap tolerance of the forward direction.
    bool near_forward = ((travel_angle < m_snap_tolerance) || (travel_angle > m_snap_tolerance.conjugate()));
    // Whether the travel angle is with the snap tolerance of the backward direction.
    bool near_backward = (travel_angle > (backwards - m_snap_tolerance))
        && (travel_angle < (backwards + m_snap_tolerance));

    // When the device is at its original position, we denote the travel angle
    // as NaN. When this occurs, invoke the forward handler.
    if (direction.norm() <= m_arrival_tolerance || travel_angle.is_nan()) {
        handler = m_handlers.arrived;
    } else if (near_forward) {
        handler = m_handlers.forward;
    } else if (near_backward) {
        handler = m_handlers.backward;
    } else if (direction.m_y > 0) {
        handler = m_handlers.left;
    } else {
        handler = m_handlers.right;
    }

    handler(m_lcd, direction);
}

} // namespace subsonic_ipt
