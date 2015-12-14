#pragma once

#include "basic_string_type.h"
#include "station.h"
#include <boost/optional.hpp>
#include <set>

class RouteSegment{
	boost::optional<double> memo_efficiency,
		memo_cost,
		memo_fitness;
	boost::optional<u64> memo_profit, memo_expenditure;
	boost::optional<unsigned> memo_quantity;
	boost::optional<bool> memo_constraints;
	double calculate_cost(bool ignore_src, bool ignore_dst) const;
	bool meets_constraints(std::set<u64> &);
public:
	RouteSegment(u64 id, Station *src, Station *dst, Commodity *c, double approx_dist, u64 profit, unsigned max_capacity = std::numeric_limits<unsigned>::max(), u64 funds = std::numeric_limits<u64>::max()) {
		this->id = id;
		this->station_src = src;
		this->station_dst = dst;
		this->commodity = c;
		this->approximate_distance = approx_dist;
		this->profit_per_unit = profit;
		this->max_capacity = max_capacity;
		this->available_funds = funds;
	}

	u64 id;
	Station *station_src,
		*station_dst;
	Commodity *commodity;
	double approximate_distance;
	u64 profit_per_unit;
	std::shared_ptr<RouteSegment> previous_segment;
	unsigned max_capacity;
	u64 available_funds;

	u64 calculate_profit(){
		if (!this->memo_profit.is_initialized())
			this->memo_profit = (this->previous_segment ? this->previous_segment->calculate_profit() : 0) + this->segment_profit();
		return this->memo_profit.value();
	}
	double calculate_fitness();
	double calculate_efficiency(){
		if (!this->memo_efficiency.is_initialized())
			this->memo_efficiency = this->calculate_profit() / this->calculate_cost();
		return this->memo_efficiency.value();
	}
	static double ls_to_cost(double);
	double calculate_cost(){
		if (!this->memo_cost.is_initialized())
			this->memo_cost = (this->previous_segment ? this->previous_segment->calculate_cost() : 0) + this->calculate_cost(false, true);
		return this->memo_cost.value();
	}
	void calculate_funds();
	unsigned max_quantity();
	u64 segment_profit();
	u64 segment_expenditure(){
		if (!this->memo_expenditure.is_initialized())
			this->max_quantity();
		return this->memo_expenditure.value();
	}
};
