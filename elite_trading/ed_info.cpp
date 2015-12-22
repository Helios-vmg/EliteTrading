#include "ed_info.h"
#include "util.h"
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>
#include <ctime>
#include <chrono>

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

void ED_Info::thread_func(thread_func_params params){
	std::vector<std::shared_ptr<SingleStopTradingRoute>> result;
	size_t n = 0;
	for (auto &kv : *params.src_locations){
		auto it2 = params.dst_locations->find(kv.first);
		if (it2 == params.dst_locations->end())
			continue;
		n += kv.second.size();
	}
	for (auto &kv : *params.src_locations){
		auto it2 = params.dst_locations->find(kv.first);
		if (it2 == params.dst_locations->end())
			continue;
		auto m = kv.second.size();
		for (size_t i = params.thread_id; i < m; i += params.thread_count){
			auto progress = params._this->progress++;
			if (progress % 1000 == 0){
				std::lock_guard<std::mutex> lg(params._this->cout_mutex);
				params._this->progress_callback(generate_progress_string("Searching all possible routes... ", progress, n).c_str());
			}
			auto &src_location = kv.second[i];
			for (auto &dst_location : it2->second){
				assert(src_location->commodity->id == dst_location->commodity->id);
				if (dst_location->price <= src_location->price || dst_location->price - src_location->price < params.min_profit_per_unit)
					continue;
				std::shared_ptr<SingleStopTradingRoute> route(new SingleStopTradingRoute(src_location.get(), dst_location.get()));
				if (params.max_stop_distance >= 0 && route->approximate_distance > params.max_stop_distance)
					continue;
				result.push_back(route);
				if (result.size() >= 10000000){
					{
						std::lock_guard<std::mutex> lg(*params.db_mutex);
						Transaction t(*params.db);
						for (auto &i : result)
							i->save(*params.route_stmt, *params.db);
					}
					result.clear();
				}
			}
		}
	}
	if (result.size()){
		{
			std::lock_guard<std::mutex> lg(*params.db_mutex);
			Transaction t(*params.db);
			for (auto &i : result)
				i->save(*params.route_stmt, *params.db);
		}
		result.clear();
	}
}

ED_Info::ED_Info(progress_callback_f callback): progress_callback(callback){
	{
		DB db(database_path);
		this->progress_callback("Reading strings...");
		this->read_strings(db);
		this->progress_callback("Reading systems...");
		this->read_systems(db);
		this->progress_callback("Reading navigation routes...");
		this->read_navigation_routes(db);
		this->progress_callback("Reading module groups...");
		this->read_module_groups(db);
		this->progress_callback("Reading modules...");
		this->read_modules(db);
		this->progress_callback("Reading ships...");
		this->read_ships(db);
		this->progress_callback("Reading economies...");
		this->read_economies(db);
		this->progress_callback("Reading stations...");
		this->read_stations(db);
		this->progress_callback("Reading commodities...");
		this->read_commodities(db);
		this->progress_callback("Reading station extras...");
		this->read_station_extras(db);
		this->progress_callback("Reading station economies...");
		this->read_station_economies(db);
	}
	this->progress_callback("Processing averages...");
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
	size_t total = n - 1;
	for (size_t i = 0; i < n - 1; i++){
		auto &first = this->systems[i];
		assert(first);
		this->progress_callback(generate_progress_string("Generating routing table... ", i, total).c_str());
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

ED_Info::ED_Info(const ImportDataCommand &, progress_callback_f callback):
		progress_callback(callback){
	this->progress_callback("Reading systems.json...");
	this->read_systems_json();
	this->progress_callback("Reading modules.json...");
	this->read_modules_json();
	this->progress_callback("Reading commodities.json...");
	this->read_commodities_json();
	this->progress_callback("Reading stations.json...");
	this->read_stations_json();
	this->progress_callback("Reading listings.csv...");
	this->read_economy_csv();
	this->progress_callback("Compacting IDs...");
	this->compact_ids();
	this->progress_callback("Generating routing table...");
	this->generate_routing_table();
}

void ED_Info::recompute_all_routes(double max_stop_distance, u64 min_profit_per_unit){
	std::map<u64, std::vector<std::shared_ptr<TradingLocation>>> src_locations,
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
				src_locations[loc->commodity->id].push_back(loc);
			}
			if (entry.sell && entry.sell > average){
				if (!loc)
					loc.reset(new TradingLocation{ station.get(), entry.commodity.get(), entry.sell });
				dst_locations[loc->commodity->id].push_back(loc);
			}
		}
	}

	this->progress_callback("Searching all possible routes...");
	unsigned thread_count = std::thread::hardware_concurrency();
	std::vector<std::shared_ptr<std::thread>> threads(thread_count);
	size_t thread_id = 0;
	this->progress = 0;
	DB db(database_path);
	db.exec("delete from single_stop_routes;");
	auto insert_route = db << "insert into single_stop_routes (station_src, system_src, station_dst, commodity_id, approximate_distance, profit_per_unit) values (?, ?, ?, ?, ?, ?);";
	std::mutex db_mutex;
	for (auto &t : threads){
		thread_func_params params = {
			this,
			(unsigned)thread_id,
			thread_count,
			&src_locations,
			&dst_locations,
			&insert_route,
			&db,
			&db_mutex,
			max_stop_distance,
			min_profit_per_unit
		};
		t.reset(new std::thread(thread_func, params));
		thread_id++;
	}
	for (auto &t : threads)
		t->join();
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
	return ret;
}

std::vector<StarSystem *> ED_Info::find_route_candidate_systems(const StarSystem *around_system) {
	std::vector<StarSystem *> ret;
	for (auto &system : this->systems){
		if (!system)
			continue;
		//TODO: Replace 70 with a read from somewhere.
		if (around_system->distance(system.get()) <= 70)
			ret.push_back(system.get());
	}
	return ret;
}

bool nodes_by_profit(const std::shared_ptr<RouteNode> &a, const std::shared_ptr<RouteNode> &b){
	return a->get_profit_fitness() > b->get_profit_fitness();
}

bool nodes_by_efficiency(const std::shared_ptr<RouteNode> &a, const std::shared_ptr<RouteNode> &b){
	return a->get_efficiency_fitness() > b->get_efficiency_fitness();
}

double get_node_profit(const std::shared_ptr<RouteNode> &a){
	return -(double)a->get_profit_fitness();
}

double get_node_efficiency(const std::shared_ptr<RouteNode> &a){
	return -a->get_efficiency_fitness();
}

typedef bool (*route_node_sort_f)(const std::shared_ptr<RouteNode> &, const std::shared_ptr<RouteNode> &);
typedef double (*route_node_criterium_f)(const std::shared_ptr<RouteNode> &);

void insert_into_multimap(
		std::multimap<double, std::shared_ptr<RouteNode>> &routes,
		double max_in_map,
		size_t max_routes_per_loop,
		const std::shared_ptr<RouteNode> &item,
		route_node_criterium_f criterium){
	auto value = criterium(item);
	if (!routes.size()){
		max_in_map = value;
		routes.insert(std::make_pair(max_in_map, item));
	}else if (routes.size() < max_routes_per_loop){
		routes.insert(std::make_pair(value, item));
		auto it = routes.end();
		--it;
		max_in_map = it->first;
	}else if (value < max_in_map){
		routes.insert(std::make_pair(value, item));
		auto it = routes.end();
		--it;
		auto it2 = it;
		--it2;
		routes.erase(it);
		max_in_map = it2->first;
	}
}

//#define MEASURE_TIMES

std::vector<RouteNodeInterop *> ED_Info::find_routes(
		Station *around_station,
		unsigned max_capacity,
		u64 initial_funds,
		unsigned required_stops,
		OptimizationType optimization,
		u64 minimum_profit_per_unit,
		bool require_large_pad,
		bool avoid_loops,
		double laden_jump_distance,
		int max_price_age_days){
	std::multimap<double, std::shared_ptr<RouteNode>> routes;
	double max_in_map = 0;
	RouteConstraints constraints(max_capacity, initial_funds, require_large_pad, avoid_loops, laden_jump_distance);
	std::shared_ptr<RouteNode> first_node(new RouteNode(around_station, constraints));
	auto around_system = around_station->system;
	route_node_sort_f sort;
	route_node_criterium_f criterium;
	switch (optimization) {
		case OptimizationType::OptimizeEfficiency:
			sort = nodes_by_efficiency;
			criterium = get_node_efficiency;
			break;
		case OptimizationType::OptimizeProfit:
			sort = nodes_by_profit;
			criterium = get_node_profit;
			break;
		default:
			throw std::exception("Invalid enum value.");
			break;
	}
	const size_t max_routes_per_loop = 10000;
	auto now_timestamp = time(nullptr);
	auto max_price_age_seconds = max_price_age_days * 86400;
	DB db(database_path);
	{
		auto candidate_systems = this->find_route_candidate_systems(around_system);

		auto routes_from_system = db << "select station_src, station_dst, commodity_id, approximate_distance, profit_per_unit from single_stop_routes where system_src = ? and profit_per_unit >= ?;";
		for (auto system : candidate_systems){
			routes_from_system << Reset() << system->id << minimum_profit_per_unit;
			while (routes_from_system.step() == SQLITE_ROW){
				u64 src, dst, commodity_id, profit_per_unit;
				double approximate_distance;
				routes_from_system >> src >> dst >> commodity_id >> approximate_distance >> profit_per_unit;
				auto station = this->stations[src];
				auto collected_at = station->find_economic_entry(this->commodities[commodity_id].get()).collected_at;
				if (max_price_age_days >= 0 && collected_at <= now_timestamp && (now_timestamp - collected_at) >= max_price_age_seconds)
					continue;
				std::shared_ptr<RouteNode> first(new RouteNode(station.get(), constraints));
				first->previous_node = first_node;
				std::shared_ptr<RouteNode> second(new RouteNode(
					station.get(),
					this->commodities[commodity_id].get(),
					approximate_distance,
					profit_per_unit,
					constraints
				));
				second->previous_node = first;
				insert_into_multimap(routes, max_in_map, max_routes_per_loop, second, criterium);
			}
		}
	}

	auto routes_from_station = db << "select station_dst, commodity_id, approximate_distance, profit_per_unit from single_stop_routes where station_src = ? and profit_per_unit >= ?;";
	unsigned loop = required_stops;
#ifdef MEASURE_TIMES
	double times[5] = {0};
	auto t00 = std::chrono::high_resolution_clock::now();
#endif

	while (true){
		this->progress_callback(generate_progress_string("Searching for routes... ", required_stops - loop, required_stops).c_str());
#ifdef MEASURE_TIMES
		auto t10 = std::chrono::high_resolution_clock::now();
#endif
		if (!loop){
#ifdef MEASURE_TIMES
			auto t11 = std::chrono::high_resolution_clock::now();
			times[1] += std::chrono::duration_cast<std::chrono::microseconds>(t11 - t10).count() * 1e-6;
#endif
			break;
		}
		--loop;
#ifdef MEASURE_TIMES
		auto t11 = std::chrono::high_resolution_clock::now();
		times[1] += std::chrono::duration_cast<std::chrono::microseconds>(t11 - t10).count() * 1e-6;
#endif

#ifdef MEASURE_TIMES
		auto t20 = std::chrono::high_resolution_clock::now();
#endif
		std::map<u64, std::vector<std::shared_ptr<RouteNode>>> routes_by_station;
		for (auto &route : routes)
			routes_by_station[route.second->station->id].clear();
		
		for (auto &station_route_pair : routes_by_station){
			auto station = this->stations[station_route_pair.first];
			routes_from_station << Reset() << station_route_pair.first << minimum_profit_per_unit;
			while (routes_from_station.step() == SQLITE_ROW){
				u64 dst, commodity_id, profit_per_unit;
				double approximate_distance;
				routes_from_station >> dst >> commodity_id >> approximate_distance >> profit_per_unit;
				auto collected_at = station->find_economic_entry(this->commodities[commodity_id].get()).collected_at;
				if (max_price_age_days >= 0 && collected_at <= now_timestamp && (now_timestamp - collected_at) >= max_price_age_seconds)
					continue;
				std::shared_ptr<RouteNode> segment(new RouteNode(
					this->stations[dst].get(),
					this->commodities[commodity_id].get(),
					approximate_distance,
					profit_per_unit,
					constraints
				));
				station_route_pair.second.push_back(segment);
			}
		}
#ifdef MEASURE_TIMES
		auto t21 = std::chrono::high_resolution_clock::now();
		times[2] += std::chrono::duration_cast<std::chrono::microseconds>(t21 - t20).count() * 1e-6;

		auto t30 = std::chrono::high_resolution_clock::now();
#endif
		std::multimap<double, std::shared_ptr<RouteNode>> new_routes;
		double new_max_in_map = 0;
		for (auto &route : routes){
			const auto &segments = routes_by_station[route.second->station->id];
			for (auto &segment : segments){
				std::shared_ptr<RouteNode> new_segment(new RouteNode(*segment));
				new_segment->previous_node = route.second;
				new_segment->get_funds();
				insert_into_multimap(new_routes, max_in_map, max_routes_per_loop, new_segment, criterium);
			}
		}
#ifdef MEASURE_TIMES
		auto t31 = std::chrono::high_resolution_clock::now();
		times[3] += std::chrono::duration_cast<std::chrono::microseconds>(t31 - t30).count() * 1e-6;
#endif
		if (!new_routes.size())
			break;
		routes = std::move(new_routes);
	}
#ifdef MEASURE_TIMES
	auto t01 = std::chrono::high_resolution_clock::now();
	times[0] += std::chrono::duration_cast<std::chrono::microseconds>(t01 - t00).count() * 1e-6;
	{
		std::ofstream file("times.log");
		file
			<< times[0] << std::endl
			<< times[1] << std::endl
			<< times[2] << std::endl
			<< times[3] << std::endl;
	}
#endif

	for (auto &route : routes)
		route.second->reset_cost();
	{
		auto n = routes.size();
		decltype(n) i = 0;
		for (auto &route : routes){
			this->progress_callback(generate_progress_string("Computing exact costs and sorting... ", i, n).c_str());
			i++;
			route.second->get_exact_cost();
		}

		std::multimap<double, std::shared_ptr<RouteNode>> new_routes;
		double new_max_in_map = 0;
		for (auto &route : routes)
			insert_into_multimap(new_routes, max_in_map, max_routes_per_loop, route.second, criterium);
		routes = std::move(new_routes);
	}

	std::vector<RouteNodeInterop *> ret;
	ret.reserve(routes.size());
	for (auto &route : routes)
		if (route.second->meets_constraints())
			ret.push_back(route.second->to_interop());
	return ret;
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
	this->progress_callback("Saving systems...");
	this->save_systems(db);
	this->progress_callback("Saving stations...");
	this->save_stations(db);
	this->progress_callback("Saving commodities...");
	this->save_commodities(db);
	this->progress_callback("Saving strings...");
	for (auto &p : string_tables)
		this->save_string_table(db, *p.first, p.second);
	this->progress_callback("Saving modules...");
	this->save_modules(db);
	this->progress_callback("Saving module groups...");
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
	auto total = this->systems.size();
	decltype(total) i = 0;
	for (auto &system : this->systems){
		this->progress_callback(generate_progress_string("Saving systems... ", i++, total).c_str());
		system->save(new_system, new_navigation_route);
	}
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
