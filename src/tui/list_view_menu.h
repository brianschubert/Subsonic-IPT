//
// Created by brian on 2/20/20.
//

#ifndef SUBSONIC_IPT_LIST_VIEW_MENU_H
#define SUBSONIC_IPT_LIST_VIEW_MENU_H

#include "SerLCD.h"
#include "ipt_menu.h"

namespace subsonic_ipt {

class ListViewMenu : public IPTMenu {
  private:
    size_t m_selected_entry{0};

  protected:
    enum class LabelStyle {
        Number,
        Bullet,
        None
    };

    [[nodiscard]]
    virtual LabelStyle label_style() const = 0;

    [[nodiscard]]
    virtual size_t entry_count() const = 0;

    [[nodiscard]]
    virtual bool entry_is_active(size_t index) const = 0;

    virtual void print_entry(char (& entry)[20], size_t index) = 0;

    virtual void interact_entry(size_t index) = 0;

  public:
    explicit ListViewMenu(IPTState* device_state) : IPTMenu(device_state) {}

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_LIST_VIEW_MENU_H
