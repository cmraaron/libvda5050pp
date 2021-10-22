// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// TODO: Short description
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_MATH_GEOMETRY
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_MATH_GEOMETRY

#include <cmath>

namespace vda5050pp::core::common::math {

///
/// \brief 2 Dimensional vector
///
/// \tparam Num numeric type
///
template <typename Num> struct Vector2 {
  Num x;
  Num y;
};

///
/// \brief Circle type
///
/// \tparam Num numeric type
///
template <typename Num> struct Circle {
  Vector2<Num> origin;
  Num radius;
};

///
/// \brief Calculate the (euclid) norm of a vector
///
/// \tparam Floating the number type (must be floating)
/// \param vec the vector
///
template <typename Floating> constexpr auto norm(const Vector2<Floating> &vec) {
  static_assert(std::is_floating_point_v<Floating>);
  auto x2 = vec.x * vec.x;
  auto y2 = vec.y * vec.y;
  return std::sqrt(x2 + y2);
}

///
/// \brief Calculate the euclid distance between two points (pointed to by vectors)
///
/// \tparam Num the number type (must be floating)
/// \param v1 vector 1
/// \param v2 vector 2
///
template <typename Num>
constexpr auto euclidDistance(const Vector2<Num> &v1, const Vector2<Num> &v2) {
  return norm(Vector2<decltype(v1.x - v2.x)>{v1.x - v2.x, v1.y - v2.y});
}

///
/// \brief Check if two circles intersect (or touch)
///
/// \tparam Num the number type (must be floating)
/// \param c1 circle 1
/// \param c2 circle 2
///
template <typename Num>
constexpr bool circleIntersection(const Circle<Num> &c1, const Circle<Num> &c2) {
  auto threshold = c1.radius + c2.radius;
  auto distance = euclidDistance(c1.origin, c2.origin);
  return distance <= threshold;
}

///
/// \brief Check if a circle fully encloses another circle
///
/// \tparam Num the number type (must be floating)
/// \param enclosure the outer circle
/// \param enclosed the potentially enclosed circle
///
template <typename Num>
constexpr bool circleEnclosureOf(const Circle<Num> &enclosure, const Circle<Num> &enclosed) {
  // Trivial case (also ensures curvature of enclosure is not greater then curvature of enclosed)
  if (enclosed.radius > enclosure.radius) {
    return false;
  }

  // Farthest point of enclosed from enclosure origin
  auto farthest_point = euclidDistance(enclosure.origin, enclosed.origin) + enclosed.radius;
  return farthest_point <= enclosure.radius;
}

///
/// \brief Calculate the difference between two angles (rad, -pi ... pi)
///
/// \param a1 [rad] angle 1 in [-pi, pi]
/// \param a2 [rad] angle 2 in [-pi, pi]
/// \return double absolute angle difference [rad]
///
inline double angleDifference(double a1, double a2) noexcept(true) {
  auto d = a1 - a2;
  return std::abs(std::atan2(std::sin(d), std::cos(d)));
}

}  // namespace vda5050pp::core::common::math

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_MATH_GEOMETRY */
