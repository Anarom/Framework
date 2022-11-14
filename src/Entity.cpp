#pragma once
#include "classes.h"

Entity::Entity(unsigned int id, Position pos, unsigned int faction) {
	this->id = id;
	this->pos = pos;
	this->faction = faction;	
}

double Entity::get_attr_by_id(unsigned int attr_id) {
	return 0;
}

void Entity::set_attr_by_id(unsigned int attr_id, double val) {
}
