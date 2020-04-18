/**
 * debug_menu.cpp - Implementation for LCD debug menu.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "debug_menu.h"

namespace subsonic_ipt {

ListViewMenu::LabelStyle DebugMenu::label_style() const
{
    return LabelStyle::Bullet;
}

size_t DebugMenu::entry_count() const
{
    return 4;
}

bool DebugMenu::entry_is_active(size_t index) const
{
    return false;
}

void DebugMenu::print_entry(char (& entry)[20], size_t index)
{
    switch (index) {
        case 0: {
            sprintf(entry + 5, "T: %10lu", millis());
            break;
        }
        case 1: {
            sprintf(entry + 5,
                "Agl: %3d",
                static_cast<int>(m_device_state->facing.deg()));
            break;
        }

        case 2: {
            sprintf(entry + 5,
                "%4d,%4d",
                static_cast<int>(m_device_state->position.m_x),
                static_cast<int>(m_device_state->position.m_y)
            );
            break;
        }
        case 3: {
            sprintf(entry + 5,
                "%3d,%3d,%3d",
                static_cast<int>(m_device_state->device_motion.yaw * 10),
                static_cast<int>(m_device_state->device_motion.pitch),
                static_cast<int>(m_device_state->device_motion.roll)
            );
        }

    }
}

const char* DebugMenu::get_menu_name() const noexcept
{
    return "DBUG";
}

void DebugMenu::refresh_display(SerLCD& lcd)
{
    // Update the last-refreshed timestamp during each screen refresh.
    m_last_refresh = millis();
    ListViewMenu::refresh_display(lcd);
}

bool DebugMenu::content_changed() const
{
    // Signal that this screens content has changed if the refresh timeout has
    // expired or if the content changed flag has been manually toggled.
    return (millis() - m_last_refresh) >= m_refresh_timeout || Menu::content_changed();
}
} // namespace subsonic_ipt
