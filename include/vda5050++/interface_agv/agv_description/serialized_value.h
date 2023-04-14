// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// This file contains the SerializedValue struct
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_SERIALIZED_VALUE
#define INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_SERIALIZED_VALUE

#include <stdexcept>
#include <string>

namespace vda5050pp::interface_agv::agv_description {

enum class Type {
  UNSPECIFIED = 0,
  BOOLEAN = 1,
  INTEGER = 2,
  FLOAT = 3,
  STRING = 4,
};

class BadSerializedValueCast : public std::bad_cast {};

///
/// \brief This struct represents a serialized value
/// It contains a type identified and a serialized value (std::string)
///
struct SerializedValue {
  ///\brief the serialized value
  std::string value;
  ///\brief the type of the serialized value
  Type type;

  /* Conversion functions */
  SerializedValue() noexcept(true) : type(Type::UNSPECIFIED) {}
  explicit SerializedValue(bool b) noexcept(true) : value(std::to_string(b)), type(Type::BOOLEAN) {}
  explicit SerializedValue(int64_t i) noexcept(true)
      : value(std::to_string(i)), type(Type::INTEGER) {}
  explicit SerializedValue(double f) noexcept(true) : value(std::to_string(f)), type(Type::FLOAT) {}
  explicit SerializedValue(const char *s) noexcept(true) : value(s), type(Type::STRING) {}
  explicit SerializedValue(std::string_view s) noexcept(true) : value(s), type(Type::STRING) {}
  explicit SerializedValue(const std::string &s) noexcept(true) : value(s), type(Type::STRING) {}

  void operator=(bool b) { *this = SerializedValue(b); }
  void operator=(int64_t i) { *this = SerializedValue(i); }
  void operator=(double f) { *this = SerializedValue(f); }
  void operator=(const std::string &s) { *this = SerializedValue(s); }
  void operator=(const char *s) { *this = SerializedValue(s); }
  void operator=(std::string_view *s) { *this = SerializedValue(s); }

  ///
  /// \brief Cast the serialized value to bool
  ///
  /// \throws std::invalid_argument if the serialized value is not parsable
  /// \return parsed bool
  ///
  explicit operator bool() const noexcept(false) {
    static const auto true_ = std::to_string(true);
    static const auto false_ = std::to_string(false);
    if (type != Type::BOOLEAN) {
      throw BadSerializedValueCast();
    }
    if (value == true_) {
      return true;
    }
    if (value == false_) {
      return false;
    }
    throw std::invalid_argument("Could not parse Boolean");
  }

  ///
  /// \brief Cast the serialized value to signed 64bit int
  ///
  /// \throws std::invalid_argument if the serialized value is not parsable
  /// \return parsed int64
  ///
  explicit operator int64_t() const noexcept(false) {
    if (type != Type::INTEGER) {
      throw BadSerializedValueCast();
    }
    return std::stoi(value);
  }

  ///
  /// \brief Cast the serialized value to 64bit ieee float
  ///
  /// \throws std::invalid_argument if the serialized value is not parsable
  /// \return parsed double
  ///
  explicit operator double() const noexcept(false) {
    if (type != Type::FLOAT) {
      throw BadSerializedValueCast();
    }
    return std::stod(value);
  }

  ///
  /// \brief Cast the serialized value to the string it represents
  ///
  /// \throws std::invalid_argument if the serialized value is not parsable
  /// \return literal string
  ///
  explicit operator std::string() const noexcept(false) {
    if (type != Type::STRING) {
      throw BadSerializedValueCast();
    }
    return value;
  }

  ///
  /// \brief Compare two SerializedValues with equal type
  ///
  /// \param other
  /// \return less?
  ///
  bool operator<(const SerializedValue &other) const noexcept(false) {
    if (this->type != other.type) {
      throw BadSerializedValueCast();
    }
    switch (this->type) {
      case Type::INTEGER:
        return int64_t(*this) < int64_t(other);
      case Type::FLOAT:
        return double(*this) < double(other);
      case Type::BOOLEAN:
        if (other) return true;
        return false;
      default:
      case Type::UNSPECIFIED:
        /* fallthrough */
      case Type::STRING:
        return this->value < other.value;
    }
  }

  ///
  /// \brief Compare two SerializedValues with equal type
  ///
  /// \param other
  /// \return equal?
  ///
  bool operator==(const SerializedValue &other) const noexcept(false) {
    if (this->type != other.type) {
      throw BadSerializedValueCast();
    }
    switch (this->type) {
      case Type::INTEGER:
        return int64_t(*this) == int64_t(other);
      case Type::FLOAT:
        return double(*this) == double(other);
      case Type::BOOLEAN:
        return bool(*this) == bool(other);
      default:
      case Type::UNSPECIFIED:
        /* fallthrough */
      case Type::STRING:
        return this->value == other.value;
    }
  }

  ///
  /// \brief Compare two SerializedValues with equal type
  ///
  /// \param other
  /// \return gt?
  ///
  bool operator>(const SerializedValue &other) const noexcept(false) { return other < *this; }

  ///
  /// \brief Compare two SerializedValues with equal type
  ///
  /// \param other
  /// \return leq?
  ///
  bool operator<=(const SerializedValue &other) const noexcept(false) { return !(other < *this); }

  ///
  /// \brief Compare two SerializedValues with equal type
  ///
  /// \param other
  /// \return geq?
  ///
  bool operator>=(const SerializedValue &other) const noexcept(false) { return other <= *this; }
};
}  // namespace vda5050pp::interface_agv::agv_description

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_AGV_AGV_DESCRIPTION_SERIALIZED_VALUE */
