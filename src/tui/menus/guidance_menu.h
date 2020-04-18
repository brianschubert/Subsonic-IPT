/**
 * guidance_menu.h - LCD menu for providing the user with live directions to the
 * currently selected waypoint.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_GUIDANCE_MENU_H
#define SUBSONIC_IPT_GUIDANCE_MENU_H

#include "SerLCD.h"

#include "../menu.h"
#include "../ipt_menu.h"
#include "../../state.h"
#include "../../navigator.h"

namespace subsonic_ipt {

class GuidanceMenu : public IPTMenu {

    /**
     * The global navigator instances for tracking the user's waypoints.
     */
    Navigator* const m_navigator;

    /**
     * The tolerance used when determining whether the user needs to travel
     * "forward" or "backward".
     *
     * Angles within this tolerance of `0` or `pi` radians will result in
     * the direction "snapping" to a forward or backward directions instead of
     * a small rotation direction.
     */
    const Angle m_snap_tolerance;

    /**
     * The tolerance used when determining whether the user has arrived at
     * a waypoint.
     *
     * Whenever the user is within this distance in meters of a waypoint, they
     * will receive a "you have arrived" message instead of a direction.
     */
    const double m_arrival_tolerance;

    /**
     * The time in milliseconds that this screen should wait before signalling
     * for a refresh.
     */
    const unsigned long m_refresh_timeout;

    /**
     * The time in milliseconds since startup when this screen was last
     * refreshed.
     */
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

    [[nodiscard]]
    bool content_changed() const override;

  private:
    /**
     * Prints the waypoint title to the lcd screen that is common to all
     * direction messages.
     */
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
