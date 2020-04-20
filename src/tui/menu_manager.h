/**
 * menu_manager.h - A collection of menus sharing the same screen.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

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

    /**
     * The maximum length of the menu titles that will be displayed by this
     * menu manager.
     *
     * Titles that are longer than this length will be truncated when displayed.
     */
    constexpr static inline size_t MAX_MENU_NAME_LEN = 5;

    /**
     * The menus contained in this menu manager.
     */
    Menu* m_menus[S]{};

    /**
     * The index of the menu that is currently being displayed by this menu
     * manager.
     */
    size_t m_current_menu{0};

  public:
    explicit MenuManager(Menu* const menus[])
    {
        memcpy(m_menus, menus, sizeof(Menu*) * S);
    }

    [[nodiscard]]
    /**
     * The number of menus contained in this menu manager
     */
    constexpr size_t size() const noexcept
    {
        return S;
    }

    [[nodiscard]]
    const char* get_menu_name() const noexcept override
    {
        return "MNGR";
    }

    void interact(const Input& input) override;

    void refresh_display(SerLCD& lcd) override
    {
        // Clear and refresh the screen if either the state of this menu manager
        // has changed or if the state of the currently displayed menu has
        // changed.
        if (m_content_changed || m_menus[m_current_menu]->content_changed()) {
            m_content_changed = false;
            lcd.clear();
            force_refresh_display(lcd);
        }
    }

  private:
    /**
     * Helper function to print the titles of the menus contained in this menu
     * manager.
     */
    static void lcd_print_title(SerLCD& lcd, const char* title, bool current, size_t count = MAX_MENU_NAME_LEN)
    {
        char title_buff[MAX_MENU_NAME_LEN + 1];
        memcpy(title_buff, title, count);
        if (current) {
            lcd.print('[');
        }
        lcd.print(title_buff);
        if (current) {
            lcd.print(']');
        }
    }

    /**
     * Writes this menu manager's title bar to the given LCD and calls the
     * refresh display function of the currently active menu.
     */
    void force_refresh_display(SerLCD& lcd);

};

template<>
void MenuManager<1>::force_refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), true);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<>
void MenuManager<2>::force_refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
    lcd.print(' ');
    lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<>
void MenuManager<3>::force_refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);
    lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
    lcd.print(' ');
    lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
    lcd.print(' ');
    lcd_print_title(lcd, m_menus[2]->get_menu_name(), m_current_menu == 2);
    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(0, 1);
    m_menus[m_current_menu]->refresh_display(lcd);
}

template<size_t S>
void MenuManager<S>::force_refresh_display(SerLCD& lcd)
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
        lcd.print('<');
    }
    lcd_print_title(lcd, m_menus[center_menu - 1]->get_menu_name(), end_left);
    lcd.print(' ');
    lcd_print_title(
        lcd,
        m_menus[center_menu]->get_menu_name(),
        !(end_left || end_right),
//        (arrow_left && arrow_right) ? MAX_MENU_NAME_LEN - 1 : MAX_MENU_NAME_LEN // Temporarily disable variable width menu titles.
        MAX_MENU_NAME_LEN
    );
    lcd.print(' ');
    lcd_print_title(lcd, m_menus[center_menu + 1]->get_menu_name(), end_right);
    if (arrow_right) {
        lcd.print('>');
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
        if (input.left || input.right) {
            // Rotate the current menu
            if (input.left) {
                m_current_menu -= 1;
            }
            if (input.right) {
                m_current_menu += 1;
            }
            m_current_menu %= S;
            // Signal that the content has changed
            m_content_changed = true;
        }
    }
    m_menus[m_current_menu]->interact(input);
}
} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_MENU_MANAGER_H
