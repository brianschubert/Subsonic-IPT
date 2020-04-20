/**
 * list_menu.h - Abstract class for menus that display content as a list of
 * entries.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_LIST_VIEW_MENU_H
#define SUBSONIC_IPT_LIST_VIEW_MENU_H

#include "SerLCD.h"
#include "ipt_menu.h"

namespace subsonic_ipt {

class ListViewMenu : public IPTMenu {
  private:
    /**
     * The index of the entry currently selected by the user.
     */
    size_t m_selected_entry{0};

  protected:
    /**
     * The possible styles for displaying list menu entries.
     */
    enum class LabelStyle {
        Number,
        Bullet,
        None
    };

    [[nodiscard]]
    /**
     * Returns the style of list entries that this menu should display.
     */
    virtual LabelStyle label_style() const = 0;

    [[nodiscard]]
    /**
     * Returns the number of entries that this menu has.
     */
    virtual size_t entry_count() const = 0;

    [[nodiscard]]
    /**
     * Returns `true` if the entry at the given index is "active" in in this
     * menu.
     */
    virtual bool entry_is_active(size_t index) const = 0;

    /**
     * Writes the contents of the entry associated with the given index to the
     * given character buffer.
     */
    virtual void print_entry(char (& entry)[20], size_t index) = 0;

    /**
     * Performs the action associated with the given entry.
     */
    virtual void interact_entry(size_t index) = 0;

  public:
    explicit ListViewMenu(IPTState* device_state) : IPTMenu(device_state) {}

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_LIST_VIEW_MENU_H
