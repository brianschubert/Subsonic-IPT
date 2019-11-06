//
// Created by brian on 11/2/19.
//

#ifndef SUBSONIC_IPT_NAVIGATOR_H
#define SUBSONIC_IPT_NAVIGATOR_H

#include <stddef.h>

#include "point.h"

namespace subsonic_ipt {

/// The number of destinations that a Navigator should store.
constexpr size_t DESTINATION_COUNT{4};

/**
 * A Navigator that provides directions to one of several target destinations
 * by maintaining a current position and direction facing.
 */
class Navigator {

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

    /**
     * Updates the current position of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    void update_position(Point displacement)
    {
        m_current_pos = m_current_pos + displacement;
    }

    /**
     * Updates the direction that this navigator is facing.
     *
     * Declared as inline since the implementation is trivial
     */
    void update_facing(Angle turn)
    {
        m_facing = m_facing + turn;
    }

    /**
     * Rotates the current target destination of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    void cycle_destination()
    {
        m_current_dest = (m_current_dest + 1) % DESTINATION_COUNT;
    }

    [[nodiscard]]
    /**
     * Returns the index of the current destination of this navigator.
     *
     * Declared as inline since the implementation is trivial
     */
    size_t current_destination_index() const
    {
        return m_current_dest;
    }

    Point current_destination() const
    {
        return m_destinations[m_current_dest];
    }

    Angle current_facing() const {
        return m_facing;
    }

    /**
     * Changes the current destination of this navigator to the specified
     * point.
     *
     * Declared as inline since the implementation is trivial
     */
    void overwrite_destination(Point new_dest)
    {
        m_destinations[m_current_dest] = new_dest;
    }

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
    Point current_pos() const
    {
        return m_current_pos;
    }
};

} // namespace subsonic_ipt

#endif //SUBSONIC_IPT_NAVIGATOR_H
