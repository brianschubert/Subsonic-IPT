//
// Created by brian on 2/25/20.
//

#include "debug_menu.h"

namespace subsonic_ipt {

ListViewMenu::LabelStyle DebugMenu::label_style() const
{
    return LabelStyle::Bullet;
}

size_t DebugMenu::entry_count() const
{
    return 3;
}

bool DebugMenu::entry_is_active(size_t index) const
{
    return false;
}

void DebugMenu::print_entry(char (& entry)[20], size_t index)
{
    switch (index) {
        case 0: {
            sprintf(entry + 5, "%10lu", millis());
            break;
        }
        case 1: {
            sprintf(entry + 5,
                "%3d",
                static_cast<int>(m_device_state->facing.deg()));
            break;
        }

        case 2: {
            sprintf(entry + 5,
                "%4d,%4d",
                static_cast<int>(m_device_state->position.m_x),
                static_cast<int>(m_device_state->position.m_y)
                );
//            sprintf(entry + 5,
//                "%3d,%3d,%3d",
//                static_cast<int>(m_device_state->device_motion.yaw),
//                static_cast<int>(m_device_state->device_motion.pitch),
//                static_cast<int>(m_device_state->device_motion.roll)
//                );
            break;
        }

    }
}

const char* DebugMenu::get_menu_name() const noexcept
{
    return "DBUG";
}
} // namespace subsonic_ipt
