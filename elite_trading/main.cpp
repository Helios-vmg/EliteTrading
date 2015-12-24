#include "ed_info.h"
#include <sstream>
#include <boost/filesystem.hpp>

#define EXPORT extern "C" __declspec(dllexport)

EXPORT void *initialize_info(ED_Info::progress_callback_f progress_callback){
	return new ED_Info(progress_callback);
}

EXPORT void destroy_info(void *p){
	auto info = (ED_Info *)p;
	delete info;
}

EXPORT i32 database_exists(){
	return boost::filesystem::exists(database_path);
}

EXPORT void *import_data(ED_Info::progress_callback_f progress_callback){
	auto ret = new ED_Info(ImportDataCommand(), progress_callback);
	ret->save_to_db();
	return ret;
}

EXPORT void recompute_all_routes(void *p, double max_stop_distance, u64 min_profit_per_unit){
	auto info = (ED_Info *)p;
	info->recompute_all_routes(max_stop_distance, min_profit_per_unit);
}

EXPORT std::int64_t *get_suggestions(i32 *ret_size, void *p, const char *input){
	auto info = (ED_Info *)p;
	auto options = info->location_search(input);
	auto ret = new std::int64_t[options.size()];
	*ret_size = (i32)options.size();
	for (size_t i = 0; i < options.size(); i++)
		ret[i] = (options[i].is_station ? -1 : 1) * ((std::int64_t)options[i].id + 1);
	return ret;
}

EXPORT void destroy_suggestions(void *p, i32 n){
	auto array = (std::int64_t *)p;
	delete[] array;
}

char *return_string(const std::string &s){
	auto n = s.size();
	auto ret = new char[n + 1];
	memcpy(ret, s.c_str(), n);
	ret[n] = 0;
	return ret;
}

EXPORT char *get_name(void *p, i32 is_station, u64 id){
	auto info = (ED_Info *)p;
	std::string *s;
	if (is_station){
		if (info->stations.size() <= id)
			return nullptr;
		s = &info->stations[id]->name;
	}else{
		if (info->systems.size() <= id)
			return nullptr;
		s = &info->systems[id]->name;
	}
	return return_string(*s);
}

EXPORT void destroy_string(void *p, void *s){
	delete (char *)s;
}

EXPORT u64 get_system_for_station(void *p, u64 id){
	auto info = (ED_Info *)p;
	if (info->stations.size() <= id)
		return std::numeric_limits<u64>::max();
	return info->stations[id]->system->id;
}

#define BIT(x) (1 << (x))

const u32 current_location_is_station_flag = BIT(0);
const u32 avoid_loops_flag = BIT(1);
const u32 require_large_pad_flag = BIT(2);
const u32 avoid_permit_systems_flag = BIT(3);

bool check_flag(u32 var, u32 flag){
	return (var & flag) == flag;
}

EXPORT void *search_nearby_routes(void *p, i32 *result_size, u64 current_location, i32 location_is_station, const RouteSearchConstraints *constraints){
	if (!constraints->required_stops)
		return nullptr;
	auto info = (ED_Info *)p;
	std::vector<RouteNodeInterop *> routes;
	if (location_is_station){
		if (info->stations.size() <= current_location)
			return nullptr;
		auto location = info->stations[current_location];
		routes = info->find_routes(location, *constraints);
	}else{
		if (info->systems.size() <= current_location)
			return nullptr;
		auto location = info->systems[current_location];
		routes = info->find_routes(location, *constraints);
	}
	auto ret = new RouteNodeInterop *[routes.size()];
	if (routes.size())
		memcpy(ret, &routes[0], routes.size() * sizeof(routes[0]));
	*result_size = (i32)routes.size();
	return ret;
}

EXPORT void destroy_routes(void *p, void *r, i32 size){
	auto routes = (RouteNodeInterop **)r;
	while (size--)
		delete routes[size];
	delete[] routes;
}

EXPORT char *get_commodity_name(void *p, u64 commodity_id){
	auto info = (ED_Info *)p;
	if (info->commodities.size() <= commodity_id)
		return nullptr;
	return return_string(info->commodities[commodity_id]->name);
}

struct SystemPoint{
	u64 system_id;
	double x, y, z;
};

EXPORT void *get_system_point_list(void *p, i32 *size){
	auto info = (ED_Info *)p;
	size_t n = info->systems.size();
	auto ret = new SystemPoint[n];
	for (size_t i = n; i--;){
		auto &point = ret[i];
		auto &system = *info->systems[i];
		point.system_id = system.id;
		point.x = system.x;
		point.y = system.y;
		point.z = system.z;
	}
	*size = (i32)n;
	return ret;
}

EXPORT void destroy_system_point_list(void *p, void *list, i32 size){
	delete[] (SystemPoint *)list;
}
