// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#include "test/dump.hpp"

#include "vda5050++/core/common/formatting.h"

void test::dumpErrors(vda5050pp::core::interface_agv::HandleAccessor &ha,
                      const std::list<vda5050pp::Error> &errors) {
  ha.getLogger().logDebug(
      vda5050pp::core::common::logstring("Printing ", errors.size(), " errors:"));

  for (const auto &error : errors) {
    ha.getLogger().logDebug(vda5050pp::core::common::logstring("Type: ", error.errorType));
    ha.getLogger().logDebug(vda5050pp::core::common::logstring(
        "Description: ", error.errorDescription.value_or("<none>")));
    ha.getLogger().logDebug(vda5050pp::core::common::logstring("Level: ", error.errorLevel));
    ha.getLogger().logDebug(vda5050pp::core::common::logstring("References:"));

    for (const auto &ref :
         error.errorReferences.value_or(std::vector<vda5050pp::ErrorReference>())) {
      ha.getLogger().logDebug(
          vda5050pp::core::common::logstring("  ", ref.referenceKey, ": ", ref.referenceValue));
    }
  }

  ha.getLogger().logDebug(vda5050pp::core::common::logstring("---- DONE ----"));
}