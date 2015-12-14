#pragma once

#include "basic_string_type.h"
#include <boost/optional.hpp>
#include <memory>
#include "sqlitepp.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

using namespace sqlite3pp;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> json_value;

class Commodity{
public:
	u64 id;
	u64 category_id;
	std::string name;
	boost::optional<u64> average_price;
	std::shared_ptr<CommodityCategory> category;

	Commodity(Statement &stmt){
		stmt >> this->id >> this->category_id >> this->name >> this->average_price;
	}
	Commodity(const json_value &);
	void save(Statement &new_commodity);
};
