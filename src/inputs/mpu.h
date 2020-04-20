/**
 * mpu.h - Interface for monitoring motion data from an MPU6050 IMU.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_MPU_H
#define SUBSONIC_IPT_MPU_H

#include <stdint.h>

#include "../vendor/i2cdevlib/helper_3dmath.h"
#include "../vendor/i2cdevlib/MPU6050.h"

namespace subsonic_ipt {

/**
 * Structure containing the world-frame acceleration and
 * yaw-pitch-roll orientation of the device from a single MPU
 * packet.
 */
struct DeviceMotion {
    VectorInt16 world_accel{};
    VectorInt16 real_accel{};
    VectorInt16 raw_accel{};
    VectorFloat gravity{};
    union {
        struct {
            float yaw{};
            float pitch{};
            float roll{};
        };
        float ypr[3];
    };
};

[[nodiscard]]
/**
 * Configures the MPU for DMP over I2C..
 *
 * Returns the device status from the DMP library, with 0 indicating success.
 */
uint8_t setup_mpu();

/**
 * Arduino loop body function for handling data from the MPU.
 *
 * The `waiting_callback` function will be executed repeatedly while waiting
 * for sufficient data to be produced by the MPU.
 *
 * The `update_position` function will be executed each time a packet is read
 * from the MPU.
 *
 * Note that this function does not internally loop and must be called
 * repeatedly by the Arduino `loop` function.
 */
void run_mpu_loop(void waiting_callback(), void update_state(const DeviceMotion& world_accel));

} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_MPU_H
