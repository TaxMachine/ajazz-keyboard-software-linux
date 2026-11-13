//
// Created by Vixcho on 13/10/26.
//

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <chrono>
#include <ctime>
#include <mutex>
#include <string>
#include <vector>

enum class LogLevel { INFO, SUCCESS, WARNING, ERR };

struct LogEntry {
  std::string timestamp;
  std::string message;
  LogLevel level;
};

class Logger {
public:
  static Logger &getInstance() {
    static Logger instance;
    return instance;
  }

  void log(const std::string &message, LogLevel level = LogLevel::INFO) {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
    localtime_r(&tt, &local_tm);

    char timeBuf[32];
    std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &local_tm);

    m_entries.push_back({timeBuf, message, level});
    if (m_entries.size() > 1000) {
      m_entries.erase(m_entries.begin());
    }
  }

  void info(const std::string &message) { log(message, LogLevel::INFO); }
  void success(const std::string &message) { log(message, LogLevel::SUCCESS); }
  void warning(const std::string &message) { log(message, LogLevel::WARNING); }
  void error(const std::string &message) { log(message, LogLevel::ERR); }

  std::vector<LogEntry> getEntries() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_entries;
  }

  std::string getAllText() {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string all;
    for (const auto &entry : m_entries) {
      std::string lvlStr = "[INFO]";
      if (entry.level == LogLevel::SUCCESS)
        lvlStr = "[SUCCESS]";
      else if (entry.level == LogLevel::WARNING)
        lvlStr = "[WARNING]";
      else if (entry.level == LogLevel::ERR)
        lvlStr = "[ERROR]";

      all += "[" + entry.timestamp + "] " + lvlStr + " " + entry.message + "\n";
    }
    return all;
  }

  void clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_entries.clear();
  }

private:
  Logger() = default;
  std::mutex m_mutex;
  std::vector<LogEntry> m_entries;
};

#endif // LOGGER_HPP
