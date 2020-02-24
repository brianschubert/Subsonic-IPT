//
// Created by brian on 1/21/20.
//

#include "menu_manager.h"

#include <stdio.h>

namespace {

void lcd_print_title(SerLCD& lcd, const char* title, bool current);
}

namespace subsonic_ipt {

void MenuManager::refresh_display(SerLCD& lcd)
{
    lcd.clear();
    lcd.setCursor(0, 0);

    if (m_menu_count < 3) {
        char title_row[20];
        if (m_menu_count == 1) {
            sprintf(title_row, "       %-5s        ", m_menus[0]->get_menu_name());
        } else if (m_menu_count == 2) {
            sprintf(title_row, "   %-5s    %-5s  ", m_menus[0]->get_menu_name(), m_menus[1]->get_menu_name());
        }
        lcd.write(title_row);
    } else {
        const bool menu_left = m_current_menu == 0;
        const bool menu_right = m_current_menu == m_menu_count - 1;

        if (m_current_menu > 1) {
            lcd.write('<');
        }
        lcd_print_title(lcd, m_menus[m_current_menu]->get_menu_name(), menu_left);
        lcd.write(' ');
        lcd_print_title(lcd, m_menus[m_current_menu + 1]->get_menu_name(), !(menu_left || menu_right));
        lcd.write(' ');
        lcd_print_title(lcd, m_menus[m_current_menu + 2]->get_menu_name(), menu_right);

        if (m_menu_count < m_menu_count - 2) {
            lcd.write('>');
        }
    }

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

void lcd_print_title(SerLCD& lcd, const char* title, bool current)
{
    if (current) {
        lcd.write('[');
    }
    lcd.write(title);
    if (current) {
        lcd.write(']');
    }
}
}
