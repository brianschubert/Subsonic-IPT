//
// Created by brian on 2/20/20.
//

#include "list_view_menu.h"

namespace {
constexpr size_t DISPLAY_WIDTH = 20;
}

void subsonic_ipt::ListViewMenu::refresh_display(SerLCD& lcd)
{
    const size_t max_index = entry_count() - 1;
    const size_t entries_to_print = min(3, entry_count() + 1);

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
                entry_buffer[2] = index;
            } else if (style == LabelStyle::Bullet) {
                entry_buffer[2] = '*';
            }
        }

        print_entry(entry_buffer, index);
        lcd.write(entry_buffer);
        lcd.write("\r\n");
    }

}

void subsonic_ipt::ListViewMenu::interact(const subsonic_ipt::Menu::Input& input)
{
    if (input.down) {
        m_selected_entry += 1;
    }
    if (input.up) {
        m_selected_entry -= 1;
    }
    m_selected_entry %= entry_count();

    if (input.enter) {
        interact_entry(m_selected_entry);
    }
}
