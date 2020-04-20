/**
 * list_menu.cpp - Implementation for menus that display lists of results.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "list_view_menu.h"

// TODO: refactor all instances of screen width into a single configuration

/**
 * The maximum screen width that list entries should occupy.
 */
constexpr size_t DISPLAY_WIDTH = 20;

void subsonic_ipt::ListViewMenu::refresh_display(SerLCD& lcd)
{
    m_content_changed = false;

    const size_t max_index = entry_count() - 1;

    // Determine which entries should be printed
    const size_t top_entry = m_selected_entry == max_index
        ? max(m_selected_entry - 2, 0)
        : m_selected_entry == 0 ? 0 : m_selected_entry - 1;
    const size_t bottom_entry = min(top_entry + 2, entry_count() - 1);

    char entry_buffer[DISPLAY_WIDTH];

    for (size_t index = top_entry; index <= bottom_entry; ++index) {
        for (char& i : entry_buffer) {
            i = ' ';
        }
        if (index == m_selected_entry) {
            entry_buffer[0] = '>';
        }

        const auto style = label_style();
        if (style != LabelStyle::None) {
            if (entry_is_active(index)) {
                entry_buffer[1] = '(';
                entry_buffer[3] = ')';
            }
            if (style == LabelStyle::Number) {
                entry_buffer[2] = static_cast<char>(index) + 48;
            } else if (style == LabelStyle::Bullet) {
                entry_buffer[2] = '*';
            }
        }

        print_entry(entry_buffer, index);
        lcd.println(entry_buffer);
    }

}

void subsonic_ipt::ListViewMenu::interact(const subsonic_ipt::Menu::Input& input)
{
    if (input.down) {
        m_selected_entry += 1;
        m_content_changed = true;
    }
    if (input.up) {
        m_selected_entry -= 1;
        m_content_changed = true;
    }
    m_selected_entry %= entry_count();

    if (input.enter) {
        interact_entry(m_selected_entry);
        m_content_changed = true;
    }
}
