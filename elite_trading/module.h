#pragma once

#include "basic_string_type.h"
#include <boost/optional.hpp>
#include <memory>
#include <string>
#include "sqlitepp.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

using namespace sqlite3pp;
class Station;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> json_value;
class ED_Info;

class ModuleGroup{
public:
	u64 id;
	std::string name;
	std::shared_ptr<ModuleCategory> category;

	ModuleGroup(Statement &stmt, ED_Info &);
	ModuleGroup(const json_value &, ED_Info &);
	void save(Statement &new_module_group);
};

class Module{
public:
	u64 id;
	u64 group_id;
	u64 klass;
	std::string rating;
	boost::optional<u64> price;
	std::string weapon_mode;
	std::string missile_type;
	std::string name;
	std::string belongs_to;
	std::string ship;
	std::shared_ptr<ModuleGroup> group;

	Module(Statement &stmt);
	Module(const json_value &, ED_Info &);
	void save(Statement &new_modules);
};
