//
// Created by brian on 11/2/19.
//

#include "navigator.h"

namespace subsonic_ipt {

Point Navigator::compute_direction() const
{
    // Compute the difference vector between the current position and
    // the destination.
    Point displacement = m_destinations[m_current_dest] - m_current_pos;
    // Compute the true direction that the device must travel relative
    // to the direction it is currently facing.
    Angle direction = displacement.angle() - m_facing;

    // Compute the true displacement vector that the device must travel
    // along relative to the direction it is currently facing.
    return displacement.norm() * Point::unit_from_angle(direction);
}

} // namespace subsonic_ipt
