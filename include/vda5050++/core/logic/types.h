// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//
// This file contains the types for the logic module
//

#ifndef INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TYPES_HPP_
#define INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TYPES_HPP_

#include <SimplePTN/petri_net.hpp>
#include <ostream>

namespace vda5050pp::core::logic {

using SeqNrT = uint16_t;

enum class LogicTaskNetTypes {
  k_ready,
  k_pre_done,
  k_done,
  k_enabled,
  k_disabled,
  k_disable,
  k_skip,
  k_fail,
  k_any_failed,
  k_failed,
  k_finish,
  k_finished,
  k_initializing,
  k_initializing_fail,
  k_start,
  k_started,
  k_running,
  k_running_fail,
  k_pause,
  k_paused,
  k_paused_fail,
  k_resume,
  k_entered,
  k_exited,
  k_intercept,
  k_intercept_parallel,
  k_intercept_sequential,
  k_intercepted,
  k_intercepted_parallel,
  k_intercepted_sequential,
  k_un_intercepted,
  k_intercept_sync,
  k_intercepting_begin,
  k_intercepting_end,
  k_pre_to_done,
  k_sequential_to_intercepting,
  k_parallel_to_intercepting,
  k_intercepting_to_done,
  k_combinator_parallel,
  k_combinator_sync,
  k_combinator_sync_transition,
  k_combinator_next_time_step,
  k_combinator_cancel,
  k_combinator_cancel_post,
  k_combinator_cancel_pre_transition,
  k_combinator_cancel_post_transition,
  k_combinator_dangling_begin,
  k_combinator_dangling_transition,
};

struct LogicTaskNetID {
  LogicTaskNetTypes type;
  SeqNrT seq;

  constexpr bool operator==(const LogicTaskNetID &other) const {
    return this->type == other.type && this->seq == other.seq;
  }

  constexpr bool operator<(const LogicTaskNetID &other) const {
    return this->seq < other.seq || (this->seq == other.seq && this->type < other.type);
  }
};

using Net = sptn::PetriNet<LogicTaskNetID, uint8_t>;

inline std::ostream &operator<<(std::ostream &os, const LogicTaskNetTypes &type) {
  switch (type) {
    case LogicTaskNetTypes::k_ready:
      os << "ready";
      break;
    case LogicTaskNetTypes::k_done:
      os << "done";
      break;
    case LogicTaskNetTypes::k_pre_done:
      os << "pre_done";
      break;
    case LogicTaskNetTypes::k_fail:
      os << "fail";
      break;
    case LogicTaskNetTypes::k_any_failed:
      os << "any_failed";
      break;
    case LogicTaskNetTypes::k_failed:
      os << "failed";
      break;
    case LogicTaskNetTypes::k_finish:
      os << "finish";
      break;
    case LogicTaskNetTypes::k_finished:
      os << "finished";
      break;
    case LogicTaskNetTypes::k_initializing:
      os << "initializing";
      break;
    case LogicTaskNetTypes::k_initializing_fail:
      os << "initializing_fail";
      break;
    case LogicTaskNetTypes::k_start:
      os << "start";
      break;
    case LogicTaskNetTypes::k_started:
      os << "started";
      break;
    case LogicTaskNetTypes::k_running:
      os << "running";
      break;
    case LogicTaskNetTypes::k_running_fail:
      os << "running_fail";
      break;
    case LogicTaskNetTypes::k_pause:
      os << "pause";
      break;
    case LogicTaskNetTypes::k_paused:
      os << "paused";
      break;
    case LogicTaskNetTypes::k_paused_fail:
      os << "paused_fail";
      break;
    case LogicTaskNetTypes::k_resume:
      os << "resume";
      break;
    case LogicTaskNetTypes::k_entered:
      os << "entered";
      break;
    case LogicTaskNetTypes::k_intercept_parallel:
      os << "intercept_parallel";
      break;
    case LogicTaskNetTypes::k_intercept_sequential:
      os << "intercept_sequential";
      break;
    case LogicTaskNetTypes::k_intercepted_parallel:
      os << "intercepted_parallel";
      break;
    case LogicTaskNetTypes::k_intercepted_sequential:
      os << "intercepted_sequential";
      break;
    case LogicTaskNetTypes::k_un_intercepted:
      os << "un_intercepted";
      break;
    case LogicTaskNetTypes::k_intercept_sync:
      os << "un_intercepted";
      break;
    case LogicTaskNetTypes::k_intercepting_begin:
      os << "intercepting_begin";
      break;
    case LogicTaskNetTypes::k_intercepting_end:
      os << "intercepting_end";
      break;
    case LogicTaskNetTypes::k_pre_to_done:
      os << "pre_to_done";
      break;
    case LogicTaskNetTypes::k_sequential_to_intercepting:
      os << "sequential_to_intercepting";
      break;
    case LogicTaskNetTypes::k_parallel_to_intercepting:
      os << "parallel_to_intercepting";
      break;
    case LogicTaskNetTypes::k_intercepting_to_done:
      os << "intercepting_to_done";
      break;
    case LogicTaskNetTypes::k_exited:
      os << "exited";
      break;
    case LogicTaskNetTypes::k_combinator_parallel:
      os << "combinator_parallel";
      break;
    case LogicTaskNetTypes::k_combinator_sync:
      os << "combinator_sync";
      break;
    case LogicTaskNetTypes::k_combinator_sync_transition:
      os << "combinator_sync_transition";
      break;
    case LogicTaskNetTypes::k_combinator_next_time_step:
      os << "combinator_next_time_step";
      break;
    case LogicTaskNetTypes::k_combinator_cancel:
      os << "combinator_cancel";
      break;
    case LogicTaskNetTypes::k_combinator_cancel_post:
      os << "combinator_cancel_post";
      break;
    case LogicTaskNetTypes::k_combinator_cancel_pre_transition:
      os << "combinator_cancel_pre_transition";
      break;
    case LogicTaskNetTypes::k_combinator_cancel_post_transition:
      os << "combinator_cancel_post_transition";
      break;
    case LogicTaskNetTypes::k_combinator_dangling_begin:
      os << "combinator_dangling_begin";
      break;
    case LogicTaskNetTypes::k_combinator_dangling_transition:
      os << "combinator_dangling_transition";
      break;
    default:
      os.fail();
      break;
  }
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const LogicTaskNetID &id) {
  os << id.type << "@" << id.seq;
  return os;
}
}  // namespace vda5050pp::core::logic

#endif  // INCLUDE_VDA5050_2B_2B_CORE_LOGIC_TYPES_HPP_
