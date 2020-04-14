#include "brightness_menu.h"

namespace subsonic_ipt {

const char* BrightnessMenu::get_menu_name() const noexcept
{
    return "DIM ";
}

void BrightnessMenu::refresh_display(SerLCD& lcd)
{
    // Only update the brightness if the refresh was caused by this
    // menu changing.
    if (m_content_changed) {
        lcd.setContrast(SCREEN_CONTRAST_CHOICES[m_screen_contrast_selection]);
    }
    m_content_changed = false;
    lcd.setCursor(0, 1);
    lcd.print("Screen brightness:  ");

    char slider[21] = "[                  ]";
    const size_t slider_position =  (17 * ((choice_count() - static_cast<size_t>(m_screen_contrast_selection)))) / choice_count();
    for (size_t i = 1; i <= slider_position; ++i) {
        slider[i] = '#';
    }
    slider[slider_position] = ')';
    lcd.setCursor(0, 2);
    lcd.print(slider);
    lcd.setCursor(0, 3);
    lcd.print(slider);
}

void BrightnessMenu::interact(const Menu::Input& input)
{
    if (input.down && m_screen_contrast_selection < choice_count() - 1) {
        m_screen_contrast_selection += 1;
        m_content_changed = true;
    }
    if (input.up && m_screen_contrast_selection != 0) {
        m_screen_contrast_selection -= 1;
        m_content_changed = true;
    }
}

} // namespace subsonic_ipt
