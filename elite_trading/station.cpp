#include "station.h"
#include "ed_info.h"
#include "util.h"

Station::Station(Statement &stmt, ED_Info &info){
	unsigned hasness_bitmap;
	boost::optional<u64> government, allegiance, state, type;
	stmt
		>> this->id
		>> this->system_id
		>> this->name
		>> this->max_landing_pad_size
		>> this->distance_to_star
		>> this->faction
		>> government
		>> allegiance
		>> state
		>> type
		>> hasness_bitmap
		>> this->updated_at
		>> this->shipyard_updated_at;
	this->parse_hasness_bitmap(hasness_bitmap);
	this->government = info.get_government(government);
	this->allegiance = info.get_allegiance(allegiance);
	this->state = info.get_state(state);
	this->type = info.get_station_type(type);
}

Station::Station(const json_value &obj, ED_Info &info){
	SET_MEMBER(id, obj);
	SET_MEMBER(system_id, obj);
	SET_MEMBER(name, obj);
	std::string max_landing_pad_size;
	assign(max_landing_pad_size, obj, "max_landing_pad_size");
	if (max_landing_pad_size == "M")
		this->max_landing_pad_size = 0;
	else if (max_landing_pad_size == "L")
		this->max_landing_pad_size = 1;
	else
		throw std::exception("Invalid data.");
	SET_MEMBER(distance_to_star, obj);
	SET_MEMBER(faction, obj);
	std::string government, allegiance, state, type;
	SET_VARIABLE(government, obj);
	SET_VARIABLE(allegiance, obj);
	SET_VARIABLE(state, obj);
	SET_VARIABLE(type, obj);
	this->government = info.get_government(government);
	this->allegiance = info.get_allegiance(allegiance);
	this->state = info.get_state(state);
	this->type = info.get_station_type(type);
	SET_MEMBER(has_blackmarket, obj);
	SET_MEMBER(has_market, obj);
	SET_MEMBER(has_refuel, obj);
	SET_MEMBER(has_repair, obj);
	SET_MEMBER(has_rearm, obj);
	SET_MEMBER(has_outfitting, obj);
	SET_MEMBER(has_shipyard, obj);
	SET_MEMBER(has_commodities, obj);
	SET_MEMBER(updated_at, obj);
	SET_MEMBER(shipyard_updated_at, obj);
	{
		auto &import_commodities = obj["import_commodities"];
		if (import_commodities.IsArray()){
			auto n = import_commodities.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &val = import_commodities[i];
				if (val.IsString()){
					std::string name = val.GetString();
					this->import_commodities.push_back(info.get_commodity(name));
				}
			}
		}
	}
	{
		auto &export_commodities = obj["export_commodities"];
		if (export_commodities.IsArray()){
			auto n = export_commodities.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &val = export_commodities[i];
				if (val.IsString()){
					std::string name = val.GetString();
					this->export_commodities.push_back(info.get_commodity(name));
				}
			}
		}
	}
	{
		auto &prohibited_commodities = obj["prohibited_commodities"];
		if (prohibited_commodities.IsArray()){
			auto n = prohibited_commodities.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &val = prohibited_commodities[i];
				if (val.IsString()){
					std::string name = val.GetString();
					this->prohibited_commodities.push_back(info.get_commodity(name));
				}
			}
		}
	}
	{
		auto &economies = obj["economies"];
		if (economies.IsArray()){
			auto n = economies.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &val = economies[i];
				if (val.IsString()){
					std::string name = val.GetString();
					this->main_economies.push_back(info.get_economy(name));
				}
			}
		}
	}
	{
		auto &selling_ships = obj["selling_ships"];
		if (selling_ships.IsArray()){
			auto n = selling_ships.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &val = selling_ships[i];
				if (val.IsString()){
					std::string name = val.GetString();
					this->selling_ships.push_back(info.get_ship(name));
				}
			}
		}
	}
	{
		auto &selling_modules = obj["selling_modules"];
		if (selling_modules.IsArray()){
			auto n = selling_modules.Size();
			for (decltype(n) i = 0; i < n; i++){
				auto &mod = selling_modules[i];
				u64 id;
				if (mod.IsInt())
					id = selling_modules[i].GetInt();
				else if (mod.IsInt64())
					id = selling_modules[i].GetInt64();
				else
					continue;
				this->selling_modules.push_back(info.modules[id]);
			}
		}
	}
}

void Station::add_extra(Statement &stmt, const ED_Info &info){
	u64 foreign_id;
	int kind;
	stmt >> foreign_id >> kind;
	switch ((StationExtra)kind) {
		case StationExtra::Import:
			this->import_commodities.push_back(info.commodities[foreign_id]);
			break;
		case StationExtra::Export:
			this->export_commodities.push_back(info.commodities[foreign_id]);
			break;
		case StationExtra::Prohibited:
			this->prohibited_commodities.push_back(info.commodities[foreign_id]);
			break;
		case StationExtra::Economy:
			this->main_economies.push_back(info.economies[foreign_id]);
			break;
		case StationExtra::Ship:
			this->selling_ships.push_back(info.ships[foreign_id]);
			break;
		case StationExtra::Module:
			this->selling_modules.push_back(info.modules[foreign_id]);
			break;
	}
}

void Station::add_economy(Statement &stmt, const std::shared_ptr<Commodity> &commodity){
	this->economy.emplace_back(EconomicEntry(stmt, commodity));
	this->economy.back().station = this;
}

EconomicEntry::EconomicEntry(u64 listing[9], const std::shared_ptr<Commodity> &commodity){
	//id,station_id,commodity_id,supply,buy,sell,demand,collected_at,update_count
	this->commodity = commodity;
	int i = 3;
	this->supply = listing[i++];
	this->buy = listing[i++];
	this->sell = listing[i++];
	this->demand = listing[i++];
	this->collected_at = listing[i++];
	this->update_count = listing[i++];
}

void Station::add_economy(u64 listing[9], const std::shared_ptr<Commodity> &commodity){
	this->economy.emplace_back(EconomicEntry(listing, commodity));
	this->economy.back().station = this;
}

const EconomicEntry &Station::find_economic_entry(Commodity *commodity) const{
	for (auto &entry : this->economy)
		if (entry.commodity->id == commodity->id)
			return entry;
	throw std::exception("Cannot find commodity.");
}

unsigned Station::get_hasness_bitmap() const{
	unsigned ret = 0;
	//Order matters!
	const boost::optional<bool> *array[] = {
		&this->has_commodities,
		&this->has_shipyard,
		&this->has_outfitting,
		&this->has_rearm,
		&this->has_repair,
		&this->has_refuel,
		&this->has_market,
		&this->has_blackmarket,
	};

	for (auto p : array){
		ret <<= 2;
		if (!p->is_initialized())
			continue;
		ret |= ((unsigned)p->value() << 1) | 1;
	}
	return ret;
}

void Station::parse_hasness_bitmap(unsigned n){
	//Order matters!
	boost::optional<bool> *array[] = {
		&this->has_blackmarket,
		&this->has_market,
		&this->has_refuel,
		&this->has_repair,
		&this->has_rearm,
		&this->has_outfitting,
		&this->has_shipyard,
		&this->has_commodities,
	};
	for (auto p : array){
		bool is_value = (n & 1) != 0;
		bool value = (n & 2) != 0;
		n >>= 2;
		if (is_value)
			*p = value;
	}
}

void Station::save(
		Statement &new_station,
		Statement &new_extra,
		Statement &new_station_economy){
	new_station
		<< Reset()
		<< this->id
		<< this->system->id
		<< this->name
		<< this->max_landing_pad_size
		<< this->distance_to_star
		<< this->faction;
	BasicStringType *array[] = {
		this->government.get(),
		this->allegiance.get(),
		this->state.get(),
		this->type.get(),
	};
	for (auto p : array){
		if (p)
			new_station << p->id;
		else
			new_station << Null();
	}
	new_station
		<< this->get_hasness_bitmap()
		<< this->updated_at
		<< this->shipyard_updated_at
		<< Step();

	for (auto &commodity : this->import_commodities)
		new_extra
			<< Reset()
			<< this->id
			<< commodity->id
			<< (int)StationExtra::Import
			<< Step();
	for (auto &commodity : this->export_commodities)
		new_extra
			<< Reset()
			<< this->id
			<< commodity->id
			<< (int)StationExtra::Export
			<< Step();
	for (auto &commodity : this->prohibited_commodities)
		new_extra
			<< Reset()
			<< this->id
			<< commodity->id
			<< (int)StationExtra::Prohibited
			<< Step();
	for (auto &econ : this->main_economies)
		new_extra
			<< Reset()
			<< this->id
			<< econ->id
			<< (int)StationExtra::Economy
			<< Step();
	for (auto &ship : this->selling_ships)
		new_extra
			<< Reset()
			<< this->id
			<< ship->id
			<< (int)StationExtra::Ship
			<< Step();
	for (auto &module : this->selling_modules)
		new_extra
			<< Reset()
			<< this->id
			<< module->id
			<< (int)StationExtra::Module
			<< Step();

	for (auto &econ : this->economy){
		new_station_economy
			<< Reset()
			<< this->id
			<< econ.commodity->id
			<< econ.supply
			<< econ.demand
			<< econ.buy
			<< econ.sell
			<< econ.collected_at
			<< econ.update_count
			<< Step();
	}
}

std::shared_ptr<Station> Station::create_virtual_station(StarSystem *system){
	std::shared_ptr<Station> ret(new Station);
	ret->id = std::numeric_limits<decltype(ret->id)>::max();
	ret->system_id = system->id;
	ret->max_landing_pad_size = std::numeric_limits<decltype(ret->max_landing_pad_size)>::max();
	ret->distance_to_star = 0;
	ret->updated_at = 0;
	ret->system = system;
	return ret;
}
