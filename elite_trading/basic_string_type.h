#pragma once

#include <string>
#include <cstdint>

typedef std::int32_t i32;
typedef std::uint32_t u32;
typedef std::int64_t i64;
typedef std::uint64_t u64;

class BasicStringType{
public:
	u64 id;
	std::string name;
};

//#define DEFINE_BASIC_STRING_TYPE(x) \
//class x{ \
//public: \
//	u64 id; \
//	std::string name; \
//}
#define DEFINE_BASIC_STRING_TYPE(x) typedef BasicStringType x

DEFINE_BASIC_STRING_TYPE(Economy);
DEFINE_BASIC_STRING_TYPE(Ship);
DEFINE_BASIC_STRING_TYPE(Government);
DEFINE_BASIC_STRING_TYPE(Allegiance);
DEFINE_BASIC_STRING_TYPE(PlaceState);
DEFINE_BASIC_STRING_TYPE(Security);
DEFINE_BASIC_STRING_TYPE(Power);
DEFINE_BASIC_STRING_TYPE(PowerState);
DEFINE_BASIC_STRING_TYPE(StationType);
DEFINE_BASIC_STRING_TYPE(ModuleCategory);
DEFINE_BASIC_STRING_TYPE(CommodityCategory);
