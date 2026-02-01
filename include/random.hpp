/*
 * Copyright (C) 2026 Omega493

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SELENA_RANDOM_HPP
#define SELENA_RANDOM_HPP

#include <vector>
#include <array>
#include <random>

namespace selena {
class random_prng {
public:
  // Since a "random_prng" object can't even be initialized, copy / move is by default blocked.
  // "Construction" occurs when a thread calls any of the four functions here, which internally
  // calls "_impl_prng_engine()" - "_impl_prng_engine()" does the actual "construction" work.
  // For dtor, since a "random_prng" obj. was never initialized, ~random_prng() doesn't exist.
  // As for a general "cleanup", cleanup occurs when the thread accessing this class returns.
  // That is wjhy we have "static thread_local _" in the private section.
  random_prng() = delete;

  /*
   * Usage: "random(vec);"
   * @param vec A reference to a std::vector object
   * @return T A copy value randomly picked from the given vector
   */
  template<typename T>
  static T random(const std::vector<T>& vec) {
    if (vec.empty()) return {};
    std::uniform_int_distribution<size_t> distribution{ 0, vec.size() - 1 };
    return vec[distribution(_impl_prng_engine())];
  }

  /*
   * Usage "random(vec, x)"
   * @param vec A reference to a std::vector obj.
   * @param count A size_t number specifying the number of elements to be generated
   * @returns std::vector<T> A std::vector<T> object
   */
  template<typename T>
  static std::vector<T> random(const std::vector<T>& vec, const size_t count) {
    if (vec.empty()) return {};
    if (!count) return {};
    if (count == 1) return { random(vec) };

    std::vector<T> ret_vec{};
    ret_vec.reserve(count);

    std::uniform_int_distribution<size_t> distribution{ 0, vec.size() - 1 };
    std::mt19937_64& engine{ _impl_prng_engine() };

    for (size_t i{ 0 }; i < count; ++i) ret_vec.push_back(vec[distribution(engine)]);
    return ret_vec;
  }

  /*
   * Usage: "random(arr)"
   * @param arr A reference to a std::array obj.
   * @returns T A copy value randomly picked from the given array
   */
  template<typename T, size_t N>
  static T random(const std::array<T, N>& arr) {
    if (arr.empty()) return {};
    std::uniform_int_distribution<size_t> distribution{ 0, arr.size() - 1 };
    return arr[distribution(_impl_prng_engine())];
  }

  /*
   * Usage: "random<x>(arr)"
   * @param arr A reference to a std::array obj.
   * @returns std::array<T, x> A std::array object
   */
  template<size_t Count, typename T, size_t N>
  static std::array<T, Count> random(const std::array<T, N>& arr) {
    if (arr.empty()) return {};
    if constexpr (!Count) return {};

    std::array<T, Count> ret_arr{};
    std::uniform_int_distribution<size_t> distribution{ 0, arr.size() - 1 };
    std::mt19937_64& engine{ _impl_prng_engine() };

    for (size_t i{ 0 }; i < Count; ++i) ret_arr[i] = arr[distribution(engine)];
    return ret_arr;
  }

private:
  static inline std::mt19937_64& _impl_prng_engine() {
    static thread_local std::random_device random_device_seed{};
    static thread_local std::mt19937_64 generator{ random_device_seed() };
    return generator;
  }
}; // class random_prng

// You likely do not need to use this class, unless you're doing something related to cryptography.
// All four functions here repeatedly trigger system calls, which is slower than using a seed.
// The "cost" is near-negligible if you're triggering this once in a while. But, if you're calling this
// repeatedly, or using the functions which return std::array or std::vector, the cost quick adds up. Why?
// because each call to any of the four functions, incl. every single loop iteration of the ones which
// return array / vector, you are forcing the CPU to fetch the OS's entropy pool for a truly random integer.
// Moreover, you shouldn't use this even for "cryptographic" purposes to begin with - there are a lot more
// "battle-tested" libraries available, such as OpenSSL and libsodium.
class random_trng {
public:
  // Since a "random_trng" object can't even be initialized, copy / move is by default blocked.
  // "Construction" occurs when a thread calls any of the four functions here, which internally
  // calls "_impl_trng_engine()" - it does the actual "construction" work.
  // For dtor, since a "random_trng" obj. was never initialized, ~random_trng() doesn't exist.
  // As for a general "cleanup", cleanup occurs when the thread accessing this class returns.
  // That is why we have "static thread_local _" in the private section.
  random_trng() = delete;

  /*
   * Usage: "random(vec);"
   * @param vec A reference to a std::vector obj.
   * @return T A copy value randomly picked from the given vector
   */
  template<typename T>
  static T random(const std::vector<T>& vec) {
    if (vec.empty()) return {};
    std::uniform_int_distribution<size_t> distribution{ 0, vec.size() - 1 };
    return vec[distribution(_impl_trng_engine())];
  }

  /*
   * Usage "random(vec, x)"
   * @param vec A reference to a std::vector obj.
   * @param count A size_t number specifying the number of elements to be generated
   * @returns std::vector<T> A std::vector<T> object
   */
  template<typename T>
  static std::vector<T> random(const std::vector<T>& vec, const size_t count) {
    if (vec.empty()) return {};
    if (!count) return {};
    if (count == 1) return { random(vec) };

    std::vector<T> ret_vec{};
    ret_vec.reserve(count);

    std::uniform_int_distribution<size_t> distribution{ 0, vec.size() - 1 };
    std::random_device& engine{ _impl_trng_engine() };

    for (size_t i{ 0 }; i < count; ++i) ret_vec.push_back(vec[distribution(engine)]);
    return ret_vec;
  }

  /*
   * Usage: "random(arr)"
   * @param arr A reference to a std::array obj.
   * @returns T A copy value randomly picked from the given array
   */
  template<typename T, size_t N>
  static T random(const std::array<T, N>& arr) {
    if (arr.empty()) return {};
    std::uniform_int_distribution<size_t> distribution{ 0, arr.size() - 1 };
    return arr[distribution(_impl_trng_engine())];
  }

  /*
   * Usage: "random<x>(arr)"
   * @param arr A reference to a std::array obj.
   * @returns std::array<T, x> A std::array object
   */
  template<size_t Count, typename T, size_t N>
  static std::array<T, Count> random(const std::array<T, N>& arr) {
    if (arr.empty()) return {};
    if constexpr (!Count) return {};

    std::array<T, Count> ret_arr{};
    std::uniform_int_distribution<size_t> distribution{ 0, arr.size() - 1 };
    std::random_device& engine{ _impl_trng_engine() };

    for (size_t i{ 0 }; i < Count; ++i) ret_arr[i] = arr[distribution(engine)];
    return ret_arr;
  }

private:
  static inline std::random_device& _impl_trng_engine() {
    static thread_local std::random_device device{};
    return device;
  }
}; // class "random_trng"
} // namespace selena

#endif // SELENA_RANDOM_HPP
