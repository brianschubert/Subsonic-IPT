//
// Created by brian on 1/21/20.
//

#ifndef SUBSONIC_IPT_MENU_H
#define SUBSONIC_IPT_MENU_H

#include <SerLCD.h>

namespace subsonic_ipt {

class Menu {
  public:
    struct Input {
        bool left;
        bool right;
        bool up;
        bool down;
        bool enter;
    };

    [[nodiscard]]
    virtual const char* get_menu_name() const noexcept = 0;

    virtual void refresh_display(SerLCD& lcd) = 0;

    virtual void interact(const Input& input) = 0;

    [[nodiscard]]
    virtual bool request_priority() const {
        return false;
    };
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_MENU_H
