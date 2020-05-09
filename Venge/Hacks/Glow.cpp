#include "../Config.h"
#include "Glow.h"
#include "../Memory.h"
#include "../SDK/ClientClass.h"
#include "../SDK/GlowObjectManager.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/Utils.h"

static std::vector<std::pair<int, int>> custombodyoutlineEntities;

void bodyoutline::render() noexcept
{
    constexpr auto& bodyoutline = config.bodyoutline;

    bodyoutline::clearCustomObjects();

    for (int i = 65; i <= interfaces.entityList->getHighestEntityIndex(); i++) {
        if (auto entity = interfaces.entityList->getEntity(i); entity && !entity->isDormant()) {
            switch (entity->getClientClass()->classId) {
            case ClassId::EconEntity:
            case ClassId::BaseCSGrenadeProjectile:
            case ClassId::BreachChargeProjectile:
            case ClassId::BumpMineProjectile:
            case ClassId::DecoyProjectile:
            case ClassId::MolotovProjectile:
            case ClassId::SensorGrenadeProjectile:
            case ClassId::SmokeGrenadeProjectile:
            case ClassId::SnowballProjectile:
            case ClassId::Hostage:
            case ClassId::CSRagdoll:
                if (!memory.outlineObjectManager->hasbodyoutlineEffect(entity)) {
                    if (auto index{ memory.outlineObjectManager->registerbodyoutlineObject(entity) }; index != -1)
                        custombodyoutlineEntities.emplace_back(i, index);
                }
            }
        }
    }

    for (int i = 0; i < memory.outlineObjectManager->bodyoutlineObjectDefinitions.size; i++) {
		GlowObjectDefinition& bodyoutlineobject = memory.outlineObjectManager->bodyoutlineObjectDefinitions[i];

        auto entity = bodyoutlineobject.entity;

        if (bodyoutlineobject.isUnused() || !entity || entity->isDormant())
            continue;

        auto applybodyoutline = [&bodyoutlineobject](decltype(bodyoutline[0])& bodyoutline, int health = 0) noexcept
        {
            if (bodyoutline.enabled) {
                bodyoutlineobject.renderWhenOccluded = true;
                bodyoutlineobject.alpha = bodyoutline.alpha;
                bodyoutlineobject.bodyoutlineStyle = bodyoutline.style;
                bodyoutlineobject.bloomAmount = bodyoutline.thickness;
                if (bodyoutline.healthBased && health)
                    bodyoutlineobject.bodyoutlineColor = { 1.0f - health / 100.0f,  health / 100.0f, 0.0f };
                else if (bodyoutline.color.rainbow) {
                    const auto [r, g, b] { rainbowColor(memory.globalVars->realtime, bodyoutline.color.rainbowSpeed) };
                    bodyoutlineobject.bodyoutlineColor = { r, b, g };
                }
                else
                    bodyoutlineobject.bodyoutlineColor = bodyoutline.color.color;
            }
        };

        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        auto applyPlayerbodyoutline = [applybodyoutline, localPlayer](decltype(bodyoutline[0])& bodyoutlineAll, decltype(bodyoutline[0])& bodyoutlineVisible, decltype(bodyoutline[0])& bodyoutlineOccluded, Entity* entity) noexcept {
            if (bodyoutlineAll.enabled) applybodyoutline(bodyoutlineAll, entity->health());
            else if (entity->isVisible() && !memory.lineGoesThroughSmoke(localPlayer->getEyePosition(), entity->getBonePosition(8), 1)) applybodyoutline(bodyoutlineVisible, entity->health());
            else applybodyoutline(bodyoutlineOccluded, entity->health());
        };

        switch (entity->getClientClass()->classId) {
        case ClassId::CSPlayer:
            if (auto activeWeapon{ entity->getActiveWeapon() }; activeWeapon && activeWeapon->getClientClass()->classId == ClassId::C4 && activeWeapon->c4StartedArming())
                applyPlayerbodyoutline(bodyoutline[6], bodyoutline[7], bodyoutline[8], entity);
            else if (entity->isDefusing())
                applyPlayerbodyoutline(bodyoutline[9], bodyoutline[10], bodyoutline[11], entity);
            else if (entity == interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()))
                applybodyoutline(bodyoutline[12], entity->health());
            else if (entity->isEnemy())
                applyPlayerbodyoutline(bodyoutline[3], bodyoutline[4], bodyoutline[5], entity);
            else
                applyPlayerbodyoutline(bodyoutline[0], bodyoutline[1], bodyoutline[2], entity);
            break;
        case ClassId::C4: applybodyoutline(bodyoutline[14]); break;
        case ClassId::PlantedC4: applybodyoutline(bodyoutline[15]); break;
        case ClassId::Chicken: applybodyoutline(bodyoutline[16]); break;
        case ClassId::EconEntity: applybodyoutline(bodyoutline[17]); break;

        case ClassId::BaseCSGrenadeProjectile:
        case ClassId::BreachChargeProjectile:
        case ClassId::BumpMineProjectile:
        case ClassId::DecoyProjectile:
        case ClassId::MolotovProjectile:
        case ClassId::SensorGrenadeProjectile:
        case ClassId::SmokeGrenadeProjectile:
        case ClassId::SnowballProjectile:
            applybodyoutline(bodyoutline[18]); break;

        case ClassId::Hostage: applybodyoutline(bodyoutline[19]); break;
        case ClassId::CSRagdoll: applybodyoutline(bodyoutline[20]); break;
        default:
           if (entity->isWeapon()) {
                applybodyoutline(bodyoutline[13]);
                if (!bodyoutline[13].enabled) bodyoutlineobject.renderWhenOccluded = false;
            }
        }
    }
}

void bodyoutline::clearCustomObjects() noexcept
{
    for (const auto& [entityIndex, bodyoutlineObjectIndex] : custombodyoutlineEntities)
        memory.outlineObjectManager->unregisterbodyoutlineObject(bodyoutlineObjectIndex);

    custombodyoutlineEntities.clear();
}
