#pragma once
#include <cmath>
#include <stdexcept>
#include <random>
#include <iostream>
#include <bitset>

#include "classes.h"

EntityFilterRule::EntityFilterRule(
	std::uint8_t facing_type,
	std::uint8_t faction_type,
	std::uint8_t origin_type,
	std::uint8_t area_form,
	bool include_src,
	bool include_target,
	std::array<double, 5> area_params,
	std::array<double, 2> dist_params,
	std::array<double, 2> rand_dist_params)
{
	flags = { facing_type, faction_type, origin_type, area_form, include_src, include_target };

	std::copy(area_params.begin(), area_params.end(), std::next(params.begin(), 0));
	std::copy(dist_params.begin(), dist_params.end(), std::next(params.begin(), 5));
	std::copy(rand_dist_params.begin(), rand_dist_params.end(), std::next(params.begin(), 7));
}


EntityFilterRule::operator std::string() const {
	return "This works";
}

template <typename T> void EntityFilterRule::add_data_to_signature(RuleSignature& signature, T data, unsigned short int& offset) {
	RuleSignature addition(data);
	offset += sizeof(T) * CHAR_BIT;
	addition <<= signature.size() - offset;
	signature |= addition;
}

void EntityFilterRule::add_params_to_signature(RuleSignature& signature, unsigned short int& offset) {
	for (double param : params) {
		add_data_to_signature<double>(signature, param, offset);
	}
}

void EntityFilterRule::add_flags_to_signature(RuleSignature& signature, unsigned short int& offset) {
	for (std::uint8_t flag : flags) {
		add_data_to_signature<std::uint8_t>(signature, flag, offset);
	}
}
RuleSignature EntityFilterRule::get_signature() {
	unsigned short int offset = 0;
	RuleSignature signature;
	add_flags_to_signature(signature, offset);
	add_params_to_signature(signature, offset);
	
	return signature;
}

template <typename T> T extract_attr(RuleSignature& signature, unsigned short int& offset) {
	std::bitset<sizeof(T)* CHAR_BIT> rule_bits;
	for (unsigned short int n = 0; n < sizeof(T) * CHAR_BIT; n++) {
		rule_bits[n] = signature[n + offset];
	}
	std::cout << rule_bits << "\n";
	std::cout << "Read from " << offset << 't'
	offset += rule_bits.size();
	T attr;
	memcpy(&attr, &rule_bits, rule_bits.size());
	std::cout << attr << "\n";
	return attr;
}

EntityFilterRule::EntityFilterRule(RuleSignature& signature) {
	unsigned short int offset = 0;
	std::cout << signature << "\n";
	for (unsigned short int n = 0; n < flags.size(); n++) {
		flags[n] = extract_attr<std::uint8_t>(signature, offset);
	}
	for (unsigned short int n = 0; n < params.size(); n++) {
		params[n] = extract_attr<double>(signature, offset);
	}
}



bool EntityFilterRule::apply(Entity* src, Entity* target, Entity* provided_entity, Position* provided_position) {
	if (target == NULL) target = src;
	if (src == provided_entity and not flags[4]) return false;
	if (target == provided_entity and not flags[5]) return false;
	return
		check_faction(src, provided_entity) and
		check_distance(src, provided_entity) and
		check_facing(src, provided_entity) and
		check_area(src, target, provided_entity, provided_position);
}

double EntityFilterRule::distance(Entity* e1, Entity* e2) {
	return std::sqrt(std::pow(e2->pos.x - e1->pos.x, 2) + std::pow(e2->pos.y - e1->pos.y, 2));
}

double EntityFilterRule::distance(std::array<double, 2> v1, std::array<double, 2> v2) {
	return  std::sqrt(std::pow(v2[0] - v1[0], 2) + std::pow(v2[1] - v1[1], 2));
}

bool EntityFilterRule::check_faction(Entity* src, Entity* target) {
	if (flags[1] == FACTION_ANY) return true;
	if (src == NULL) throw std::logic_error("NULL src Entity and not TARGET_TYPE_ANY");
	if (flags[1] == FACTION_ALLY) return src->faction == target->faction;
	if (flags[1] == FACTION_ENEMY) return src->faction != target->faction;
	throw std::logic_error("Unknown TARGET_TYPE");
}

bool EntityFilterRule::check_facing(Entity* src, Entity* target) {
	if (flags[0] == FACING_ANY) return true;
	if (src == NULL) throw std::logic_error("NULL src Entity and not FACING_TYPE_ANY");
	std::array<double, 2> vec = { target->pos.x - src->pos.x, target->pos.y - src->pos.y };
	double angle = std::acos(vec[0] / distance(vec, { 0,0 })) * 57.2957795;
	if (vec[1] > 0) angle = -angle;
	angle = abs(src->pos.angle - angle);
	if (angle > 180) angle = angle - 360;
	switch (flags[0]) {
	case FACING_FRONT: return abs(angle) <= FACING_FRONT_ANGLE;
	case FACING_BACK: return abs(angle) > FACING_BACK_ANGLE;
	case FACING_SIDE: return abs(angle) > FACING_FRONT_ANGLE and abs(angle) <= FACING_BACK_ANGLE;
	}
	throw std::logic_error("Unknown facing_type");
}

bool EntityFilterRule::check_distance(Entity* src, Entity* target) {
	if (src == NULL and params[5] != NULL and params[6] != NULL) throw std::logic_error("Can't apply distance filter - no src provided");
	if (src == NULL) return true;
	double dist = distance(src, target);
	if (params[5] != NULL and dist < params[5]) {
		return false;
	}
	if (params[6] != NULL and dist > params[6]) {
		return false;
	}
	return true;
}

Position EntityFilterRule::get_random_position(Position* origin) {
	std::uniform_int_distribution<> distr(0, 1);
	static auto mt = std::mt19937(std::random_device()());
	if (params[7] > params[8]) throw std::logic_error("min dist > max dist");
	double dist = params[7] + (params[8] - params[7]) * distr(mt);
	double x = dist / std::sqrt(2) * distr(mt) * (2 * std::round(distr(mt)) - 1);  // random signed double between 0 and max possible cathetus (dist/sqrt(2))
	double y = std::sqrt(std::pow(dist, 2) - std::pow(x, 2)) * (2 * std::round(distr(mt)) - 1); // 2nd cathetus calculated from dist and 1st cathetus
	return Position{ origin->x + x, origin->y + y, origin->angle };
}

void EntityFilterRule::rotate_position(Position* pos, double angle) {
	if (angle < 0) angle = 360 + angle;
	angle = angle / 57.2957795;
	double x = pos->x * std::cos(angle) - pos->y * std::sin(angle);
	double y = pos->x * std::sin(angle) + pos->y * std::cos(angle);
	pos->x = x;
	pos->y = y;
	pos->angle += angle;
	if (pos->angle >= 360) pos->angle = 360 - pos->angle;
}

Position EntityFilterRule::get_AOE_origin(Entity* src, Entity* target, Entity* provided_entity, Position* provided_position) {
	switch (flags[2]) {
	case ORIGIN_SRC:
		if (src == NULL) throw std::logic_error("ORIGIN_SRC but no origin");
		return src->pos;
	case ORIGIN_TARGET: return target->pos;
	case ORIGIN_PROVIDED_POINT: return *provided_position;
	case ORIGIN_RANDOM_POINT: return get_random_position(&provided_entity->pos); //TODO random point around different origins
	}
	throw std::logic_error("Unknown origin_type");
}


bool EntityFilterRule::check_area(Entity* src, Entity* target, Entity* provided_entity, Position* provided_position) {
	Position origin = get_AOE_origin(src, target, provided_entity, provided_position);
	Position adjusted_pos = { provided_entity->pos.x - origin.x, provided_entity->pos.y - origin.y };
	if (src != NULL) rotate_position(&adjusted_pos, params[0] + src->pos.angle);
	else rotate_position(&adjusted_pos, params[0]);
	rotate_position(&adjusted_pos, params[0]);
	switch (flags[3]) {
	case AREA_FORM_ELLIPSE:
		return std::pow(adjusted_pos.x, 2) / std::pow(params[1], 2) + std::pow(adjusted_pos.y, 2) / std::pow(params[2], 2) <= 1;
	case AREA_FORM_CONE:
		return true;
	case AREA_FORM_BOX:
		return adjusted_pos.x <= params[1] and adjusted_pos.x >= params[2] and adjusted_pos.y <= params[3] and adjusted_pos.y >= params[4];
	}
	throw std::logic_error("Unknown area_type");
}

EntityFilter::EntityFilter(std::set<RuleSignature> signatures, std::uint8_t filter_type, unsigned short int filter_amount) {
	this->signatures = signatures;
	this->filter_type = filter_type;
	this->filtter_amount = filter_amount;
}

std::set<Entity*> EntityFilter::apply(std::set<Entity*> entities, Entity* src, Entity* target, Position* provided_position) {
	if (filter_type == ENTITY_FILTER_AT_LEAST_N and filtter_amount > signatures.size()) throw std::logic_error("Number of required rules > total number of rules");
	unsigned int rules_satisfied;
	std::set<Entity*> filtered_entities;
	for (Entity* entity : entities) {
		rules_satisfied = 0;
		for (RuleSignature signature : signatures) {
			EntityFilterRule rule = EntityFilterRule(signature);
			if (rule.apply(src, target, entity, provided_position)) {
				rules_satisfied++;
				if (filter_type == ENTITY_FILTER_AT_LEAST_N and rules_satisfied == filtter_amount) {
					filtered_entities.insert(entity);
					break;
				}
			}
		}
		if (filter_type == ENTITY_FILTER_ALL and rules_satisfied == signatures.size()) filtered_entities.insert(entity);
	}
	return filtered_entities;
}