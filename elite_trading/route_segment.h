#pragma once

#include "basic_string_type.h"
#include "station.h"
#include <boost/optional.hpp>
#include <set>

struct RouteConstraints{
	unsigned max_capacity;
	u64 initial_funds;
	bool require_large_pad;
	bool avoid_loops;
	double laden_jump_distance;

	RouteConstraints(
			unsigned max_capacity,
			u64 initial_funds,
			bool require_large_pad,
			bool avoid_loops,
			double laden_jump_distance){
		this->max_capacity = max_capacity;
		this->initial_funds = initial_funds;
		this->require_large_pad = require_large_pad;
		this->avoid_loops = avoid_loops;
		this->laden_jump_distance = laden_jump_distance;
	}
};

struct RouteNodeInterop;

inline double zero_div(double a, double b){
	if (!b)
		return 0;
	return a / b;
}

template <typename T>
class Maybe{
	bool initialized;
	T data;
public:
	Maybe(): initialized(false), data(){}
	Maybe(const Maybe<T> &b){
		*this = b;
	}
	Maybe(const T &b){
		*this = b;
	}
	const Maybe<T> &operator=(const Maybe<T> &b){
		this->initialized = b.initialized;
		this->data = b.data;
		return *this;
	}
	const Maybe<T> &operator=(const T &b){
		this->initialized = true;
		this->data = b;
		return *this;
	}
	const T &value() const{
		if (!this->initialized)
			throw std::exception("!Maybe<T>::is_initialized()");
		return this->data;
	}
	const T &value_or(const T &v) const{
		if (!this->initialized)
			return v;
		return this->data;
	}
	bool is_initialized() const{
		return this->initialized;
	}
};

//#define OPTIONAL Maybe
#define OPTIONAL boost::optional

class RouteNode{
	OPTIONAL<double> memo_efficiency,
		memo_cost,
		memo_efficiency_fitness;
	OPTIONAL<u64> memo_profit, memo_expenditure, memo_profit_fitness;
	OPTIONAL<unsigned> memo_quantity;
	OPTIONAL<bool> memo_constraints;
	double get_segment_cost() const;
	double get_exact_segment_cost();
	bool meets_constraints(std::set<u64> &);
public:
	RouteNode(Station *dst, const RouteConstraints &constraints){
		this->station = dst;
		this->commodity = nullptr;
		this->approximate_distance = 0;
		this->profit_per_unit = 0;
		this->constraints = &constraints;
		this->available_funds = constraints.initial_funds;
	}
	RouteNode(
			Station *dst,
			Commodity *c,
			double approx_dist,
			u64 profit,
			const RouteConstraints &constraints){
		this->station = dst;
		this->commodity = c;
		this->approximate_distance = approx_dist;
		this->profit_per_unit = profit;
		this->constraints = &constraints;
	}

	Station *station;
	Commodity *commodity;
	double approximate_distance;
	u64 profit_per_unit;
	std::shared_ptr<RouteNode> previous_node;
	const RouteConstraints *constraints;
	u64 available_funds;
	unsigned hops;
	double true_distance;

	u64 get_profit(){
		if (!this->memo_profit.is_initialized())
			this->memo_profit = (this->previous_node ? this->previous_node->get_profit() : 0) + this->get_segment_profit();
		return this->memo_profit.value();
	}
	double get_efficiency_fitness();
	u64 get_profit_fitness();
	double get_efficiency(){
		if (!this->memo_efficiency.is_initialized())
			this->memo_efficiency = zero_div(this->get_profit(), this->get_cost());
		return this->memo_efficiency.value();
	}
	static double ls_to_cost(double);
	double get_cost(){
		if (!this->memo_cost.is_initialized())
			this->memo_cost = (this->previous_node ? this->previous_node->get_cost() : 0) + this->get_segment_cost();
		return this->memo_cost.value();
	}
	void get_funds();
	unsigned get_max_quantity();
	u64 get_segment_profit();
	u64 get_segment_expenditure(){
		if (!this->memo_expenditure.is_initialized())
			this->get_max_quantity();
		return this->memo_expenditure.value();
	}
	void reset_cost(){
		for (auto current = this; current; current = current->previous_node.get())
			current->memo_cost.reset();
	}
	double get_exact_cost();
	RouteNodeInterop *to_interop();
	bool meets_constraints(){
		return this->memo_constraints.value();
	}
};

struct RouteNodeInterop{
	RouteNodeInterop *previous;
	u64 station_id;
	double distance_to_star;
	u64 system_id;
	i64 commodity_id;
	u64 quantity;
	u64 profit_per_unit;
	double efficiency;
	u64 accumulated_profit;
	u64 expenditure;
	double cost;
	double distance;
	u32 hops;
	~RouteNodeInterop() {
		if (this->previous)
			delete this->previous;
	}
};
