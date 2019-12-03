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

    double m_arrival_tolerance;

    void (*m_title_line)(LiquidCrystal&);

    struct {
        DirectionHandler arrived;
        DirectionHandler forward;
        DirectionHandler left;
        DirectionHandler right;
        DirectionHandler backward;
    } m_handlers;

  public:
    DirectionScreen(
        const LiquidCrystal& lcd,
        Angle snap_tolerance,
        double arrival_tolerance,
        void title(LiquidCrystal&),
        DirectionHandler arrival_handler,
        DirectionHandler forward_handler,
        DirectionHandler left_handler,
        DirectionHandler right_handler,
        DirectionHandler backward_handler
    ) : m_lcd{lcd},
        m_snap_tolerance{snap_tolerance.normalize()},
        m_arrival_tolerance{arrival_tolerance},
        m_title_line{title},
        m_handlers{arrival_handler, forward_handler, left_handler, right_handler, backward_handler} {}

    LiquidCrystal& lcd()
    {
        return m_lcd;
    }

    void print_direction(Point direction);
};

} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_DIRECTION_SCREEN_H
