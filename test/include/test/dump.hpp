// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#ifndef TEST_INCLUDE_TEST_DUMP_HPP_
#define TEST_INCLUDE_TEST_DUMP_HPP_

#include <list>

#include "vda5050++/core/interface_agv/handle_accessor.h"
#include "vda5050++/model/Error.h"

namespace test {

void dumpErrors(vda5050pp::core::interface_agv::HandleAccessor &ha,
                const std::list<vda5050pp::Error> &errors);

}

#endif  // TEST_INCLUDE_TEST_DUMP_HPP_
