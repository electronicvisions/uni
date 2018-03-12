#pragma once

#include <stdint.h>
#include <bitset>
#include <ostream>

namespace uni {

  typedef uint64_t Time;
  typedef uint32_t Address;
  typedef uint32_t Word;
  typedef uint8_t Byte;
  typedef uint8_t Event_address;
  typedef std::bitset<64> Fire_set;

  struct Spike {
    Time time;
    uint64_t address;

    Spike() : time(0), address(0) {};

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
  inline ostream& operator << (ostream& os, uni::Spike const& s) {
    return os << "(t:" << s.time << ", a:" << s.address << ")";
  }
}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
