/**
 * pin.h - Arduino pin definitions.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_PIN_H
#define SUBSONIC_IPT_PIN_H

namespace subsonic_ipt {

/**
 * Integer typed used to represent Arduino pins.
 */
using Pin = uint8_t;

/**
 * The Arduino pins associated with LEDs.
 */
inline constexpr Pin LED_PINS[] = {3, 5, 6};

inline constexpr Pin INTERRUPT_PIN = 2;

/**
 * The possible inputs that this sketch can receive from button activations.
 */
enum class ButtonPin : Pin {
    Left = 7,
    Right = 12,
    Up = 13,
    Down = 4,
    Enter = 8,
};

/**
 * The Arduino pins associated with buttons.
 */
inline constexpr Pin BUTTON_PINS[] = {
    static_cast<Pin>(ButtonPin::Left),
    static_cast<Pin>(ButtonPin::Right),
    static_cast<Pin>(ButtonPin::Up),
    static_cast<Pin>(ButtonPin::Down),
    static_cast<Pin>(ButtonPin::Enter),
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_PIN_H
