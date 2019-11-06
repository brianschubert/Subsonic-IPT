//
// Created by brian on 11/6/19.
//

#include "direction_screen.h"

namespace subsonic_ipt {

void DirectionScreen::print_direction(Point direction)
{
    m_lcd.clear();
    m_lcd.setCursor(0,0);
    m_title_line(m_lcd);
    m_lcd.setCursor(0,1);

    constexpr Angle backwards = Angle{M_PI};
    auto travel_angle = direction.angle();
    DirectionHandler handler;

    if (travel_angle < m_snap_tolerance || travel_angle > -m_snap_tolerance) {
        handler = m_handlers.forward;
    } else if (travel_angle > (backwards - m_snap_tolerance)
        || travel_angle < (backwards + m_snap_tolerance)
        ) {
        handler = m_handlers.backward;
    } else if (direction.m_x < 0){
        handler = m_handlers.left;
    } else {
        handler = m_handlers.right;
    }

    handler(m_lcd, direction);
}

} // namespace subsonic_ipt
