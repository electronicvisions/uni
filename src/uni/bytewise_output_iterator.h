#pragma once

#include <cstdint>
#include <iterator>
#include <ostream>
#include <type_traits>

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


namespace detail {

template<typename T, typename Enable = void>
struct give_real_value {
	static_assert(sizeof(T) == -1, "Not defined for this type");
};

template<typename T>
struct give_real_value<T,
	typename std::enable_if<std::is_pod<
		typename std::remove_pointer<T>::type>::value>::type>
{
	typedef typename std::remove_pointer<T>::type type;
};

template<typename T>
struct give_real_value<T,
	typename std::enable_if<!std::is_pod<
		typename std::remove_pointer<T>::type>::value>::type>
{
	// we believe it's a container...
	typedef typename T::value_type type;
};

// TODO: move to HATE or use boost?
template<typename T>
struct has_iterator {
private:
	typedef char yes;
	typedef struct { char array[2]; } no;

	template<typename C> static yes test(typename C::iterator*);
	template<typename C> static no test(...);
public:
	static const bool value = sizeof(test<T>(0)) == sizeof(yes);
	typedef T type;
};

template<typename T, typename Enable = void>
struct give_iterator_or_pointer {
	// use raw pointer if no iterator is available
	typedef typename std::remove_pointer<T>::type* type;
};

template<typename T>
struct give_iterator_or_pointer<T,
	typename std::enable_if<
		has_iterator<T>::value
	>::type
>
{
	typedef typename T::iterator type;
};

} // namespace detail


template<typename T>
class raw_byte_iterator
{
public:
	// cf. standard
	typedef int difference_type; // FIXME: +detail and select type?
	typedef std::uint8_t value_type;
	typedef value_type& reference;
	typedef value_type* pointer;
	typedef std::forward_iterator_tag iterator_category;

private:
	typedef typename detail::give_real_value<T>::type real_value_type;
	typedef typename detail::give_iterator_or_pointer<T>::type real_iterator_type;

public:

	enum class byte_order {
		NORMAL,
		FLIPPED
	};

	raw_byte_iterator(real_iterator_type it, byte_order const mode = byte_order::NORMAL) :
		m_it(it),
		m_ptr(mode == byte_order::NORMAL
				? reinterpret_cast<value_type*>(&*it)
				: (reinterpret_cast<value_type*>(&*it)+ sizeof(real_value_type) - 1)),
		m_byte_order(mode)
	{}


	raw_byte_iterator operator++() {
		m_idx++;
		if (m_idx < sizeof(real_value_type)) {
			// advance byte-in-word by 1
			if (m_byte_order == byte_order::FLIPPED) {
				m_ptr--;
			} else {
				m_ptr++;
			}
		} else {
			// go to next word
			m_idx = 0;
			std::advance(m_it, 1);
			if (m_byte_order == byte_order::FLIPPED) {
				m_ptr = reinterpret_cast<value_type*>(&*m_it) + sizeof(real_value_type) - 1;
			} else {
				m_ptr = reinterpret_cast<value_type*>(&*m_it);
			}
		}
		return *this;
	}

	reference operator*() {
		return *m_ptr;
	}

	// not needed as we only contain plain values?
	// pointer operator->();

	bool operator==(raw_byte_iterator const& rhs) const {
		return m_ptr == rhs.m_ptr;
	}

	bool operator!=(raw_byte_iterator const& rhs) const {
		return m_ptr != rhs.m_ptr;
	}

private:
	// the word iterator
	real_iterator_type m_it; // or iterator
	// byte pointer
	value_type* m_ptr;
	// byte index
	size_t m_idx = 0;
	// support old access mode... (read: HACK!)
	byte_order const m_byte_order;
};


// for use with containers (providing T::{begin,end}), e.g. in for loops
template <typename T>
class container_proxy_raw_byte_iterator
{
public:
	using byte_order = typename raw_byte_iterator<T>::byte_order;

	container_proxy_raw_byte_iterator(T& d, byte_order const mode = byte_order::NORMAL) :
		m_data(d),
		m_byte_order(mode)
	{}

	raw_byte_iterator<T> begin() {
		return raw_byte_iterator<T>(m_data.begin(), m_byte_order);
	}

	raw_byte_iterator<T> end() {
		return raw_byte_iterator<T>(m_data.end(), m_byte_order);
	}

	/* TODO: incomplete implementation? */
	raw_byte_iterator<T> begin() const {
		return raw_byte_iterator<T>(m_data.begin(), m_byte_order);
	}

	raw_byte_iterator<T> end() const {
		return raw_byte_iterator<T>(m_data.end(), m_byte_order);
	}


private:
	T& m_data;
	typename raw_byte_iterator<T>::byte_order const m_byte_order;
};

} // namespace uni

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
