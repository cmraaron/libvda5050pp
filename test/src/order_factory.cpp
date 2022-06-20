// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "test/order_factory.hpp"

#include <optional>

vda5050pp::Edge test::mkEdge(std::string id, uint32_t seq, bool released, std::string prev,
                             std::string next, std::vector<vda5050pp::Action> actions) {
  return {id,
          seq,
          std::nullopt,
          released,
          prev,
          next,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          vda5050pp::OrientationType::TANGENTIAL,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          std::nullopt,
          actions};
}
vda5050pp::Node test::mkNode(std::string id, uint32_t seq, bool released,
                             std::vector<vda5050pp::Action> actions) {
  return {id, seq, std::nullopt, released, std::nullopt, actions};
}