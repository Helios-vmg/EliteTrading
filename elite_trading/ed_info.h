#pragma once

#include "basic_string_type.h"
#include "route_segment.h"
#include "star_system.h"
#include "station.h"
#include "module.h"
#include <iostream>
#include <mutex>
#include <vector>
#include <map>
#include <memory>
#include <atomic>
#include <boost/format.hpp>
#include "sqlitepp.h"
#include "rapidjson/rapidjson.h"

extern const char * const database_path;
extern const char * const database_journal_path;

using namespace sqlite3pp;
class Station;
class Module;
class Commodity;
class TradingLocation;
class ED_Info;
typedef rapidjson::GenericValue<rapidjson::UTF8<>> json_value;

struct LocationOption{
	bool is_station;
	u64 id;
	std::string name;
	size_t similarity;
};

template <typename T>
std::shared_ptr<T> get_basic_string_type(std::vector<std::shared_ptr<T>> &vector, std::map<std::string, std::shared_ptr<T>> &map, const std::string &name){
	std::shared_ptr<T> ret;
	if (!name.size())
		return ret;
	auto it = map.find(name);
	if (it == map.end()){
		ret.reset(new T{vector.size(), name});
		vector.push_back(ret);
		map[name] = ret;
	}else
		ret = it->second;
	return ret;
}

enum class OptimizationType{
	OptimizeEfficiency = 1,
	OptimizeProfit = 2,
};

struct RouteSearchConstraints{
	u64 initial_funds;
	u64 minimum_profit_per_unit;
	double laden_jump_distance;
	double search_radius;
	u32 max_capacity;
	u32 required_stops;
	u32 optimization;
	i32 max_price_age_days;
	u8 require_large_pad;
	u8 avoid_loops;
	u8 avoid_permit_systems;
};

struct ImportDataCommand{};

class ED_Info{
public:
	typedef void (*progress_callback_f)(const char *);
private:
	void read_strings(DB &db);
	void read_systems(DB &db);
	void read_navigation_routes(DB &db);
	void read_stations(DB &db);
	void read_module_groups(DB &db);
	void read_modules(DB &db);
	void read_ships(DB &db);
	void read_economies(DB &db);
	void read_station_extras(DB &db);
	void read_commodities(DB &db);
	void read_station_economies(DB &db);
	void read_systems_json();
	void read_modules_json();
	void read_stations_json();
	void read_commodities_json();
	void read_economy_csv();
	void compact_ids();
	void generate_routing_table();
	void save_systems(DB &db);
	void save_stations(DB &db);
	void save_commodities(DB &db);
	void save_modules(DB &db);
	void save_module_groups(DB &db);
	void fill_averages();
	void save_string_table(DB &db, const std::vector<std::shared_ptr<BasicStringType>> &vector, const char *table_name);
	std::atomic<u64> progress;
	std::mutex cout_mutex;
	struct thread_func_params{
		ED_Info *_this;
		unsigned thread_id;
		unsigned thread_count;
		std::map<u64, std::vector<std::shared_ptr<TradingLocation>>> *src_locations;
		std::map<u64, std::vector<std::shared_ptr<TradingLocation>>> *dst_locations;
		Statement *route_stmt;
		DB *db;
		std::mutex *db_mutex;
		double max_stop_distance;
		u64 min_profit_per_unit;
	};
	static void thread_func(thread_func_params params);
	std::vector<StarSystem *> find_route_candidate_systems(const StarSystem *around_system, double radius);
	std::map<std::string, std::shared_ptr<Commodity>> commodities_by_name;
	progress_callback_f progress_callback;
public:
	std::vector<std::shared_ptr<StarSystem>> systems;
	std::vector<std::shared_ptr<ModuleGroup>> module_groups;
	std::vector<std::shared_ptr<Module>> modules;
	std::vector<std::shared_ptr<Station>> stations;
	std::vector<std::shared_ptr<Commodity>> commodities;
#define DEFINE_SIMPLE_STRING_TYPE_THINGS(type, singular, plural) \
	std::vector<std::shared_ptr<type>> plural; \
private: \
	std::map<std::string, std::shared_ptr<type>> plural##_by_name; \
public: \
	std::shared_ptr<type> ED_Info::get_##singular(const std::string &name){ \
		return get_basic_string_type(this->plural, this->plural##_by_name, name); \
	} \
	std::shared_ptr<type> ED_Info::get_##singular(const boost::optional<u64> &id){ \
		if (!id.is_initialized()) \
			return nullptr; \
		return this->plural[id.value()]; \
	}
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Economy, economy, economies)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Ship, ship, ships)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Government, government, governments)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Allegiance, allegiance, allegiances)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(PlaceState, state, states)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Security, security, security_types)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(Power, power, powers)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(PowerState, power_state, power_states)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(StationType, station_type, station_type)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(ModuleCategory, module_category, module_categories)
	DEFINE_SIMPLE_STRING_TYPE_THINGS(CommodityCategory, commodity_category, commodity_categories)

	ED_Info(progress_callback_f);
	ED_Info(const ImportDataCommand &, progress_callback_f);
	void recompute_all_routes(double max_stop_distance, u64 min_profit_per_unit);
	std::vector<LocationOption> location_search(const std::string &cs);
	std::vector<RouteNodeInterop *> find_routes(const std::shared_ptr<Station> &around_station, const RouteSearchConstraints &constraints){
		return this->find_routes(around_station.get(), constraints);
	}
	std::vector<RouteNodeInterop *> find_routes(const std::shared_ptr<StarSystem> &around_system, const RouteSearchConstraints &constraints){
		auto temp_station = Station::create_virtual_station(around_system.get());
		return this->find_routes(temp_station.get(), constraints);
	}
	std::vector<RouteNodeInterop *> find_routes(Station *around_station, const RouteSearchConstraints &constraints);
	void save_to_db();
	std::shared_ptr<Commodity> ED_Info::get_commodity(const std::string &name);
};
