#pragma once

#include <bitset>
#include <string>
#include <vector>

#include "uni/types.h"
#include "uni/errors.h"

/** Top-level namespace for UNI */
namespace uni {


  //---------------------------------------------------------------------------
  // Instruction coding
  //---------------------------------------------------------------------------

  template<typename InOutIterator, typename T>
  InOutIterator fill_data(InOutIterator it, T w) {
    for(std::size_t i=0; i<sizeof(T); i++) {
      T b = (w >> ((sizeof(T) - i - 1) * 8));
      *it = b & 0xff;
      ++it;
    }
    return it;
  }

#define FILL_INST_0(name) \
  template<typename InOutIterator> \
  InOutIterator name (InOutIterator it)
#define FILL_INST_1(name, arg) \
  template<typename InOutIterator> \
  InOutIterator name (InOutIterator it, arg)
#define FILL_INST_2(name, arg1, arg2) \
  template<typename InOutIterator> \
  InOutIterator name (InOutIterator it, arg1, arg2)


  /** Write a SET_TIME instruction.
   *
   * @param it Where to write.
   * @param t Argument for the instruction.
   * @returns Iterator after end of instruction.
   *
   * SET_TIME sets the global system timer to the value specified with t.
   * */
  FILL_INST_1(fill_set_time, Time t) {
    *it = 0x00;
    ++it;
    return fill_data(it, t);
  }

  FILL_INST_0(fill_halt) {
    *it = 0x0e;
    return ++it;
  }

  FILL_INST_1(fill_read, Address addr) {
    *it = 0x0b;
    ++it;
    return fill_data(it, addr);
  }

  FILL_INST_2(fill_write, Address addr, Word w) {
    *it = 0x0a;
    ++it;
    it = fill_data(it, addr);
    return fill_data(it, w);
  }

  FILL_INST_1(fill_raw, std::vector<Byte> const& data) {
    if( data.size() > 256 )
      throw Encode_error(__func__, "raw",
          "data may not contain more than 256 elements");
    *it = 0x02;
    ++it;
    *it = data.size();
    ++it;
    for(auto const& d : data) {
      *it = d;
      ++it;
    }

    return it;
  }

  FILL_INST_1(fill_wait_until, Time t) {
    *it = 0x01;
    ++it;
    return fill_data(it, t);
  }

  FILL_INST_1(fill_wait_for_32, uint32_t t) {
    *it = 0x05;
    ++it;
    return fill_data(it, t);
  }

  FILL_INST_1(fill_wait_for_16, uint16_t t) {
    *it = 0x04;
    ++it;
    return fill_data(it, t);
  }

  FILL_INST_1(fill_wait_for_7, uint8_t t) {
    *it = 0x80 | t;
    return ++it;
  }

  FILL_INST_0(fill_rec_start) {
    *it = 0x0c;
    return ++it;
  }

  FILL_INST_0(fill_rec_stop) {
    *it = 0x0d;
    return ++it;
  }

  FILL_INST_2(fill_fire_one, uint8_t /*idx*/, Event_address /*evaddr*/) {
    throw Encode_error(__func__, "fire_one", "Not implemented yet for v3");
    return it;
  }

#undef FILL_INST_0
#undef FILL_INST_1
#undef FILL_INST_2



  //---------------------------------------------------------------------------
  // Instruction classes
  //---------------------------------------------------------------------------

  /** Base class for UNI instructions.
   *
   * The derived classes of Instruction constitute the internal representation
   * of UNI instructions. The read_ functions implement decoding of these
   * instructions into buffers.
   * */
  struct Instruction {
    /** Maximum length of names for pretty printing. */
    static int const name_width = 15;

    /** Name of instruction. */
    std::string const name;

    /** Construct named instruction.
     *
     * @param name Name of the instruction. */
    explicit Instruction(std::string const& name)
      : name(name) {
    }

    /** No-op assignment operator to allow for automatic deduction of
     * assignment operators for derived classes.
     * FIXME (ECM@2016-12-23): This is very unexpected behavior... we shall fix this.
     */
    Instruction const& operator=(Instruction const& /*other*/) {
      return *this;
    }
  };


  /** Base class for timing control instructions. */
  struct Timing_inst : public Instruction {
    /** Time argument for this instruction. */
    Time t;

    Timing_inst(std::string const& name)
      : Instruction(name) {
    }
  };


  struct Set_time_inst : public Timing_inst {
    Set_time_inst()
      : Timing_inst("set_time") {
    }
  };


  struct Wait_until_inst : public Timing_inst {
    Wait_until_inst()
      : Timing_inst("wait_until") {
    }
  };


  struct Read_inst : public Instruction {
    Address address;

    Read_inst()
      : Instruction("read") {
    }
  };

  struct Write_inst : public Instruction {
    Address address;
    Word data;

    Write_inst()
      : Instruction("write") {
    }
  };

  struct Halt_inst : public Instruction {
    Halt_inst()
      : Instruction("halt") {
    }
  };

  struct Wait_for_7_inst : public Timing_inst {
    Wait_for_7_inst()
      : Timing_inst("wait_for_7") {
    }
  };

  struct Wait_for_16_inst : public Timing_inst {
    Wait_for_16_inst()
      : Timing_inst("wait_for_16") {
    }
  };

  struct Wait_for_32_inst : public Timing_inst {
    Wait_for_32_inst()
      : Timing_inst("wait_for_32") {
    }
  };

  struct Raw_inst : public Instruction {
    std::vector<Byte> data;

    Raw_inst()
      : Instruction("raw") {
    }

    bool operator == (Raw_inst const& other) const {
      return (data.size() == other.data.size())
        && std::equal(data.begin(), data.end(), other.data.begin());
    }
  };

  struct Rec_start_inst : public Instruction {
    Rec_start_inst()
      : Instruction("rec_start") {
    }
  };

  struct Rec_stop_inst : public Instruction {
    Rec_stop_inst()
      : Instruction("rec_stop") {
    }
  };


  struct Fire_one_or_madc_inst : public Instruction {
    uint8_t key;
    uint32_t payload;

    Fire_one_or_madc_inst()
      : Instruction("fire_one") {
    }
  };


  //---------------------------------------------------------------------------
  // Output operators
  //---------------------------------------------------------------------------


  inline std::ostream& operator << (std::ostream& os, Instruction const& inst) {
    return os << std::setfill(' ') << std::setw(inst.name_width)
      << inst.name;
  }

  inline std::ostream& operator << (std::ostream& os, Timing_inst const& inst) {
    return os << static_cast<Instruction>(inst)
      << " "
      << std::dec
      << inst.t;
  }

  inline std::ostream& operator << (std::ostream& os, Write_inst const& inst) {
    std::stringstream strm;

    strm << static_cast<Instruction>(inst)
      << " ["
      << std::hex
      << std::setfill('0') << std::setw(8)
      << inst.address
      << "] = 0x"
      << std::setfill('0') << std::setw(8)
      << inst.data;
    return os << strm.str();
  }

  inline std::ostream& operator << (std::ostream& os, Read_inst const& inst) {
    std::stringstream strm;

    strm << static_cast<Instruction>(inst)
      << " ["
      << std::hex
      << std::setfill('0') << std::setw(8)
      << inst.address
      << "]";
    return os << strm.str();
  }

  inline std::ostream& operator << (std::ostream& os, Raw_inst const& inst) {
    std::stringstream strm;

    strm << static_cast<Instruction>(inst)
      << " x" << inst.data.size()
      << " { ";
    for(auto const& d : inst.data)
      strm << std::hex << std::setfill('0') << std::setw(2)
        << +d << ' ';
    strm << "}";
    return os << strm.str();
  }

  inline std::ostream& operator << (std::ostream& os, Fire_one_or_madc_inst const& inst) {
    std::stringstream strm;

    strm << static_cast<Instruction>(inst) << " key=" << inst.key
         << " Payload=" << std::bitset<32>(inst.payload);
    return os << strm.str();
  }


  //---------------------------------------------------------------------------
  // Instruction decoding
  //---------------------------------------------------------------------------

  template<typename InputIt, typename T>
  InputIt read_data(InputIt it, T& data) {
    data = 0;
    for(std::size_t i=0; i<sizeof(T); i++) {
      std::size_t byte_i = sizeof(T) - i - 1;
      data |= static_cast<T>(*it) << (byte_i * 8);
      ++it;
    }
    return it;
  }


#define READ_INST(name, classname, opcode, body) \
  template<typename InputIt> \
  InputIt read_ ## name (InputIt it, classname & inst) { \
    if( *it != opcode ) \
      throw Decode_error(__func__, #name, *it, "wrong opcode for " #name ); \
    body \
  }


  READ_INST(set_time, Set_time_inst, 0x00, return read_data(++it, inst.t);)
  READ_INST(wait_until, Wait_until_inst, 0x01, return read_data(++it, inst.t);)
  READ_INST(write, Write_inst, 0x0a,
      it = read_data(++it, inst.address);
      return read_data(it, inst.data);)
  READ_INST(read, Read_inst, 0x0b,
      return read_data(++it, inst.address);)
  READ_INST(halt, Halt_inst, 0x0e,
      static_cast<void>(inst);
      return ++it;)

  READ_INST(wait_for_16, Wait_for_16_inst, 0x04,
      uint16_t tmp;
      it = read_data(++it, tmp);
      inst.t = tmp;
      return it;)
  READ_INST(wait_for_32, Wait_for_32_inst, 0x05,
      uint32_t tmp;
      it = read_data(++it, tmp);
      inst.t = tmp;
      return it;)
  READ_INST(rec_start, Rec_start_inst, 0x0c,
      static_cast<void>(inst);
      return ++it;)
  READ_INST(rec_stop, Rec_stop_inst, 0x0d,
      static_cast<void>(inst);
      return ++it;)
  READ_INST(raw, Raw_inst, 0x02,
      ++it;
      uint8_t sz = *it;
      ++it;
      inst.data.reserve(sz);
      for(size_t i=0; i<sz; ++i) {
        inst.data.push_back(*it);
        ++it;
      }
      return it;)
// The following implementation differs from the original definition of the
// fire_one opcode. This is a work around of #2468.
  READ_INST(fire_one, Fire_one_or_madc_inst, 0x0f,
      uint64_t tmp;
      it = read_data(++it, tmp);
      inst.key = (tmp >> 30) & 0x3; // 2 MSB decode if spike or number of samples
      inst.payload = (tmp & 0x3FFFFFFF); // rest is payload
      // dstoe: I was told there is no index passed with the spike, so it's
      // supposed to be zero.
      if ( *it != 0) {
        throw Decode_error(
            __func__,
            "fire_one",
            *it,
            "The index of the spike/madc sample is supposed to be zero");
      }
      return ++it;)

  template<typename InputIt>
  InputIt read_wait_for_7 (InputIt it, Wait_for_7_inst& inst) {
    if( ! (*it & 0x80) )
      throw Decode_error(__func__,
        "wait_for_7",
        *it,
        "MSB must be set for wait_for_7");

    inst.t = *it & 0x7f;
    return ++it;
  }


#undef READ_INST


#define CHECK_INST(name, sz) \
  template<typename InputIt> \
  bool check_ ## name (InputIt a, InputIt stop) { \
    for(size_t i=0; i< sz ; ++i) \
      if( ++a == stop ) \
        return false; \
    return true; \
  }

  CHECK_INST(set_time, 1 + sizeof(Time))
  CHECK_INST(wait_until, 1 + sizeof(Time))
  CHECK_INST(write, 1 + sizeof(Address) + sizeof(Word))
  CHECK_INST(read, 1 + sizeof(Word))
  CHECK_INST(halt, 1)
  CHECK_INST(wait_for_16, 1 + sizeof(uint16_t))
  CHECK_INST(wait_for_32, 1 + sizeof(uint32_t))
  CHECK_INST(rec_start, 1)
  CHECK_INST(rec_stop, 1)
  CHECK_INST(wait_for_7, 1)
  CHECK_INST(fire_one, 1 + sizeof(uint64_t) + sizeof(uint8_t))

  template<typename InputIt>
  bool check_raw(InputIt a, InputIt stop) {
    if( ++a == stop )
      return false;

    uint8_t sz = *a;
    for(size_t i=0; i<sz; ++i)
      if( ++a == stop )
        return false;

    return true;
  }

#undef CHECK_INST

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */

