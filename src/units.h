/**
 * unit.h - Definitions for handling lengths in various units of measurement.
 *
 * Copyright (c) 2020 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */

#ifndef SUBSONIC_IPT_UNITS_H
#define SUBSONIC_IPT_UNITS_H

/**
 * The units of length that the user may select.
 */
enum class LengthUnit {
    Meters,
    Feet,
    Miles,
    Kilometers,
    LightYears,
//    Steps,
};

/**
 * All supported units of length.
 */
constexpr inline LengthUnit ALL_UNITS[]{
    LengthUnit::Meters,
    LengthUnit::Feet,
    LengthUnit::Miles,
    LengthUnit::Kilometers,
    LengthUnit::LightYears
};

/**
 * Converts the given distance in meters to the specified units.
 *
 * Marked as inline to allow for multiple definition with external linkage.
 */
constexpr inline double meters_to_unit(double meters, LengthUnit new_units)
{
    switch (new_units) {
        case LengthUnit::Meters: return meters;
        case LengthUnit::Feet: return meters * 3.28084;
        case LengthUnit::Miles: return meters * 0.000621371;
        case LengthUnit::Kilometers:return meters * 0.001;
        case LengthUnit::LightYears: return meters * 1.057e-16;
    }
}

/**
 * Returns the symbol associated with the given unit of length.
 *
 * Marked as inline to allow for multiple definitions with external linkage.
 */
constexpr inline const char* unit_symbol(LengthUnit unit)
{
    switch (unit) {
        case LengthUnit::Meters: return "m";
        case LengthUnit::Feet: return "ft";
        case LengthUnit::Miles: return "mi";
        case LengthUnit::Kilometers:return "km";
        case LengthUnit::LightYears: return "ly";
    }
}

#endif //SUBSONIC_IPT_UNITS_H
