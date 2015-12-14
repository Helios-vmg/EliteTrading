#pragma once

#include "basic_string_type.h"
#include "commodity.h"
#include "star_system.h"
#include "module.h"
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

enum class StationExtra{
	Import = 0,
	Export,
	Prohibited,
	Economy,
	Ship,
	Module,
};

class EconomicEntry{
public:
	std::shared_ptr<Commodity> commodity;
	u64 id, demand, supply, buy, sell, collected_at, update_count;
	Station *station;

	EconomicEntry(Statement &stmt, const std::shared_ptr<Commodity> &commodity)
	{
		this->commodity = commodity;
		stmt
			>> this->supply
			>> this->demand
			>> this->buy
			>> this->sell
			>> this->collected_at
			>> this->update_count;
	}
	EconomicEntry(u64 listing[9], const std::shared_ptr<Commodity> &commodity);
};

class Station{
public:
	u64 id;
	std::string name;
	u64 system_id;
	unsigned max_landing_pad_size;
	boost::optional<double> distance_to_star;
	std::string faction;
	std::shared_ptr<Government> government;
	std::shared_ptr<Allegiance> allegiance;
	std::shared_ptr<PlaceState> state;
	std::shared_ptr<StationType> type;
	boost::optional<bool> has_blackmarket;
	boost::optional<bool> has_market;
	boost::optional<bool> has_refuel;
	boost::optional<bool> has_repair;
	boost::optional<bool> has_rearm;
	boost::optional<bool> has_outfitting;
	boost::optional<bool> has_shipyard;
	boost::optional<bool> has_commodities;
	std::vector<std::shared_ptr<Commodity>> import_commodities;
	std::vector<std::shared_ptr<Commodity>> export_commodities;
	std::vector<std::shared_ptr<Commodity>> prohibited_commodities;
	std::vector<std::shared_ptr<Economy>> main_economies;
	std::vector<std::shared_ptr<Ship>> selling_ships;
	std::vector<std::shared_ptr<Module>> selling_modules;
	u64 updated_at;
	boost::optional<u64> shipyard_updated_at;
	StarSystem *system;
	std::vector<EconomicEntry> economy;

	Station(Statement &stmt, ED_Info &);
	Station(const json_value &, ED_Info &);
	void add_extra(Statement &stmt, const ED_Info &info);
	void add_economy(Statement &stmt, const std::shared_ptr<Commodity> &commodity);
	void add_economy(u64 listing[9], const std::shared_ptr<Commodity> &commodity);
	std::vector<StarSystem*> find_fastest_route(Station *station, double max_distance = -1){
		return this->system->find_fastest_route(station->system, max_distance);
	}

	u64 find_sell_price(Commodity *commodity);
	void save(
		Statement &new_station,
		Statement &new_extra,
		Statement &new_station_economy
	);
	unsigned get_hasness_bitmap() const;
	void parse_hasness_bitmap(unsigned);
};
