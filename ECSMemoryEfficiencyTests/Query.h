#include <vector>
#include "World.h"

#pragma once
class Query
{
public:
	Query(std::vector<uint16_t> components)
	{
		matched_components = components;
	}

	std::vector<Archetype> matched_archetypes;
	std::vector<uint16_t> matched_components;
};