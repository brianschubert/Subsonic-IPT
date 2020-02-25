//
// Created by brian on 2/23/20.
//

#ifndef SUBSONIC_IPT_UNITS_H
#define SUBSONIC_IPT_UNITS_H

enum class LengthUnit {
    Meters,
    Feet,
    Miles,
    Kilometers,
    LightYears,
//    Steps,
};

constexpr inline LengthUnit ALL_UNITS[]{
    LengthUnit::Meters,
    LengthUnit::Feet,
    LengthUnit::Miles,
    LengthUnit::Kilometers,
    LengthUnit::LightYears
};

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
