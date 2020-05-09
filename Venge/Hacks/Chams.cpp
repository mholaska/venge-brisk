#include <fstream>

#include "Chams.h"
#include "../Config.h"
#include "../Hooks.h"
#include "../Interfaces.h"
#include "Backtrack.h"
#include "../SDK/Material.h"
#include "../SDK/MaterialSystem.h"

colouredbody::colouredbody() noexcept
{
    std::ofstream{ "csgo/materials/colouredbodyNormal.vmt" } <<
        "VertexLitGeneric { }";

    std::ofstream{ "csgo/materials/colouredbodyFlat.vmt" } <<
        "UnlitGeneric { }";

    std::ofstream{ "csgo/materials/colouredbodyAnimated.vmt" } <<
        "VertexLitGeneric { $envmap editor/cube_vertigo $envmapcontrast 1 $envmaptint \"[.7 .7 .7]\" $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } } }";

    normal = interfaces.materialSystem->findMaterial("colouredbodyNormal");
    normal->incrementReferenceCount();
    flat = interfaces.materialSystem->findMaterial("colouredbodyFlat");
    flat->incrementReferenceCount();
    animated = interfaces.materialSystem->findMaterial("colouredbodyAnimated");
    animated->incrementReferenceCount();
    platinum = interfaces.materialSystem->findMaterial("models/player/ct_fbi/ct_fbi_glass");
    platinum->incrementReferenceCount();
    glass = interfaces.materialSystem->findMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass");
    glass->incrementReferenceCount();
    crystal = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/crystal_clear");
    crystal->incrementReferenceCount();
    chrome = interfaces.materialSystem->findMaterial("models/gibs/glass/glass");
    chrome->incrementReferenceCount();
    silver = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/silver_plain");
    silver->incrementReferenceCount();
    gold = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gold");
    gold->incrementReferenceCount();
    plastic = interfaces.materialSystem->findMaterial("models/inventory_items/trophy_majors/gloss");
    plastic->incrementReferenceCount();
}

bool colouredbody::render(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{

	

    const auto isLocalPlayerAlive = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isAlive();
    if (strstr(info.model->name, "models/player"))
        return renderPlayers(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "sleeve"))
        renderSleeves(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "arms"))
        renderHands(ctx, state, info, customBoneToWorld);
    else if (isLocalPlayerAlive && strstr(info.model->name, "models/weapons/v_")
        && !strstr(info.model->name, "tablet")
        && !strstr(info.model->name, "parachute")
        && !strstr(info.model->name, "fists"))
        renderWeapons(ctx, state, info, customBoneToWorld);
    return true;
}

bool colouredbody::renderPlayers(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{

    if (interfaces.modelRender->isMaterialOverridden())
        return true;

    bool needRedraw = true;

    if (auto entity{ interfaces.entityList->getEntity(info.entityIndex) }; entity && !entity->isDormant() && entity->isAlive()) {
        auto applied{ false };
        for (size_t i = 0; i < config.colouredbody[ALLIES_ALL].materials.size(); i++) {
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming()) {
                if (config.colouredbody[PLANTING_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[PLANTING_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[PLANTING_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.colouredbody[PLANTING_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[PLANTING_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.colouredbody[PLANTING_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.colouredbody[PLANTING_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[PLANTING_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            } else if (entity->isDefusing()) {
                if (config.colouredbody[DEFUSING_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[DEFUSING_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[DEFUSING_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.colouredbody[DEFUSING_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[DEFUSING_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.colouredbody[DEFUSING_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.colouredbody[DEFUSING_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[DEFUSING_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            } else if (info.entityIndex == interfaces.engine->getLocalPlayer()) {
                if (config.colouredbody[LOCALPLAYER].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[LOCALPLAYER].materials[i], false, entity->health());
                    applied = true;
                }
            } else if (entity->isEnemy()) {
                if (config.colouredbody[ENEMIES_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[ENEMIES_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[ENEMIES_ALL].materials[i], false, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    needRedraw = false;
                    applied = true;
                } else {
                    if (config.colouredbody[ENEMIES_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[ENEMIES_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.colouredbody[ENEMIES_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.colouredbody[ENEMIES_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[ENEMIES_VISIBLE].materials[i], false, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        needRedraw = false;
                        applied = true;
                    }
                }

                if (config.colouredbody[timereverse].materials[i].enabled && config.timereverse.enabled) {
                    auto record = &timereverse::records[info.entityIndex];
                    if (record && record->size() && timereverse::valid(record->front().simulationTime)) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[timereverse].materials[i], false, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, record->back().matrix);
                        interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                }
            } else {
                if (config.colouredbody[ALLIES_ALL].materials[i].enabled) {
                    if (applied)
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[ALLIES_ALL].materials[i], true, entity->health());
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                    applycolouredbody(config.colouredbody[ALLIES_ALL].materials[i], false, entity->health());
                    applied = true;
                } else {
                    if (config.colouredbody[ALLIES_OCCLUDED].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[ALLIES_OCCLUDED].materials[i], true, entity->health());
                        hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        if (!config.colouredbody[ALLIES_VISIBLE].materials[i].enabled)
                            interfaces.modelRender->forceMaterialOverride(nullptr);
                        applied = true;
                    }
                    if (config.colouredbody[ALLIES_VISIBLE].materials[i].enabled) {
                        if (applied)
                            hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                        applycolouredbody(config.colouredbody[ALLIES_VISIBLE].materials[i], false, entity->health());
                        applied = true;
                    }
                }
            }
        }
    }
    return needRedraw;
}

void colouredbody::renderWeapons(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    if (!localPlayer->isScoped()) {
        auto applied{ false };
        for (size_t i = 0; i < config.colouredbody[WEAPONS].materials.size(); i++) {
            if (config.colouredbody[WEAPONS].materials[i].enabled) {
                if (applied)
                    hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
                applycolouredbody(config.colouredbody[WEAPONS].materials[i], false, localPlayer->health());
                applied = true;
            }
        }
    }    
}

void colouredbody::renderHands(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    auto applied{ false };
    for (size_t i = 0; i < config.colouredbody[HANDS].materials.size(); i++) {
        if (config.colouredbody[HANDS].materials[i].enabled) {
            if (applied)
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
            applycolouredbody(config.colouredbody[HANDS].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}

void colouredbody::renderSleeves(void* ctx, void* state, const ModelRenderInfo& info, matrix3x4* customBoneToWorld) const noexcept
{
    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };
    auto applied{ false };
    for (size_t i = 0; i < config.colouredbody[SLEEVES].materials.size(); i++) {
        if (config.colouredbody[SLEEVES].materials[i].enabled) {
            if (applied)
                hooks.modelRender.callOriginal<void, void*, void*, const ModelRenderInfo&, matrix3x4*>(21, ctx, state, info, customBoneToWorld);
            applycolouredbody(config.colouredbody[SLEEVES].materials[i], false, localPlayer->health());
            applied = true;
        }
    }
}
