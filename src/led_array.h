/**
 * led_array.h - Interface for illuminating a linear array of LEDs
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_LED_ARRAY_H
#define SUBSONIC_IPT_LED_ARRAY_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <Arduino.h>

#include "pin.h"

namespace subsonic_ipt {

/**
 * An array of physical LEDs that can be used to visual approximate
 * a percentage.
 */
template<size_t S>
class LEDArray {

    /**
     * The pins associated with this LED array's LEDs.
     */
    Pin m_pins[S]{};

    /**
     * Whether to use PWM to partially illuminate LEDs.
     *
     * If supported by the circuitry, using PWM allows for
     * greater resolution in LED activation percentages.
     */
    bool m_use_pwm{false};

    /**
     * The number of different brightnesses that LEDArrays should use
     * when PWM mode is enabled.
     *
     * E.g., setting this constant to 3 will result in "dim", "medium" and "full"
     * brightness settings being used.
     */
    static inline constexpr size_t PWN_DEG_FRD{3};

  public:
    explicit LEDArray(const Pin* pins) noexcept
    {
        memcpy(m_pins, pins, sizeof(m_pins));
    }

    /**
     * Set whether or not to use PWN when illuminating a percentage
     * using this array's LEDs.
     */
    void set_pwm(bool value) noexcept
    {
        m_use_pwm = value;
    };

    [[nodiscard]]
    /**
     * The number of distinctive states that this LED array can display.
     */
    constexpr size_t states() const
    {
        return S * PWN_DEG_FRD;
    }

    [[nodiscard]]
    /**
     * The difference between PWN output values used to illuminate LEDs
     * when PWM mode is enabled.
     */
    constexpr byte pwm_step() const
    {
        return UINT8_MAX / PWN_DEG_FRD;
    }

    /**
     * Illuminates the best approximation of the given percentage using
     * this array's LEDs.
     *
     * The percentage should be represented as a value between 0 and 1. All
     * other values will be ignored.
     */
    void activate_led_percent(double percent);

};

template<size_t S>
void LEDArray<S>::activate_led_percent(double percent)
{
    if (percent > 1 || percent < 0) {
        return;
    }

    const size_t leds_on = percent * S;
    for (size_t i = 0; i < leds_on; ++i) {
        digitalWrite(m_pins[i], HIGH);
    }
    for (size_t i = leds_on; i < S; ++i) {
        digitalWrite(m_pins[i], LOW);
    }

    if (m_use_pwm) {
        const size_t partial_states = static_cast<size_t>(states() * percent) - leds_on * PWN_DEG_FRD;
        if (partial_states) {
            analogWrite(m_pins[leds_on], partial_states * pwm_step());
        }
    }
}

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_LED_ARRAY_H
