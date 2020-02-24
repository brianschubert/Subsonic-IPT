/**
 * navigator.cpp - Implementation for utilities for computing travel directions.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "navigator.h"

namespace subsonic_ipt {

Point Navigator::compute_direction(const Point pos, const Angle facing) const
{
    // Compute the difference vector between the current position and
    // the destination.
    Point displacement = current_destination() - pos;
    // Compute the true direction that the device must travel relative
    // to the direction it is currently facing.
    Angle direction = displacement.angle() - facing;

    // Compute the true displacement vector that the device must travel
    // along relative to the direction it is currently facing.
    if (direction.is_nan()) {
        return {0, 0};
    } else {
        return displacement.norm() * Point::unit_from_angle(direction);
    }
}

} // namespace subsonic_ipt
