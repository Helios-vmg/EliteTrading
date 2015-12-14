#include "ed_info.h"
#include "util.h"
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

const char * const database_path = "data.sqlite";
const char * const database_journal_path = "data.sqlite-journal";

class TradingLocation {
public:
	Station *station;
	Commodity *commodity;
	u64 price;
};

class SingleStopTradingRoute{
	bool route_computed;
	std::vector<StarSystem *> route;
	double total_distance,
		max_single_hop_distance,
		config_max_hop_distance;
public:
	TradingLocation *src, *dst;
	int hops;
	double approximate_distance;
	u64 db_id;

	SingleStopTradingRoute(TradingLocation *src, TradingLocation *dst, double config_max_hop_distance = -1): config_max_hop_distance(config_max_hop_distance){
		this->src = src;
		this->dst = dst;
		this->route_computed = false;
		this->approximate_distance = dst->station->system->distance(src->station->system);
	}
	void compute_route(){
		if (this->route_computed)
			return;
		this->route_computed = true;
		this->route = src->station->find_fastest_route(dst->station, this->config_max_hop_distance);
		if (!this->route.size()){
			this->hops = -1;
			this->total_distance = -1;
			this->max_single_hop_distance = -1;
		}else{
			this->hops = int(this->route.size() - 1);
			this->total_distance = 0;
			this->max_single_hop_distance = 0;
			for (size_t i = 1; i < this->route.size(); i++){
				auto current = this->route[i];
				auto last = this->route[i - 1];
				auto d = current->distance(last);
				this->total_distance += d;
				this->max_single_hop_distance = std::max(this->max_single_hop_distance, d);
			}
		}
	}
	const std::vector<StarSystem *> &get_route(){
		this->compute_route();
		return this->route;
	}
	double get_total_distance(){
		this->compute_route();
		return this->total_distance;
	}

	void save(Statement &route_stmt, DB &db){
		route_stmt
			<< Reset()
			<< this->src->station->id
			<< this->src->station->system->id
			<< this->dst->station->id
			<< this->src->commodity->id
			<< this->approximate_distance
			//<< this->get_total_distance()
			//<< this->get_route().size() - 1
			<< (this->dst->price - this->src->price)
			<< Step();
		this->db_id = sqlite3_last_insert_rowid(db);
	}
};

void ED_Info::read_config(DB &db){
	auto stmt = db << "select value from config where key = ?;";
	stmt << Reset() << "max_stop_distance";
	if (stmt.step() == SQLITE_ROW)
		stmt >> this->max_stop_distance;
	stmt << Reset() << "max_hop_distance";
	if (stmt.step() == SQLITE_ROW)
		stmt >> this->max_hop_distance;
}

void ED_Info::read_strings(DB &db){
	std::pair<std::vector<std::shared_ptr<BasicStringType>> *, const char *> string_tables[] = {
		{ &this->economies, "economies" },
		{ &this->ships, "ships" },
		{ &this->governments, "governments" },
		{ &this->allegiances, "allegiances" },
		{ &this->states, "states" },
		{ &this->security_types, "security_types" },
		{ &this->powers, "powers" },
		{ &this->power_states, "power_states" },
		{ &this->station_type, "station_types" },
		{ &this->module_categories, "module_categories" },
		{ &this->commodity_categories, "commodity_categories" },
	};
	const char * const create_string_table = "select id, name from %1%;";
	for (auto &p : string_tables){
		auto s = (boost::format(create_string_table) % p.second).str();
		auto stmt = db << s.c_str();
		while (stmt.step() == SQLITE_ROW){
			u64 id;
			std::string name;
			stmt >> id >> name;
			std::shared_ptr<BasicStringType> ptr(new BasicStringType{ id, name });
			insert_into(*p.first, id, ptr);
		}
	}
}

void ED_Info::read_systems(DB &db){
	auto stmt = db <<
		"select "
			"id,"
			"population,"
			"x, y, z,"
			"name,"
			"faction,"
			"government_id,"
			"allegiance_id, "
			"state_id, "
			"security_id, "
			"primary_economy_id, "
			"power_id, "
			"power_state_id, "
			"needs_permit, "
			"updated_at "
		"from systems;";
	while (stmt.step() == SQLITE_ROW){
		std::shared_ptr<StarSystem> system(new StarSystem(stmt, *this));
		insert_into(this->systems, system->id, system);
	}
}

void ED_Info::read_systems_json(){
	auto json = parse_json("data/systems.json");
	if (!json->IsArray())
		throw std::exception("Invalid data.");
	auto n = json->Size();
	for (decltype(n) i = 0; i < n; i++){
		std::shared_ptr<StarSystem> system(new StarSystem((*json)[i], *this));
		insert_into(this->systems, system->id, system);
	}
}

void ED_Info::read_modules_json(){
	auto json = parse_json("data/modules.json");
	if (!json->IsArray())
		throw std::exception("Invalid data.");
	auto n = json->Size();
	for (decltype(n) i = 0; i < n; i++){
		std::shared_ptr<Module> module(new Module((*json)[i], *this));
		insert_into(this->modules, module->id, module);
		if (module->group){
			if (!contains(this->module_groups, module->group->id))
				insert_into(this->module_groups, module->group->id, module->group);
			else
				module->group = this->module_groups[module->group->id];
		}
	}
}

void ED_Info::read_stations_json(){
	auto json = parse_json("data/stations.json");
	if (!json->IsArray())
		throw std::exception("Invalid data.");
	auto n = json->Size();
	for (decltype(n) i = 0; i < n; i++){
		std::shared_ptr<Station> station(new Station((*json)[i], *this));
		insert_into(this->stations, station->id, station);
		auto &system = this->systems[station->system_id];
		station->system = system.get();
		system->stations.push_back(station);
	}
}

void ED_Info::read_navigation_routes(DB &db){
	auto stmt = db << "select src, dst, distance from navigation_routes where distance <= 20 order by distance asc;";
	while (stmt.step() == SQLITE_ROW){
		u64 src, dst;
		double d;
		stmt >> src >> dst >> d;
		this->systems[src]->add_route(this->systems[dst], d);
	}
}

void ED_Info::read_stations(DB &db){
	auto stmt = db <<
		"select "
			"id, "
			"systems_id, "
			"name, "
			"max_landing_pad_size, "
			"distance_to_star, "
			"faction, "
			"government_id, "
			"allegiance_id, "
			"state_id, "
			"type_id, "
			"hasness_bitmap, "
			"updated_at, "
			"shipyard_updated_at "
		"from stations;";
	while (stmt.step() == SQLITE_ROW){
		std::shared_ptr<Station> station(new Station(stmt, *this));
		insert_into(this->stations, station->id, station);
		this->systems[station->system_id]->add_station(station);
	}
}

void ED_Info::read_module_groups(DB &db){
	auto stmt = db << "select id, category_id, name from module_groups;";
	while (stmt.step() == SQLITE_ROW){
		std::shared_ptr<ModuleGroup> mg(new ModuleGroup(stmt, *this));
		insert_into(this->module_groups, mg->id, mg);
	}
}

void ED_Info::read_modules(DB &db){
	auto stmt = db << "select id, group_id, class, rating, price, weapon_mode, missile_type, name, belongs_to, ship from modules;";
	while (stmt.step() == SQLITE_ROW){
		std::shared_ptr<Module> module(new Module(stmt));
		module->group = this->module_groups[module->group_id];
		insert_into(this->modules, module->id, module);
	}
}

void ED_Info::read_ships(DB &db){
	auto stmt = db << "select id, name from ships;";
	while (stmt.step() == SQLITE_ROW){
		u64 id;
		std::string name;
		stmt >> id >> name;
		std::shared_ptr<Ship> ship(new Ship{id, name});
		insert_into(this->ships, id, ship);
	}
}

void ED_Info::read_economies(DB &db){
	auto stmt = db << "select id, name from economies;";
	while (stmt.step() == SQLITE_ROW){
		u64 id;
		std::string name;
		stmt >> id >> name;
		std::shared_ptr<Economy> ship(new Economy{id, name});
		insert_into(this->economies, id, ship);
	}
}
	
void ED_Info::read_station_extras(DB &db){
	auto stmt = db << "select stations_id, foreign_id, kind from stations_extra;";
	while (stmt.step() == SQLITE_ROW){
		u64 station_id;
		stmt >> station_id;
		auto &station = this->stations[station_id];
		station->add_extra(stmt, *this);
	}
}

void ED_Info::read_commodities(DB &db){
	auto stmt = db << "select id, category_id, name, average_price from commodities;";
	while (stmt.step() == SQLITE_ROW){
		std::shared_ptr<Commodity> commodity(new Commodity(stmt));
		insert_into(this->commodities, commodity->id, commodity);
		commodity->category = this->commodity_categories[commodity->category_id];
	}
}

void ED_Info::read_commodities_json(){
	auto json = parse_json("data/commodities.json");
	if (!json->IsArray())
		throw std::exception("Invalid data.");
	auto n = json->Size();
	for (decltype(n) i = 0; i < n; i++){
		std::shared_ptr<Commodity> commodity(new Commodity((*json)[i]));
		insert_into(this->commodities, commodity->id, commodity);
		if (commodity->category){
			if (!contains(this->commodity_categories, commodity->category->id))
				insert_into(this->commodity_categories, commodity->category->id, commodity->category);
			else
				commodity->category = this->commodity_categories[commodity->category->id];
		}
	}
}

void ED_Info::read_station_economies(DB &db){
	auto stmt = db << "select stations_id, commodity_id, supply, demand, buy, sell, collected_at, update_count from station_economies;";
	while (stmt.step() == SQLITE_ROW){
		u64 station_id, commodity_id;
		stmt >> station_id >> commodity_id;
		this->stations[station_id]->add_economy(stmt, this->commodities[commodity_id]);
	}
}

void ED_Info::fill_averages(){
	std::map<u64, std::pair<u64, u64>> map;
	for (auto &station : this->stations){
		for (auto &entry : station->economy){
			auto avg = entry.buy + entry.sell;
			avg /= 2;
			auto id = entry.commodity->id;
			auto it = map.find(id);
			if (it == map.end())
				map[id] = std::make_pair(avg, 1);
			else{
				it->second.first += avg;
				it->second.second++;
			}
		}
	}
	for (auto &commodity : this->commodities){
		if (!commodity)
			continue;
		if (commodity->average_price.is_initialized())
			continue;
		auto p = map[commodity->id];
		if (p.second)
			commodity->average_price = p.first / p.second;
	}
}

const int primes[] = {  2,  3,  5,  7,
					   11, 13, 17, 19,
					   23, 29, 31, 37,
					   41, 43, 47, 53 };

void ED_Info::thread_func(
		ED_Info *_this,
		unsigned thread_id,
		unsigned thread_count,
		std::vector<std::shared_ptr<TradingLocation>> *src_locations,
		std::vector<std::shared_ptr<TradingLocation>> *dst_locations,
		Statement *route_stmt,
		DB *db,
		std::mutex *db_mutex
){
	std::vector<std::shared_ptr<SingleStopTradingRoute>> result;
	auto n = src_locations->size();
	for (size_t i = thread_id; i < n; i += thread_count){
		auto progress = _this->progress++;
		if (progress % 1000 == 0){
			std::lock_guard<std::mutex> lg(_this->cout_mutex);
			std::cout << '\r' << progress << '/' << n << std::flush;
		}
		auto &src_location = (*src_locations)[i];
		for (auto &dst_location : *dst_locations){
			if (src_location->commodity->id != dst_location->commodity->id || dst_location->price <= src_location->price || dst_location->price - src_location->price < 1000)
				continue;
			std::shared_ptr<SingleStopTradingRoute> route(new SingleStopTradingRoute(src_location.get(), dst_location.get(), _this->max_hop_distance));
			if (_this->max_stop_distance >= 0 && route->approximate_distance > _this->max_stop_distance)
				continue;
			result.push_back(route);
			if (result.size() >= primes[thread_id] * 1000000){
				{
					std::lock_guard<std::mutex> lg(*db_mutex);
					Transaction t(*db);
					for (auto &i : result)
						i->save(*route_stmt, *db);
				}
				result.clear();
			}
		}
	}
	if (result.size()){
		{
			std::lock_guard<std::mutex> lg(*db_mutex);
			Transaction t(*db);
			for (auto &i : result)
				i->save(*route_stmt, *db);
		}
		result.clear();
	}
}

ED_Info::ED_Info(): max_stop_distance(-1), max_hop_distance(-1){
	{
		DB db(database_path);
		std::cout << "Reading config...\n";
		this->read_config(db);
		std::cout << "Reading strings...\n";
		this->read_strings(db);
		std::cout << "Reading systems...\n";
		this->read_systems(db);
		std::cout << "Reading navigation routes...\n";
		this->read_navigation_routes(db);
		std::cout << "Reading module groups...\n";
		this->read_module_groups(db);
		std::cout << "Reading modules...\n";
		this->read_modules(db);
		std::cout << "Reading ships...\n";
		this->read_ships(db);
		std::cout << "Reading economies...\n";
		this->read_economies(db);
		std::cout << "Reading stations...\n";
		this->read_stations(db);
		std::cout << "Reading commodities...\n";
		this->read_commodities(db);
		std::cout << "Reading station extras...\n";
		this->read_station_extras(db);
		std::cout << "Reading station economies...\n";
		this->read_station_economies(db);
	}
	std::cout << "Processing averages...\n";
	this->fill_averages();
}

void ED_Info::read_economy_csv(){
	std::ifstream file("data/listings.csv");
	if (!file)
		return;
	std::string line;
	const char * const sanity = "id,station_id,commodity_id,supply,buy_price,sell_price,demand,collected_at,update_count";
	std::getline(file, line);
	if (!file || line != sanity)
		return;
	std::shared_ptr<Station> station;
	while (true){
		std::getline(file, line);
		if (!file)
			break;
		for (auto &c : line)
			if (c == ',')
				c = ' ';
		std::stringstream stream(line);
		u64 data[9];
		for (int i = 0; i < 9; i++)
			if (!(stream >> data[i]) || !stream && i < 8)
				throw std::exception("Invalid data.");

		auto station_id = data[1];
		auto commodity_id = data[2];
		if (!station || station->id != station_id)
			station = this->stations[station_id];
		station->add_economy(data, this->commodities[commodity_id]);
	}
}

template <typename T>
void compact_id_vector(std::vector<T> &v){
	std::vector<T> v2;
	for (auto &i : v)
		if (i)
			v2.push_back(i);
	v = std::move(v2);
	for (size_t i = 0; i < v.size(); i++)
		v[i]->id = i;
}

void ED_Info::compact_ids(){
	compact_id_vector(this->systems);
	compact_id_vector(this->module_groups);
	compact_id_vector(this->modules);
	compact_id_vector(this->stations);
	compact_id_vector(this->commodity_categories);
	compact_id_vector(this->commodities);
}

void ED_Info::generate_routing_table(){
	const double max_hop_distance = 42;
	size_t n = this->systems.size();
	for (size_t i = 0; i < n - 1; i++){
		auto &first = this->systems[i];
		assert(first);
		for (size_t j = i + 1; j < n - 1; j++){
			auto &second = this->systems[j];
			assert(second);
			double d = first->distance(second);
			if (d <= max_hop_distance){
				first->add_route(second, d);
				second->add_route(first, d);
			}
		}
	}
}

ED_Info::ED_Info(const ImportDataCommand &): max_stop_distance(-1), max_hop_distance(-1){
	this->read_systems_json();
	this->read_modules_json();
	this->read_commodities_json();
	this->read_stations_json();
	this->read_economy_csv();
	this->compact_ids();
	this->generate_routing_table();
}

void ED_Info::recompute_all_routes(){
	std::vector<std::shared_ptr<TradingLocation>> src_locations,
		dst_locations;
	for (auto &station : this->stations){
		if (!station)
			continue;
		for (auto &entry : station->economy){
			assert(entry.commodity->average_price.is_initialized());
			std::shared_ptr<TradingLocation> loc;
			auto average = entry.commodity->average_price.value();
			if (entry.buy && entry.buy < average){
				loc.reset(new TradingLocation{ station.get(), entry.commodity.get(), entry.buy });
				src_locations.push_back(loc);
			}
			if (entry.sell && entry.sell > average){
				if (!loc)
					loc.reset(new TradingLocation{ station.get(), entry.commodity.get(), entry.sell });
				dst_locations.push_back(loc);
			}
		}
	}

	std::cout << "Searching all possible routes...\n";
	unsigned thread_count = std::thread::hardware_concurrency();
	std::vector<std::shared_ptr<std::thread>> threads(thread_count);
	size_t thread_id = 0;
	this->progress = 0;
	DB db(database_path);
	db.exec("delete from single_stop_routes;");
	auto insert_route = db << "insert into single_stop_routes (station_src, system_src, station_dst, commodity_id, approximate_distance, profit_per_unit) values (?, ?, ?, ?, ?, ?);";
	std::mutex db_mutex;
	for (auto &t : threads){
		t.reset(new std::thread(thread_func, this, thread_id, thread_count, &src_locations, &dst_locations, &insert_route, &db, &db_mutex));
		thread_id++;
	}
	for (auto &t : threads)
		t->join();
}

void ED_Info::set_max_hop_distance(double d){
	DB db(database_path);
	this->max_hop_distance = std::min(d, this->max_stop_distance);
	int count;
	db << "select count(*) from config where key = 'max_hop_distance';" << Step() >> count;
	if (!count)
		db << "insert into config (key, value) values ('max_hop_distance', ?);" << this->max_hop_distance << Step();
	else
		db << "update config set value = ? where key = 'max_hop_distance';" << this->max_hop_distance << Step();
}

void ED_Info::set_max_stop_distance(double d){
	DB db(database_path);
	db.exec("delete from single_stop_routes;");
	this->max_stop_distance = d;
	int count;
	db << "select count(*) from config where key = 'max_stop_distance';" << Step() >> count;
	if (!count)
		db << "insert into config (key, value) values ('max_stop_distance', ?);" << this->max_stop_distance << Step();
	else
		db << "update config set value = ? where key = 'max_stop_distance';" << this->max_stop_distance << Step();
}

size_t count_matches(const std::string &name, std::vector<std::pair<std::string, unsigned>> &search_terms){
	for (auto &term : search_terms){
		auto p = name.find(term.first);
		if (p != name.npos){
			if ((!p || name[p - 1] == ' ') && (p + term.first.size() >= name.size() || name[p + term.first.size()] == ' '))
				term.second = 2;
			else if (term.second < 2)
				term.second = 1;
		}
	}
	bool all = true;
	for (auto &term : search_terms)
		if (term.second != 2)
			all = false;
	if (all)
		return search_terms.size() * 4;
	all = true;
	for (auto &term : search_terms)
		if (term.second != 1)
			all = false;
	if (all)
		return search_terms.size() * 2;
	size_t score = 0;
	for (auto &term : search_terms)
		score += term.second;
	return score;
}

std::vector<LocationOption> ED_Info::location_search(const std::string &location){
	std::vector<LocationOption> ret;
	std::vector<std::pair<std::string, unsigned>> search_terms;
	{
		size_t first, second = 0;
		while (second != location.npos){
			first = location.find_first_not_of(" ", second);
			if (first == location.npos)
				break;
			second = location.find(' ', first);
			std::string temp;
			if (second == location.npos)
				temp = location.substr(first);
			else
				temp.assign(location.begin() + first, location.begin() + second);
			std::transform(temp.begin(), temp.end(), temp.begin(), tolower);
			search_terms.push_back({temp, 0});
		}
	}
	if (!search_terms.size())
		return ret;
	for (auto &system : this->systems){
		if (!system)
			continue;
		auto name = system->name;
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		auto score = count_matches(name, search_terms);
		if (score)
			ret.push_back(LocationOption{ false, system->id, system->name, score });
		for (auto &term : search_terms)
			term.second = 0;
	}
	for (auto &station : this->stations){
		if (!station)
			continue;
		auto name = station->name;
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		auto score = count_matches(name, search_terms);
		if (!score)
			continue;
		name = station->system->name;
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		score = count_matches(name, search_terms);
		if (score)
			ret.push_back(LocationOption{ true, station->id, station->name, score });
		for (auto &term : search_terms)
			term.second = 0;
	}
	std::sort(ret.begin(), ret.end(), [](const LocationOption &a, const LocationOption &b){ return a.similarity > b.similarity; });
	if (ret.size() > 10)
		ret.resize(10);
	return ret;
}

bool segments_by_profit(const std::shared_ptr<RouteSegment> &a, const std::shared_ptr<RouteSegment> &b){
	return a->calculate_profit() > b->calculate_profit();
}

bool segments_by_fitness(const std::shared_ptr<RouteSegment> &a, const std::shared_ptr<RouteSegment> &b){
	return a->calculate_fitness() > b->calculate_fitness();
}

void ED_Info::find_routes(StarSystem *around_system, unsigned max_capacity, u64 initial_funds){
	//const unsigned minimum_profit = 425; // trim 90%
	const unsigned minimum_profit = 1000; // trim 99.43%
	//const unsigned minimum_profit = 2000; // trim 99.77%

	std::vector<std::shared_ptr<RouteSegment>> routes;
	DB db(database_path);
	{
		std::vector<StarSystem *> systems;
		for (auto &system : this->systems){
			if (!system)
				continue;
			if (around_system->distance(system.get()) <= this->max_stop_distance)
				systems.push_back(system.get());
		}

		auto routes_from_system = db << "select id, station_src, station_dst, commodity_id, approximate_distance, profit_per_unit from single_stop_routes where system_src = ? and profit_per_unit >= ?;";
		for (auto system : systems){
			routes_from_system << Reset() << system->id << minimum_profit;
			while (routes_from_system.step() == SQLITE_ROW){
				u64 id, src, dst, commodity_id, profit_per_unit;
				double approximate_distance;
				routes_from_system >> id >> src >> dst >> commodity_id >> approximate_distance >> profit_per_unit;
				std::shared_ptr<RouteSegment> segment(new RouteSegment(
					id,
					stations[src].get(),
					stations[dst].get(),
					commodities[commodity_id].get(),
					approximate_distance,
					profit_per_unit,
					max_capacity,
					initial_funds
				));
				routes.push_back(segment);
			}
		}
	}

	auto routes_from_station = db << "select id, station_src, station_dst, commodity_id, approximate_distance, profit_per_unit from single_stop_routes where station_src = ? and profit_per_unit >= ?;";
	int loop = 6;
	const size_t max_routes_per_loop = 10000;
	while (true){
		std::cout << "Found " << routes.size() << " routes.\n";
		//if (routes.size() > 10000){
		//	std::sort(routes.begin(), routes.end(), segments_by_profit);
		//	routes.resize(routes.size() / 5);
		//}
		if (!loop || routes.size() > max_routes_per_loop){
			std::sort(routes.begin(), routes.end(), segments_by_fitness);
			if (routes.size() > max_routes_per_loop)
				routes.resize(max_routes_per_loop);
			if (!loop)
				break;
		}
		--loop;

		std::map<u64, std::vector<std::shared_ptr<RouteSegment>>> routes_by_station;
		for (auto &route : routes)
			routes_by_station[route->station_dst->id].clear();
		
		for (auto &station : routes_by_station){
			routes_from_station << Reset() << station.first << minimum_profit;
			while (routes_from_station.step() == SQLITE_ROW){
				u64 id, src, dst, commodity_id, profit_per_unit;
				double approximate_distance;
				routes_from_station >> id >> src >> dst >> commodity_id >> approximate_distance >> profit_per_unit;
				std::shared_ptr<RouteSegment> segment(new RouteSegment(
					id,
					stations[src].get(),
					stations[dst].get(),
					commodities[commodity_id].get(),
					approximate_distance,
					profit_per_unit,
					max_capacity
				));
				station.second.push_back(segment);
			}
		}
		std::vector<std::shared_ptr<RouteSegment>> new_routes;
		for (auto &route : routes){
			const auto &segments = routes_by_station[route->station_dst->id];
			for (auto &segment : segments){
				std::shared_ptr<RouteSegment> new_segment(new RouteSegment(*segment));
				new_segment->previous_segment = route;
				new_segment->calculate_funds();
				new_routes.push_back(new_segment);
			}
		}
		if (!new_routes.size())
			break;
		routes = std::move(new_routes);
	}

	if (routes.size() > 10)
		routes.resize(10);
	else if (!routes.size())
		return;

	std::vector<std::shared_ptr<RouteSegment>> segments;
	for (auto current = routes.front(); current; current = current->previous_segment)
		segments.push_back(current);
	std::reverse(segments.begin(), segments.end());

	{
		auto &segment = segments[0];
		std::cout
			<< "At station \"" << segment->station_src->name << "\" in system \"" << segment->station_src->system->name << "\", distance from star: " << segment->station_src->distance_to_star.value_or(5000) << "\n"
			   "    Buy " << segment->max_quantity() << "x " << segment->commodity->name << " (expenditure: " << segment->segment_expenditure() << ")\n";
	}

	for (size_t i = 1; i < segments.size(); i++){
		auto &segment = segments[i];
		std::cout
			<< "Travel " << segment->previous_segment->station_dst->system->distance(segment->previous_segment->station_src->system) << " ly\n"
			   "At station \"" << segment->station_src->name << "\" in system \"" << segment->station_src->system->name << "\", distance from star: " << segment->station_src->distance_to_star.value_or(5000) << "\n"
			   "    Sell all cargo. Profit: " << segment->segment_profit() << "\n"
			   "    Buy " << segment->max_quantity() << "x " << segment->commodity->name << " (expenditure: " << segment->segment_expenditure() << ")\n";
	}

	{
		auto &segment = segments.back();
		std::cout
			<< "Travel " << segment->previous_segment->station_dst->system->distance(segment->previous_segment->station_src->system) << " ly\n"
			   "At station \"" << segment->station_dst->name << "\" in system \"" << segment->station_dst->system->name << "\", distance from star: " << segment->station_src->distance_to_star.value_or(5000) << "\n"
			   "    Sell all cargo. Profit:       " << segment->segment_profit() << "\n"
			   "                    Total profit: " << segment->calculate_profit() << std::endl;
	}
}

void ED_Info::save_to_db(){
	if (boost::filesystem::exists(database_path))
		boost::filesystem::remove(database_path);
	if (boost::filesystem::exists(database_journal_path))
		boost::filesystem::remove(database_journal_path);
	DB db(database_path);
	const char * const db_string = 
		"create table systems(\n"
		"    id integer primary key,\n"
		"    population integer,\n"
		"    x real,\n"
		"    y real,\n"
		"    z real,\n"
		"    name text,\n"
		"    faction text,\n"
		"    government_id integer,\n"
		"    allegiance_id integer,\n"
		"    state_id integer,\n"
		"    security_id integer,\n"
		"    primary_economy_id integer,\n"
		"    power_id integer,\n"
		"    power_state_id integer,\n"
		"    needs_permit boolean,\n"
		"    updated_at integer\n"
		");\n"
		"create table navigation_routes(\n"
		"    src integer,\n"
		"    dst integer,\n"
		"    distance real\n"
		");\n"
		"create index navigation_routes_by_src on navigation_routes (src asc);\n"
		"create table stations(\n"
		"    id integer primary key,\n"
		"    systems_id integer,\n"
		"    name text,\n"
		"    max_landing_pad_size integer,\n"
		"    distance_to_star real,\n"
		"    faction text,\n"
		"    government_id integer,\n"
		"    allegiance_id integer,\n"
		"    state_id integer,\n"
		"    type_id integer,\n"
		"    hasness_bitmap integer,\n"
		"    updated_at integer,\n"
		"    shipyard_updated_at integer\n"
		");\n"
		"create index stations_by_systems_id on stations (systems_id asc);\n"
		"create table stations_extra(\n"
		"    stations_id integer,\n"
		"    foreign_id integer,\n"
		"    kind integer\n"
		");\n"
		"create index stations_extra_by_stations_id on stations_extra (stations_id asc);\n"
		"create table station_economies(\n"
		"    stations_id integer,\n"
		"    commodity_id integer,\n"
		"    supply integer,\n"
		"    demand integer,\n"
		"    buy integer,\n"
		"    sell integer,\n"
		"    collected_at integer,\n"
		"    update_count integer\n"
		");\n"
		"create index station_economies_by_stations_id on station_economies (stations_id asc);\n"
		"create table commodities(\n"
		"    id integer primary key,\n"
		"    name text,\n"
		"    category_id integer,\n"
		"    average_price integer\n"
		");\n"
		"create table modules(\n"
		"    id integer primary key,\n"
		"    group_id integer,\n"
		"    class integer,\n"
		"    rating text,\n"
		"    price integer,\n"
		"    weapon_mode text,\n"
		"    missile_type text,\n"
		"    name text,\n"
		"    belongs_to text,\n"
		"    ship text\n"
		");\n"
		"create table module_groups(\n"
		"    id integer primary key,\n"
		"    category_id integer,\n"
		"    name text\n"
		");\n"
		"create table single_stop_routes(\n"
		"    id integer primary key,\n"
		"    station_src integer,\n"
		"    system_src integer,\n"
		"    station_dst integer,\n"
		"    commodity_id integer,\n"
		"    approximate_distance real,\n"
		"    true_distance real,\n"
		"    route_hops integer,\n"
		"    profit_per_unit integer\n"
		");\n"
		"create index single_stop_routes_by_system_src on single_stop_routes (system_src asc);\n"
		"create index single_stop_routes_by_station_src on single_stop_routes (station_src asc);\n"
		"create table config (key text primary key, value text);\n";
	db.exec(db_string);
	const char * const create_string_table = "create table %1% (id integer primary key, name text);";
	std::pair<std::vector<std::shared_ptr<BasicStringType>> *, const char *> string_tables[] = {
		{ &this->economies, "economies" },
		{ &this->ships, "ships" },
		{ &this->governments, "governments" },
		{ &this->allegiances, "allegiances" },
		{ &this->states, "states" },
		{ &this->security_types, "security_types" },
		{ &this->powers, "powers" },
		{ &this->power_states, "power_states" },
		{ &this->station_type, "station_types" },
		{ &this->module_categories, "module_categories" },
		{ &this->commodity_categories, "commodity_categories" },
	};
	for (auto &p : string_tables)
		db.exec((boost::format(create_string_table) % p.second).str().c_str());
		
	Transaction t(db);
	std::cout << "Saving systems...\n";
	this->save_systems(db);
	std::cout << "Saving stations...\n";
	this->save_stations(db);
	std::cout << "Saving commodities...\n";
	this->save_commodities(db);
	std::cout << "Saving strings...\n";
	for (auto &p : string_tables)
		this->save_string_table(db, *p.first, p.second);
	std::cout << "Saving modules...\n";
	this->save_modules(db);
	std::cout << "Saving module groups...\n";
	this->save_module_groups(db);
}

void ED_Info::save_string_table(DB &db, const std::vector<std::shared_ptr<BasicStringType>> &vector, const char *table_name){
	auto stmt = (boost::format("insert into %1% (id, name) values (?, ?);") % table_name).str();
	auto new_category = db << stmt.c_str();
	for (auto &i : vector)
		new_category << Reset() << i->id << i->name << Step();
}

void ED_Info::save_systems(DB &db){
	auto new_system = db <<
		"insert into systems ("
			"id, "
			"population, "
			"x, "
			"y, "
			"z, "
			"name, "
			"faction, "
			"government_id, "
			"allegiance_id, "
			"state_id, "
			"security_id, "
			"primary_economy_id, "
			"power_id, "
			"power_state_id, "
			"needs_permit, "
			"updated_at"
		") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
	auto new_navigation_route = db << "insert into navigation_routes (src, dst, distance) values (?, ?, ?);";
	for (auto &system : this->systems)
		system->save(new_system, new_navigation_route);
}

void ED_Info::save_stations(DB &db){
	auto new_station = db <<
		"insert into stations ("
			"id, "
			"systems_id, "
			"name, "
			"max_landing_pad_size, "
			"distance_to_star, "
			"faction, "
			"government_id, "
			"allegiance_id, "
			"state_id, "
			"type_id, "
			"hasness_bitmap, "
			"updated_at, "
			"shipyard_updated_at"
		") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
	auto new_extra = db << "insert into stations_extra (stations_id, foreign_id, kind) values (?, ?, ?);";
	auto new_station_economy = db << "insert into station_economies (stations_id, commodity_id, supply, demand, buy, sell, collected_at, update_count) values (?, ?, ?, ?, ?, ?, ?, ?);";
	for (auto &station : this->stations)
		station->save(new_station, new_extra, new_station_economy);
}

void ED_Info::save_commodities(DB &db){
	auto new_commodity = db << "insert into commodities (id, name, category_id, average_price) values (?, ?, ?, ?);";
	for (auto &commodity : this->commodities)
		commodity->save(new_commodity);
}

void ED_Info::save_modules(DB &db){
	auto new_module = db <<
		"insert into modules ("
			"id, "
			"group_id, "
			"class, "
			"rating, "
			"price, "
			"weapon_mode, "
			"missile_type, "
			"name, "
			"belongs_to, "
			"ship"
		") values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
	for (auto &module : this->modules)
		module->save(new_module);
}

void ED_Info::save_module_groups(DB &db){
	auto new_module_group = db << "insert into module_groups (id, category_id, name) values (?, ?, ?);";
	for (auto &module_group : this->module_groups)
		module_group->save(new_module_group);
}
	
std::shared_ptr<Commodity> ED_Info::get_commodity(const std::string &name){
	if (!this->commodities_by_name.size() && this->commodities.size()){
		for (auto &commodity : this->commodities)
			if (commodity)
				this->commodities_by_name[commodity->name] = commodity;
	}
	auto it = this->commodities_by_name.find(name);
	if (it == this->commodities_by_name.end())
		throw std::exception("Invalid data.");
	return it->second;
}
