@mainpage

For a more generic introduction see the manual in doc/manual.



General structure of the source code
====================================

This software implements encoding and decoding of UNI byte-code. The
low-level coding and decoding is realized by read and fill functions
defined in instructions.h. The user should use uni::Program_builder for
encoding programs. For decoding there is the uni::decode() template
function that works in conjunction with decoder classes.


User facilities
===============

Encoding programs
-----------------

The user will generally want to use uni::Program_builder to construct
programs. In the easies case use uni::Byte_vector_allocator as
Allocator for uni::Program_builder to construct your programs into
std::vector<uint8_t> buffers.
uni::Program_builder::spiketrain() provides special support to encode
spiketrain data. It assumes spikes to be represented by uni::Spike and
requires an address map (e.g. uni::Standard_address_map) to translate
between uni::Spike::address and index and evaddr of uni::fill_fire_one() and
uni::fill_fire().


Decoding programs
-----------------

Decoding is driven by the uni::decode() template function, that takes a
decoder class as argument. A couple of standard decoder classes are
provided:

- uni::Stream_decoder
- uni::Rw_extract_decoder
- uni::Spiketrain_decoder
- uni::Raw_extract_decoder
- uni::Raw_reshape_decoder

A decoder class is a callable that can use function overloading to
handle different uni::Instruction subtypes differently.



Low-level codec
===============

The high-level uni::Program_builder and uni::decode() use these
low-level facilities to perform the actual byte-code coding and
decoding.


Encoding Instructions:
----------------------

These functions write byte-code for the respective UNI instruction into
a given buffer.

- uni::fill_set_time()
- uni::fill_wait_until()
- uni::fill_wait_for_7()
- uni::fill_wait_for_16()
- uni::fill_wait_for_32()
- uni::fill_halt()
- uni::fill_read()
- uni::fill_write()
- uni::fill_rec_start()
- uni::fill_rec_stop()
- uni::fill_fire()
- uni::fill_fire_one()


Decoding Instructions:
----------------------

These functions extract a given UNI instruction from byte-code and
convert it to an object of a type derived from uni::Instruction.

- uni::read_set_time()
- uni::read_wait_until()
- uni::read_wait_for_7()
- uni::read_wait_for_16()
- uni::read_wait_for_32()
- uni::read_halt()
- uni::read_read()
- uni::read_write()
- uni::read_rec_start()
- uni::read_rec_stop()
- uni::read_fire()
- uni::read_fire_one()


Checking if buffer is large enough to encode instruction:
---------------------------------------------------------

These functions check if the given UNI instruction can be written in the
provided buffer.

- uni::check_set_time()
- uni::check_wait_until()
- uni::check_wait_for_7()
- uni::check_wait_for_16()
- uni::check_wait_for_32()
- uni::check_halt()
- uni::check_read()
- uni::check_write()
- uni::check_rec_start()
- uni::check_rec_stop()
- uni::check_fire()
- uni::check_fire_one()



Utility types
=============

Error handling
--------------

There are a couple of predefined types for exception handling:

- uni::Error_base
- uni::Decode_error
- uni::Encode_error
- uni::Spiketrain_error


Bytewise iterations
-------------------

There is uni::Bytewise_output_iterator to iterate byte-wise through types of
larger word size, e.g. uint32_t. It is especially intended to use
uni::Program_builder to construct programs directly in kernel memory for
USB transactions using rw_api.
uni::Byte_printer allows pretty printing of binary data.


