#pragma once

#include <uni/types.h>
#include <uni/standard_address_map.h>
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
   * Spike objects that are stored in the extracted_spikes vector.
   *
   * There is a typedef for a Spiketrain_decoder using the Standard_address_map
   * as Map.
   * */
  template<typename Map>
  struct Spiketrain_and_madc_decoder {
    Time cur_t = 0;                 /**< Current time during decoding. */
    std::vector<Spike> extracted_spikes;   /**< Result spikes after decode(). */
    std::vector<MADCSample> extracted_samples;   /**< Result samples after decode(). */
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

    void operator () (Fire_one_or_madc_inst const& inst) {
      unsigned int sample_0 = inst.payload & 0x3FF;
      unsigned int sample_1 = (inst.payload >> 10) & 0x3FF;
      unsigned int sample_2 = (inst.payload >> 20) & 0x3FF;
      Event_address evaddr = (~inst.payload & 0xFF);
      switch(inst.key) {
        case 0: extracted_spikes.emplace_back(cur_t, evaddr);
                break;
        case 1: extracted_samples.emplace_back(cur_t, sample_0);
                break;
        case 2: extracted_samples.emplace_back(cur_t, sample_0);
                extracted_samples.emplace_back(cur_t, sample_1);
                break;
        case 3: extracted_samples.emplace_back(cur_t, sample_0);
                extracted_samples.emplace_back(cur_t, sample_1);
                extracted_samples.emplace_back(cur_t, sample_2);
                break;
      }
    }
  };


  typedef Spiketrain_and_madc_decoder<Standard_address_map> Standard_spiketrain_and_madc_decoder;

}

