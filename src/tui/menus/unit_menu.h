/**
 * unit_menu.h - LCD menu for allowing the user to select their preferred
 * system of units.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_UNIT_MENU_H
#define SUBSONIC_IPT_UNIT_MENU_H

#include "../list_view_menu.h"

namespace subsonic_ipt {

class UnitMenu : public ListViewMenu {

  protected:
    [[nodiscard]]
    LabelStyle label_style() const override;

    [[nodiscard]]
    size_t entry_count() const override;

    [[nodiscard]]
    bool entry_is_active(size_t index) const override;

    void print_entry(char (& entry)[20], size_t index) override;

    void interact_entry(size_t index) override;

  public:
    explicit UnitMenu(IPTState* device_state) : ListViewMenu(device_state) {}

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_UNIT_MENU_H
