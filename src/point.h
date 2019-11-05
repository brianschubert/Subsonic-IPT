//
// Created by brian on 11/2/19.
//

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
        return M_PI * m_rad / 180.0;
    }

    static Angle from_degrees(double degrees)
    {
        return {degrees * 180 / M_PI};
    }

    friend Angle operator+(Angle first, Angle second)
    {
        return {first.m_rad + second.m_rad};
    }

    friend Angle operator-(Angle first, Angle second)
    {
        return {first.m_rad - second.m_rad};
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
        return sqrt(m_x * m_x + m_x * m_y);
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
    Angle angle() const
    {
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
