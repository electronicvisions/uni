#pragma once

#include <uni/v2/types.h>
#include <uni/v2/standard_address_map.h>
#include <vector>


namespace uni {

  /** Decode a spiketrain from byte-code.
   *
   * @tparam Map Address map to translate between index and evaddr and Spike
   * addresses.
   *
   * Use together with decode() to extract spiketrains from byte-code data.
   *
   * This decoder will extract the correct timing from e.g. SET_TIME and
   * WAIT_FOR instructions and translate FIRE and FIRE_ONE instructions into
   * Spike objects that are stored in the extracted vector.
   *
   * There is a typedef for a Spiketrain_decoder using the Standard_address_map
   * as Map.
   * */
  template<typename Map>
  struct Spiketrain_decoder {
    Time cur_t = 0;                 /**< Current time during decoding. */
    std::vector<Spike> extracted;   /**< Result data after decode(). */
    Map addr_map;                   /**< Address map for address translation. */

    template<typename T> void operator () (T const& /*inst*/) {
    }

    void operator () (Set_time_inst const& inst) {
      cur_t = inst.t;
    }

    void operator () (Wait_until_inst const& inst) {
      cur_t = inst.t;
    }

    void operator () (Wait_for_7_inst const& inst) {
      cur_t += inst.t;
    }

    void operator () (Wait_for_16_inst const& inst) {
      cur_t += inst.t;
    }

    void operator () (Wait_for_32_inst const& inst) {
      cur_t += inst.t;
    }

    void operator () (Fire_inst const& inst) {
      for(size_t i=0; i<inst.fire.size(); ++i) {
        if( inst.fire.test(i) )
          extracted.emplace_back(cur_t, addr_map.address_from_fire(i, inst.evaddr));
      }
    }

    void operator () (Fire_one_inst const& inst) {
      extracted.emplace_back(cur_t, addr_map.address_from_fire_one(inst.index, inst.evaddr));
    }
  };


  typedef Spiketrain_decoder<Standard_address_map> Standard_spiketrain_decoder;

}

