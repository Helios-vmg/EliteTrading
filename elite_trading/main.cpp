#include "ed_info.h"
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>

template <typename T>
bool read_line(T &dst, std::istream &stream) {
	std::string line;
	std::getline(stream, line);
	if (!stream)
		return false;
	std::stringstream(line) >> dst;
	return true;
}

void route_search(ED_Info &info){
	while (true){
		std::cout << "Enter your location:\n";
		std::string sinput;
		std::getline(std::cin, sinput);
		auto options = info.location_search(sinput);
		int index = 0;
		std::cout << std::endl;
		for (auto &opt : options){
			std::cout << std::setw(2) << std::setfill(' ') << ++index << ". ";
			if (opt.is_station)
				std::cout << "Station \"" << opt.name << "\" in system \"" << info.stations[opt.id]->system->name << "\"\n";
			else
				std::cout << "System  \"" << opt.name << "\"\n";
		}
		std::cout << " 0. None of the above.\n"
			"\n";
		int iinput;
		if (!read_line(iinput, std::cin))
			return;
		if (iinput <= 0 || iinput > index)
			continue;
		iinput--;
		std::cout << "Max capacity: ";
		unsigned max_capacity;
		read_line(max_capacity, std::cin);
		std::cout << "Initial funds: ";
		u64 funds;
		read_line(funds, std::cin);
		std::cout << "OK. Searching for routes around " << options[iinput].name << "...\n";
		if (options[iinput].is_station)
			info.find_routes(info.stations[options[iinput].id], max_capacity, funds);
		else
			info.find_routes(info.systems[options[iinput].id], max_capacity, funds);
	}
}

#define USER_INPUT_OPTION_IMPORT_DATA           1
#define USER_INPUT_OPTION_RECOMPUTE_ROUTES      2
#define USER_INPUT_OPTION_SET_MAX_STOP_DISTANCE 3
#define USER_INPUT_OPTION_SET_MAX_HOP_DISTANCE  4
#define USER_INPUT_OPTION_FIND_ROUTE            5
#define USER_INPUT_MAX                          USER_INPUT_OPTION_FIND_ROUTE

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
