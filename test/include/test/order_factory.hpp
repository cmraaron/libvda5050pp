// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 

#ifndef TEST_INCLUDE_TEST_ORDER_FACTORY_HPP_
#define TEST_INCLUDE_TEST_ORDER_FACTORY_HPP_

#include <string>
#include <vector>

#include "vda5050++/model/Order.h"

namespace test {

vda5050pp::Edge mkEdge(std::string id, uint32_t seq, bool released, std::string prev,
                       std::string next, std::vector<vda5050pp::Action> actions);
vda5050pp::Node mkNode(std::string id, uint32_t seq, bool released,
                       std::vector<vda5050pp::Action> actions);

}  // namespace test

#endif  // TEST_INCLUDE_TEST_ORDER_FACTORY_HPP_
