/**
 * destination_menu.h - LCD menu to selecting which waypoint the users wishes
 * to navigate to.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_DESTINATION_MENU_H
#define SUBSONIC_IPT_DESTINATION_MENU_H

#include "../../navigator.h"
#include "../list_view_menu.h"

namespace subsonic_ipt {
class DestinationMenu : public ListViewMenu {
    Navigator* const m_navigator;

  public:
    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

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
    DestinationMenu(IPTState* device_state, Navigator* const navigator)
        : ListViewMenu(device_state),
          m_navigator(navigator) {}

};

} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_DESTINATION_MENU_H
