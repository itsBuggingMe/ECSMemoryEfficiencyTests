#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "Archetype.h"

#pragma once
class World
{
	std::vector<Archetype> _archetypes;
	std::vector<int32_t> _recycled;
	std::unordered_map<size_t, Archetype> _archetypesByHash;
	int32_t _next;

	struct EntityLocation
	{
		Archetype Archetype;
		int32_t Version;
		int32_t Row;
	};

	std::vector<EntityLocation> _root;

public:
	World()
	{

	}

	template<typename... Args>
	int32_t Create(Args...)
	{
		int32_t id;
		EntityLocation* loc;

		if (_recycled.size() == 0)
		{
			id = _next++;
			_root.emplace_back();
			loc = &_root.back();
		}
		else
		{
			id = _recycled.pop_back();
			loc = &_root[id]
		}

		loc->Archetype = _archetypesByHash[type_hash<Args>()];
		loc->Row = loc->Archetype.entity_count++;
		loc->Version++;

		int32_t row = loc->Row;
		((loc->Archetype.GetComponentUnsafe<std::decay_t<Args>>(row)
			= std::forward<Args>(args)), ...);

		return id;
	}

	template<typename... Args>
	std::size_t type_hash() {
		return typeid(std::tuple<Args...>).hash_code();
	}
};

