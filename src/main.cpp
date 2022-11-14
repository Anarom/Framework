#pragma once
#include <iostream>
#include <bitset>
#include <limits>

#include "consts.h"
#include "classes.h"


int main(int argc, char* argv) {
	EntityFilterRule rule(FACING_ANY, FACTION_ENEMY, ORIGIN_PROVIDED_POINT, AREA_FORM_ELLIPSE, false, true, { 0,5,5,3,4 }, { 6.2, 10.1 }, { 1.1, 5.2 });
	RuleSignature s = rule.get_signature();
	EntityFilterRule new_rule(s);
	std::cout << "Facing: " << new_rule.flags[0] << "\n";
	std::cout << "Faction: " << new_rule.flags[1] << "\n";
	std::cout << "Origin: " << new_rule.flags[2] << "\n";
	std::cout << "Area form: " << new_rule.flags[3] << "\n";
	std::cout << "Include src: " << new_rule.flags[4] << "\n";
	std::cout << "Include target: " << new_rule.flags[5] << "\n";
}