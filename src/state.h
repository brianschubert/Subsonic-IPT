/**
 * state.h - Structure for tracking global IPT device state.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_STATE_H
#define SUBSONIC_IPT_STATE_H

#include <stddef.h>
#include "point.h"
#include "units.h"
#include "inputs/mpu.h"

namespace subsonic_ipt {

struct IPTState {
    /// The current position of the device
    Point position;
    /// The direction the device is currently facing.
    Angle facing;
    /// The user-selected unit to report distances in.
    LengthUnit localized_unit;
    /// The most recently measured motion data for the device.
    DeviceMotion device_motion;
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_STATE_H
