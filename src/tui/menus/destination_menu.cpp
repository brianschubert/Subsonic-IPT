/**
 * destination_menu.cpp - Implementation for LCD destination menu.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "destination_menu.h"

#include <string.h>

namespace subsonic_ipt {

const char* DestinationMenu::get_menu_name() const noexcept
{
    return "DEST";
}

ListViewMenu::LabelStyle DestinationMenu::label_style() const
{
    return LabelStyle::Number;
}

size_t DestinationMenu::entry_count() const
{
    return m_navigator->destination_count();
}

bool DestinationMenu::entry_is_active(size_t index) const
{
    return index == m_navigator->current_destination_index();
}

void DestinationMenu::print_entry(char (& entry)[20], size_t index)
{
    strcpy(entry + 5, "Waypoint");
}

void DestinationMenu::interact_entry(size_t index)
{
    m_navigator->set_current_destination_index(index);
}
} //namespace subsonic_ipt
