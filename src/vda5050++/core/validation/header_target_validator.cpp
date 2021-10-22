// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the implementation of the HeaderTargetValidator
//

#include "vda5050++/core/validation/header_target_validator.h"

#include <optional>

#include "vda5050++/model/Error.h"
#include "vda5050++/model/Header.h"

static const std::string k_description_ =
    "Check if a header's manufacturer and sn field match the ones of this AGV";

vda5050pp::core::validation::HeaderTargetValidator::HeaderTargetValidator(
    vda5050pp::interface_agv::Handle &handle)
    : Validator(k_description_), handle_accessor_(handle) {}

std::list<vda5050pp::Error> vda5050pp::core::validation::HeaderTargetValidator::operator()(
    const vda5050pp::Header &header) const {
  const auto &agv_description = this->handle_accessor_.getAGVDescription();

  vda5050pp::Error error;
  error.errorType = "Not the correct manufacturer+sn combination";
  error.errorLevel = vda5050pp::ErrorLevel::WARNING;
  error.errorReferences = {std::vector<vda5050pp::ErrorReference>()};

  if (header.manufacturer != agv_description.manufacturer) {
    error.errorReferences->push_back({"header.manufacturer", header.manufacturer});
  }
  if (header.serialNumber != agv_description.serial_number) {
    error.errorReferences->push_back({"header.serialNumber", header.serialNumber});
  }

  if (error.errorReferences->size() > 0) {  // One of the two causes happened
    return {error};
  } else {
    return {};
  }
}
