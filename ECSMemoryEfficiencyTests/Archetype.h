#include <vector>
#include <algorithm>
#include "ComponentMeta.cpp"

#define TRADITONAL_MEMORY

#pragma once
// note: alignment isn't managed
class Archetype
{
#ifdef TRADITONAL_MEMORY
	void** component_data;
	int32_t row_count;
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
	// world class maintains this
	// componentid of the type added or removed
	std::unordered_map<uint16_t, Archetype> ArchetypeGraphEdgesAddComponent;
	std::unordered_map<uint16_t, Archetype> ArchetypeGraphEdgesRemoveComponent;

	bool MatchesComponents(const std::vector<uint16_t>& components)
	{
		for (uint16_t comp : components)
		{
			if (std::find(component_ids.begin(), component_ids.end(), comp) == component_ids.end())
				return false;
		}

		return true;
	}


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

	template<typename T>
	void MoveEntity(int32_t row, T component, Archetype& destination)
	{
		// copy components over
		int32_t destinationRow = destination.AddEntity();
#ifdef TRADITONAL_MEMORY
		for (int32_t i = 0; i < component_ids.size(); i++)	
		{
			uint8_t destinationColumnIndex = destination.map[component_ids[i]];
			memcpy(,);
			destination.component_data[destinationColumnIndex][destinationRow] = component_data[i][row];
		}
#else

#endif
	}

	int32_t AddEntity()
	{
		int32_t newRow = entity_count;
		entity_count++;

		if (entity_count > row_count)
		{
#ifdef TRADITONAL_MEMORY
			// reallocate columns

			size_t newRowCount = row_count == 0 ? 1 : row_count * 2;
			for (int32_t i = 0; i < component_ids.size(); i++)
			{
				uint16_t size = ComponentRegistry::component_sizes[component_ids[i]];

				void* newColumn = malloc(size * newRowCount);

				std::memcpy(newColumn, component_data[i], newRow * size);
				free(component_data[i]);
				component_data[i] = newColumn;
			}
#else
			// reallocate single block

			size_t newRowCount = row_count == 0 ? 1 : row_count * 2;
			size_t newSize = stride * newRowCount;
			void* newData = malloc(newSize);
			std::memcpy(newData, component_data, entity_count * stride);
			free(component_data);
			component_data = newData;
#endif
		}

		return newRow;
	}

	// todo: cleanup
};

