#ifndef TEST_INCLUDE_TEST_TEST_ODOMETRY_HANDLER_H_
#define TEST_INCLUDE_TEST_TEST_ODOMETRY_HANDLER_H_

#include <condition_variable>

#include "vda5050++/interface_agv/odometry_handler.h"

namespace test {

class OdometryHandler : public vda5050pp::interface_agv::OdometryHandler {
private:
  std::optional<vda5050pp::AGVPosition> last_init_position_;
  std::mutex barrier_mutex_;
  std::condition_variable barrier_;
  bool barrier_active_ = false;

public:
  void initializePosition(const vda5050pp::AGVPosition &pos) noexcept(false) override;

  const vda5050pp::AGVPosition &lastInitPosition() const;

  bool hasLastInitPosition() const;

  void blockFutureInitializePositionCalls() noexcept(true);

  void unblockInitializePositionCalls() noexcept(true);
};

}  // namespace test

#endif  // TEST_INCLUDE_TEST_TEST_ODOMETRY_HANDLER_H_
