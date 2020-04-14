//
// Created by brian on 1/21/20.
//

#ifndef SUBSONIC_IPT_GUIDANCE_MENU_H
#define SUBSONIC_IPT_GUIDANCE_MENU_H

#include "SerLCD.h"

#include "../menu.h"
#include "../ipt_menu.h"
#include "../../state.h"
#include "../../navigator.h"

namespace subsonic_ipt {

class GuidanceMenu : public IPTMenu {

    Navigator* const m_navigator;

    const Angle m_snap_tolerance;

    const double m_arrival_tolerance;

    const unsigned long m_refresh_timeout;

    unsigned long m_last_refresh;

  public:
    explicit GuidanceMenu(
        IPTState* device_state,
        Navigator* navigator,
        Angle snap_tolerance,
        double arrival_tolerance,
        unsigned long refresh_timeout
    )
        : IPTMenu(device_state),
          m_navigator(navigator),
          m_snap_tolerance(snap_tolerance),
          m_arrival_tolerance(arrival_tolerance),
          m_refresh_timeout(refresh_timeout),
          m_last_refresh(0) {}

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

    bool content_changed() const override;

  private:
    void print_screen_title(SerLCD& lcd)
    {
        lcd.print("Navigating to");
        lcd.setCursor(17, 1);
        lcd.print("#");
        lcd.print(m_navigator->current_destination_index());
    }

};
} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_GUIDANCE_MENU_H
