/**
 * unit_menu.cpp - Implementation for the LCD unit menu.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#include "unit_menu.h"

#include <string.h>

#include "../../units.h"

namespace subsonic_ipt {

ListViewMenu::LabelStyle UnitMenu::label_style() const
{
    return LabelStyle::Number;
}

size_t UnitMenu::entry_count() const
{
    return sizeof(ALL_UNITS) / sizeof(LengthUnit);
}

bool UnitMenu::entry_is_active(size_t index) const
{
    return ALL_UNITS[index] == m_device_state->localized_unit;
}

void UnitMenu::print_entry(char (& entry)[20], size_t index)
{
    char* const name_start = (entry + 5);
    switch (ALL_UNITS[index]) {
        case LengthUnit::Meters: {
            strcpy(name_start, "Meters");
            break;
        }
        case LengthUnit::Feet: {
            strcpy(name_start, "Feet");
            break;
        }
        case LengthUnit::Miles: {
            strcpy(name_start, "Miles");
            break;
        }
        case LengthUnit::Kilometers: {
            strcpy(name_start, "Kilometers");
            break;
        }
        case LengthUnit::LightYears: {
            strcpy(name_start, "Light years");
            break;
        }
    }
}

void UnitMenu::interact_entry(size_t index)
{
    m_device_state->localized_unit = ALL_UNITS[index];
}

const char* UnitMenu::get_menu_name() const noexcept
{
    return "UNIT";
}
} // namespace subsonic_ipt
