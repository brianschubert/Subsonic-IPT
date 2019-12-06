/**
 * navigator.h - Utilities for tracking a device's position and producing
 *               travel directions to known waypoints.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_NAVIGATOR_H
#define SUBSONIC_IPT_NAVIGATOR_H

#include <stddef.h>

#include "point.h"

namespace subsonic_ipt {

/**
 * A Navigator that provides directions to one of several target destinations
 * by maintaining a current position and direction facing.
 */
class Navigator {

    /// The number of destinations that a Navigator should store.
    static inline constexpr size_t DESTINATION_COUNT{4};

    /// The current position of this navigator.
    Point m_current_pos{};

    /// The list of target destination for this navigator.
    Point m_destinations[DESTINATION_COUNT]{{}};

    /// The index of the current target destination for this navigator.
    size_t m_current_dest{0};

    /// The direction this navigator is currently facing.
    Angle m_facing{0};

  public:
    Navigator() = default;

    [[nodiscard]]
    /**
     * Computes the vector relative to this navigator's current position
     * and direction facing that leads to this navigator's target destination.
     */
    Point compute_direction() const;

    [[nodiscard]]
    /**
     * Returns the current position of this navigator.
     */
    Point current_pos() const noexcept
    {
        return m_current_pos;
    }

    [[nodiscard]]
    /**
     * Returns the index of the current destination of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    size_t current_destination_index() const noexcept
    {
        return m_current_dest;
    }

    [[nodiscard]]
    Point current_destination() const noexcept
    {
        return m_destinations[m_current_dest];
    }

    [[nodiscard]]
    Angle current_facing() const noexcept
    {
        return m_facing;
    }

    /**
     * Updates the current position of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    void apply_displacement(Point displacement) noexcept
    {
        m_current_pos = m_current_pos + displacement;
    }

    /**
     * Updates the direction that this navigator is facing.
     *
     * Declared as inline since the implementation is trivial
     */
    void apply_turn(Angle turn)
    {
        m_facing = m_facing + turn;
    }

    /**
     * Rotates the current target destination of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    void cycle_destination() noexcept
    {
        m_current_dest = (m_current_dest + 1) % DESTINATION_COUNT;
    }

    /**
     * Changes the current destination of this navigator to the specified
     * point.
     *
     * Declared as inline since the implementation is trivial
     */
    void overwrite_destination(Point new_dest) noexcept
    {
        m_destinations[m_current_dest] = new_dest;
    }

    void set_facing(Angle facing) noexcept
    {
        m_facing = facing;
    }
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_NAVIGATOR_H
