#pragma once

#include "basic_string_type.h"
#include <boost/optional.hpp>
#include <memory>
#include <vector>
#include "sqlitepp.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

using namespace sqlite3pp;
class Station;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> json_value;
class ED_Info;

class StarSystem{
public:
	u64 id;
	boost::optional<u64> population;
	double x, y, z;
	std::string name;
	std::string faction;
	std::shared_ptr<Government> government;
	std::shared_ptr<Allegiance> allegiance;
	std::shared_ptr<PlaceState> state;
	std::shared_ptr<Security> security;
	std::shared_ptr<Economy> primary_economy;
	std::shared_ptr<Power> power;
	std::shared_ptr<PowerState> power_state;
	boost::optional<bool> needs_permit;
	u64 updated_at;

	std::vector<std::shared_ptr<Station>> stations;
	std::vector<std::pair<StarSystem *, double>> nearby_systems;

	StarSystem(Statement &stmt, ED_Info &info);
	StarSystem(const json_value &, ED_Info &info);
	void add_station(const std::shared_ptr<Station> &station);
	void add_route(const std::shared_ptr<StarSystem> &dst, double d){
		this->nearby_systems.push_back(std::make_pair(dst.get(), d));
	}

	std::vector<StarSystem*> find_fastest_route(StarSystem *system, double max_distance = -1);
	double distance(const std::shared_ptr<StarSystem> &dst) const{
		return this->distance(dst.get());
	}
	double distance(StarSystem *dst) const{
		double a = this->x - dst->x;
		double b = this->y - dst->y;
		double c = this->z - dst->z;
		return sqrt(a * a + b * b + c * c);
	}

	void save(Statement &statement, Statement &new_navigation_route);
};
