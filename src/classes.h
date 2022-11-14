#pragma once
#include <vector>
#include <array>
#include <string>
#include <set>
#include <bitset>

#include "consts.h"


class Entity {
public:
	Position pos;
	unsigned int id;
	unsigned int faction;
	double get_attr_by_id(unsigned int attr_id);
	void set_attr_by_id(unsigned int attr_id, double value);
	Entity(unsigned int id, Position pos, unsigned int faction);

};

class Effect {
public:
	std::vector<unsigned int> flags;
	Entity* origin;
	unsigned int id;
	unsigned int attr_id;
	unsigned int sign;
	void apply(Entity* target);
	Effect(Entity* origin, unsigned int id, unsigned int attr_id, unsigned int sign, std::vector<unsigned int> flags, unsigned int value_type, double exact_value = NULL, std::array<double, 2> value_range = { NULL, NULL }, unsigned int ref_attr_id = NULL, unsigned int ref_type = EFFECT_REF_TYPE_TARGET);
private:
	unsigned int units;
	unsigned int ref_type;
	unsigned int ref_attr_id;
	double exact_value;
	std::array<double, 2> value_range;
	double produce_value(Entity* ref);
	double produce_from_range();
	bool range_valid();
};

class EntityFilterRule {

private:
	bool check_facing(Entity* src, Entity* target);
	bool check_faction(Entity* src, Entity* target);
	bool check_distance(Entity* src, Entity* target);
	bool check_area(Entity* src, Entity* target, Entity* provided_entity, Position* provided_position);
	double distance(Entity* e1, Entity* e2);
	double distance(std::array<double, 2> v1, std::array<double, 2> v2);
	void rotate_position(Position* pos, double angle);
	void add_params_to_signature(RuleSignature& signature, unsigned short int& offset);
	void add_flags_to_signature(RuleSignature& signature, unsigned short int& offset);
	template <typename T> void add_data_to_signature(RuleSignature& signature, T data, unsigned short int& offset);
	Position get_random_position(Position* origin);
	Position get_AOE_origin(Entity* src, Entity* target, Entity* provided_entity, Position* provided_position);
public:
	std::array<std::unsigned short int, 6> flags;
	std::array<double, 9> params;
	EntityFilterRule(
		std::unsigned short int facing_type,
		std::unsigned short int faction_type,
		std::unsigned short int origin_type,
		std::unsigned short int area_form,
		bool include_src,
		bool include_target,
		std::array<double, 5> area_params,
		std::array<double, 2> dist_params,
		std::array<double, 2> rand_dist_params);
	EntityFilterRule(RuleSignature& signature);
	operator std::string() const;
	RuleSignature get_signature();
	bool apply(Entity* src, Entity* target, Entity* provided_entity, Position* provided_postion);
};

class EntityFilter {
public:
	EntityFilter(std::set<RuleSignature> signatures, std::unsigned short int filter_type, unsigned short int filter_amount = 0);
	std::set<Entity*> apply(std::set<Entity*> entities, Entity* src, Entity* target, Position* provided_position);
	unsigned int filter_type;
	unsigned int filtter_amount;
	std::set<RuleSignature> signatures;
};