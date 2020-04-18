/**
 * brightness_menu.h - LCD menu to dimming and brightening the screen.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_BRIGHTNESS_MENU_H
#define SUBSONIC_IPT_BRIGHTNESS_MENU_H

#include <stdint.h>

#include "../menu.h"

namespace subsonic_ipt {

class BrightnessMenu : public Menu {
  private:
    /**
     * The index of the currently selected screen contrast.
     */
    size_t m_screen_contrast_selection{0};

    /**
     * The screen contrast settings that the user may cycle through while
     * using this menu.
     */
    constexpr static inline int8_t SCREEN_CONTRAST_CHOICES[] = {
        5,
        25,
        65,
        75,
        95,
        110,
    };

  public:

    BrightnessMenu()
    {
        // Ensure that contrast is not update on first menu load.
        m_content_changed = false;
    }

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

    [[nodiscard]]
    /**
     * The number of screen contrast settings that this menu offers.
     */
    constexpr size_t choice_count() const
    {
        return sizeof(SCREEN_CONTRAST_CHOICES) / sizeof(int8_t);
    }

};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_BRIGHTNESS_MENU_H
