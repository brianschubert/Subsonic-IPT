/**
 * menu.h - Base class for all LCD menus.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_MENU_H
#define SUBSONIC_IPT_MENU_H

#include <SerLCD.h>

namespace subsonic_ipt {

class Menu {
  protected:
    /**
     * Flag indicating whether or not this menu's content has changed.
     *
     * This flag may or may not influence the return value of this menu's
     * `content_changed` implementation.
     */
    bool m_content_changed{true};

  public:
    /**
     * The user inputs used to interact with a menu.
     */
    struct Input {
        bool left;
        bool right;
        bool up;
        bool down;
        bool enter;
    };

    [[nodiscard]]
    /**
     * Returns a short name that labels this menu.
     */
    virtual const char* get_menu_name() const noexcept = 0;

    /**
     * Writes this menu's representation to the given LCD.
     */
    virtual void refresh_display(SerLCD& lcd) = 0;

    /**
     * Updates that state of this menu according to the given user input.
     */
    virtual void interact(const Input& input) = 0;

    [[nodiscard]]
    /**
     * Return `true` is this menu should be the sole consumer of the user's
     * input.
     *
     * Allows menus to utilize all controls during a user interaction without
     * containing menus interpreting said input themselves.
     */
    virtual bool request_priority() const {
        return false;
    };

    [[nodiscard]]
    /**
     * Returns `true` if the content associated with this menu has changed.
     *
     * This method allows for "opt-in" screen updates in menu subclasses. The
     * implementation of this method allows individual menus to signal when
     * the screen should be refreshed.
     */
    virtual bool content_changed() const {
        return m_content_changed;
    }
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_MENU_H
