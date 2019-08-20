#pragma once

#include <uni/v2/raw_extract_decoder.h>

namespace uni {

  /** Extract RAW instructions from byte-code.
   *
   * Use together with decode().
   *
   * Realign data to get the transmitted data frame. The byte containing the
   * valid bit is shifted to the beginning of the resulting Raw_inst. */
  struct Raw_reshape_decoder : public Raw_extract_decoder {
    size_t num_nibbles = 33;

    template<typename T> void operator () (T const& inst) {
    }

    void operator () (Raw_inst const& inst) {
      for(size_t i=0; i<inst.data.size(); ++i) {
        auto idx = inst.data.size() - i - 1;

        nibbles.push_front(inst.data[idx] & 0xf);
        if( nibbles.size() >= num_nibbles )
          append();

        if( idx != 0 ) {
          nibbles.push_front((inst.data[idx] >> 4) & 0xf);
          if( nibbles.size() >= num_nibbles )
            append();
        }
      }
    }
  };

}
