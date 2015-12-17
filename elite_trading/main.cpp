#include "ed_info.h"
#include <sstream>
#include <boost/filesystem.hpp>

#define EXPORT extern "C" __declspec(dllexport)

EXPORT void *initialize_info(){
	return new ED_Info;
}

EXPORT void destroy_info(void *p){
	auto info = (ED_Info *)p;
	delete info;
}

EXPORT int database_exists(){
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

#if 0
EXPORT char **get_suggestions(int *ret_size, void *p, const char *input){
	auto info = (ED_Info *)p;
	auto options = info->location_search(input);
	std::vector<std::string> temp;
	for (auto &opt : options){
		std::stringstream stream;
		if (opt.is_station)
			stream << "Station \"" << opt.name << "\" in system \"" << info->stations[opt.id]->system->name << "\"";
		else
			stream << "System  \"" << opt.name << "\"";
		temp.push_back(stream.str());
	}
	char **ret = new char *[temp.size()];
	size_t i = 0;
	for (auto &s : temp){
		ret[i] = new char[s.size() + 1];
		memcpy(ret[i], s.c_str(), s.size());
		ret[i][s.size()] = 0;
		i++;
	}
	*ret_size = (int)i;
	return ret;
}

EXPORT void destroy_suggestions(void *p, int n){
	char **array = (char **)p;
	for (int i = 0; i < n; i++)
		delete[] array[i];
	delete[] array;
}
#endif

#if 0
int main(){
	std::shared_ptr<ED_Info> info;
	if (boost::filesystem::exists(database_path))
		info.reset(new ED_Info);
	int input;
	std::cout << "\n\n\n";
	while (true){
		do{
			input = 0;
			std::cout <<
				"\n"
				"1. Import data.\n"
				"2. Recompute all routes.\n";
			if (info){
				std::cout << "3. Set max stop distance (";
				if (info->get_max_stop_distance() <= 0)
					std::cout << "infinity";
				else
					std::cout << info->get_max_stop_distance();
				std::cout << " ly).\n"
					"4. Set max hop distance (";
				if (info->get_max_hop_distance() <= 0)
					std::cout << "infinity";
				else
					std::cout << info->get_max_hop_distance();
				std::cout << " ly).\n";
			}
			std::cout <<
				"5. Find a route.\n"
				"0. Exit.\n"
				"\n";
			if (!read_line(input, std::cin))
				return 0;
		}while (input < 0 || input > USER_INPUT_OPTION_FIND_ROUTE);
		double dinput = -1;
		switch (input) {
			case 0:
				return 0;
			case USER_INPUT_OPTION_IMPORT_DATA:
				info.reset();
				info.reset(new ED_Info(ImportDataCommand()));
				info->save_to_db();
				break;
			case USER_INPUT_OPTION_RECOMPUTE_ROUTES:
				if (!info)
					info.reset(new ED_Info);
				info->recompute_all_routes();
				break;
			case USER_INPUT_OPTION_SET_MAX_STOP_DISTANCE:
				dinput = -1;
				if (!read_line(dinput, std::cin))
					return 0;
				if (dinput >= 0){
					if (!info)
						info.reset(new ED_Info);
					info->set_max_stop_distance(dinput);
				}
				break;
			case USER_INPUT_OPTION_SET_MAX_HOP_DISTANCE:
				dinput = -1;
				if (!read_line(dinput, std::cin))
					return 0;
				if (dinput >= 0){
					if (!info)
						info.reset(new ED_Info);
					info->set_max_hop_distance(dinput);
				}
				break;
			case USER_INPUT_OPTION_FIND_ROUTE:
				if (!info)
					info.reset(new ED_Info);
				route_search(*info);
				std::cin.clear();
				break;
		}
	}
	return 0;
}
#endif
