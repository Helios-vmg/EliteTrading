#pragma once
#include <vector>
#include <map>
#include <memory>
#include <cstdint>
#include <boost/optional.hpp>
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include <iomanip>

typedef rapidjson::GenericValue<rapidjson::UTF8<>> json_value;

template <typename T1, typename T2>
void insert_into(std::vector<T2> &dst, const T1 &idx, const T2 &src){
	if (idx >= dst.size())
		dst.resize(idx + 1);
	dst[idx] = src;
}

template <typename T1, typename T2>
void insert_into(std::map<T1, T2> &dst, const T1 &idx, const T2 &src){
	dst[idx] = src;
}

template <typename T>
bool contains(const std::vector<std::shared_ptr<T>> &v, size_t n){
	return v.size() > n && v[n];
}

template <typename T>
struct check_json_type{
};

template <>
struct check_json_type<std::uint64_t>{
	static bool check(const json_value &obj, const char *name){
		auto &v = obj[name];
		return v.IsInt64() || v.IsInt();
	}
	static std::uint64_t get(const json_value &obj, const char *name){
		auto &v = obj[name];
		if (v.IsInt64())
			return v.GetInt64();
		return v.GetInt() != 0;
	}
};

template <>
struct check_json_type<double>{
	static bool check(const json_value &obj, const char *name){
		auto &v = obj[name];
		return v.IsDouble() || v.IsInt() || v.IsInt64();
	}
	static double get(const json_value &obj, const char *name){
		auto &v = obj[name];
		if (v.IsDouble())
			return v.GetDouble();
		if (v.IsInt())
			return (double)v.GetInt();
		return (double)v.GetInt64();
	}
};

template <>
struct check_json_type<bool>{
	static bool check(const json_value &obj, const char *name){
		auto &v = obj[name];
		return v.IsBool() || v.IsInt();
	}
	static bool get(const json_value &obj, const char *name){
		auto &v = obj[name];
		if (v.IsBool())
			return v.GetBool();
		return v.GetInt() != 0;
	}
};

template <>
struct check_json_type<std::string>{
	static bool check(const json_value &obj, const char *name){
		auto &v = obj[name];
		return v.IsString() || v.IsNull();
	}
	static std::string get(const json_value &obj, const char *name){
		auto &v = obj[name];
		if (v.IsString())
			return v.GetString();
		return std::string();
	}
};

template <typename T>
void assign(T &dst, const json_value &obj, const char *name){
	typedef check_json_type<T> check_t;
	if (!check_t::check(obj, name))
		throw std::exception("Invalid data.");
	dst = check_t::get(obj, name);
}

template <typename T>
void assign(boost::optional<T> &dst, const json_value &obj, const char *name){
	typedef check_json_type<T> check_t;
	if (check_t::check(obj, name))
		dst = check_t::get(obj, name);
}

void assign(std::string &dst, const json_value &obj, const char *name);

#define SET_MEMBER(dst, src) assign(this->dst, src, #dst)
#define SET_VARIABLE(dst, src) assign(dst, src, #dst)

std::shared_ptr<rapidjson::Document> parse_json(const char *path);

template <typename T1, typename T2>
std::string generate_progress_string(const char *constant, const T1 &progress, const T2 &total){
	std::stringstream stream;
	stream << constant << std::setprecision(3) << double(progress) / total * 100 << '%';
	return stream.str();
}
