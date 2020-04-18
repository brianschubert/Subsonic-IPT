/**
 * ipt_menu.h - Abstract class for mensu that need to reference the IPT device
 * state.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_IPT_MENU_H
#define SUBSONIC_IPT_IPT_MENU_H

#include "menu.h"
#include "../state.h"

namespace subsonic_ipt {

class IPTMenu : public Menu {
  protected:
    /**
     * The current state of the IPT device.
     */
    IPTState* const m_device_state{};

  public:
    explicit IPTMenu(IPTState* device_state) : m_device_state(device_state) {}
};

}

#endif //SUBSONIC_IPT_IPT_MENU_H
