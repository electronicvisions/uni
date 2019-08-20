#include <gtest/gtest.h>
#include <iostream>
#include <algorithm>

#include <uni/v2/bytewise_output_iterator.h>

TEST(bytewise, writing) {
	using namespace uni;

	std::vector<uint32_t> words(2);
	std::vector<uint32_t> words_new(2);

	Bytewise_output_iterator<uint32_t, std::vector<uint32_t>::iterator> oit(words.begin());

	// SF wants the 0th byte to come first, i.e. to be big endian? use HACK
	raw_byte_iterator<std::vector<std::uint32_t>> oit_new(words_new.begin(),
		raw_byte_iterator<std::vector<std::uint32_t>>::byte_order::FLIPPED);

	for(int i=0; i<8; i++) {
		*oit = 0xff & i;
		++oit;
		*oit_new = 0xff & i;
		++oit_new;
	}

	std::cout << std::hex << std::setfill('0')
		<< "0: " << std::setw(8) << words[0] << '\n'
		<< "1: " << std::setw(8) << words[1]
		<< std::endl;

	EXPECT_EQ(0x00010203, words[0]);
	EXPECT_EQ(0x04050607, words[1]);

	EXPECT_EQ(0x00010203, words_new[0]);
	EXPECT_EQ(0x04050607, words_new[1]);
}


TEST(bytewise, std_algol) {
	using namespace uni;

	std::vector<uint32_t> words(2);
	std::vector<uint32_t> words_new(2);

	std::fill_n(bytewise(words.begin()), 8, 0x01);
	std::fill_n(raw_byte_iterator<std::vector<std::uint32_t>>(words_new.begin(),
		raw_byte_iterator<std::vector<std::uint32_t>>::byte_order::FLIPPED), 8, 0x01);

	std::cout << std::hex << std::setfill('0')
		<< "0: " << std::setw(8) << words[0] << '\n'
		<< "1: " << std::setw(8) << words[1]
		<< std::endl;

	EXPECT_EQ(0x01010101, words[0]);
	EXPECT_EQ(0x01010101, words[1]);

	EXPECT_EQ(0x01010101, words_new[0]);
	EXPECT_EQ(0x01010101, words_new[1]);

	std::fill(bytewise(words.begin()), bytewise(words.end()), 0x02);
	auto proxy = container_proxy_raw_byte_iterator<std::vector<uint32_t>>(words_new,
			raw_byte_iterator<std::vector<uint32_t>>::byte_order::FLIPPED);
	std::fill(proxy.begin(), proxy.end(), 0x02);

	EXPECT_EQ(0x02020202, words[0]);
	EXPECT_EQ(0x02020202, words[1]);

	EXPECT_EQ(0x02020202, words_new[0]);
	EXPECT_EQ(0x02020202, words_new[1]);

	words[0] = 0;
	words[1] = 0;
	words_new[0] = 0;
	words_new[1] = 0;
	std::fill_n(bytewise(words.begin()), 7, 0x03);
	std::fill_n(raw_byte_iterator<std::vector<std::uint32_t>>(words_new.begin(), decltype(proxy)::byte_order::FLIPPED), 7, 0x03);
	EXPECT_EQ(0x03030303, words[0]);
	EXPECT_EQ(0x03030300, words[1]);
	EXPECT_EQ(0x03030303, words_new[0]);
	EXPECT_EQ(0x03030300, words_new[1]);
}

#if !(GTEST_HAS_TYPED_TEST)
#error "We need gtest's typed tests"
#endif

template <typename T>
class TypedBytewise : public testing::Test {};

typedef ::testing::Types<std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
	std::int8_t, std::int16_t, std::int32_t, std::int64_t> TypedBytewiseTestTypes;

TYPED_TEST_CASE(TypedBytewise, TypedBytewiseTestTypes);

TYPED_TEST(TypedBytewise, iterator) {
	using namespace uni;

	typedef std::vector<TypeParam> container_type;
	container_type words;
	// create test data:
	//       0x01
	//     0x0200
	//   0x030000
	// 0x04000000
	// ...
	for (size_t i = 0; i < sizeof(TypeParam); i++) {
		words.push_back((i+1) << 8*i);
	}

	auto create_reverse = [](container_type const& d) {
		std::vector<TypeParam> d_reverse(d.size());
		std::reverse_copy(std::begin(d), std::end(d), std::begin(d_reverse));
		return d_reverse;
	};

	{ // input iterator
		{ // for range loop
			auto words_reverse = create_reverse(words);
			TypeParam tmp = 0;
			size_t i = 0;
			// iterate bytewise and build words to compare against original data
			for (auto elem : container_proxy_raw_byte_iterator<container_type>(words)) {
				tmp |= (static_cast<TypeParam>(elem) << (8*i++));
				if (i == sizeof(TypeParam)) {
					EXPECT_EQ(tmp, words_reverse.back());
					words_reverse.pop_back();
					i = 0;
					tmp = 0;
				}
			}
		}

		{ // std::copy
			std::vector<std::uint8_t> bytes;
			auto proxy = container_proxy_raw_byte_iterator<container_type>(words);
			std::copy(proxy.begin(), proxy.end(), std::back_inserter(bytes));

			EXPECT_EQ(bytes.size(), sizeof(TypeParam) * words.size());

			auto words_reverse = create_reverse(words);
			TypeParam tmp = 0;
			size_t i = 0;
			// same
			for (auto elem : bytes) {
				tmp |= (static_cast<TypeParam>(elem) << (8*i++));
				if (i == sizeof(TypeParam)) {
					EXPECT_EQ(tmp, words_reverse.back());
					words_reverse.pop_back();
					i = 0;
					tmp = 0;
				}
			}
		}
	}

	{ // output iterator test
		container_type words_fill(4);
		std::fill_n(raw_byte_iterator<container_type>(words_fill.begin()), sizeof(TypeParam) * words_fill.size(), 0x12);

		TypeParam tmp = 0;
		for (size_t ii = 0; ii < sizeof(TypeParam); ii++) {
			tmp |= static_cast<TypeParam>(0x12) << (8*ii);
		}
		EXPECT_EQ(tmp, words_fill[0]);
		EXPECT_EQ(tmp, words_fill[1]);
		EXPECT_EQ(tmp, words_fill[2]);
		EXPECT_EQ(tmp, words_fill[3]);
	}

	{ // output iterator test
		container_type words_fill(4);
		auto proxy = container_proxy_raw_byte_iterator<container_type>(words_fill);
		std::fill(proxy.begin(), proxy.end(), 0x12);

		TypeParam tmp = 0;
		for (size_t ii = 0; ii < sizeof(TypeParam); ii++) {
			tmp |= static_cast<TypeParam>(0x12) << (8*ii);
		}
		EXPECT_EQ(tmp, words_fill[0]);
		EXPECT_EQ(tmp, words_fill[1]);
		EXPECT_EQ(tmp, words_fill[2]);
		EXPECT_EQ(tmp, words_fill[3]);
	}

	{
		// some more compile checks
		auto it1 = container_proxy_raw_byte_iterator<container_type>(words);
		it1.begin();
		it1.end();
		std::begin(it1);
		std::end(it1);
		std::cbegin(it1);
		std::cend(it1);
	}

	{
		container_type mywords;
		raw_byte_iterator<container_type>(mywords.begin());
		raw_byte_iterator<TypeParam>(mywords.data());

		TypeParam raw[100];
		static_cast<void>(raw_byte_iterator<TypeParam>(raw));
	}
}
