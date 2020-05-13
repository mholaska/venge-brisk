#pragma once

#include <array>
#include <filesystem>

#include "imgui/imgui.h"
#include "nSkinz/config_.hpp"

class Config {
public:
    explicit Config(const char*) noexcept;
    void load(size_t) noexcept;
    void save(size_t) const noexcept;
    void add(const char*) noexcept;
    void remove(size_t) noexcept;
    void rename(size_t, const char*) noexcept;
    void reset() noexcept;

    constexpr auto& getConfigs() noexcept
    {
        return configs;
    }

    struct Color {
        float color[3]{ 1.0f, 0.0f, 1.0f };
        bool rainbow{ false };
        float rainbowSpeed{ 0.6f };
    };
    
    struct ColorToggle : public Color {
        bool enabled{ false };
    };

    struct aimhelper {
        bool enabled{ false };
        bool onKey{ false };
		bool blatantMode{ false }; 
        int key{ 0 };
        int keyMode{ 0 };
        bool aimlock{ false };
        bool silent{ false };
        bool friendlyFire{ false };
        bool visibleOnly{ true };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        bool autoShot{ false };
        bool autoScope{ false };
        bool recoilbasedFov{ false };
        float fov{ 0.0f };
        float smooth{ 1.0f };
        int bone{ 0 };
        float recoilControlX{ 0.0f };
        float recoilControlY{ 0.0f };
        float maxAimInaccuracy{ 1.0f };
        float maxShotInaccuracy{ 1.0f };
        int minDamage{ 1 };
        bool killshot{ false };
        bool betweenShots{ false };
    };
    std::array<aimhelper, 40> aimhelper;

    struct fastshotbot {
        bool enabled{ false };
		bool blatantMode{ false };
        bool onKey{ false };
        int key{ 0 };
        bool friendlyFire{ false };
        bool scopedOnly{ true };
        bool ignoreFlash{ false };
        bool ignoreSmoke{ false };
        int hitgroup{ 0 };
        int shotDelay{ 115 };
        int minDamage{ 1 };
        bool killshot{ false };
    };
    std::array<fastshotbot, 40> fastshotbot;

    struct {
        bool enabled{ false };
        bool ignoreSmoke{ false };
        bool recoilBasedFov{ false };
        int timeLimit{ 1 };
    } timereverse;


    struct bodyoutline {
        bool enabled{ false };
        bool healthBased{ false };
        float thickness{ 1.0f };
        float alpha{ 1.0f };
        int style{ 0 };
        Color color;
    };
    std::array<bodyoutline, 21> bodyoutline;

    struct colouredbody {
        struct Material {
            bool enabled{ false };
            bool healthBased{ false };
			
            Color color;
            bool blinking{ false };
            int material{ 0 };
            bool wireframe{ false };
            float alpha{ 1.0f };
        };
		std::array<Material, 2> materials;

    };
    std::array<colouredbody, 18> colouredbody;

    struct esp {
        struct Shared {
            bool enabled{ false };
            int font{ 0x1d };
            ColorToggle snaplines;
            ColorToggle box;
            int boxType{ 0 };
            ColorToggle name;
            ColorToggle outline{ 0.0f, 0.0f, 0.0f };
            ColorToggle distance;
            float maxDistance{ 0.0f };
        };
       
        struct Player : public Shared {
            ColorToggle eyeTraces;
            ColorToggle health;
            ColorToggle healthBar;
            ColorToggle armor;
            ColorToggle armorBar;
            ColorToggle money;
            ColorToggle headDot;
            ColorToggle activeWeapon;
        };

        struct Weapon : public Shared { } weapon;

        struct Projectile : public Shared { };
        std::array<Projectile, 9> projectiles;

        struct DangerZone : public Shared { };
        std::array<DangerZone, 18> dangerZone;

        std::array<Player, 6> players;
    } esp;

    struct {
        bool disablePostProcessing{ false };
        bool inverseRagdollGravity{ false };
        bool noFog{ false };
        bool no3dSky{ false };
        bool noAimPunch{ false };
        bool noViewPunch{ false };
        bool noHands{ false };
        bool noSleeves{ false };
        bool noWeapons{ false };
        bool noSmoke{ false };
        bool noBlur{ false };
        bool noScopeOverlay{ false };
        bool noGrass{ false };
        bool noShadows{ false };
        bool wireframeSmoke{ false };
        bool zoom{ false };
        int zoomKey{ 0 };
        bool thirdperson{ false };
        int thirdpersonKey{ 0 };
        int thirdpersonDistance{ 0 };
        int viewmodelFov{ 0 };
        int fov{ 0 };
        int farZ{ 0 };
        int flashReduction{ 0 };
        float brightness{ 0.0f };
        int skybox{ 0 };
        Color world{ 0.0f, 0.0f, 0.0f };
        bool deaglespinner{ false };
        int screenEffect{ 0 };
        int hitMarker{ 0 };
        float hitMarkerTime{ 0.6f };
    } visuals;

    std::array<item_setting, 36> SkinChanger;

    struct {
        int chickenVolume{ 100 };

        struct Player {
            int masterVolume{ 100 };
            int headshotVolume{ 100 };
            int weaponVolume{ 100 };
            int footstepVolume{ 100 };
        };

        std::array<Player, 3> players;
    } sound;

    struct {
        int menuStyle{ 1 };
        int menuColors{ 0 };
    } style;

    struct {
        int menuKey{ 0x2D }; // VK_INSERT
        bool antiAfkKick{ false };
        bool autoStrafe{ false };
        bool fastmovementjump{ false };
        int fastmovementjum_hitchance{ 0 };
        int fastmovementjum_maxhops{ 0 };
        int fastmovementjum_minhops{ 0 };
        int fastmovementjum_restricted{ 0 };
        bool customClanTag{ false };
        bool clocktag{ false };
        char clanTag[16]{ "" };
        bool animatedClanTag{ false };
        bool useClanTag{ false };
        bool fastDuck{ false };
        bool spacemove{ false };
        bool scopegunpoint{ false };
        bool gunmovepoint{ false };
        bool autoPistol{ false };
        bool autoReload{ false };
        bool autoAccept{ false };
        bool radarHack{ false };
        bool revealRanks{ false };
        bool revealMoney{ false };
        ColorToggle spectatorList;
        bool watermark{ true };
        bool fixAnimationLOD{ false };
        bool fixBoneMatrix{ false };
        bool fixMovement{ false };
        bool disableModelOcclusion{ false };
        bool killMessage{ false };
        char killMessageString[230]{ "Brisk.Club" };
        bool nameStealer{ false };
        bool disablePanoramablur{ false };
        char voteText[50]{ "" };
        int banColor{ 6 };
        char banText[150]{ "Cheater has been permanently banned from official CS:GO servers." };
        bool fastPlant{ false };
        ColorToggle bombTimer{ 1.0f, 0.0f, 1.0f };
        bool quickReload{ false };
        bool prepareRevolver{ false };
        int prepareRevolverKey{ 0 };
        int hitSound{ 0 };
        int chokedPackets{ 0 };
        int chokedPacketsKey{ 0 };
        int quickHealthshotKey{ 0 };
        bool nadePredict{ false };
        bool fixTabletSignal{ false };
        float maxAngleDelta{ 255.0f };

        

    } misc;

private:
    std::filesystem::path path;
    std::vector<std::string> configs;
};

extern Config config;
