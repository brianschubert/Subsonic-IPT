//
// Created by brian on 11/2/19.
//

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
constexpr Pin LED_PINS[] = {3, 5, 6};

/**
 * The possible inputs that this sketch can receive from button activations.
 */
enum class ButtonAction : Pin {
    Set = 4,
    Cycle = 2,
    None = 0,
};

/**
 * The Arduino pins associated with buttons.
 */
constexpr Pin BUTTON_PINS[] = {static_cast<Pin>(ButtonAction::Set), static_cast<Pin>(ButtonAction::Cycle)};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_PIN_H
