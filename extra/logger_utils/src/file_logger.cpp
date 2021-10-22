// Copyright 2021 Open Logistics Foundation
//
// Licensed under the Open Logistics License 1.0.
// For details on the licensing terms, see the LICENSE file.
//

#include "vda5050++/extra/file_logger.h"

#include <fstream>

using namespace vda5050pp::extra;

FileLogger::FileLogger(const std::filesystem::path &path) noexcept(false)
    : PrettyLogger(nullptr, "") {
  // Trust std::fstream to throw exceptions ect.
  this->fstream_.open(path, std::fstream::app);

  PrettyLogger::setOutLogger(std::make_unique<StreamLogger>(&this->fstream_, &this->fstream_,
                                                            &this->fstream_, &this->fstream_,
                                                            &this->fstream_));
}