#include "route_segment.h"

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
