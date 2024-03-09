#pragma once

#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>
#include <regex>

namespace CipherFS {
  class Utils {
  public:
    static std::string join(const std::vector<std::string>& strings, const std::string& delim) {
      std::ostringstream joinedString;
      for (auto it = strings.begin(); it != strings.end(); ++it) {
        joinedString << *it;
        if (it != strings.end() - 1) {
          joinedString << delim;
        }
      }
      return joinedString.str();
    }

    static std::vector<std::string> split(const std::string& input, char delim) {
      std::vector<std::string> segments;
      std::istringstream ss(input);
      std::string segment;
      while (getline(ss, segment, delim)) {
        if (!segment.empty()) {
          segments.push_back(segment);
        }
      }
      return segments;
    }

    static std::string fetchOwner(const std::string& v_path) {
      auto segments = split(v_path, '/');
      if (!segments.empty()) {
        return segments.front(); // Returns the first segment as the owner
      }
      return "";
    }

    static bool checkPersonal(const std::string& v_path, const std::string& username) {
      auto segments = split(v_path, '/');
      return segments.size() >= 2 && segments[0] == username && segments[1] == "personal";
    }

    static bool checkName(const std::string& name) {
      if (name.empty() || name == "." || name == ".." || name.find('/') != std::string::npos || name[0] == '.') {
        return false;
      }
      const std::regex pattern(R"(^[a-zA-Z0-9_.-]+$)");
      return std::regex_match(name, pattern);
    }
  };
}
