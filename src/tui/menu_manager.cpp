//
// Created by brian on 1/21/20.
//

#include "menu_manager.h"

#include <stdio.h>

namespace {

void lcd_print_title(SerLCD& lcd, const char* title, bool current, size_t count = 5);
}

namespace subsonic_ipt {

void MenuManager::refresh_display(SerLCD& lcd)
{
    lcd.setCursor(0, 0);

    // Write the row of menu title to the display.
    // We explicitly handle all cases where there are three or less menus being managed.
    if (m_menu_count == 1) {
        lcd_print_title(lcd, m_menus[0]->get_menu_name(), true);
    } else if (m_menu_count == 2) {
        lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
        lcd.write(' ');
        lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
    } else if (m_menu_count == 3) {
        lcd_print_title(lcd, m_menus[0]->get_menu_name(), m_current_menu == 0);
        lcd.write(' ');
        lcd_print_title(lcd, m_menus[1]->get_menu_name(), m_current_menu == 1);
        lcd.write(' ');
        lcd_print_title(lcd, m_menus[2]->get_menu_name(), m_current_menu == 2);
    } else {
        // Whether the current menu is the leftmost option.
        const bool end_left = m_current_menu == 0;
        // Whether the current menu is the rightmost option.
        const bool end_right = m_current_menu == m_menu_count - 1;
        // Whether to print a scroll arrow on the LHS of the display.
        const bool arrow_left = m_current_menu > 1;
        // Whether to print a scroll arrow on the RHS of the display
        const bool arrow_right = m_current_menu < m_menu_count - 2;

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
    }

    // Allow the submenu to produce the output for the remaining rows.
    lcd.setCursor(1, 0);
    m_menus[m_current_menu]->refresh_display(lcd);
}

const char* MenuManager::get_menu_name() const noexcept
{
    return "MNGR";
}

void MenuManager::interact(const Menu::Input& input)
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
        m_current_menu = (m_current_menu + menu_offset) % m_menu_count;
    }
    m_menus[m_current_menu]->interact(input);
}
} // namespace subsonic_ipt

namespace {

void lcd_print_title(SerLCD& lcd, const char* title, bool current, size_t count)
{
    if (current) {
        lcd.write('[');
    }
    lcd.write(reinterpret_cast<const uint8_t*>(title), count);
    if (current) {
        lcd.write(']');
    }
}
}
