#include <sstream>

#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../Netvars.h"
#include "Misc.h"
#include "../SDK/ConVar.h"
#include "../SDK/Surface.h"
#include "../SDK/GlobalVars.h"
#include "../SDK/NetworkChannel.h"
#include "../SDK/WeaponData.h"

void Misc::inverseRagdollGravity() noexcept
{
    static auto ragdollGravity = interfaces.cvar->findVar("cl_ragdoll_gravity");
    ragdollGravity->setValue(config.visuals.inverseRagdollGravity ? -600 : 600);
}

int server_time;

void tag(std::string clantag)
{
    std::string clantag_ = clantag;
    if (!interfaces.engine->isInGame())
        return;

    static size_t lastTime = 0;

    if (GetTickCount64() > lastTime)
    {
        /*
        clantag_ += clantag_.at(0);
        clantag_.erase(0, 1);*/

        Misc::apply_clan_tag(clantag_.c_str());
        //memory.setClanTag(clantag_.c_str(), clantag_.c_str());

        lastTime = GetTickCount64() + 650;
    }
}

void Misc::updateClanTag(bool tagChanged) noexcept
{    
    if (!interfaces.engine->isInGame())
        return;
    if (config.misc.useClanTag)
    {
        auto local_player = reinterpret_cast<Entity*>(interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()));
        server_time = ((float)memory.globalVars->intervalPerTick * local_player->tickBase());
        //auto local_player = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        //server_time = ((float)memory.globalVars->intervalPerTick * local_player->tickBase());
        static int counter = 0;

        switch (int(server_time) % 28)
        {
        case 0: tag("                  "); break;
        case 1: tag("                 b"); break;
        case 2: tag("                br"); break;
        case 3: tag("               bri"); break;
        case 4: tag("              bris"); break;
        case 5: tag("             brisk"); break;
        case 6: tag("            brisk."); break;
        case 7: tag("           brisk.c"); break;
        case 8: tag("          brisk.club"); break;
        case 9: tag("         brisk.club"); break;
        case 10:tag("        brisk.club "); break;
        case 11:tag("       brisk.club  "); break;
        case 12:tag("      brisk.club   "); break;
        case 13:tag("     brisk.club    "); break;
        case 14:tag("    brisk.club     "); break;
        case 15:tag("   brisk.club      "); break;
        case 16:tag("  brisk.club       "); break;
        case 17:tag(" brisk.club        "); break;
        case 18:tag("brisk.club         "); break;
        case 19:tag("risk.club          "); break;
        case 20:tag("isk.club           "); break;
        case 22:tag("sk.club            "); break;
        case 23:tag("k.club             "); break;
        case 24:tag(".club              "); break;
        case 25:tag("club               "); break;
        case 26:tag("lub                "); break;
        case 27:tag("ub                 "); break;
        case 28:tag("b                 "); break;

        }

        /*
        int value = server_time % 11;
        if (counter % 11 == 0)
        {
            switch (value)
            {
            case 0: tag(" Brisk.club ");
            case 1: tag(" bRisk.club "); 
            case 2: tag(" brIsk.club "); 
            case 3: tag(" briSk.club "); 
            case 4: tag(" brisK.club "); 
            case 5: tag(" brisk.club "); 
            case 6: tag(" brisk.Club "); 
            case 7: tag(" brisk.cLub "); 
            case 8: tag(" brisk.clUb "); 
            case 9: tag(" Brisk.cluB "); 
            case 10: tag(" Brisk.club "); 
                break;
            }
            counter++;
        } */
    }
    else
    {
        if (tagChanged)
        {
            tag(" ");
        }
    }
}

std::uint8_t* pattern_scan(void* module, const char* signature) {
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dos_headers = (PIMAGE_DOS_HEADER)module;
    auto nt_headers = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dos_headers->e_lfanew);

    auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
    auto pattern_bytes = pattern_to_byte(signature);
    auto scan_bytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = pattern_bytes.size();
    auto d = pattern_bytes.data();

    for (auto i = 0ul; i < size_of_image - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scan_bytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scan_bytes[i];
        }
    }
    return nullptr;
}

void Misc::apply_clan_tag(const char* name) {
    using Fn = int(__fastcall*)(const char*, const char*);
    static auto apply_clan_tag_fn = reinterpret_cast<Fn>(pattern_scan(GetModuleHandleA("engine.dll"), "53 56 57 8B DA 8B F9 FF 15"));
    apply_clan_tag_fn(name, name);
};

void Misc::spectatorList() noexcept
{
    if (config.misc.spectatorList.enabled && interfaces.engine->isInGame()) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        if (!localPlayer->isAlive())
            return;

        interfaces.surface->setTextFont(Surface::font);

        if (config.misc.spectatorList.rainbow)
            interfaces.surface->setTextColor(rainbowColor(memory.globalVars->realtime, config.misc.spectatorList.rainbowSpeed));
        else
            interfaces.surface->setTextColor(config.misc.spectatorList.color);

        const auto [width, height] = interfaces.surface->getScreenSize();

        int textPositionY{ static_cast<int>(0.5f * height) };

        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            auto entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isAlive() || entity->isDormant())
                continue;

            if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(i, playerInfo) && entity->getObserverTarget() == localPlayer) {
                if (wchar_t name[128]; MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                    const auto [textWidth, textHeight] = interfaces.surface->getTextSize(Surface::font, name);
                    interfaces.surface->setTextPosition(width - textWidth - 5, textPositionY);
                    textPositionY -= textHeight;
                    interfaces.surface->printText(name);
                }
            }
        }
    }
}

void Misc::scopegunpoint() noexcept
{
    static auto showSpread = interfaces.cvar->findVar("weapon_debug_spread_show");
    showSpread->setValue(config.misc.scopegunpoint && !interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->isScoped() ? 3 : 0);
}

void Misc::gunmovepoint() noexcept
{
    static auto gunmovepoint = interfaces.cvar->findVar("cl_crosshair_recoil");
    gunmovepoint->setValue(config.misc.gunmovepoint ? 1 : 0);
}

void Misc::watermark() noexcept
{
    if (config.misc.watermark) {
        static auto frameRate = 1.0f;
        frameRate = 0.9f * frameRate + 0.1f * memory.globalVars->absoluteFrameTime;
        interfaces.surface->getScreenSize();
        const auto [width, height] = interfaces.surface->getScreenSize();


        //rectangles
        interfaces.surface->setDrawColor(12, 61, 140, 255);
        interfaces.surface->drawFilledRect(width - 182, 13, 13 + width - 225 + 188, 9 + 25);
        //interfaces.surface->setDrawColor(60, 60, 60, 255);
        interfaces.surface->drawFilledRect(width - 185, 10, 13 + width - 224 + 190, 10 + 27);
        interfaces.surface->setDrawColor(25, 25, 25, 255);
        interfaces.surface->drawFilledRect(width - 184, 11, 13 + width - 223 + 188, 11 + 25);
        //interfaces.surface->setDrawColor(60, 60, 60, 255);
        interfaces.surface->drawFilledRect(width - 181, 14, 13 + width - 220 + 182, 14 + 19);
        interfaces.surface->setDrawColor(25, 25, 25, 255);
        interfaces.surface->drawFilledRect(width - 180, 15, 13 + width - 219 + 180, 15 + 17);

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextPosition(width - 175, 15);
        interfaces.surface->setTextColor(12, 61, 140, 235);
        interfaces.surface->printText(L"Brisk");

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextPosition(width - 222 + 76, 15);
        interfaces.surface->setTextColor(255, 255, 255, 255);
        interfaces.surface->printText(L"  .Club");

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextPosition(width - 222 + 117, 15);
        interfaces.surface->setTextColor(225, 225, 225, 225);
        interfaces.surface->printText(L" | ");

        interfaces.surface->setTextFont(Surface::font);
        interfaces.surface->setTextColor(225, 225, 225, 255);
        interfaces.surface->setTextPosition(width - 222 + 130, 15);
        interfaces.surface->printText(L"FPS: " + std::to_wstring(static_cast<int>(1 / frameRate)));
    }
}

void Misc::prepareRevolver(UserCmd* cmd) noexcept
{
    constexpr auto timeToTicks = [](float time) {  return static_cast<int>(0.5f + time / memory.globalVars->intervalPerTick); };
    constexpr float revolverPrepareTime{ 0.234375f };

    static float readyTime;
    if (config.misc.prepareRevolver && (!config.misc.prepareRevolverKey || GetAsyncKeyState(config.misc.prepareRevolverKey))) {
        const auto activeWeapon = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer())->getActiveWeapon();
        if (activeWeapon && activeWeapon->itemDefinitionIndex2() == WeaponId::Revolver) {
            if (!readyTime) readyTime = memory.globalVars->serverTime() + revolverPrepareTime;
            auto ticksToReady = timeToTicks(readyTime - memory.globalVars->serverTime() - interfaces.engine->getNetworkChannel()->getLatency(0));
            if (ticksToReady > 0 && ticksToReady <= timeToTicks(revolverPrepareTime))
                cmd->buttons |= UserCmd::IN_ATTACK;
            else
                readyTime = 0.0f;
        }
    }
}

void Misc::fastPlant(UserCmd* cmd) noexcept
{
    if (config.misc.fastPlant) {
        static auto plantAnywhere = interfaces.cvar->findVar("mp_plant_c4_anywhere");

        if (plantAnywhere->getInt()) return;

        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        if (!localPlayer->isAlive() || localPlayer->inBombZone()) return;

        const auto activeWeapon = localPlayer->getActiveWeapon();
        if (!activeWeapon || activeWeapon->getClientClass()->classId != ClassId::C4)
            return;

        cmd->buttons &= ~UserCmd::IN_ATTACK;

        constexpr float doorRange{ 200.0f };
        Vector viewAngles{ cos(degreesToRadians(cmd->viewangles.x)) * cos(degreesToRadians(cmd->viewangles.y)) * doorRange,
                           cos(degreesToRadians(cmd->viewangles.x)) * sin(degreesToRadians(cmd->viewangles.y)) * doorRange,
                          -sin(degreesToRadians(cmd->viewangles.x)) * doorRange };
        static Trace trace;
        interfaces.engineTrace->traceRay({ localPlayer->getEyePosition(), localPlayer->getEyePosition() + viewAngles }, 0x46004009, localPlayer, trace);

        if (!trace.entity || trace.entity->getClientClass()->classId != ClassId::PropDoorRotating)
            cmd->buttons &= ~UserCmd::IN_USE;
    }
}

void Misc::drawBombTimer() noexcept
{
    if (config.misc.bombTimer.enabled) {
        for (int i = interfaces.engine->getMaxClients(); i <= interfaces.entityList->getHighestEntityIndex(); i++) {
            Entity* entity = interfaces.entityList->getEntity(i);
            if (!entity || entity->isDormant() || entity->getClientClass()->classId != ClassId::PlantedC4 || !entity->c4Ticking())
                continue;

            constexpr unsigned font{ 0xc1 };
            interfaces.surface->setTextFont(font);
            interfaces.surface->setTextColor(255, 255, 255);
            auto drawPositionY{ interfaces.surface->getScreenSize().second / 8 };
            auto bombText{ (std::wstringstream{ } << L"Bomb on " << (!entity->c4BombSite() ? 'A' : 'B') << L" : " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f) << L" s").str() };
            const auto bombTextX{ interfaces.surface->getScreenSize().first / 2 - static_cast<int>((interfaces.surface->getTextSize(font, bombText.c_str())).first / 2) };
            interfaces.surface->setTextPosition(bombTextX, drawPositionY);
            drawPositionY += interfaces.surface->getTextSize(font, bombText.c_str()).second;
            interfaces.surface->printText(bombText.c_str());

            const auto progressBarX{ interfaces.surface->getScreenSize().first / 3 };
            const auto progressBarLength{ interfaces.surface->getScreenSize().first / 3 };
            constexpr auto progressBarHeight{ 5 };

            interfaces.surface->setDrawColor(50, 50, 50);
            interfaces.surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
            if (config.misc.bombTimer.rainbow)
                interfaces.surface->setDrawColor(rainbowColor(memory.globalVars->realtime, config.misc.bombTimer.rainbowSpeed));
            else
                interfaces.surface->setDrawColor(config.misc.bombTimer.color);

            static auto c4Timer = interfaces.cvar->findVar("mp_c4timer");

            interfaces.surface->drawFilledRect(progressBarX, drawPositionY + 5, static_cast<int>(progressBarX + progressBarLength * std::clamp(entity->c4BlowTime() - memory.globalVars->currenttime, 0.0f, c4Timer->getFloat()) / c4Timer->getFloat()), drawPositionY + progressBarHeight + 5);

            if (entity->c4Defuser() != -1) {
                if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(interfaces.entityList->getEntityFromHandle(entity->c4Defuser())->index(), playerInfo)) {
                    if (wchar_t name[128];  MultiByteToWideChar(CP_UTF8, 0, playerInfo.name, -1, name, 128)) {
                        drawPositionY += interfaces.surface->getTextSize(font, L" ").second;
                        const auto defusingText{ (std::wstringstream{ } << name << L" is defusing: " << std::fixed << std::showpoint << std::setprecision(3) << (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) << L" s").str() };

                        interfaces.surface->setTextPosition((interfaces.surface->getScreenSize().first - interfaces.surface->getTextSize(font, defusingText.c_str()).first) / 2, drawPositionY);
                        interfaces.surface->printText(defusingText.c_str());
                        drawPositionY += interfaces.surface->getTextSize(font, L" ").second;

                        interfaces.surface->setDrawColor(50, 50, 50);
                        interfaces.surface->drawFilledRect(progressBarX - 3, drawPositionY + 2, progressBarX + progressBarLength + 3, drawPositionY + progressBarHeight + 8);
                        interfaces.surface->setDrawColor(0, 255, 0);
                        interfaces.surface->drawFilledRect(progressBarX, drawPositionY + 5, progressBarX + static_cast<int>(progressBarLength * (std::max)(entity->c4DefuseCountDown() - memory.globalVars->currenttime, 0.0f) / (interfaces.entityList->getEntityFromHandle(entity->c4Defuser())->hasDefuser() ? 5.0f : 10.0f)), drawPositionY + progressBarHeight + 5);

                        drawPositionY += interfaces.surface->getTextSize(font, L" ").second;
                        const wchar_t* canDefuseText;

                        if (entity->c4BlowTime() >= entity->c4DefuseCountDown()) {
                            canDefuseText = L"Can Defuse";
                            interfaces.surface->setTextColor(0, 255, 0);
                        } else {
                            canDefuseText = L"Cannot Defuse";
                            interfaces.surface->setTextColor(255, 0, 0);
                        }

                        interfaces.surface->setTextPosition((interfaces.surface->getScreenSize().first - interfaces.surface->getTextSize(font, canDefuseText).first) / 2, drawPositionY);
                        interfaces.surface->printText(canDefuseText);
                    }
                }
            }
            break;
        }
    }
}

void Misc::stealNames() noexcept
{
    if (config.misc.nameStealer) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());

        bool allNamesStolen = true;
        static std::vector<int> stolenIds;
        for (int i = 1; i <= interfaces.engine->getMaxClients(); i++) {
            if (auto entity = interfaces.entityList->getEntity(i); entity && entity != localPlayer) {
                if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(entity->index(), playerInfo) && !playerInfo.fakeplayer && std::find(std::begin(stolenIds), std::end(stolenIds), playerInfo.userId) == std::end(stolenIds)) {
                    allNamesStolen = false;
                    if (changeName(false, std::string{ playerInfo.name }.append("\x1").c_str(), 1.0f))
                        stolenIds.push_back(playerInfo.userId);
                    break;
                }
            }
        }
        if (allNamesStolen)
            stolenIds.clear();
    }
}

void Misc::disablePanoramablur() noexcept
{
    static auto blur = interfaces.cvar->findVar("@panorama_disable_blur");
    blur->setValue(config.misc.disablePanoramablur);
}

void Misc::quickReload(UserCmd* cmd) noexcept
{
    if (config.misc.quickReload) {
        const auto localPlayer = interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer());
        static Entity* reloadedWeapon{ nullptr };

        if (reloadedWeapon) {
            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (interfaces.entityList->getEntityFromHandle(weaponHandle) == reloadedWeapon) {
                    cmd->weaponselect = reloadedWeapon->index();
                    cmd->weaponsubtype = reloadedWeapon->getWeaponSubType();
                    break;
                }
            }
            reloadedWeapon = nullptr;
        }

        if (auto activeWeapon{ localPlayer->getActiveWeapon() }; activeWeapon && activeWeapon->isInReload() && activeWeapon->clip() == activeWeapon->getWeaponData()->maxClip) {
            reloadedWeapon = activeWeapon;

            for (auto weaponHandle : localPlayer->weapons()) {
                if (weaponHandle == -1)
                    break;

                if (auto weapon{ interfaces.entityList->getEntityFromHandle(weaponHandle) }; weapon && weapon != reloadedWeapon) {
                    cmd->weaponselect = weapon->index();
                    cmd->weaponsubtype = weapon->getWeaponSubType();
                    break;
                }
            }
        }
    }
}

bool Misc::changeName(bool reconnect, const char* newName, float delay) noexcept
{
    static auto exploitInitialized{ false };

    static auto name{ interfaces.cvar->findVar("name") };

    if (reconnect) {
        exploitInitialized = false;
        return false;
    }

    if (!exploitInitialized && interfaces.engine->isInGame()) {
        if (PlayerInfo playerInfo; interfaces.engine->getPlayerInfo(interfaces.engine->getLocalPlayer(), playerInfo) && (!strcmp(playerInfo.name, "?empty") || !strcmp(playerInfo.name, "\n\xAD\xAD\xAD"))) {
            exploitInitialized = true;
        } else {
            name->onChangeCallbacks.size = 0;
            name->setValue("\n\xAD\xAD\xAD");
            return false;
        }
    }

    static auto nextChangeTime{ 0.0f };
    if (nextChangeTime <= memory.globalVars->realtime) {
        name->setValue(newName);
        nextChangeTime = memory.globalVars->realtime + delay;
        return true;
    }
    return false;
}


void Misc::fastmovementjump(UserCmd* cmd) noexcept
{

    const int hitchance = config.misc.fastmovementjum_hitchance;
    const int hop_limit = config.misc.fastmovementjum_maxhops;
    const int min_hop = config.misc.fastmovementjum_minhops;
    const int restrict_limit = config.misc.fastmovementjum_restricted;
    static int hops_restricted = 0;
    static int hops_hit = 0;

    const auto localPlayer{ interfaces.entityList->getEntity(interfaces.engine->getLocalPlayer()) };

    static auto wasLastTimeOnGround{ localPlayer->flags() & 1 };

    if (!config.misc.fastmovementjump)
        return;

    if (!(cmd->buttons & UserCmd::IN_JUMP))
        return;

    if (!(localPlayer->flags() & wasLastTimeOnGround))
    {
        cmd->buttons &= ~UserCmd::IN_JUMP;
        hops_restricted = 0;
    }
    else if ((rand() % 100 > hitchance&& hops_restricted < restrict_limit || (hop_limit > 0 && hops_hit > hop_limit))) // If a random number out of 100 is larger than the bhop hitchance, and hops restricted is less than the restrict limit, or if the bhop limit is greater than 0 and hops_hit is greater than the hop limit
    {
        cmd->buttons &= ~UserCmd::IN_JUMP; // Stops sending jump command.
            hops_restricted++; // Increases the hops_restricted integer.
            hops_hit = 0; // Sets the number of hops_hit to 0.
    }
    else
        hops_hit++; // Increases the number of bhops hit.

    /*if (config.misc.fastmovementjump && !(localPlayer->flags() & 1) && localPlayer->moveType() != MoveType::LADDER && !wasLastTimeOnGround)
        cmd->buttons &= ~UserCmd::IN_JUMP;

    wasLastTimeOnGround = localPlayer->flags() & 1;*/
}

void Misc::nadePredict() noexcept
{ 
    static auto nadeVar{ interfaces.cvar->findVar("cl_grenadepreview") }; 
    
    nadeVar->onChangeCallbacks.size = 0; 
    nadeVar->setValue(config.misc.nadePredict); 
}

void Misc::nadeTrajectory() noexcept
{
    float value = 0.1;

    interfaces.cvar->findVar("sv_grenade_trajectory")->setValue(1);
    interfaces.cvar->findVar("sv_grenade_trajectory_time")->setValue(1);
    interfaces.cvar->findVar("sv_grenade_trajectory_thickness")->setValue(value);
}


