//
// Created by brian on 2/23/20.
//

#ifndef SUBSONIC_IPT_IPT_MENU_H
#define SUBSONIC_IPT_IPT_MENU_H

#include "menu.h"
#include "../state.h"

namespace subsonic_ipt {

class IPTMenu : public Menu {
  protected:
    IPTState* const m_device_state{};

  public:
    explicit IPTMenu(IPTState* device_state) : m_device_state(device_state) {}
};

}

#endif //SUBSONIC_IPT_IPT_MENU_H
