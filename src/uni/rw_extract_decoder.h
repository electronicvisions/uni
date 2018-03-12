#pragma once


namespace uni {

  /** Decode READ and WRITE instructions from byte-code.
   *
   * Use together with the decode() function.
   *
   * Only read and write instructions are considered and collected in the
   * extracted vector. */
  struct Rw_extract_decoder {
    /** Type for result collection */
    struct Entry {
      bool is_write;      /**< Entry is a write operation. */
      Read_inst read;     /**< Decoded READ instruction. */
      Write_inst write;   /**< Decoded WRITE instruction. */
    };

    /** Array of extracted data. */
    std::vector<Entry> extracted;


    template<typename T> void operator () (T const& /*inst*/) {
    }

    void operator () (Read_inst const& inst) {
      Entry e;
      e.is_write = false;
      e.read = inst;

      extracted.push_back(e);
    }

    void operator () (Write_inst const& inst) {
      Entry e;
      e.is_write = true;
      e.write = inst;

      extracted.push_back(e);
    }
  };

}

