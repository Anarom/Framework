#pragma once
#include <bitset>

const double FACING_FRONT_ANGLE = 60.0;
const double FACING_BACK_ANGLE = 120.0;
static const short int SIGNATURE_SIZE = 624;

typedef std::bitset<SIGNATURE_SIZE> RuleSignature;

enum EntityFilterTypes : std::uint8_t {
	ENTITY_FILTER_AT_LEAST_N = 0 ,
	ENTITY_FILTER_ALL = 1
};

enum FACTION_TYPE : std::uint8_t {
	FACTION_ANY = 0,
	FACTION_ALLY = 1,
	FACTION_ENEMY = 2
};

enum FACING_TYPE : std::uint8_t {
	FACING_ANY = 0,
	FACING_FRONT = 1,
	FACING_BACK = 2,
	FACING_SIDE = 3
};

enum QUANT_TYPE : std::uint8_t {
	QUANT_ALL = 0,
	QUANT_CLOSEST_N = 1,
	QUANT_FURTHEREST_N = 2,
	QUANT_RANDOM_N = 3
};

enum AREA_FORM : std::uint8_t {
	AREA_FORM_ELLIPSE = 1,
	AREA_FORM_CONE = 2,
	AREA_FORM_BOX = 3
};

enum AREA_ORIGIN : std::uint8_t {
	ORIGIN_SRC = 0,
	ORIGIN_TARGET = 1,
	ORIGIN_PROVIDED_POINT = 2,
	ORIGIN_RANDOM_POINT = 3
};

enum VALUE_UNIT : std::uint8_t {
	VALUE_POINTS = 0,
	VALUE_PERCENT = 1
};

enum EFFECT_REF_TYPE : std::uint8_t {
	EFFECT_REF_TYPE_ORIGIN = 0,
	EFFECT_REF_TYPE_TARGET = 1
};

struct TickInfo {
	unsigned int tick_amount;
	unsigned int tick_duration;
	bool tick_delayed;
};

struct Position {
	double x = 0;
	double y = 0;
	double angle = 0;
};