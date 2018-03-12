#pragma once


namespace uni {

  /** Extract RAW instructions from byte-code.
   *
   * Use together with decode().
   *
   * Collects the raw data as Raw_inst if a valid bit within the * raw data is
   * set. The valid bit is checked within each byte of the instructions data
   * part. */
  struct Raw_extract_decoder {
    /** Indicate bit position of the valid bit (from the right). */
    static const size_t valid_loopback_idx = 2;

    /** Extracted Raw_inst. */
    std::vector<Raw_inst> extracted;

    /** Temporary variable for extracted nibbles. */
    std::deque<uint8_t> nibbles;


    template<typename T> void operator () (T const& /*inst*/) {
    }

    /** Append a new Raw_inst from currently collected nibbles. */
    void append() {
      Raw_inst e;

      size_t i;
      for(i=0; i<nibbles.size(); i += 2) {
        uint8_t tmp = nibbles[i] << 4;
        if( i+1 < nibbles.size() )
          tmp |= nibbles[i+1];
        e.data.push_back(tmp);
      }

      extracted.push_back(e);
      nibbles.clear();
    }

    void operator () (Raw_inst const& inst) {
      for(size_t i=0; i<inst.data.size(); ++i) {
        auto idx = inst.data.size() - i - 1;

        if( inst.data[idx] & (1 << valid_loopback_idx) )
          nibbles.push_front(inst.data[idx] & 0xf);
        else if( !nibbles.empty() )
          append();

        if( idx != 0 ) {
          if( inst.data[idx] & (1 << (valid_loopback_idx + 4)) )
            nibbles.push_front((inst.data[idx] >> 4) & 0xf);
          else if( !nibbles.empty() )
            append();
        }

      }
    }

  };

}


