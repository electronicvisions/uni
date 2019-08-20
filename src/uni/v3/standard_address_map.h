#pragma once


namespace uni {

  /** Translate logic spike-event addresses to index and address for the
   * UNI program */
  struct Standard_address_map {
    uint8_t index(uint64_t address) const {
      return (address & 0x1f00) >> 8;
    }

    uint8_t evaddr(uint64_t address) const {
      return address & 0x3f;
    }

    /* Address map specific to v3: There is no neuron index attached to the
     * spikes, only the address. */
    uint64_t address(uint8_t /*index*/, uint8_t evaddr) const
    {
      return evaddr;
    }
  };

}

