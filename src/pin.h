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

constexpr Pin INTERRUPT_PIN = 2;

constexpr struct {
    Pin rs = 13;
    Pin enable = 12;
    Pin data[4] = {11, 10, 9, 8};
} LCD_PINS;

/**
 * The possible inputs that this sketch can receive from button activations.
 */
enum class ButtonPin : Pin {
    Set = 4,
    Cycle = 7,
};

/**
 * The Arduino pins associated with buttons.
 */
constexpr Pin BUTTON_PINS[] = {static_cast<Pin>(ButtonPin::Set), static_cast<Pin>(ButtonPin::Cycle)};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_PIN_H
