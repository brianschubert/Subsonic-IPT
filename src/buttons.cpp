//
// Created by brian on 11/29/19.
//

#include <Arduino.h>

#include "pin.h"

#include "buttons.h"

namespace {
using namespace subsonic_ipt;

Button button_status = Button::ButtonNone;

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
} // namespace

namespace subsonic_ipt {

Button read_buttons()
{
    Button current_buttons{ButtonNone};

    // Check each button bin for a low voltage.
    for (auto pin : BUTTON_PINS) {
        if (digitalRead(pin) == LOW) {
            current_buttons = static_cast<Button>(current_buttons & pin_to_flag(static_cast<ButtonPin>(pin)));
        }
    }

    // Filter out all of the buttons there were already depressed
    auto new_buttons = static_cast<Button>(current_buttons & ~button_status);

    // Remove buttons that are now off from button status, and then add
    // buttons that were just turned on.
    button_status = static_cast<Button>((button_status & current_buttons) | new_buttons);

    return new_buttons;
}

bool check_buttons(Button button_flags)
{
    return button_flags & button_status;
}

} // namespace subsonic_ipt

