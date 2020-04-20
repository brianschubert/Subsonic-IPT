/**
 * debug_menu.h - LCD menu for displaying live readouts of the device's state.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_DEBUG_MENU_H
#define SUBSONIC_IPT_DEBUG_MENU_H

#include "../list_view_menu.h"

namespace subsonic_ipt {

class DebugMenu : public ListViewMenu {
    /**
     * The during in milliseconds that this screen should wait before
     * signalling for a refresh.
     */
    const unsigned long m_refresh_timeout;

    /**
     * The time in milliseconds since startup when this screen was last
     * refreshed.
     */
    unsigned long m_last_refresh{};

  protected:
    [[nodiscard]]
    LabelStyle label_style() const override;

    [[nodiscard]]
    size_t entry_count() const override;

    [[nodiscard]]
    bool entry_is_active(size_t index) const override;

    void print_entry(char (& entry)[20], size_t index) override;

    void interact_entry(size_t index) override {}

  public:
    explicit DebugMenu(IPTState* device_state, unsigned long refresh_timeout)
        : ListViewMenu(device_state),
          m_refresh_timeout(refresh_timeout),
          m_last_refresh(0) {}

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

    void refresh_display(SerLCD& lcd) override;

    [[nodiscard]]
    bool content_changed() const override;
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_UNIT_MENU_H
