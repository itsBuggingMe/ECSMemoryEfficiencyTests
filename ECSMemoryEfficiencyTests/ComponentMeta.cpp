#include <cstdint>
#include <vector>
#include "ComponentMeta.h"

class ComponentRegistry
{
    inline static uint16_t next_id = 0;
    uint16_t create_type_id(uint16_t size)
    {
        component_sizes.push_back(size);
        return ComponentRegistry::next_id++;
    }

public:
    inline static std::vector<uint16_t> component_sizes;

    template<typename T>
    uint16_t TypeId()
    {
        static uint16_t id = create_type_id(sizeof(T));
        return id;
    }
};