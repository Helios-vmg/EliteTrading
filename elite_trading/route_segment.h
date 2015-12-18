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
	RouteConstraints(
			unsigned max_capacity,
			u64 initial_funds,
			bool require_large_pad,
			bool avoid_loops){
		this->max_capacity = max_capacity;
		this->initial_funds = initial_funds;
		this->require_large_pad = require_large_pad;
		this->avoid_loops = avoid_loops;
	}
};

struct RouteNodeInterop;

inline double zero_div(double a, double b){
	if (!b)
		return 0;
	return a / b;
}

class RouteNode{
	boost::optional<double> memo_efficiency,
		memo_cost,
		memo_efficiency_fitness;
	boost::optional<u64> memo_profit, memo_expenditure, memo_profit_fitness;
	boost::optional<unsigned> memo_quantity;
	boost::optional<bool> memo_constraints;
	double get_cost(bool ignore_src, bool ignore_dst) const;
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
	std::shared_ptr<RouteNode> previous_segment;
	const RouteConstraints *constraints;
	u64 available_funds;

	u64 get_profit(){
		if (!this->memo_profit.is_initialized())
			this->memo_profit = (this->previous_segment ? this->previous_segment->get_profit() : 0) + this->get_segment_profit();
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
			this->memo_cost = (this->previous_segment ? this->previous_segment->get_cost() : 0) + this->get_cost(false, true);
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
	RouteNodeInterop *to_interop();
};

struct RouteNodeInterop{
	RouteNodeInterop *previous;
	u64 station_id;
	u64 system_id;
	i64 commodity_id;
	u64 quantity;
	u64 profit_per_unit;
	double efficiency;
	u64 accumulated_profit;
	u64 expenditure;
	double cost;
	~RouteNodeInterop() {
		if (this->previous)
			delete this->previous;
	}
};
