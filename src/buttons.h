//
// Created by brian on 11/29/19.
//

#ifndef SUBSONIC_IPT_BUTTONS_H
#define SUBSONIC_IPT_BUTTONS_H

#include <stdint.h> // cstdint not available

namespace subsonic_ipt {
enum Button : uint8_t {
    ButtonNone = 0,
    ButtonSet = 1 << 0,
    ButtonCycle = 1 << 1,
};

Button read_buttons();

bool check_buttons(Button button_flags);
}

#endif //SUBSONIC_IPT_BUTTONS_H
