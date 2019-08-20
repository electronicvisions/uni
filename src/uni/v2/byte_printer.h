#pragma once

#include <ostream>
#include <iomanip>

#include <uni/v2/bytewise_output_iterator.h>


namespace uni {

  /** Type for pretty printing binary data in hexadecimal. */
  template<typename InOutIt>
  struct Byte_printer {
    InOutIt a, b;
    unsigned int base = 0;

    Byte_printer(InOutIt a, InOutIt b)
      : a(a), b(b) {
    }
  };


  /** Function to convert Iterators to Byte_printer object. */
  template<typename InOutIt>
  Byte_printer<InOutIt> as_bytes(InOutIt a, InOutIt b) {
    return Byte_printer<InOutIt>(a, b);
  }


  /** Output operator to print a Byte_printer object. 
   *
   * @tparam InOutIt Iterator for underlying data.
   *
   * Typical usage:
   * @code
   * std:vector<unsigned> a;
   * std::cout << uni::as_bytes(std::begin(a), std::end(a));
   * @endcode
   * */
  template<typename InOutIt>
  std::ostream& operator << (std::ostream& os, Byte_printer<InOutIt> const& data) {
    auto byte_a = bytewise(data.a);
    auto byte_b = bytewise(data.b);
    std::size_t i = 0;

    os << "0x" << std::setfill('0') << std::setw(4) << std::hex
      << i << ":  ";

    for(; byte_a != byte_b; ++byte_a) {
      os << std::setfill('0') << std::setw(2) << std::hex
        << *byte_a << ' ';

      if( i++ != 0 ) {
        if( i % 16 == 0 ) {
          os << '\n';
          os << "0x" << std::setfill('0') << std::setw(4) << std::hex
            << i << ":  ";
        } else if( i % 4 == 0 )
          os << ' ';
      }
    }

    return os;
  }

}
