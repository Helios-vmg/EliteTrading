#include "star_system.h"
#include "ed_info.h"
#include "util.h"
#include <unordered_set>
#include <queue>

StarSystem::StarSystem(Statement &stmt, ED_Info &info){
	boost::optional<u64> government, allegiance, state, security, primary_economy, power, power_state;
	stmt
		>> this->id
		>> this->population
		>> this->x >> this->y >> this->z
		>> this->name
		>> this->faction
		>> government
		>> allegiance
		>> state
		>> security
		>> primary_economy
		>> power
		>> power_state
		>> this->needs_permit
		>> this->updated_at;
	this->government = info.get_government(government);
	this->allegiance = info.get_allegiance(allegiance);
	this->state = info.get_state(state);
	this->security = info.get_security(security);
	this->primary_economy = info.get_economy(primary_economy);
	this->power = info.get_power(power);
	this->power_state = info.get_power_state(power_state);
}

StarSystem::StarSystem(const json_value &obj, ED_Info &info){
	SET_MEMBER(id, obj);
	SET_MEMBER(population, obj);
	SET_MEMBER(x, obj);
	SET_MEMBER(y, obj);
	SET_MEMBER(z, obj);
	SET_MEMBER(name, obj);
	SET_MEMBER(faction, obj);
	std::string government, allegiance, state, security, primary_economy, power, power_state;
	SET_VARIABLE(government, obj);
	SET_VARIABLE(allegiance, obj);
	SET_VARIABLE(state, obj);
	SET_VARIABLE(security, obj);
	SET_VARIABLE(primary_economy, obj);
	SET_VARIABLE(power, obj);
	SET_VARIABLE(power_state, obj);
	this->government = info.get_government(government);
	this->allegiance = info.get_allegiance(allegiance);
	this->state = info.get_state(state);
	this->security = info.get_security(security);
	this->primary_economy = info.get_economy(primary_economy);
	this->power = info.get_power(power);
	this->power_state = info.get_power_state(power_state);
	SET_MEMBER(needs_permit, obj);
	SET_MEMBER(updated_at, obj);
}

void StarSystem::add_station(const std::shared_ptr<Station> &station){
	this->stations.push_back(station);
	station->system = this;
}

std::shared_ptr<std::vector<StarSystem *>> rebuild_list(const std::vector<std::pair<size_t, StarSystem *>> &came_from){
	auto ret = std::make_shared<std::vector<StarSystem *>>();
	size_t i = came_from.size() - 1;
	do{
		ret->push_back(came_from[i].second);
		i = came_from[i].first;
	}while (i);
	std::reverse(ret->begin(), ret->end());
	return ret;
}

std::shared_ptr<std::vector<StarSystem *>> StarSystem::find_fastest_route(StarSystem *dst, double max_distance){
	if (dst->id == this->id)
		return std::make_shared<std::vector<StarSystem *>>();
	std::deque<std::pair<size_t, StarSystem *>> queue;
	std::unordered_set<uintptr_t> visited;
	std::vector<std::pair<size_t, StarSystem *>> came_from;
	came_from.push_back({0, this});
	queue.push_back({0, this});
	visited.insert((uintptr_t)this);

	while (queue.size()){
		auto top = queue.front();
		queue.pop_front();
		auto n = top.second->nearby_systems.size();
		for (size_t i = 0; i < n; i++){
			auto distance = top.second->nearby_systems[i].second;
			if (max_distance >= 0 && distance > max_distance)
				continue;
			auto nearby_system = top.second->nearby_systems[i].first;
			if (visited.find((uintptr_t)nearby_system) != visited.end())
				continue;
			came_from.push_back({top.first, nearby_system});
			if (nearby_system->id == dst->id)
				return rebuild_list(came_from);
			queue.push_back({came_from.size() - 1, nearby_system});
			visited.insert((uintptr_t)nearby_system);
		}
	}
	return std::shared_ptr<std::vector<StarSystem *>>();
}

typedef std::tuple<double, size_t, StarSystem *> Astar_tuple_t;
struct tuple_compare{
	bool operator()(const std::tuple<double, size_t, StarSystem *> &a, const std::tuple<double, size_t, StarSystem *> &b) const{
		return std::get<0>(a) < std::get<0>(b);
	}
};

std::shared_ptr<std::vector<StarSystem *>> StarSystem::find_fastest_route_Astar(StarSystem *dst, double max_distance){
	if (dst->id == this->id)
		return std::make_shared<std::vector<StarSystem*>>();
	std::priority_queue<Astar_tuple_t, std::vector<Astar_tuple_t>, tuple_compare> queue;
	std::unordered_set<uintptr_t> visited;
	std::vector<std::pair<size_t, StarSystem *>> came_from;
	came_from.push_back({0, this});
	queue.push(Astar_tuple_t{this->distance(dst), 0, this});
	visited.insert((uintptr_t)this);

	while (queue.size()){
		auto top = queue.top();
		queue.pop();
		auto top_distance = std::get<0>(top);
		auto top_came_from = std::get<1>(top);
		auto top_system = std::get<2>(top);
		auto n = top_system->nearby_systems.size();
		for (size_t i = 0; i < n; i++){
			auto distance = top_system->nearby_systems[i].second;
			if (max_distance >= 0 && distance > max_distance)
				continue;
			auto nearby_system = top_system->nearby_systems[i].first;
			if (visited.find((uintptr_t)nearby_system) != visited.end())
				continue;
			came_from.push_back({top_came_from, nearby_system});
			if (nearby_system->id == dst->id)
				return rebuild_list(came_from);
			queue.push(Astar_tuple_t{this->distance(nearby_system), came_from.size() - 1, nearby_system});
			visited.insert((uintptr_t)nearby_system);
		}
	}
	return std::shared_ptr<std::vector<StarSystem *>>();
}

void StarSystem::save(Statement &new_system, Statement &new_navigation_route){
	new_system
		<< Reset()
		<< this->id
		<< this->population
		<< this->x
		<< this->y
		<< this->z
		<< this->name
		<< this->faction;
	BasicStringType *array[] = {
		this->government.get(),
		this->allegiance.get(),
		this->state.get(),
		this->security.get(),
		this->primary_economy.get(),
		this->power.get(),
		this->power_state.get(),
	};
	for (auto p : array){
		if (p)
			new_system << p->id;
		else
			new_system << Null();
	}
	new_system
		<< this->needs_permit
		<< this->updated_at
		<< Step();

	for (auto &route : this->nearby_systems)
		new_navigation_route
			<< Reset()
			<< this->id
			<< route.first->id
			<< route.second
			<< Step();
}
