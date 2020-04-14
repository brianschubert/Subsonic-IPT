//
// Created by brian on 4/14/20.
//

#ifndef SUBSONIC_IPT_BRIGHTNESS_MENU_H
#define SUBSONIC_IPT_BRIGHTNESS_MENU_H

#include <stdint.h>

#include "../menu.h"

namespace subsonic_ipt {
class BrightnessMenu : public Menu {
  private:
    size_t m_screen_contrast_selection{0};

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
    constexpr size_t choice_count() const
    {
        return sizeof(SCREEN_CONTRAST_CHOICES) / sizeof(int8_t);
    }

};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_BRIGHTNESS_MENU_H
