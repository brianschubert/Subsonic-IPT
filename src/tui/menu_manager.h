//
// Created by brian on 1/21/20.
//

#ifndef SUBSONIC_IPT_MENU_MANAGER_H
#define SUBSONIC_IPT_MENU_MANAGER_H

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "menu.h"
#include "SerLCD.h"

namespace subsonic_ipt {

template<size_t S>
class MenuManager : public Menu {
    // Implementation not designed with the possibility of zero menus in mind.
    static_assert(S != 0);

    constexpr static inline size_t MAX_MENU_NAME_LEN = 5;

    Menu* m_menus[S]{};

    size_t m_current_menu{0};

  public:
    explicit MenuManager(Menu* const menus[])
    {
        memcpy(m_menus, menus, sizeof(Menu) * S);
    }

    [[nodiscard]]
    constexpr size_t size() const noexcept
    {
        return S;
    }

    [[nodiscard]]
    const char* get_menu_name() const noexcept override
    {
        return "MNGR";
    }

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

  private:
    static void lcd_print_title(SerLCD& lcd, const char* title, bool current, size_t count = MAX_MENU_NAME_LEN)
    {
        if (current) {
            lcd.write('[');
        }
        lcd.write(reinterpret_cast<const uint8_t*>(title), count);
        if (current) {
            lcd.write(']');
        }
    }

};

template<>
void MenuManager<1>::refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), true);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<>
void MenuManager<2>::refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
    lcd.write(' ');
    lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<>
void MenuManager<3>::refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
    lcd.write(' ');
    lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
    lcd.write(' ');
    lcd_print_title(lcd, m_menus[2]->get_menu_name(), m_current_menu == 2);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<size_t S>
void MenuManager<S>::refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);

    // Whether the current menu is the leftmost option.
    const bool end_left = m_current_menu == 0;
    // Whether the current menu is the rightmost option.
    const bool end_right = m_current_menu == S - 1;
    // Whether to print a scroll arrow on the LHS of the display.
    const bool arrow_left = m_current_menu > 1;
    // Whether to print a scroll arrow on the RHS of the display
    const bool arrow_right = m_current_menu < S - 2;

    const size_t center_menu = end_left
        ? m_current_menu + 1
        : (end_right ? m_current_menu - 1 : m_current_menu);

    if (arrow_left) {
        lcd.write('<');
    }
    lcd_print_title(lcd, m_menus[center_menu - 1]->get_menu_name(), end_left);
    lcd.write(' ');
    lcd_print_title(
        lcd,
        m_menus[center_menu]->get_menu_name(),
        !(end_left || end_right),
        (arrow_left && arrow_right) ? 4 : 5
    );
    lcd.write(' ');
    lcd_print_title(lcd, m_menus[center_menu + 1]->get_menu_name(), end_right);
    if (arrow_right) {
        lcd.write('>');
    }

    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<size_t S>
void MenuManager<S>::interact(const Menu::Input& input)
{
    // Only react to the given input if the current menu is NOT requesting
    // input priority.
    if (!m_menus[m_current_menu]->request_priority()) {
        size_t menu_offset{0};
        if (input.right) {
            menu_offset += 1;
        }
        if (input.left) {
            menu_offset -= 1;
        }
        m_current_menu = (m_current_menu + menu_offset) % S;
    }
    m_menus[m_current_menu]->interact(input);
}
} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_MENU_MANAGER_H
