#pragma once
#include <random>

#include "classes.h"


Effect::Effect(
	Entity* origin,
	unsigned int id,
	unsigned int attr_id,
	unsigned int sign,
	std::vector<unsigned int> flags,
	unsigned int units,
	double exact_value,
	std::array<double, 2> value_range,
	unsigned int ref_attr_id,
	unsigned int ref_type) {
	this->origin = origin;
	this->id = id;
	this->attr_id = attr_id;
	this->sign = sign;
	this->flags = flags;
	this->units = units;
	this->exact_value = exact_value;
	this->value_range = value_range;
	this->ref_type = ref_type;
	this->ref_attr_id = ref_attr_id;
};

bool Effect::range_valid() {
	if (value_range[0] == NULL and value_range[1] == NULL) return false;
	if (value_range[0] >= value_range[1]) return false;
	return true;
}

double Effect::produce_from_range() {
	if (value_range[0] == NULL) value_range[0] = 0;
	if (value_range[1] == NULL) value_range[1] = INT_MAX;
	std::uniform_int_distribution<> distr(value_range[0], value_range[1]);
	static auto mt = std::mt19937(std::random_device()());
	return distr(mt);
}

double Effect::produce_value(Entity* target) {
	Entity* ref = ref_type == EFFECT_REF_TYPE_ORIGIN ?  origin : target;
	if (units == VALUE_POINTS) {
		if (exact_value != NULL) return exact_value;
		if (range_valid()) return produce_from_range();
		if (ref_attr_id != NULL) return ref->get_attr_by_id(ref_attr_id);
		throw std::logic_error("Empty Value instance (p)");
	}
	if (units == VALUE_PERCENT) {
		if (ref_attr_id == NULL) throw std::logic_error("Percent value with no reference");
		double ref_attr = ref->get_attr_by_id(ref_attr_id);
		if (exact_value != NULL) return ref_attr * exact_value;
		if (range_valid()) return ref_attr * produce_from_range();
		throw std::logic_error("Empty Value instance (%)");
	}
	throw std::logic_error("Unkown Value unit type");
}

void Effect::apply(Entity* target) {
}