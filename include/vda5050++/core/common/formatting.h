// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains helper functions for string formatting purposes
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_COMMON_FORMATTING
#define INCLUDE_VDA5050_2B_2B_CORE_COMMON_FORMATTING

#include <functional>
#include <sstream>
#include <string>
#include <string_view>

///
///\brief Create a logstring with file:line stamp and some arguments
///
///\param __VA_ARGS__ a list of streamable objects (operator<<(ostream, obj) overload)
///                   to concatenate
///\return concatenated string
///
#define LOGSTRING(...) \
  vda5050pp::core::common::logstring("@", __FILE__, ":", __LINE__, "  ", __VA_ARGS__)

namespace vda5050pp::core::common {

///
///\brief Concatenate streamable objects to a string
///
///\tparam T all object types (operator<<(ostream, obj) overload)
///\param ts a list of streamable objects of type T to concatenate
///\return concatenated string
///
template <typename... T>[[nodiscard]] std::string logstring(T... ts) {
  std::stringstream ss;
  (ss << ... << ts) << "\n";
  return ss.str();
}

///
/// \brief Tail case of format when no values are left. Simply put fmt on ss
///
/// \param ss the sstream to append to
/// \param fmt the remaining format string
/// \return std::string empty
///
[[nodiscard]] inline std::string format(std::stringstream &ss, std::string_view fmt) {
  ss << fmt;
  return "";
}

///
/// \brief Printf style formatting with arbitrary types. Wildcard string is "{}"
/// The resulting string has each occurrance of "{}" in fmt replaced with a value of vns
/// until there aren't any left.
///
/// This version does not return the string but appends to ss
/// NOTE: This functions is only for interlan usage of format()
///
/// \tparam TN all object types (operator<<(ostream, obj) overload)
/// \tparam T0 next object type (operator<<(ostream, obj) overload)
/// \param ss the stringstream to append to
/// \param fmt the format string
/// \param v0 the next value to fill into the next occurrance of {}
/// \param vns  all values to fill in
/// \return std::string fmt with filled in values
///
template <typename T0, typename... TN>
[[nodiscard]] inline std::string format(std::stringstream &ss, std::string_view fmt, T0 v0,
                                        TN... vns) {
  auto i = fmt.find("{}");
  if (i == std::string_view::npos) {
    ss << fmt;
    return "";
  }

  ss << fmt.substr(0, i) << v0;
  return format(ss, fmt.substr(i + 2), vns...);
}

///
/// \brief Printf style formatting with arbitrary types. Wildcard string is "{}"
/// The resulting string has each occurrance of "{}" in fmt replaced with a value of vns
/// until there aren't any left.
///
/// \tparam TN all object types (operator<<(ostream, obj) overload)
/// \param fmt the format string
/// \param vns  all values to fill in
/// \return std::string fmt with filled in values
///
template <typename... TN>
[[nodiscard]] inline std::string format(const std::string &fmt, TN... vns) {
  std::stringstream ss;
  auto discard = format(ss, std::string_view(fmt), vns...);
  return ss.str();
}

}  // namespace vda5050pp::core::common

#endif /* INCLUDE_VDA5050_2B_2B_CORE_COMMON_FORMATTING */
