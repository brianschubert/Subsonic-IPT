//
// Created by brian on 1/21/20.
//

#ifndef SUBSONIC_IPT_MENU_MANAGER_H
#define SUBSONIC_IPT_MENU_MANAGER_H

#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "menu.h"
#include "SerLCD.h"
#include "../state.h"
#include "ipt_menu.h"

namespace subsonic_ipt {
class MenuManager : public IPTMenu {
    constexpr static inline size_t MAX_MENUS = 8;

    constexpr static inline size_t MAX_MENU_NAME_LEN = 5;

    Menu* m_menus[MAX_MENUS];

    size_t m_menu_count;

    size_t m_current_menu{0};

  public:
    MenuManager(IPTState* device_state, Menu** menus, size_t menu_count)
        : IPTMenu(device_state),
          m_menus{nullptr},
          m_menu_count(menu_count)
    {
        assert(menu_count > 0 && menu_count <= MAX_MENUS);
        memcpy(m_menus, menus, sizeof(Menu) * menu_count);
    }

    void refresh_display(SerLCD& lcd) override;

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

    void interact(const Input& input) override;

};
} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_MENU_MANAGER_H
