// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the implementation of the HeaderVersionValidator
//

#include "vda5050++/core/validation/header_version_validator.h"

#include <algorithm>
#include <optional>
#include <string>

#include "vda5050++/core/version.h"
#include "vda5050++/model/Error.h"
#include "vda5050++/model/Header.h"

#define PNAME(x) #x
#define STR(x) PNAME(x)

static const std::string k_description =
    "Check if a header's version number matches an allowed version number";

vda5050pp::core::validation::HeaderVersionValidator::HeaderVersionValidator()
    : Validator(k_description) {}

std::list<vda5050pp::Error> vda5050pp::core::validation::HeaderVersionValidator::operator()(
    const vda5050pp::Header &header) const {
  auto it = std::find(cbegin(vda5050pp::core::version::compatible),
                      cend(vda5050pp::core::version::compatible), header.version);

  if (it == cend(vda5050pp::core::version::compatible)) {
    return {{"UnsupportedVersion",
             {{{"header.version", header.version}}},
             "This version is not supported, some things may not work as expected.",
             ErrorLevel::WARNING}};
  } else {
    return {};
  }
}
