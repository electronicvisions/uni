#include <uni/uni.h>
#include <uni/rw_extract_decoder.h>
#include <uni/spiketrain_decoder.h>

#include <gtest/gtest.h>
#include <iostream>
#include <array>


//TEST(uni, general_usage) {
  //std::vector<uint32_t> words;

  //uni::fill_set_time(std::back_inserter(words), 0);
  //uni::fill_wait_until(std::back_inserter(words), 100);


  //[>uni::Program_builder bld;

  //bld.set_time(0);
  //bld.wait_until(100);
  //bld.write(0, 0);
  //bld.wait_for(20);
  //bld.read(0);
  //bld.halt();


  //std::vector<uint32_t> words;
  //uni::export_to_words(bld.begin(), bld.end(), std::back_inserter(words));*/
//}


TEST(uni, instruction_generation) {
  using namespace uni;

  std::array<uint32_t,32> words;
  words.fill(0);

  auto p = bytewise(words.begin());
  p = fill_set_time(p, 9);
  p = fill_wait_until(p, 0x1000);
  p = fill_write(p, 0, 0xdeadface);
  p = fill_read(p, 0);
  p = fill_wait_for_7(p, 113);
  p = fill_halt(p);

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  EXPECT_EQ(0x00000000, words[0]);
  EXPECT_EQ(0x00000000, words[1]);
  EXPECT_EQ(0x09010000, words[2]);
  EXPECT_EQ(0x00000000, words[3]);
  EXPECT_EQ(0x10000a00, words[4]);
  EXPECT_EQ(0x000000de, words[5]);
  EXPECT_EQ(0xadface0b, words[6]);
  EXPECT_EQ(0x00000000, words[7]);
  EXPECT_EQ(0xf10e0000, words[8]);

  for(std::size_t i=9; i<words.size(); i++)
    EXPECT_EQ(0, words[i]);
}




TEST(uni, instruction_decoding) {
  using namespace uni;

  std::array<uint32_t,9> words {
    0x00000000,
    0x00000000,
    0x09010000,
    0x00000000,
    0x10000a00,
    0x000000de,
    0xadface0b,
    0x00000000,
    0xf10e0000
  };

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;


  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(words)),
      bytewise(std::end(words)),
      printer);
}


TEST(uni, instruction_decoding_2) {
  using namespace uni;

  std::array<uint32_t,2> words {
    0x850baffe,
    0xaffe0e00
  };

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(words)),
      bytewise(std::end(words)),
      printer);
}


TEST(uni, instruction_decoding_3) {
  using namespace uni;

  std::array<uint32_t,32> words;

  words.fill(0);

  auto p = bytewise(words.begin());
  p = fill_set_time(p, 9);
  p = fill_wait_until(p, 0x1000);
  p = fill_write(p, 0, 0xdeadface);
  p = fill_read(p, 0);
  p = fill_wait_for_7(p, 113);
  p = fill_wait_for_16(p, 0xfeef);
  p = fill_wait_for_32(p, 0xdeadface);
  p = fill_halt(p);

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(words)),
      bytewise(std::end(words)),
      printer);
}


TEST(uni, raw_encode_decode) {
  using namespace uni;

  std::array<uint32_t,32> words;
  std::vector<Byte> raw_data(16, 0xaa);

  words.fill(0);

  auto p = bytewise(words.begin());

  p = fill_set_time(p, 0);
  p = fill_rec_start(p);
  p = fill_raw(p, raw_data);
  p = fill_wait_for_16(p, 100);
  p = fill_rec_stop(p);
  p = fill_halt(p);

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(words)),
      bytewise(std::end(words)),
      printer);
}


TEST(uni, rw_extract) {
  using namespace uni;

  std::array<uint32_t,32> words;

  words.fill(0);

  auto p = bytewise(words.begin());
  p = fill_set_time(p, 9);
  p = fill_wait_until(p, 0x1000);
  p = fill_write(p, 0, 0xdeadface);
  p = fill_read(p, 0);
  p = fill_wait_for_7(p, 113);
  p = fill_wait_for_16(p, 0xfeef);
  p = fill_wait_for_32(p, 0xdeadface);
  p = fill_halt(p);

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(words)),
      bytewise(std::end(words)),
      printer);

  Rw_extract_decoder rws;

  decode(bytewise(words.begin()),
      bytewise(words.end()),
      rws);

  ASSERT_EQ(2, rws.extracted.size());

  EXPECT_EQ(true, rws.extracted[0].is_write);
  EXPECT_EQ(0, rws.extracted[0].write.address);
  EXPECT_EQ(0xdeadface, rws.extracted[0].write.data);

  EXPECT_FALSE(rws.extracted[1].is_write);
  EXPECT_EQ(0, rws.extracted[1].read.address);
}


TEST(uni, program_builder) {
  using namespace uni;


  Byte_vector_allocator alloc;
  Program_builder<Byte_vector_allocator> bld(alloc);

  bld.wait_for(3405435);

  for(auto& c : bld.containers) {
    std::cout << "chunk:\n"
      << as_bytes(std::begin(c), std::end(c))
      << '\n';
  }
}


TEST(uni, fire_coding) {
  using namespace uni;

  std::array<uint32_t,32> words;
  words.fill(0);

  auto p = bytewise(words.begin());
  p = fill_fire(p, 0x1, 0x3f);
  p = fill_fire_one(p, 0x3f, 0xaf);
  p = fill_halt(p);

  std::cout << as_bytes(words.begin(), words.end()) << std::endl;

  EXPECT_EQ(0x0f000000, words[0]);
  EXPECT_EQ(0x00000000, words[1]);
  EXPECT_EQ(0x013f7faf, words[2]);
  EXPECT_EQ(0x0e000000, words[3]);

  for(std::size_t i=4; i<words.size(); i++)
    EXPECT_EQ(0, words[i]);
}

TEST(uni, spiketrain_codec) {
  using namespace uni;

  static int const num_addr = 64;
  static int const num_spikes = 1000;

  Byte_vector_allocator alloc;
  Program_builder<Byte_vector_allocator> bld(alloc);
  std::vector<Spike> spiketrain;

  int row_i = 0;
  for(int i=0; i<num_spikes; ++i) {
    spiketrain.emplace_back(1000 + i*100, (row_i << 8) | (i % num_addr));
    if( (i % num_addr) == num_addr - 1 )
      ++row_i;
    //bld.fire_one(0, i % 64);
    //bld.wait_for(100);
  }

  Standard_address_map addr_map;
  bld.spiketrain(std::begin(spiketrain),
      std::end(spiketrain),
      addr_map);

  bld.halt();

  ASSERT_EQ(1, bld.containers.size());

  //std::cout << "coded program:\n"
    //<< as_bytes(std::begin(bld.containers[0]), std::end(bld.containers[0]))
    //<< std::endl;
  Stream_decoder printer(std::cout);
  decode(bytewise(std::begin(bld.containers[0])),
      bytewise(std::end(bld.containers[0])),
      printer);


  Standard_spiketrain_decoder spike_dec;
  decode(std::begin(bld.containers[0]),
      std::end(bld.containers[0]),
      spike_dec);

  ASSERT_EQ(spiketrain.size(), spike_dec.extracted.size());

  for(size_t i=0; i<spiketrain.size(); ++i) {
    EXPECT_EQ(spiketrain[i], spike_dec.extracted[i]);
  }
}

/* vim: set et fenc= ff=unix sts=0 sw=2 ts=2 : */
