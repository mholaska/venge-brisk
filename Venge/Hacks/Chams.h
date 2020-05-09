#pragma once

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/Material.h"
#include "../SDK/ModelRender.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/RenderView.h"
#include "../SDK/Utils.h"

class colouredbody {
public:
    colouredbody() noexcept;
    bool render(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
private:
    bool renderPlayers(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderWeapons(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderHands(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;
    void renderSleeves(void*, void*, const ModelRenderInfo&, matrix3x4*) const noexcept;

    enum colouredbodyId {
        ALLIES_ALL = 0,
        ALLIES_VISIBLE,
        ALLIES_OCCLUDED,

        ENEMIES_ALL,
        ENEMIES_VISIBLE,
        ENEMIES_OCCLUDED,

        PLANTING_ALL,
        PLANTING_VISIBLE,
        PLANTING_OCCLUDED,

        DEFUSING_ALL,
        DEFUSING_VISIBLE,
        DEFUSING_OCCLUDED,

        LOCALPLAYER,
        WEAPONS,
        HANDS,
        timereverse,
        SLEEVES
    };

    Material* normal;
    Material* flat;
    Material* animated;
    Material* platinum;
    Material* glass;
    Material* crystal;
    Material* chrome;
    Material* silver;
    Material* gold;
    Material* plastic;

    constexpr auto dispatchMaterial(int id) const noexcept
    {
        switch (id) {
        default:
        case 0: return normal;
        case 1: return flat;
        case 2: return animated;
        case 3: return platinum;
        case 4: return glass;
        case 5: return chrome;
        case 6: return crystal;
        case 7: return silver;
        case 8: return gold;
        case 9: return plastic;
        }
    }

    constexpr void applycolouredbody(const Config::colouredbody::Material& colouredbody, bool ignorez, int health = 0) const noexcept
    {
        auto material = dispatchMaterial(colouredbody.material);

        if (colouredbody.healthBased && health)
            material->colorModulate(1.0f - health / 100.0f, health / 100.0f, 0.0f);
        else if (colouredbody.color.rainbow) {
            const auto [r, g, b] { rainbowColor(memory.globalVars->realtime, colouredbody.color.rainbowSpeed) };
            material->colorModulate(r, g, b);
        } else
            material->colorModulate(colouredbody.color.color);
        material->alphaModulate(colouredbody.alpha * (colouredbody.blinking ? sinf(memory.globalVars->currenttime * 5) * 0.5f + 0.5f : 1.0f));

        material->setMaterialVarFlag(MaterialVarFlag::IGNOREZ, ignorez);
        material->setMaterialVarFlag(MaterialVarFlag::WIREFRAME, colouredbody.wireframe);
        interfaces.modelRender->forceMaterialOverride(material);
    }
};
