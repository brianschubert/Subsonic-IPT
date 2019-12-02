//
// Created by brian on 11/29/19.
//

#include <Arduino.h>

#include "pin.h"

#include "buttons.h"

namespace {
using namespace subsonic_ipt;

/**
 * The current and previous state of the buttons used by this sketch.
 */
struct {
    /// The state of the buttons during the previous refresh.
    Button previous_buttons{ButtonNone};
    /// The current state of the buttons since the last refresh.
    Button current_buttons{ButtonNone};
} button_status;

/**
 * Returns the one-bit mask associated with the given button pin.
 */
Button pin_to_flag(ButtonPin pin)
{
    switch (pin) {
        case ButtonPin::Set: {
            return Button::ButtonSet;
        }
        case ButtonPin::Cycle: {
            return Button::ButtonCycle;
        }
        default: {
            return Button::ButtonNone;
        }
    }
}

/**
 * Returns `true`  if all of the bits set in the given mask are 1s in
 * the specified target.
 */
inline bool button_mask_matches(Button target, Button mask)
{
    return (target & mask) == mask;
}
} // namespace

namespace subsonic_ipt {

void refresh_buttons()
{
    button_status.previous_buttons = button_status.current_buttons;

    // Check each button bin for a low voltage.
    for (auto pin : BUTTON_PINS) {
        if (digitalRead(pin) == LOW) {
            auto flag = pin_to_flag(static_cast<ButtonPin>(pin));
            button_status.current_buttons = static_cast<Button>(button_status.current_buttons & flag);
        }
    }
}

bool button_closed(Button button_flag)
{
    return button_mask_matches(button_status.current_buttons, button_flag);
}

bool button_open(Button button_flag)
{
    return button_mask_matches(static_cast<Button>(~button_status.current_buttons), button_flag);
}

bool button_closed_once(Button button_flag)
{
    return button_open(button_flag) && !button_mask_matches(button_status.previous_buttons, button_flag);
}

bool button_open_once(Button button_flag)
{
    return button_closed(button_flag) && button_mask_matches(button_status.previous_buttons, button_flag);
}

} // namespace subsonic_ipt

