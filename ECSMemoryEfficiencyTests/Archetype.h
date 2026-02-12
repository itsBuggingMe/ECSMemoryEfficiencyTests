#include <vector>
#include <algorithm>
#include "ComponentMeta.cpp"

#define TRADITONAL_MEMORY

#pragma once
// note: alignment isn't managed
class Archetype
{
#ifdef TRADTIONAL_MEMORY
	void** component_data;
#else
	void* component_data;
	size_t stride;
#endif

	// # of rows
	size_t entity_count;
	std::vector<uint16_t> component_ids;
	
	// component id -> column index for traditional memory
	// component id -> offset for traditional memory
	uint16_t* map;
	size_t map_length;
	
public:
	Archetype(std::vector<uint16_t> components)
	{
		component_ids = components;
		if (components.empty()) {
			map_length = 0;
			map = nullptr;
		} else {
			uint16_t maxId = *std::max_element(components.begin(), components.end());
			map_length = static_cast<size_t>(maxId) + 1;
			map = new uint16_t[map_length];
			std::fill(map, map + map_length, -1);
		}

#ifdef TRADITONAL_MEMORY
		for (int32_t i = 0; i < components.size(); i++)
		{
			map[components[i]] = i;
		}
#else
		uint16_t offset = 0;
		for (int32_t i = 0; i < components.size(); i++)
		{
			auto type_id = components[i];
			map[type_id] = offset;
			offset += ComponentRegistry.component_sizes[type_id];
		}
#endif
	}

	template<typename T>
	T& GetComponentUnsafe(size_t rowIndex)
	{
		size_t componentId = ComponentRegistry.TypeId<T>();

#ifdef TRADITONAL_MEMORY
		uint8_t** bytePtr = static_cast<uint8_t**>(component_data);
		return *reinterpret_cast<T*>(&bytePtr[map[componentId]][rowIndex]);
#else
		uint8_t* bytePtr = static_cast<uint8_t*>(component_data);
		return bytePtr[stride * rowIndex + map[componentId]];
#endif
	}

	// todo: cleanup
};

