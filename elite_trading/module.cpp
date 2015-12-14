#include "module.h"
#include "util.h"
#include "ed_info.h"

ModuleGroup::ModuleGroup(Statement &stmt, ED_Info &info){
	boost::optional<u64> category_id;
	stmt >> this->id >> category_id >> this->name;
	this->category = info.get_module_category(category_id);
}

ModuleGroup::ModuleGroup(const json_value &obj, ED_Info &info){
	SET_MEMBER(id, obj);
	std::string category;
	SET_MEMBER(name, obj);
	SET_VARIABLE(category, obj);
	this->category = info.get_module_category(category);
}

void ModuleGroup::save(Statement &new_module_group){
	new_module_group
		<< Reset()
		<< this->id
		<< this->name;
	if (this->category)
		new_module_group << this->category->id;
	else
		new_module_group << Null();
	new_module_group << Step();
}

Module::Module(Statement &stmt){
	stmt
		>> this->id
		>> this->group_id
		>> this->klass
		>> this->rating
		>> this->price
		>> this->weapon_mode
		>> this->missile_type
		>> this->name
		>> this->belongs_to
		>> this->ship;
}

Module::Module(const json_value &obj, ED_Info &info){
	SET_MEMBER(id, obj);
	SET_MEMBER(group_id, obj);
	assign(this->klass, obj, "class");
	SET_MEMBER(rating, obj);
	SET_MEMBER(price, obj);
	SET_MEMBER(weapon_mode, obj);
	SET_MEMBER(missile_type, obj);
	SET_MEMBER(name, obj);
	SET_MEMBER(belongs_to, obj);
	SET_MEMBER(ship, obj);

	auto &obj2 = obj["group"];
	if (obj2.IsObject()){
		this->group.reset(new ModuleGroup(obj2, info));
		if (this->group_id != this->group->id)
			throw std::exception("Invalid data.");
	}else if (this->group_id)
		throw std::exception("Invalid data.");
}

void Module::save(Statement &new_module){
	new_module << Reset() << this->id;
	if (this->group)
		new_module << this->group->id;
	else
		new_module << Null();
	new_module
		<< this->rating
		<< this->price
		<< this->weapon_mode
		<< this->missile_type
		<< this->name
		<< this->belongs_to
		<< this->ship
		<< Step();
}
