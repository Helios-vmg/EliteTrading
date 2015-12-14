#include "route_segment.h"

bool RouteSegment::meets_constraints(std::set<u64> &visited){
	if (!this->memo_constraints.is_initialized()){
		if (this->station_src->max_landing_pad_size < 1)
			this->memo_constraints = false;
		else{
			if (visited.find(this->station_src->id) != visited.end())
				this->memo_constraints = false;
			else{
				if (!this->previous_segment)
					this->memo_constraints = true;
				else{
					visited.insert(this->station_src->id);
					this->memo_constraints = this->previous_segment->meets_constraints(visited);
				}
			}
		}
	}
	return this->memo_constraints.value();
}

double RouteSegment::calculate_fitness(){
	if (!this->memo_fitness.is_initialized()){
		if (this->station_dst->max_landing_pad_size < 1)
			this->memo_fitness = 0;
		else{
			std::set<u64> visited;
			visited.insert(this->station_dst->id);
			if (!this->meets_constraints(visited))
				this->memo_fitness = 0;
			else
				this->memo_fitness = this->calculate_efficiency();
		}
	}
	return this->memo_fitness.value();
}

double RouteSegment::ls_to_cost(double ls){
	return ls > 0 ? sqrt(ls) * 1.8973665961010275991993361266596 : 0;
}

double RouteSegment::calculate_cost(bool ignore_src, bool ignore_dst) const{
	double ret = this->approximate_distance / 7.5 * 30;
	double ls = 0;
	if (this->station_src->system->id == this->station_dst->system->id){
		if (!this->station_src->distance_to_star.is_initialized() && !this->station_dst->distance_to_star.is_initialized())
			ls = 2500;
		else{
			ls = this->station_src->distance_to_star.value_or(5000) - this->station_dst->distance_to_star.value_or(5000);
			if (ls < 0)
				ls = -ls;
		}
	}else{
		if (!ignore_src)
			ls += this->station_src->distance_to_star.value_or(5000);
		if (!ignore_dst)
			ls += this->station_dst->distance_to_star.value_or(5000);
	}
	ret += this->ls_to_cost(ls);
	return ret;
}

unsigned RouteSegment::max_quantity(){
	if (!this->memo_quantity.is_initialized()){
		auto sell_price = this->station_src->find_sell_price(this->commodity);
		auto ret = std::min(unsigned(this->available_funds / sell_price), this->max_capacity);
		this->memo_expenditure = ret * sell_price;
		this->memo_quantity = ret;
	}
	return this->memo_quantity.value();
}

void RouteSegment::calculate_funds(){
	assert(this->previous_segment);
	this->available_funds = this->previous_segment->available_funds + (u64)this->previous_segment->segment_profit();
}

u64 RouteSegment::segment_profit(){
	return this->max_quantity() * this->profit_per_unit;
}
