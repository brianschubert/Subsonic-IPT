/**
 * Basic hand-crafted utilities to substitute functionalities of the C++ standard
 * library, which this Arduino sketch cannot link to.
 *
 * Copyright (c) 2019 Brian Schubert
 *
 * This file is distributed under the MIT License. If a copy of the
 * MIT License was not distributed with this file, you can obtain one
 * at https://opensource.org/licenses/MIT.
 */


#ifndef SUBSONIC_IPT_STDFIX_H
#define SUBSONIC_IPT_STDFIX_H

/**
 * Imitation of std::move.
 *
 * Has the same effect as casting to an r-value reference.
 * Let's the compiler know that the argument's lifetime is about
 * to end, so it can be safely moved as opposed to copied.
 */
template<typename T>
constexpr T&& std_move(T&& type) noexcept
{
    return static_cast<T&&>(type);
}

#endif //SUBSONIC_IPT_STDFIX_H
