#include "util.h"
#include <fstream>

std::shared_ptr<rapidjson::Document> parse_json(const char *path){
	std::vector<char> buffer;
	std::ifstream file(path, std::ios::binary);
	if (!file)
		return nullptr;
	file.seekg(0, std::ios::end);
	buffer.resize(file.tellg());
	file.seekg(0);
	file.read(&buffer[0], buffer.size());
	std::shared_ptr<rapidjson::Document> ret(new rapidjson::Document);
	buffer.push_back(0);
	ret->Parse(&buffer[0]);
	return ret;
}

void assign(std::string &dst, const json_value &obj, const char *name){
	typedef check_json_type<std::string> check_t;
	if (check_t::check(obj, name))
		dst = check_t::get(obj, name);
}
