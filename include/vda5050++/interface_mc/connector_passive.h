// Copyright Open Logistics Foundation
// 
// Licensed under the Open Logistics Foundation License 1.3.
// For details on the licensing terms, see the LICENSE file.
// SPDX-License-Identifier: OLFL-1.3
// 
// TODO: Short description
//

#ifndef INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR_PASSIVE
#define INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR_PASSIVE

#include "vda5050++/interface_mc/connector.h"

namespace vda5050pp::interface_mc {

///
///\brief Interface class for the MC connection (passive)
///
/// The ConnectorPassive contains all member funtions of the normal Connector
/// plus the spinOnce function, which is used to poll messages once
///
class ConnectorPassive : public Connector {
public:
  virtual ~ConnectorPassive() = default;

  ///
  /// \brief poll messages once and pass them to the consumer
  ///
  virtual void spinOnce() noexcept(true) = 0;
};

}  // namespace vda5050pp::interface_mc

#endif /* INCLUDE_VDA5050_2B_2B_INTERFACE_MC_CONNECTOR_PASSIVE */
