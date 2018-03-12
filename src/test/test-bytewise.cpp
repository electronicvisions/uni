#include <gtest/gtest.h>
#include <iostream>
#include <algorithm>

#include <uni/bytewise_output_iterator.h>

TEST(bytewise, writing) {
	using namespace uni;

	std::vector<uint32_t> words(2);

	Bytewise_output_iterator<uint32_t, std::vector<uint32_t>::iterator> oit(words.begin());

	for(int i=0; i<8; i++) {
		*oit = 0xff & i;
		++oit;
	}

	std::cout << std::hex << std::setfill('0')
		<< "0: " << std::setw(8) << words[0] << '\n'
		<< "1: " << std::setw(8) << words[1]
		<< std::endl;

	EXPECT_EQ(0x00010203, words[0]);
	EXPECT_EQ(0x04050607, words[1]);
}


TEST(bytewise, std_algol) {
	using namespace uni;

	std::vector<uint32_t> words(2);

	std::fill_n(bytewise(words.begin()), 8, 0x01);

	std::cout << std::hex << std::setfill('0')
		<< "0: " << std::setw(8) << words[0] << '\n'
		<< "1: " << std::setw(8) << words[1]
		<< std::endl;

	EXPECT_EQ(0x01010101, words[0]);
	EXPECT_EQ(0x01010101, words[1]);


	std::fill(bytewise(words.begin()), bytewise(words.end()), 0x02);

	EXPECT_EQ(0x02020202, words[0]);
	EXPECT_EQ(0x02020202, words[1]);


	words[0] = 0;
	words[1] = 0;
	std::fill_n(bytewise(words.begin()), 7, 0x03);
	EXPECT_EQ(0x03030303, words[0]);
	EXPECT_EQ(0x03030300, words[1]);
}

