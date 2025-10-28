#ifndef FRONTEND_INTERVAL_LOG_H
#define FRONTEND_INTERVAL_LOG_H

#include <chrono>
#include <iostream>
#include <memory>

class IntervalLog {
private:
  std::unique_ptr<std::ostream> Stream;
  std::chrono::time_point<std::chrono::steady_clock> Timer;

public:
  IntervalLog() : Stream(nullptr) {}

  void setStream(std::unique_ptr<std::ostream> Stream);

  void log(const std::string &Message);

  void start() { Timer = std::chrono::steady_clock::now(); }

  void logElapsedTime(const std::string &Message);
};

#endif // FRONTEND_INTERVAL_LOG_H