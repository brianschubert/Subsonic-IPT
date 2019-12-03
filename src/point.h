/**
 * point.h - Utilities for working with points in 2D space.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_POINT_H
#define SUBSONIC_IPT_POINT_H
#include <math.h> // cmath not available

namespace subsonic_ipt {

/**
 * A POD angle in two-dimensional space.
 */
struct Angle {
    /// This angle expressed in radians.
    double m_rad;

    [[nodiscard]]
    /// Returns this angle expressed in degrees.
    double deg() const
    {
        return m_rad * 180.0 / M_PI;
    }

    [[nodiscard]]
    /// Returns true is this angle's value is NaN.
    bool is_nan() const
    {
        return isnan(m_rad);
    }

    /**
     * Normalizes this angles value to a radian on the interval [0, 2pi).
     */
    Angle& normalize()
    {
        m_rad = fmod(m_rad, 2 * M_PI);
        if (m_rad < 0) {
            m_rad += 2 * M_PI;
        }
        return *this;
    }

    [[nodiscard]]
    /**
     * Returns the conjugate of this angle.
     */
    Angle conjugate() {
        auto conj = Angle{-m_rad};
        conj.normalize();
        return conj;
    }

    [[nodiscard]]
    /**
     * Constructs an angle with the given value in degrees.
     */
    static Angle from_degrees(double degrees)
    {
        return Angle{degrees * M_PI / 180}.normalize();
    }

    friend Angle operator+(Angle first, Angle second)
    {
        return Angle{first.m_rad + second.m_rad}.normalize();
    }

    friend Angle operator-(Angle first, Angle second)
    {
        return Angle{first.m_rad - second.m_rad}.normalize();
    }

    friend bool operator<(Angle first, Angle second)
    {
        return first.m_rad < second.m_rad;
    }

    friend bool operator>(Angle first, Angle second)
    {
        return first.m_rad > second.m_rad;
    }

    Angle operator-() const
    {
        return {-m_rad};
    }
};

/**
 * A POD representation of a point in two-dimensional space.
 */
struct Point {

    /// The horizontal displacement of this point.
    double m_x;

    /// The vertical displacement of this point.
    double m_y;

    [[nodiscard]]
    /// Returns the norm of the R2 vector associated with this point.
    double norm() const
    {
        return sqrt(m_x * m_x + m_y * m_y);
    }

    /**
     * Returns the euclidean distance between this point and the specified
     * point.
     */
    double dist_to(Point other)
    {
        return (*this - other).norm();
    }

    [[nodiscard]]
    /**
     * Returns the angle between the vector associated with this point and
     * the +x axis.
     */
    Angle angle() const
    {
        if (m_x == 0) {
            if (m_y == 0) {
                return {NAN};
            }
            return Angle{
                (m_y > 0
                    ? M_PI / 2
                    : M_PI * 3.0 / 2.0
                )
            };
        }

        double offset = atan(m_y / m_x);

        if (m_x > 0 && m_y > 0) {
            // This point is in Q1.
            return {offset};
        } else if (m_x < 0) {
            // This point is in Q2 or Q3.
            return {offset + M_PI};
        } else {
            // This point is in Q4.
            return {offset + 2 * M_PI};
        }
    }

    /**
     * Constructs a point that corresponds to the unit vector with the given
     * angle between itself and the +x axis.
     */
    static Point unit_from_angle(Angle angle)
    {
        return {cos(angle.m_rad), sin(angle.m_rad)};
    }

    friend Point operator+(Point first, Point second)
    {
        return {first.m_x + second.m_x, first.m_y + second.m_y};
    }

    friend Point operator-(Point first, Point second)
    {
        return first + -second;
    }

    friend Point operator*(double scalar, Point point)
    {
        return {scalar * point.m_x, scalar * point.m_y};
    }

    Point operator-() const
    {
        return {-m_x, -m_y};
    }
};

} // namespace subsonic_ipt
#endif //SUBSONIC_IPT_POINT_H
