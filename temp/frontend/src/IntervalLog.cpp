#include "IntervalLog.h"

void IntervalLog::setStream(std::unique_ptr<std::ostream> Stream) {
  this->Stream = std::move(Stream);
}

void IntervalLog::log(const std::string &Message) {
  if (Stream)
    *Stream << Message << std::endl;
}

void IntervalLog::logElapsedTime(const std::string &Message) {
  if (!Stream)
    return;
  auto Now = std::chrono::steady_clock::now();
  const auto Elapsed =
      std::chrono::duration_cast<std::chrono::microseconds>(Now - Timer)
          .count();
  *Stream << Message
          << std::format("\n  - Elapsed: {}.{:03} ms", Elapsed / 1000,
                         Elapsed % 1000)
          << std::endl;
}
