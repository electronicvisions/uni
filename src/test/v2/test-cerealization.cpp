#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cereal/archives/json.hpp>

#include "uni/v2/program_builder.h"

TEST(Program_builder, CerealizeCoverage)
{
	uni::Byte_vector_allocator allocator;

	uni::Program_builder<decltype(allocator)> obj1(allocator);
	uni::Program_builder<decltype(allocator)> obj2(allocator);

	std::ostringstream ostream;
	{
		cereal::JSONOutputArchive oa(ostream);
		oa(obj1);
	}

	std::istringstream istream(ostream.str());
	{
		cereal::JSONInputArchive ia(istream);
		ia(obj2);
	}
	ASSERT_TRUE(obj1 == obj2);
}
