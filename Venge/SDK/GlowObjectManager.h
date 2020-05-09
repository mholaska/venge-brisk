#pragma once

#include "UtlVector.h"
#include "Vector.h"

class Entity;

struct GlowObjectDefinition {
    Entity* entity;
    Vector bodyoutlineColor;
    float   alpha;
    std::byte pad[4];
    float   m_flSomeFloat;
    float   bloomAmount;
    float   m_flAnotherFloat;
    bool    renderWhenOccluded;
    bool    renderWhenUnoccluded;
    bool    fullBloomRender;
    std::byte pad1;
    int fullBloomStencilTestValue;
    int bodyoutlineStyle;
    int splitScreenSlot;
    int nextFreeSlot;

    constexpr bool isUnused() const noexcept { return nextFreeSlot != ENTRY_IN_USE; }

    static constexpr int END_OF_FREE_LIST = -1;
    static constexpr int ENTRY_IN_USE = -2;
};

struct bodyoutlineObjectManager {
    UtlVector<GlowObjectDefinition> bodyoutlineObjectDefinitions;

    constexpr bool hasbodyoutlineEffect(Entity* entity) noexcept
    {
        for (int i = 0; i < bodyoutlineObjectDefinitions.size; i++)
            if (!bodyoutlineObjectDefinitions[i].isUnused() && bodyoutlineObjectDefinitions[i].entity == entity)
                return true;

        return false;
    }

    constexpr int registerbodyoutlineObject(Entity* entity) noexcept
    {
        int index = firstFreeSlot;
        if (index != -1) {
            firstFreeSlot = bodyoutlineObjectDefinitions[index].nextFreeSlot;
            bodyoutlineObjectDefinitions[index].entity = entity;
            bodyoutlineObjectDefinitions[index].fullBloomRender = false;
            bodyoutlineObjectDefinitions[index].fullBloomStencilTestValue = 0;
            bodyoutlineObjectDefinitions[index].splitScreenSlot = -1;
            bodyoutlineObjectDefinitions[index].nextFreeSlot = -2;
        }
        return index;
    }

    constexpr void unregisterbodyoutlineObject(int index) noexcept
    {
        bodyoutlineObjectDefinitions[index].nextFreeSlot = firstFreeSlot;
        bodyoutlineObjectDefinitions[index].entity = nullptr;
        bodyoutlineObjectDefinitions[index].renderWhenOccluded = false;
        bodyoutlineObjectDefinitions[index].renderWhenUnoccluded = false;
        firstFreeSlot = index;
    }

    int firstFreeSlot;
};
