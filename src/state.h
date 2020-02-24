//
// Created by brian on 2/23/20.
//

#ifndef SUBSONIC_IPT_STATE_H
#define SUBSONIC_IPT_STATE_H

#include <stddef.h>
#include "point.h"
#include "units.h"

namespace subsonic_ipt {

struct IPTState {
    /// The current position of the device
    Point position;
    /// The direction the device is currently facing.
    Angle facing;
    /// The user-selected unit to report distances in.
    LengthUnit localized_unit;
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_STATE_H
