#pragma once

#include <ostream>

#include "uni/v2/instructions.h"
#include "uni/v2/errors.h"


namespace uni {

  /** Decode a byte-code buffer using a special decoder.
   *
   * @tparam It InputIterator over the byte-code buffer.
   * @tparam Decoder Type to use for decoding of byte-code.
   *
   * @param a Beginning of byte-code.
   * @param b Past the end of byte-code.
   * @param dec Decoder object to collect results.
   * @returns Iterator past the end of the last found instruction.
   *
   * This function goes through the byte-code until it find a HALT instruction
   * or reaches the end of the buffer indicated by b.
   * For each found instruction it uses the appropriate read_ function to
   * decode it into an Instruction object inst and calls dec(inst) to collect
   * the result.
   *
   * For printing the decoded program there is the Stream_decoder.
   * For extracting READ and WRITE instructions only, there is the
   * Rw_extract_decoder.
   * To decode spiketrains there is Spiketrain_decoder.
   * To extract RAW instructions there are Raw_extract_decoder and
   * Raw_reshape_decoder.
   * */
  template<typename It, typename Decoder>
  It decode(It a, It b, Decoder& dec) {
#define CASE_INST(opcode, classname, name) \
  case opcode : \
    { \
      if( check_ ## name (a, b) ) { \
        classname inst; \
        a = read_ ## name (a, inst); \
        dec(inst); \
      } else \
        end_of_program = true; \
    } \
    break

    bool end_of_program = false;

    while( !end_of_program && (a != b) ) {
      if( *a & 0x80 ) {
        Wait_for_7_inst inst;
        a = read_wait_for_7(a, inst);
        dec(inst);
      } else if( *a & 0x40 ) {
        Fire_one_inst inst;
        a = read_fire_one(a, inst);
        dec(inst);
      } else {
        switch( *a ) {
          CASE_INST(0x00, Set_time_inst, set_time);
          CASE_INST(0x01, Wait_until_inst, wait_until);
          CASE_INST(0x02, Raw_inst, raw);
          CASE_INST(0x04, Wait_for_16_inst, wait_for_16);
          CASE_INST(0x05, Wait_for_32_inst, wait_for_32);
          CASE_INST(0x0a, Write_inst, write);
          CASE_INST(0x0b, Read_inst, read);
          CASE_INST(0x0c, Rec_start_inst, rec_start);
          CASE_INST(0x0d, Rec_stop_inst, rec_stop);
          CASE_INST(0x0f, Fire_inst, fire);

          case 0x0e:
            {
              Halt_inst inst;
              a = read_halt(a, inst);
              dec(inst);
              end_of_program = true;
            }
            break;

          default:
            throw Decode_error(__func__,
                "unknonw",
                static_cast<unsigned>(*a),
                "encountered unknonw opcode");
        }
      }
    }

    return a;

#undef CASE_INST
  }


  /** Print programs from byte-code using decode() */
  struct Stream_decoder {
    Stream_decoder(std::ostream& os)
      : os(os) {
    }

    template<typename T> void operator () (T const& inst) {
      os << inst << '\n';
    }

    std::ostream& os;
  };

}
