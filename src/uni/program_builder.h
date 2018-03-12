#pragma once

#include <uni/types.h>
#include <uni/instructions.h>
#include <uni/errors.h>

#include <vector>


namespace uni {

  /** Build programs out of UNI instructions.
   *
   * @tparam Allocator Object to create buffer blocks.
   *
   * Program_builder constructs sequences of instructions (programs) into a
   * vector of buffers. The user uses simple methods such as set_time(),
   * write(), etc. that handle pointer and buffer management. The construction
   * into multiple buffers allows for back-ends that can not transport
   * arbitrarily sized buffers. For example, when there is a maximum amount of
   * kernel memory space available in a zero-copy approach.
   * */
  template<typename Allocator>
  class Program_builder {
    public:
      /** Vector of buffer blocks. */
      std::vector<typename Allocator::Container> containers;


      Program_builder(Allocator& alloc)
        : m_alloc(alloc) {
        containers.push_back(alloc.allocate(Allocator::block_size));
        m_it = alloc.begin(containers.back());
        m_stop = alloc.end(containers.back());
      }


      void set_time(Time t) {
        if( !check_set_time(m_it, m_stop) )
          alloc();

        m_it = fill_set_time(m_it, t);
      }


      void wait_until(Time t) {
        if( !check_wait_until(m_it, m_stop) )
          alloc();

        m_it = fill_wait_until(m_it, t);
      }


      void write(Address addr, Word data) {
        if( !check_write(m_it, m_stop) )
          alloc();

        m_it = fill_write(m_it, addr, data);
      }


      void wait_for(Time t) {
        if( t > 0xfffffffful ) {
          throw Encode_error(__func__, "wait_for_*",
              "Delay exceeds wait_for_32. Use wait_until instead.");
        } else if( t > 0xfffful ) {
          if( !check_wait_for_32(m_it, m_stop) )
            alloc();

          m_it = fill_wait_for_32(m_it, t);
        } else if( t > 0x7ful ) {
          if( !check_wait_for_16(m_it, m_stop) )
            alloc();

          m_it = fill_wait_for_16(m_it, t);
        } else {
          if( !check_wait_for_7(m_it, m_stop) )
            alloc();
          m_it = fill_wait_for_7(m_it, t);
        }
      }


      void read(Address addr) {
        if( !check_read(m_it, m_stop) )
          alloc();

        m_it = fill_read(m_it, addr);
      }


      void fire(Fire_set fire, Event_address evaddr) {
        if( !check_fire(m_it, m_stop) )
          alloc();
        m_it = fill_fire(m_it, fire, evaddr);
      }


      void fire_one(uint8_t index, Event_address evaddr) {
        if( !check_fire_one(m_it, m_stop) )
          alloc();
        m_it = fill_fire_one(m_it, index, evaddr);
      }


      /** Encode a spiketrain.
       *
       * @tparam It InputIterator over spikes.
       * @tparam Map Address map for event addresses.
       *
       * @param a Iterator to first spike.
       * @param b Iterator past the end of spikes
       * @param addr_map Address map object.
       *
       * Uses wait_until(), fire_one(), and wait_for() to encode the
       * spiketrain. addr_map translates the address field of spikes into index
       * and evaddr for fire_one().
       * Use Spike to store spikes and Standard_address_map for addr_map.
       * */
      template<typename It, typename Map>
      void spiketrain(It a, It b, Map addr_map) {
        if( a == b )
          return;

        Time t = a->time;

        wait_until(t);
        fire_one(addr_map.index(a->address), addr_map.evaddr(a->address));
        ++a;

        while( a != b ) {
          if( a->time > t ) {
            wait_for(a->time - t);
            t = a->time;
          } else if( a->time < t )
            throw Spiketrain_error(__func__,
                "time in spiketrain must increase monotonically");

          fire_one(addr_map.index(a->address), addr_map.evaddr(a->address));
          ++a;
        }
      }


      void halt() {
        if( !check_halt(m_it, m_stop) )
          alloc();

        m_it = fill_halt(m_it);
      }




    protected:
      Allocator& m_alloc;
      typename Allocator::Iterator m_it, m_stop;


      void alloc() {
        // fill with no-ops
        while( m_it != m_stop ) {
          m_it = fill_wait_for_7(m_it, 0);
        }

        containers.push_back(m_alloc.allocate(Allocator::block_size));
        m_it = m_alloc.begin(containers.back());
        m_stop = m_alloc.end(containers.back());
      }
  };


  /** Simple allocator for use with Program_builder.
   *
   * Creates std::vector<Byte> as buffer block with a maximum size of block_size.
   * */
  struct Byte_vector_allocator {
    static size_t const block_size = 4096;

    /** Container type to use by Program_builder. */
    typedef std::vector<Byte> Container;

    /** Iterator type to point to current insertion location by Program_builder. */
    typedef std::vector<Byte>::iterator Iterator;


    /** Get Iterator to first byte in Container. */
    Iterator begin(Container& c) {
      return std::begin(c);
    }

    /** Get Iterator to last byte in Container. */
    Iterator end(Container& c) {
      return std::end(c);
    }

    /** Allocate a new container.
     *
     * @param capacity Size of container. */
    Container allocate(size_t capacity) {
      return std::vector<Byte>(capacity);
    }
  };

}


/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
