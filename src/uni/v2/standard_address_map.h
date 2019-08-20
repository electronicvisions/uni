#pragma once


namespace uni {

/** Translate logic spike-event addresses to index and address for the
 * UNI program */
struct Standard_address_map
{
	uint8_t index(uint64_t address) const
	{
		return (address & 0x1f00) >> 8;
	}

	uint8_t evaddr(uint64_t address) const
	{
		return address & 0x3f;
	}

	uint64_t address_from_fire(uint8_t index, uint8_t /*evaddr*/) const
	{
		if (index < 32)
			return 31 - index;
		else
			return 63 - index + 32;
	}

	uint64_t address_from_fire_one(uint8_t index, uint8_t evaddr) const
	{
		return ((index & 0x1f) << 8) | (evaddr & 0x3f);
	}
};
}
