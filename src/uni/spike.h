#pragma once

#include <ostream>

namespace uni {
  struct Spike {
    Time time;
    uint64_t address;

    Spike(Time t, uint64_t address)
      : time(t),
        address(address) {
    }

    bool operator == (Spike const& other) const {
      return (time == other.time) && (address == other.address);
    }
  };
}

namespace std {
  inline std::ostream& operator << (std::ostream& os, Spike const& s) {
    return os << "(t:" << s.time << ", a:" << s.address << ")";
  }
}


