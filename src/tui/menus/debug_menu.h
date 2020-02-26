//
// Created by brian on 2/25/20.
//

#ifndef SUBSONIC_IPT_DEBUG_MENU_H
#define SUBSONIC_IPT_DEBUG_MENU_H

#include "../list_view_menu.h"

namespace subsonic_ipt {

class DebugMenu : public ListViewMenu {

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
    explicit DebugMenu(IPTState* device_state) : ListViewMenu(device_state) {}

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_UNIT_MENU_H
