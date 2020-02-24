/**
 * mpu.cpp - Implementation for motion processing utilities.
 *
 * Parts of this implementation are based on I2C device class (I2Cdev)
 * demonstration Arduino sketch for MPU6050 class using DMP (MotionApps v2.0).
 * A copy of the license for these sources may be found in vendor/i2cdevlib.
 *
 * Copyright (c) 2019 Brian Schubert
 * Copyright (c) 2012 Jeff Rowberg
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#define I2CDEV_IMPLEMENTATION I2CDEV_ARDUINO_WIRE

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "../vendor/i2cdevlib/I2Cdev.h"
#include "../vendor/i2cdevlib/MPU6050_6Axis_MotionApps20.h"
#include "../vendor/i2cdevlib/helper_3dmath.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"

#endif

#include "mpu.h"
#include "../pin.h"

constexpr uint8_t CALIBRATION_LOOPS{20};


/******************************************************************************\
 * Internal definitions
\******************************************************************************/
namespace {
/**
 * MPU instance used by this sketch.
 */
MPU6050 g_mpu;

/**
 * Global state variables for controlling the MPU.
 */
struct mpu_control_t {
    uint8_t fifo_buffer[64];
    uint16_t fifo_count;
    uint16_t packet_size;
    bool dmp_ready;
    uint8_t mpu_int_status;
    uint8_t dev_status;
} g_mpu_control{};

/**
 * Whether an interupt was raised by the MPU.
 */
volatile bool g_mpu_interrupt = false;

/**
 * Arduino interupt handler to allow signal the presence of a DMP interupt.
 */
void dmp_data_ready() noexcept
{
    g_mpu_interrupt = true;
}

/**
 * Read the world-frame acceleration and yaw-pitch-roll orientation of the
 * device from the given MPU fifo buffer.
 */
void compute_device_motion(subsonic_ipt::DeviceMotion& device_motion, const uint8_t* fifo_buffer) {
    // Orientation and motion data from the packet in the fifo buffer.
    Quaternion device_quaternion;
    VectorInt16 raw_accel;
    VectorInt16 real_accel;
    VectorFloat gravity;

    // Populate the device_motion structure from the fifo buffer packet
    g_mpu.dmpGetQuaternion(&device_quaternion, fifo_buffer);
    g_mpu.dmpGetAccel(&raw_accel, fifo_buffer);
    g_mpu.dmpGetGravity(&gravity, &device_quaternion);
    g_mpu.dmpGetLinearAccel(&real_accel, &raw_accel, &gravity);
    g_mpu.dmpGetLinearAccelInWorld(&device_motion.accel, &real_accel, &device_quaternion);
    g_mpu.dmpGetYawPitchRoll(device_motion.ypr, &device_quaternion, &gravity);
}

} // namespace

/******************************************************************************\
 * Public definitions
\******************************************************************************/
namespace subsonic_ipt {

uint8_t setup_mpu(uint32_t clock_rate)
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(clock_rate); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(clock_rate / 1000, true);
#endif

    // Initialize device
    Serial.println(F("Initializing I2C devices..."));

    g_mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // Verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(g_mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

#ifdef SUBSONIC_MPU_INIT_WAIT
    // Wait for ready
    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial.available() && Serial.read()) {} // empty buffer
    while (!Serial.available()) {}                 // wait for data
    while (Serial.available() && Serial.read()) {} // empty buffer again
#endif

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    g_mpu_control.dev_status = g_mpu.dmpInitialize();

    // make sure it worked (returns 0 if so)
    if (g_mpu_control.dev_status == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        g_mpu.CalibrateAccel(CALIBRATION_LOOPS);
        g_mpu.CalibrateGyro(CALIBRATION_LOOPS);
        g_mpu.PrintActiveOffsets();

        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        g_mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
        Serial.println(F(")..."));
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmp_data_ready, RISING);
        g_mpu_control.mpu_int_status = g_mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        g_mpu_control.dmp_ready = true;

        // get expected DMP packet size for later comparison
        g_mpu_control.packet_size = g_mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(g_mpu_control.dev_status);
        Serial.println(F(")"));
    }

    return g_mpu_control.dev_status;
}

void run_mpu_loop(
    void waiting_callback(),
    void update_position(const DeviceMotion& world_accel)
)
{
    // If programming failed, don't try to do anything
    if (!g_mpu_control.dmp_ready) { return; }

    // Wait for MPU interrupt or extra packet(s) available
    do {
        if (g_mpu_interrupt && g_mpu_control.fifo_count < g_mpu_control.packet_size) {
            // Try to get out of the infinite loop
            g_mpu_control.fifo_count = g_mpu.getFIFOCount();
        }
        waiting_callback();
    } while (!g_mpu_interrupt && g_mpu_control.fifo_count < g_mpu_control.packet_size);

    // Reset interrupt flag and get INT_STATUS byte
    g_mpu_interrupt = false;
    g_mpu_control.mpu_int_status = g_mpu.getIntStatus();

    // get current FIFO count
    g_mpu_control.fifo_count = g_mpu.getFIFOCount();
    if (g_mpu_control.fifo_count < g_mpu_control.packet_size) {
        // Lets go back and wait for another interrupt. We shouldn't be here, we got an interrupt from another event
        // This is blocking so don't do it   while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    }
        // check for overflow (this should never happen unless our code is too inefficient)
    else if ((g_mpu_control.mpu_int_status & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || g_mpu_control.fifo_count >= 1024) {
        // reset so we can continue cleanly
        g_mpu.resetFIFO();
        //  fifoCount = mpu.getFIFOCount();  // will be zero after reset no need to ask
        Serial.println(F("FIFO overflow!"));

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (g_mpu_control.mpu_int_status & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {

        // Read a packet from FIFO
        while (g_mpu_control.fifo_count >= g_mpu_control.packet_size) {
            // Lets catch up to NOW, someone is using the dreaded delay()!
            g_mpu.getFIFOBytes(g_mpu_control.fifo_buffer, g_mpu_control.packet_size);
            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            g_mpu_control.fifo_count -= g_mpu_control.packet_size;
        }

        // Send the acceleration and orientation data to the `update_position`
        // callback.
        DeviceMotion device_motion;
        compute_device_motion(device_motion, g_mpu_control.fifo_buffer);
        update_position(device_motion);
    }
}

}
