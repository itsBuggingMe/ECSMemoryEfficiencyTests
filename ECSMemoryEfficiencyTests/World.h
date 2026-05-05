#include <cstdint>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string>
#include "Archetype.h"

class Query;

#pragma once
class World
{
	std::vector<Archetype> _archetypes;
	std::vector<int32_t> _recycled;
	std::unordered_map<size_t, Archetype> _archetypesByHash;
	std::vector<Query> _queries;
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

	// TODO: creating archetypes

    void OnArchetypeCreation(Archetype archetype)
	{
		for (Query& query : _queries)
		{
			if (archetype.MatchesComponents(query.matched_components))
			{
				query.matched_archetypes.push_back(archetype);
			}
		}
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
	Query CreateQuery()
	{
		var q = Query({ ComponentRegistry.TypeId<Args>()... });
		_queries.push_back(q);
		return q;
	}

	template<typename TComponent>
	void AddComponent(int32_t entityId, TComponent component)
{
	EntityLocation& loc = _root[entityId];

	std::vector<uint16_t> newComponentIds = loc.Archetype.component_ids;
	uint16_t newTypeId = ComponentRegistry.TypeId<TComponent>();
	if (std::find(newComponentIds.begin(), newComponentIds.end(), newTypeId) == newComponentIds.end()) {
		newComponentIds.push_back(newTypeId);
		std::sort(newComponentIds.begin(), newComponentIds.end());
	}

	Archetype* destArchetypePtr = nullptr;
	if (loc.Archetype.ArchetypeGraphEdgesAddComponent.count(newTypeId)) {
		destArchetypePtr = &loc.Archetype.ArchetypeGraphEdgesAddComponent[newTypeId];
	} else {
		size_t newHash;
		std::string hashStr;
		for (auto id : newComponentIds) hashStr += std::to_string(id) + ",";
		newHash = std::hash<std::string>{}(hashStr);
		Archetype newArchetype(newComponentIds);
		_archetypesByHash[newHash] = newArchetype;
		loc.Archetype.ArchetypeGraphEdgesAddComponent[newTypeId] = _archetypesByHash[newHash];
		destArchetypePtr = &loc.Archetype.ArchetypeGraphEdgesAddComponent[newTypeId];
	}

	Archetype& destArchetype = *destArchetypePtr;
	int32_t newRow = destArchetype.AddEntity();
	loc.Archetype.MoveEntity(newRow, component, destArchetype);

	loc.Archetype = destArchetype;
	loc.Row = newRow;
}

	template<typename... Args>
	std::size_t type_hash() {
		return typeid(std::tuple<Args...>).hash_code();
	}
};

