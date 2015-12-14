#include "commodity.h"
#include "util.h"

Commodity::Commodity(const json_value &obj){
	SET_MEMBER(id, obj);
	SET_MEMBER(name, obj);
	SET_MEMBER(average_price, obj);
	SET_MEMBER(category_id, obj);

	auto &obj2 = obj["category"];
	if (obj2.IsObject()){
		u64 category_id;
		std::string category_name;
		assign(category_id, obj2, "id");
		assign(category_name, obj2, "name");
		this->category.reset(new CommodityCategory{ category_id, category_name });
		if (this->category_id != this->category->id)
			throw std::exception("Invalid data.");
	}else if (this->category_id)
		throw std::exception("Invalid data.");
}

void Commodity::save(Statement &new_commodity){
	new_commodity << Reset() << this->id << this->name << this->category->id << this->average_price << Step();
}
