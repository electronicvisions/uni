#pragma once

#include <iterator>
#include <ostream>
#include <cstdint>

namespace uni {


  /** Proxy used by Bytewise_output_iterator. */
  template<typename T, typename InOutIterator>
  struct Byte_proxy {
    InOutIterator it;
    std::size_t i;

    Byte_proxy(InOutIterator it, std::size_t i)
      : it(it),
        i(i) {
    }

    Byte_proxy& operator = (uint8_t c) {
      std::size_t byte_i = sizeof(T) - i - 1;
      *it = (*it & ~(0xff << (byte_i * 8))) | (static_cast<T>(c) << (byte_i * 8));
      return *this;
    }

    bool operator == (Byte_proxy const& o) const {
      return ((*it >> (8 * i)) & 0xff) == ((*(o.it) >> (8 * o.i)) & 0xff);
    }

    bool operator != (Byte_proxy const& o) const {
      return !(*this == o);
    }

    // behave like uint8_t
    operator uint8_t() {
      std::size_t byte_i = sizeof(T) - i - 1;
      return ((*it >> (8 * byte_i)) & 0xff);
    }
  };


  /** Printing of Byte_proxy. */
  template<typename T, typename InOutIt>
  std::ostream& operator << (std::ostream& os, Byte_proxy<T, InOutIt> const& pr) {
    std::size_t byte_i = sizeof(T) - pr.i - 1;
    os << static_cast<unsigned>( (*(pr.it) >> (byte_i * 8)) & 0xff );
    return os;
  }


  /** Iterator type to iterate by bytes through data of other
   * granularity.
   *
   * @tparam T Underlying value type.
   * @tparam InOutIterator Iterator over underlying data.
   *
   * This iterator adapts InOutIterator to go byte-for-byte through its
   * underlying data of type T.
   * For example to go bytewise through uint64_t data.
   * Dereferencing this iterator returns a Byte_proxy object to access
   * the original data.
   * */
  template<typename T, typename InOutIterator>
  struct Bytewise_output_iterator {
    InOutIterator it;
    std::size_t i = 0;


    Bytewise_output_iterator() {
    }

    Bytewise_output_iterator(InOutIterator it)
      : it(it) {
    }

    Byte_proxy<T, InOutIterator> operator * () {
      return Byte_proxy<T, InOutIterator>(it, i);
    }

    Bytewise_output_iterator& operator ++ () {
      if( ++i >= sizeof(T) ) {
        ++it;
        i = 0;
      }

      return *this;
    }

    bool operator == (Bytewise_output_iterator const& o) {
      return !(*this != o);
    }

    bool operator != (Bytewise_output_iterator const& o) {
      if( (i != o.i) || (it != o.it) )
        return true;
      else
        return false;
    }
  };


  /** Helper function to create Bytewise_output_iterator. */
  template<typename It>
  Bytewise_output_iterator<typename std::iterator_traits<It>::value_type, It>
  bytewise(It it) {
    Bytewise_output_iterator<typename std::iterator_traits<It>::value_type, It> rv(it);
    return rv;
  }



}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
