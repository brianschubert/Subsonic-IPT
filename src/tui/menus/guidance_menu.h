//
// Created by brian on 1/21/20.
//

#ifndef SUBSONIC_IPT_GUIDANCE_MENU_H
#define SUBSONIC_IPT_GUIDANCE_MENU_H

#include "../../stdfix.h"
#include "../../state.h"
#include "../../navigator.h"
#include "../menu.h"
#include "SerLCD.h"
#include "../ipt_menu.h"

namespace subsonic_ipt {

class GuidanceMenu : public IPTMenu {

    Navigator* const m_navigator;

    const Angle m_snap_tolerance;

    const double m_arrival_tolerance;

  public:
    explicit GuidanceMenu(IPTState* device_state, Navigator* navigator, Angle snap_tolerance, double arrival_tolerance)
        : IPTMenu(device_state),
          m_navigator(navigator),
          m_snap_tolerance(snap_tolerance),
          m_arrival_tolerance(arrival_tolerance) {}

    [[nodiscard]]
    const char* get_menu_name() const noexcept override;

    void refresh_display(SerLCD& lcd) override;

    void interact(const Input& input) override;

  private:
    void print_screen_title(SerLCD& lcd)
    {
        lcd.write("To #");
        lcd.write(m_navigator->current_destination_index());
        lcd.write("(");
        const auto destination = m_navigator->current_destination();
        lcd.write(static_cast<int>(destination.m_x));
        lcd.write(",");
        lcd.write(static_cast<int>(destination.m_y));
        lcd.write(")");
    }

};
} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_GUIDANCE_MENU_H
