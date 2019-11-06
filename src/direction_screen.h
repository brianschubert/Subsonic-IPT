//
// Created by brian on 11/6/19.
//

#ifndef SUBSONIC_IPT_DIRECTION_SCREEN_H
#define SUBSONIC_IPT_DIRECTION_SCREEN_H

#include <LiquidCrystal.h>

#include "point.h"

namespace subsonic_ipt {

class DirectionScreen {

    using DirectionHandler = void (*)(LiquidCrystal&, Point);

    /// Copy of a SparkFun LCD api wrapper.
    LiquidCrystal m_lcd;

    Angle m_snap_tolerance;

    void (*m_title_line)(LiquidCrystal&);

    struct {
        DirectionHandler forward;
        DirectionHandler left;
        DirectionHandler right;
        DirectionHandler backward;
    } m_handlers;

  public:
    DirectionScreen(
        const LiquidCrystal& lcd,
        Angle snap_tolerance,
        void title(LiquidCrystal&),
        DirectionHandler forward_handler,
        DirectionHandler left_handler,
        DirectionHandler right_handler,
        DirectionHandler backward_handler
    ) : m_lcd{lcd},
        m_snap_tolerance{snap_tolerance},
        m_title_line{title},
        m_handlers{forward_handler, left_handler, right_handler, backward_handler} {}

    LiquidCrystal& lcd()
    {
        return m_lcd;
    }

    void print_direction(Point direction);
};

} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_DIRECTION_SCREEN_H
