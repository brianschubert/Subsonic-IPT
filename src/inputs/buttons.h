/**
 * buttons.h - Interface for detecting the state of the switches/buttons used
 *             by this sketch.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_BUTTONS_H
#define SUBSONIC_IPT_BUTTONS_H

#include <stdint.h> // cstdint not available

namespace subsonic_ipt {
enum Button : uint8_t {
    ButtonNone = 0,
    ButtonLeft = 1u << 0u,
    ButtonRight = 1u << 1u,
    ButtonUp = 1u << 2u,
    ButtonDown = 1u << 3u,
    ButtonEnter = 1u << 4u,
};

/**
 * Refreshes the state of the buttons used by this sketch.
 *
 * This function should be called prior to checking for changes in the
 * states of the buttons.
 */
void refresh_buttons();

[[nodiscard]]
/**
 * Returns `true` if all of the given buttons are closed (i.e. pressed).
 */
bool button_closed(Button button_flag) noexcept;

[[nodiscard]]
/**
 * Returns `true` is all of the given buttons are open (i.e. not pressed).
 */
bool button_open(Button button_flag) noexcept;

[[nodiscard]]
/**
 * Returns `true` if all of the given buttons are closed (i.e. pressed)
 * and the buttons were not all closed during the previous refresh.
 */
bool button_closed_once(Button button_flag) noexcept;

[[nodiscard]]
/**
 * Returns `true` if all of the given buttons are open (i.e. not pressed)
 * and the buttons were not all open during the previous refresh.
 */
bool button_open_once(Button button_flag) noexcept;

[[nodiscard]]
/**
 * Returns true if any button was closed since the last refresh.
 */
bool button_any_tap_once() noexcept;

}

#endif //SUBSONIC_IPT_BUTTONS_H
