#include "route_segment.h"

bool RouteNode::meets_constraints(std::set<u64> &visited){
	if (!this->memo_constraints.is_initialized()){
		if (this->constraints->require_large_pad && this->station->max_landing_pad_size < 1)
			this->memo_constraints = false;
		else if (this->constraints->avoid_loops && visited.find(this->station->id) != visited.end())
			this->memo_constraints = false;
		else if (!this->previous_segment)
			this->memo_constraints = true;
		else{
			visited.insert(this->station->id);
			this->memo_constraints = this->previous_segment->meets_constraints(visited);
		}
	}
	return this->memo_constraints.value();
}

double RouteNode::get_efficiency_fitness(){
	if (!this->memo_efficiency_fitness.is_initialized()){
		if (this->constraints->require_large_pad && this->station->max_landing_pad_size < 1)
			this->memo_efficiency_fitness = 0;
		else{
			std::set<u64> visited;
			if (!this->meets_constraints(visited))
				this->memo_efficiency_fitness = 0;
			else
				this->memo_efficiency_fitness = this->get_efficiency();
		}
	}
	return this->memo_efficiency_fitness.value();
}

u64 RouteNode::get_profit_fitness() {
	if (!this->memo_profit_fitness.is_initialized()){
		if (this->constraints->require_large_pad && this->station->max_landing_pad_size < 1)
			this->memo_profit_fitness = 0;
		else{
			std::set<u64> visited;
			visited.insert(this->station->id);
			if (!this->meets_constraints(visited))
				this->memo_profit_fitness = 0;
			else
				this->memo_profit_fitness = this->get_profit();
		}
	}
	return this->memo_profit_fitness.value();
}

double RouteNode::ls_to_cost(double ls){
	return ls > 0 ? sqrt(ls) * 1.8973665961010275991993361266596 : 0;
}

double RouteNode::get_cost(bool ignore_src, bool ignore_dst) const{
	if (!this->previous_segment)
		return 0;
	double ret = this->approximate_distance / 7.5 * 30;
	double ls = 0;
	auto station_src = previous_segment->station;
	if (station_src->system->id == this->station->system->id){
		if (!station_src->distance_to_star.is_initialized() && !this->station->distance_to_star.is_initialized())
			ls = 2500;
		else{
			ls = station_src->distance_to_star.value_or(5000) - this->station->distance_to_star.value_or(5000);
			if (ls < 0)
				ls = -ls;
		}
	}else{
		if (!ignore_src)
			ls += station_src->distance_to_star.value_or(5000);
		if (!ignore_dst)
			ls += this->station->distance_to_star.value_or(5000);
	}
	ret += this->ls_to_cost(ls);
	return ret;
}

unsigned RouteNode::get_max_quantity(){
	if (!this->previous_segment)
		throw std::exception("Incorrect implementation");
	if (!this->memo_quantity.is_initialized()){
		auto sell_price = this->previous_segment->station->find_sell_price(this->commodity);
		auto ret = std::min(unsigned(this-> available_funds / sell_price), this->constraints->max_capacity);
		this->memo_expenditure = ret * sell_price;
		this->memo_quantity = ret;
	}
	return this->memo_quantity.value();
}

void RouteNode::get_funds(){
	assert(this->previous_segment);
	this->available_funds = this->previous_segment->available_funds + (u64)this->previous_segment->get_segment_profit();
}

u64 RouteNode::get_segment_profit(){
	if (!this->previous_segment || !this->commodity)
		return 0;
	return this->get_max_quantity() * this->profit_per_unit;
}

RouteNodeInterop *RouteNode::to_interop(){
	auto ret = new RouteNodeInterop;
	memset(ret, 0, sizeof(*ret));
	ret->station_id = this->station->id;
	ret->system_id = this->station->system->id;
	if (this->previous_segment){
		ret->previous = this->previous_segment->to_interop();
		if (this->commodity){
			ret->commodity_id = this->commodity->id;
			ret->quantity = this->get_max_quantity();
			ret->expenditure = this->get_segment_expenditure();
		}else
			ret->commodity_id = std::numeric_limits<decltype(ret->commodity_id)>::max();
		ret->profit_per_unit = this->profit_per_unit;
		ret->efficiency = this->get_efficiency();
		ret->accumulated_profit = this->get_profit();
		ret->cost = this->get_cost();
	}
	return ret;
}
