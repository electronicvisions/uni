#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <exception>


namespace uni {


  /** Exception base class derived from std::exception. */
  class Error_base : public std::exception {
    public:
      Error_base()
        : std::exception() {
      }

      Error_base(std::string const& where, std::string const& what)
        : std::exception(),
          m_where(where),
          m_what(what) {
      }


      virtual const char* what() const noexcept {
        std::stringstream strm;
        strm << m_where << ": " << m_what;
        return strm.str().c_str();
      }


    protected:
      std::string m_where;
      std::string m_what;

  };



  /** Exception class for reporting errors when decoding programs. */
  class Decode_error : public Error_base {
    public:
      Decode_error(std::string const& where,
          std::string const& inst_name,
          uint8_t current_byte,
          std::string const& description)
        : Error_base() {
        std::stringstream strm;
        strm << "decoding error for instruction '"
          << inst_name
          << "' (current byte: 0x"
          << std::hex << std::setfill('0') << std::setw(2)
          << static_cast<unsigned>(current_byte)
          << "): "
          << description;

        m_where = where;
        m_what = strm.str();
      }
  };



  /** Exception class for reporting errors when creating programs. */
  class Encode_error : public Error_base {
    public:
      Encode_error(std::string const& where,
          std::string const& inst_name,
          std::string const& description)
        : Error_base() {
        std::stringstream strm;
        strm << "encoding error for instruction '"
          << inst_name
          << "': "
          << description;

        m_where = where;
        m_what = strm.str();
      }
  };


  /** Exception class for reporting errors with spiketrains. */
  class Spiketrain_error : public Error_base {
    public:
      Spiketrain_error(std::string const& where,
          std::string const& description) {
        std::stringstream strm;
        strm << "spiketrain error: " << description;
        m_where = where;
        m_what = strm.str();
      }
  };


}


#undef NOEXCEPT

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
