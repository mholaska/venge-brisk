#include <fstream>
#include <functional>
#include <string>
#include <ShlObj.h>
#include <Windows.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"

#include "imguiCustom.h"

#include "GUI.h"
#include "Config.h"
#include "Hacks/Misc.h"
#include "Hacks/SkinChanger.h"
#include "Hacks/Visuals.h"
#include "Hooks.h"
#include "Icons.h"
#include "SDK/InputSystem.h"

#include <fstream>
#include <iostream>
#include "imgui/imgui_internal.h"

using namespace std;

#pragma comment(lib, "ntdll.lib")

#ifndef _NTDEF_
typedef _Return_type_success_(return >= 0) LONG NTSTATUS;
typedef NTSTATUS* PNTSTATUS;
#endif

EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);

EXTERN_C NTSTATUS NTAPI NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, ULONG  UnicodeStringParamenterMask,
	PULONG_PTR Parameters, ULONG ValidResponseOption, PULONG Response);

constexpr auto windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize
| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

GUI::GUI() noexcept
{
	
	/*
	* The code below is a verification that makes sure the cheat is being loaded.
	* During development, make sure to leave it commented out. When live build goes up,
	* make sure that you uncomment lines 52-66 and comment line 67.
	*/


	/*string key;
	bool found = false;
	ifstream verify("C:\\Windows\\DNPIM.txt");

	while (!verify.eof())
	{
		getline(verify, key);

		if (key == "hXODteXcqq")
			found = true;

		verify.close();
	}*/

	//if (!found)
		if (0 > 1)
	{
		//MessageBox(NULL, "Cracked DLL can't be used. Please purchase cheat.", "Anti-Crack Detection", MB_OK | MB_ICONINFORMATION);
		MessageBox(NULL, "Purchase Brisk.Club to use our software!", "Anti-Crack Detection", MB_OK | MB_ICONINFORMATION);
		/*BOOLEAN bl;
		unsigned long response;
		RtlAdjustPrivilege(19, true, false, &bl);
		NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, 0, 6, &response);
		return;
		_Exit(10);*/
		exit(1);
	}
	else
	{


		ImGui::CreateContext();
		ImGui_ImplWin32_Init(FindWindowW(L"Valve001", NULL));

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();

		style.ScrollbarSize = 9.0f;

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		io.LogFilename = nullptr;


		if (PWSTR pathToFonts; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &pathToFonts)))
		{
			const std::filesystem::path path{ pathToFonts };
			CoTaskMemFree(pathToFonts);

			ImFontConfig font_config;
			font_config.OversampleH = 1; //or 2 is the same
			font_config.OversampleV = 1;
			font_config.PixelSnapH = 1;

			static ImWchar ranges[] = { 0x0020, 0x00FF, 0x0100, 0x017f, 0 }; // 0x0020, 0x00FF, 0x0100, 0x017f, 0
			fonts.tahoma = io.Fonts->AddFontFromFileTTF((path / "arial.ttf").string().c_str(), 16.0f, nullptr, ranges);
			fonts.segoeui = io.Fonts->AddFontFromFileTTF((path / "arial.ttf").string().c_str(), 16.0f, nullptr, ranges);
			fonts.tabfont = io.Fonts->AddFontFromMemoryTTF(&menuicons, sizeof menuicons, 35.0f, &font_config, ranges);
			//fonts.tabfont = io.Fonts->AddFontFromFileTTF((path / "osmiummenufont.ttf").string().c_str(), 35.0f, nullptr, ranges);
		}
	}
}

void GUI::render() noexcept
{
	renderGuiStyle2();
}

void GUI::updateColors() const noexcept
{
	switch (config.style.menuColors) {
	case 0: ImGui::StyleColorsDark(); break;
	case 1: ImGui::StyleColorsClassic(); break;
	}
}

void GUI::hotkey(int& key) noexcept
{
	constexpr bool stringDisplayTest = true;

	if constexpr (stringDisplayTest)
		key ? ImGui::Text("[ %s ]", interfaces.inputSystem->virtualKeyToString(key)) : ImGui::TextUnformatted("[ key ]");
	else
		key ? ImGui::Text("[ 0x%x ]", key) : ImGui::TextUnformatted("[ key ]");

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Press any key to change keybind");
		ImGuiIO& io = ImGui::GetIO();
		for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
			if (ImGui::IsKeyPressed(i) && i != config.misc.menuKey)
				key = i != VK_ESCAPE ? i : 0;

		for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
			if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != config.misc.menuKey)
				key = i + (i > 1 ? 2 : 1);
	}
}

void GUI::renderMenuBar() noexcept
{
	if (ImGui::BeginMainMenuBar()) {
		ImGui::MenuItem("Aimbot", nullptr, &window.aimhelper);
		ImGui::MenuItem("Trigger", nullptr, &window.fastshotbot);
		ImGui::MenuItem("Glow", nullptr, &window.bodyoutline);
		ImGui::MenuItem("Chams", nullptr, &window.colouredbody);
		ImGui::MenuItem("ESP", nullptr, &window.esp);
		ImGui::MenuItem("Visuals", nullptr, &window.visuals);
		ImGui::MenuItem("Skins", nullptr, &window.skinChanger);
		ImGui::MenuItem("Misc", nullptr, &window.misc);
		ImGui::MenuItem("Config", nullptr, &window.config);
		ImGui::EndMainMenuBar();
	}
}

void GUI::renderAimbotWindow() noexcept
{
	if (window.aimhelper) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 600.0f, 0.0f });
			ImGui::Begin("Aimbot", &window.aimhelper, windowFlags);
		}

		ImGui::Spacing();

		ImGui::BeginChild("Test", ImVec2(609, 401), false);
		{
			static int currentCategory{ 0 };
			ImGui::PushItemWidth(110.0f);
			//ImGui::PushFont(fonts.tahoma);
			ImGui::PushID(0);
			ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0");
			ImGui::PopID();
			ImGui::SameLine();
			static int currentWeapon{ 0 };
			ImGui::PushID(1);

			static int selectedRifle;

			switch (currentCategory) {
			case 0:
				currentWeapon = 0;
				ImGui::NewLine();
				break;
			case 1: {
				static int currentPistol{ 0 };
				static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };
				ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
					if (config.aimhelper[idx ? idx : 35].enabled) {
						static std::string name;
						name = pistols[idx];
						*out_text = name.append(" (ON)").c_str();
					}
					else {
						*out_text = pistols[idx];
					}
					return true;
				}, nullptr, IM_ARRAYSIZE(pistols));

				currentWeapon = currentPistol ? currentPistol : 35;
				break;
			}
			case 2: {
				static int currentHeavy{ 0 };
				static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

				ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
					if (config.aimhelper[idx ? idx + 10 : 36].enabled) {
						static std::string name;
						name = heavies[idx];
						*out_text = name.append(" (ON)").c_str();
					}
					else {
						*out_text = heavies[idx];
					}
					return true;
				}, nullptr, IM_ARRAYSIZE(heavies));

				currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
				break;
			}
			case 3: {
				static int currentSmg{ 0 };
				static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

				ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
					if (config.aimhelper[idx ? idx + 16 : 37].enabled) {
						static std::string name;
						name = smgs[idx];
						*out_text = name.append(" (ON)").c_str();
					}
					else {
						*out_text = smgs[idx];
					}
					return true;
				}, nullptr, IM_ARRAYSIZE(smgs));

				currentWeapon = currentSmg ? currentSmg + 16 : 37;
				break;
			}
			case 4: {
				static int currentRifle{ 0 };
				static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

				ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
					if (config.aimhelper[idx ? idx + 23 : 38].enabled) {
						static std::string name;
						name = rifles[idx];
						*out_text = name.append(" (ON)").c_str();
					}
					else {
						*out_text = rifles[idx];
					}
					return true;
				}, nullptr, IM_ARRAYSIZE(rifles));

				currentWeapon = currentRifle ? currentRifle + 23 : 38;
				selectedRifle = currentRifle;
				break;
			}
			}
			ImGui::PopID();
			ImGui::SameLine();
			ImGui::Checkbox("Enabled", &config.aimhelper[currentWeapon].enabled);
			ImGui::SameLine();
			ImGui::Checkbox("Blatant Mode", &config.aimhelper[currentWeapon].blatantMode);
			//ImGui::Separator();
			ImGui::NewLine();
			ImGui::Columns(2, nullptr, false);
			ImGui::SetColumnOffset(1, 235.0f);
			ImGui::Checkbox("On Key", &config.aimhelper[currentWeapon].onKey);
			ImGui::SameLine();
			hotkey(config.aimhelper[currentWeapon].key);
			ImGui::SameLine();
			ImGui::PushID(2);
			ImGui::PushItemWidth(70.0f);
			ImGui::Combo("", &config.aimhelper[currentWeapon].keyMode, "Hold\0Toggle\0");
			ImGui::PopItemWidth();
			ImGui::PopID();
			if (config.aimhelper[currentWeapon].blatantMode)
			{
				ImGui::Checkbox("Aimlock", &config.aimhelper[currentWeapon].aimlock);
				ImGui::Checkbox("Silent", &config.aimhelper[currentWeapon].silent);
			}
			ImGui::Checkbox("Friendly Fire", &config.aimhelper[currentWeapon].friendlyFire);
			ImGui::Checkbox("Visible Only", &config.aimhelper[currentWeapon].visibleOnly);
			if (currentCategory == 0 || currentCategory == 4)
			{
				if (currentCategory == 0)
					ImGui::Checkbox("Scoped Only", &config.aimhelper[currentWeapon].scopedOnly);
				else if (selectedRifle == 0 || selectedRifle == 6 || selectedRifle == 7 || selectedRifle == 8 || selectedRifle == 9 || selectedRifle == 10 || selectedRifle == 11)
					ImGui::Checkbox("Scoped Only", &config.aimhelper[currentWeapon].scopedOnly);
			}
			if (config.aimhelper[currentWeapon].blatantMode)
			{
				ImGui::Checkbox("Ignore Flash", &config.aimhelper[currentWeapon].ignoreFlash);
				ImGui::Checkbox("Ignore Smoke", &config.aimhelper[currentWeapon].ignoreSmoke);
				ImGui::Checkbox("Auto Shot", &config.aimhelper[currentWeapon].autoShot);
				ImGui::Checkbox("Auto Scope", &config.aimhelper[currentWeapon].autoScope);
			}

			ImGui::Checkbox("Recoil-Based FOV", &config.aimhelper[currentWeapon].recoilbasedFov);
			ImGui::Combo("Hitbox", &config.aimhelper[currentWeapon].bone, "Nearest\0Critical Shot\0Head\0Neck\0Upper Chest\0Lower Chest\0Stomach\0Pelvis\0");
			ImGui::PushItemWidth(220.0f);
			ImGui::Checkbox("Backtrack", &config.timereverse.enabled);
			//ImGui::Checkbox("Ignore smoke", &config.timereverse.ignoreSmoke);
			//ImGui::Checkbox("Recoil based fov", &config.timereverse.recoilBasedFov);
			ImGui::PushItemWidth(220.0f);
			ImGui::SliderInt("", &config.timereverse.timeLimit, 1, 200, "Backtrack Time: %d ms");
			ImGui::PopItemWidth();
			ImGui::NextColumn();
			ImGui::SliderFloat("Smoothing", &config.aimhelper[currentWeapon].smooth, 1.0f, 50.0f, "%.2f");
			if (config.aimhelper[currentWeapon].blatantMode)
			{
				ImGui::SliderFloat("FOV", &config.aimhelper[currentWeapon].fov, 0.0f, 200.0f, "%.2f");
			}
			else
			{
				ImGui::SliderFloat("FOV", &config.aimhelper[currentWeapon].fov, 0.0f, 10.0f, "%.2f");
			}
			ImGui::Text("");
			ImGui::SliderFloat("RCS X", &config.aimhelper[currentWeapon].recoilControlX, 0.0f, 1.0f, "%.2f");
			ImGui::SliderFloat("RCS Y", &config.aimhelper[currentWeapon].recoilControlY, 0.0f, 1.0f, "%.2f");
			ImGui::Text("");
			//ImGui::SliderFloat("Aim Accuracy", &config.aimhelper[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);
			//ImGui::SliderFloat("Shot Accuracy", &config.aimhelper[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f", 2.0f);

			/*if (config.aimhelper[currentWeapon].blatantMode)
			{
				ImGui::Text("");
				ImGui::InputInt("Min damage", &config.aimhelper[currentWeapon].minDamage);
				config.aimhelper[currentWeapon].minDamage = std::clamp(config.aimhelper[currentWeapon].minDamage, 0, 250);
				//ImGui::Checkbox("Killshot", &config.aimhelper[currentWeapon].killshot);
			}*/
			//ImGui::Checkbox("Between shots", &config.aimhelper[currentWeapon].betweenShots);

			//ImGui::PopFont();

			ImGui::Columns(1);
			if (!config.style.menuStyle)
				ImGui::End();
		}
		ImGui::EndChild();
	}
}

void GUI::renderfastshotbotWindow() noexcept
{

	if (window.fastshotbot) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin("Triggerbot", &window.fastshotbot, windowFlags);
		}

		ImGui::Text("Triggerbot is in beta. Use at own risk!"); 

		//ImGui::PushFont(fonts.tahoma);
		static int selectedRifle;
		static int currentCategory{ 0 };
		ImGui::PushItemWidth(110.0f);
		ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, "All\0Pistols\0Heavy\0SMG\0Rifles\0Zeus x27\0");
		ImGui::PopID();
		ImGui::SameLine();
		static int currentWeapon{ 0 };
		ImGui::PushID(1);
		switch (currentCategory) {
		case 0:
			currentWeapon = 0;
			ImGui::NewLine();
			break;
		case 5:
			currentWeapon = 39;
			ImGui::NewLine();
			break;

		case 1: {
			static int currentPistol{ 0 };
			static constexpr const char* pistols[]{ "All", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

			ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
				if (config.fastshotbot[idx ? idx : 35].enabled) {
					static std::string name;
					name = pistols[idx];
					*out_text = name.append(" *").c_str();
				} else {
					*out_text = pistols[idx];
				}
				return true;
			}, nullptr, IM_ARRAYSIZE(pistols));

			currentWeapon = currentPistol ? currentPistol : 35;
			break;
		}
		case 2: {
			static int currentHeavy{ 0 };
			static constexpr const char* heavies[]{ "All", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

			ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
				if (config.fastshotbot[idx ? idx + 10 : 36].enabled) {
					static std::string name;
					name = heavies[idx];
					*out_text = name.append(" *").c_str();
				} else {
					*out_text = heavies[idx];
				}
				return true;
			}, nullptr, IM_ARRAYSIZE(heavies));

			currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
			break;
		}
		case 3: {
			static int currentSmg{ 0 };
			static constexpr const char* smgs[]{ "All", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

			ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
				if (config.fastshotbot[idx ? idx + 16 : 37].enabled) {
					static std::string name;
					name = smgs[idx];
					*out_text = name.append(" *").c_str();
				} else {
					*out_text = smgs[idx];
				}
				return true;
			}, nullptr, IM_ARRAYSIZE(smgs));

			currentWeapon = currentSmg ? currentSmg + 16 : 37;
			break;
		}
		case 4: {
			static int currentRifle{ 0 };
			static constexpr const char* rifles[]{ "All", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

			ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
				if (config.fastshotbot[idx ? idx + 23 : 38].enabled) {
					static std::string name;
					name = rifles[idx];
					*out_text = name.append(" *").c_str();
				} else {
					*out_text = rifles[idx];
				}
				return true;
			}, nullptr, IM_ARRAYSIZE(rifles));

			currentWeapon = currentRifle ? currentRifle + 23 : 38;
			selectedRifle = currentRifle;
			break;
		}
		}
		ImGui::PopID();
		ImGui::SameLine();
		ImGui::Checkbox("Enabled", &config.fastshotbot[currentWeapon].enabled);
		ImGui::SameLine();
		ImGui::Checkbox("Blatant Mode", &config.fastshotbot[currentWeapon].blatantMode);
		ImGui::NewLine();
		ImGui::Checkbox("On Key", &config.fastshotbot[currentWeapon].onKey);
		ImGui::SameLine();
		hotkey(config.fastshotbot[currentWeapon].key);
		ImGui::Checkbox("Friendly Fire", &config.fastshotbot[currentWeapon].friendlyFire);


		if (currentCategory == 0 || currentCategory == 4)
		{
			if (currentCategory == 0)
				ImGui::Checkbox("Scoped Only", &config.fastshotbot[currentWeapon].scopedOnly);
			else if (selectedRifle == 0 || selectedRifle == 6 || selectedRifle == 7 || selectedRifle == 8 || selectedRifle == 9 || selectedRifle == 10 || selectedRifle == 11)
				ImGui::Checkbox("Scoped Only", &config.fastshotbot[currentWeapon].scopedOnly);
		}


		if (config.fastshotbot[currentWeapon].blatantMode)
		{
			ImGui::Checkbox("Ignore Flash", &config.fastshotbot[currentWeapon].ignoreFlash);
			ImGui::Checkbox("Ignore Smoke", &config.fastshotbot[currentWeapon].ignoreSmoke);
			
		}
		
		ImGui::Text("");
		ImGui::SetNextItemWidth(85.0f);
		ImGui::Combo("Hitgroup", &config.fastshotbot[currentWeapon].hitgroup, "All\0Head\0Chest\0Stomach\0Left Arm\0Right Arm\0Left Leg\0Right Leg\0");
		ImGui::PushItemWidth(220.0f);
		if (config.fastshotbot[currentWeapon].blatantMode)
		{
			ImGui::SliderInt("", &config.fastshotbot[currentWeapon].shotDelay, 0, 250, "Shot Delay: %d ms");
		}
		else
		{
			ImGui::SliderInt("", &config.fastshotbot[currentWeapon].shotDelay, 115, 250, "Shot Delay: %d ms");
		}
		
		ImGui::InputInt("Min Damage", &config.fastshotbot[currentWeapon].minDamage);
		config.fastshotbot[currentWeapon].minDamage = std::clamp(config.fastshotbot[currentWeapon].minDamage, 0, 250);
		if (config.fastshotbot[currentWeapon].blatantMode)
		{
			ImGui::Checkbox("Killshot", &config.fastshotbot[currentWeapon].killshot);
		}

		//ImGui::PopFont();

		if (!config.style.menuStyle)
			ImGui::End();
	}
}

void GUI::renderbodyoutlineWindow() noexcept
{
	if (window.bodyoutline) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 450.0f, 0.0f });
			ImGui::Begin("Glow", &window.bodyoutline, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		if (ImGui::Button("Main", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = true;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Chams", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = true;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Glow", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = true;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("ESP", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = true;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}


		//ImGui::NewLine();

		static int currentCategory{ 0 };
		ImGui::PushItemWidth(110.0f);
		ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local Player\0Weapons\0C4\0Planted C4\0Chickens\0Defuse Kits\0Projectiles\0Hostages\0Ragdolls\0");
		ImGui::PopID();
		static int currentItem{ 0 };
		if (currentCategory <= 3) {
			ImGui::SameLine();
			static int currentType{ 0 };
			ImGui::PushID(1);
			ImGui::Combo("", &currentType, "All\0Visible\0Invisible\0");
			ImGui::PopID();
			currentItem = currentCategory * 3 + currentType;
		}
		else {
			currentItem = currentCategory + 8;
		}

		ImGui::SameLine();
		ImGui::Checkbox("Enabled", &config.bodyoutline[currentItem].enabled);
		ImGui::Separator();
		ImGui::Checkbox("Health Based", &config.bodyoutline[currentItem].healthBased);

		ImGuiCustom::colorPicker("Color", config.bodyoutline[currentItem].color.color, nullptr, &config.bodyoutline[currentItem].color.rainbow, &config.bodyoutline[currentItem].color.rainbowSpeed);

		ImGui::Text("");
		//ImGui::NextColumn();
		ImGui::PushItemWidth(220.0f);
		ImGui::SliderFloat("Thickness", &config.bodyoutline[currentItem].thickness, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Alpha", &config.bodyoutline[currentItem].alpha, 0.0f, 1.0f, "%.2f");
		ImGui::SliderInt("Style", &config.bodyoutline[currentItem].style, 0, 3);
		ImGui::Columns(1);

		//ImGui::PopFont();

		if (!config.style.menuStyle)
			ImGui::End();
	}
}

void GUI::rendercolouredbodyWindow() noexcept
{
	if (window.colouredbody) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin("Chams", &window.colouredbody, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		if (ImGui::Button("Main", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = true;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Chams", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = true;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Glow", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = true;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("ESP", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = true;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}


		//ImGui::NewLine();

		static int currentCategory{ 0 };
		ImGui::PushItemWidth(110.0f);
		ImGui::PushID(0);
		ImGui::Combo("", &currentCategory, "Allies\0Enemies\0Planting\0Defusing\0Local Player\0Weapons\0Hands\0Backtrack\0Sleeves\0");
		ImGui::PopID();
		static int currentItem{ 0 };

		if (currentCategory <= 3) {
			ImGui::SameLine();
			static int currentType{ 0 };
			ImGui::PushID(1);
			ImGui::Combo("", &currentType, "All\0Visible\0Invisible\0");
			ImGui::PopID();
			currentItem = currentCategory * 3 + currentType;
		}
		else {
			currentItem = currentCategory + 8;
		}

		ImGui::SameLine();
		static auto material{ 1 };
		//ImGui::InputInt("##mat", &material, 1, 2);
		material = std::clamp(material, 1, 2);
		ImGui::SameLine();
		auto& colouredbody{ config.colouredbody[currentItem].materials[material - 1] };

		ImGui::Checkbox("Enabled", &colouredbody.enabled);
		ImGui::Separator();
		ImGui::Checkbox("Health Based", &colouredbody.healthBased);
		ImGui::Checkbox("Blinking", &colouredbody.blinking);
		ImGui::Combo("Material", &colouredbody.material, "Normal\0Flat\0Animated\0Platinum\0Glass\0Chrome\0Crystal\0Silver\0Gold\0Plastic\0");
		ImGui::Checkbox("Wireframe", &colouredbody.wireframe);
		ImGuiCustom::colorPicker("Color", colouredbody.color.color, nullptr, &colouredbody.color.rainbow, &colouredbody.color.rainbowSpeed);

		//ImGui::PopFont();

		if (!config.style.menuStyle) {
			ImGui::End();
		}
	}
}

void GUI::renderespWindow() noexcept
{
	if (window.esp) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 0.0f, 0.0f });
			ImGui::Begin("esp", &window.esp, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		if (ImGui::Button("Main", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = true;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Chams", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = true;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Glow", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = true;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("ESP", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = true;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}


		//ImGui::NewLine();

		static int currentCategory = 0;
		static int currentItem = 0;

		if (ImGui::ListBoxHeader("##", { 135.0f, 410.0f })) {
			static constexpr const char* players[]{ "All", "Visible", "Invisible" };

			ImGui::Text("Allies");
			ImGui::Indent();
			ImGui::PushID("Allies");
			ImGui::PushFont(fonts.segoeui);

			for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
				bool isSelected = currentCategory == 0 && currentItem == i;

				if ((i == 0 || !config.esp.players[0].enabled) && ImGui::Selectable(players[i], isSelected)) {
					currentItem = i;
					currentCategory = 0;
				}
			}

			ImGui::PopFont();
			ImGui::PopID();
			ImGui::Unindent();
			ImGui::Text("Enemies");
			ImGui::Indent();
			ImGui::PushID("Enemies");
			ImGui::PushFont(fonts.segoeui);

			for (int i = 0; i < IM_ARRAYSIZE(players); i++) {
				bool isSelected = currentCategory == 1 && currentItem == i;

				if ((i == 0 || !config.esp.players[3].enabled) && ImGui::Selectable(players[i], isSelected)) {
					currentItem = i;
					currentCategory = 1;
				}
			}

			ImGui::PopFont();
			ImGui::PopID();
			ImGui::Unindent();
			if (bool isSelected = currentCategory == 2; ImGui::Selectable("Weapons", isSelected))
				currentCategory = 2;

			ImGui::Text("Projectiles");
			ImGui::Indent();
			ImGui::PushID("Projectiles");
			ImGui::PushFont(fonts.segoeui);
			static constexpr const char* projectiles[]{ "Flashbang", "HE Grenade", "Breach Charge", "Bump Mine", "Decoy Grenade", "Molotov", "TA Grenade", "Smoke Grenade", "Snowball" };

			for (int i = 0; i < IM_ARRAYSIZE(projectiles); i++) {
				bool isSelected = currentCategory == 3 && currentItem == i;

				if (ImGui::Selectable(projectiles[i], isSelected)) {
					currentItem = i;
					currentCategory = 3;
				}
			}

			ImGui::PopFont();
			ImGui::PopID();
			ImGui::Unindent();

			ImGui::Text("Danger Zone");
			ImGui::Indent();
			ImGui::PushID("Danger Zone");
			ImGui::PushFont(fonts.segoeui);
			static constexpr const char* dangerZone[]{ "Sentries", "Drones", "Cash", "Cash Dufflebag", "Pistol Case", "Light Case", "Heavy Case", "Explosive Case", "Tools Case", "Full Armor", "Armor", "Helmet", "Parachute", "Briefcase", "Tablet Upgrade", "ExoJump", "Ammobox", "Radar Jammer" };

			for (int i = 0; i < IM_ARRAYSIZE(dangerZone); i++) {
				bool isSelected = currentCategory == 4 && currentItem == i;

				if (ImGui::Selectable(dangerZone[i], isSelected)) {
					currentItem = i;
					currentCategory = 4;
				}
			}

			//ImGui::PopFont();
			ImGui::PopID();
			ImGui::ListBoxFooter();
		}
		ImGui::SameLine();
		if (ImGui::BeginChild("##child", { 400.0f, 0.0f })) {
			switch (currentCategory) {
			case 0:
			case 1: {
				int selected = currentCategory * 3 + currentItem;
				ImGui::Checkbox("Enabled", &config.esp.players[selected].enabled);
				ImGui::SameLine(0.0f, 50.0f);
				ImGui::SetNextItemWidth(85.0f);
				ImGui::InputInt("Font", &config.esp.players[selected].font, 1, 294);
				config.esp.players[selected].font = std::clamp(config.esp.players[selected].font, 1, 294);

				ImGui::Separator();

				constexpr auto spacing{ 200.0f };
				ImGuiCustom::colorPicker("Snaplines", config.esp.players[selected].snaplines);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Box", config.esp.players[selected].box);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(95.0f);
				ImGui::Combo("", &config.esp.players[selected].boxType, "2D\0""2D Corners\0""3D\0""3D Corners\0");
				ImGuiCustom::colorPicker("Eye Traces", config.esp.players[selected].eyeTraces);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Health", config.esp.players[selected].health);
				ImGuiCustom::colorPicker("Head Dot", config.esp.players[selected].headDot);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Health Bar", config.esp.players[selected].healthBar);
				ImGuiCustom::colorPicker("Name", config.esp.players[selected].name);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Armor", config.esp.players[selected].armor);
				ImGuiCustom::colorPicker("Money", config.esp.players[selected].money);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Armor Bar", config.esp.players[selected].armorBar);
				//ImGuiCustom::colorPicker("Outline", config.esp.players[selected].outline);
				//ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Distance", config.esp.players[selected].distance);
				ImGuiCustom::colorPicker("Active Weapon", config.esp.players[selected].activeWeapon);
				ImGui::SliderFloat("Max Distance", &config.esp.players[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
				break;
			}
			case 2: {
				ImGui::Checkbox("Enabled", &config.esp.weapon.enabled);
				ImGui::SameLine(0.0f, 50.0f);
				ImGui::SetNextItemWidth(85.0f);
				ImGui::InputInt("Font", &config.esp.weapon.font, 1, 294);
				config.esp.weapon.font = std::clamp(config.esp.weapon.font, 1, 294);

				ImGui::Separator();

				constexpr auto spacing{ 200.0f };
				ImGuiCustom::colorPicker("Snaplines", config.esp.weapon.snaplines);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Box", config.esp.weapon.box);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(95.0f);
				ImGui::Combo("", &config.esp.weapon.boxType, "2D\0""2D Corners\0""3D\0""3D Corners\0");
				ImGuiCustom::colorPicker("Name", config.esp.weapon.name);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Outline", config.esp.weapon.outline);
				ImGuiCustom::colorPicker("Distance", config.esp.weapon.distance);
				ImGui::SliderFloat("Max Distance", &config.esp.weapon.maxDistance, 0.0f, 200.0f, "%.2fm");
				break;
			}
			case 3: {
				ImGui::Checkbox("Enabled", &config.esp.projectiles[currentItem].enabled);
				ImGui::SameLine(0.0f, 50.0f);
				ImGui::SetNextItemWidth(85.0f);
				ImGui::InputInt("Font", &config.esp.projectiles[currentItem].font, 1, 294);
				config.esp.projectiles[currentItem].font = std::clamp(config.esp.projectiles[currentItem].font, 1, 294);

				ImGui::Separator();

				constexpr auto spacing{ 200.0f };
				ImGuiCustom::colorPicker("Snaplines", config.esp.projectiles[currentItem].snaplines);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Box", config.esp.projectiles[currentItem].box);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(95.0f);
				ImGui::Combo("", &config.esp.projectiles[currentItem].boxType, "2D\0""2D Corners\0""3D\0""3D Corners\0");
				ImGuiCustom::colorPicker("Name", config.esp.projectiles[currentItem].name);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Outline", config.esp.projectiles[currentItem].outline);
				ImGuiCustom::colorPicker("Distance", config.esp.projectiles[currentItem].distance);
				ImGui::SliderFloat("Max Distance", &config.esp.projectiles[currentItem].maxDistance, 0.0f, 200.0f, "%.2fm");
				break;
			}
			case 4: {
				int selected = currentItem;
				ImGui::Checkbox("Enabled", &config.esp.dangerZone[selected].enabled);
				ImGui::SameLine(0.0f, 50.0f);
				ImGui::SetNextItemWidth(85.0f);
				ImGui::InputInt("Font", &config.esp.dangerZone[selected].font, 1, 294);
				config.esp.dangerZone[selected].font = std::clamp(config.esp.dangerZone[selected].font, 1, 294);

				ImGui::Separator();

				constexpr auto spacing{ 200.0f };
				ImGuiCustom::colorPicker("Snaplines", config.esp.dangerZone[selected].snaplines);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Box", config.esp.dangerZone[selected].box);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(95.0f);
				ImGui::Combo("", &config.esp.dangerZone[selected].boxType, "2D\0""2D Corners\0""3D\0""3D Corners\0");
				ImGuiCustom::colorPicker("Name", config.esp.dangerZone[selected].name);
				ImGui::SameLine(spacing);
				ImGuiCustom::colorPicker("Outline", config.esp.dangerZone[selected].outline);
				ImGuiCustom::colorPicker("Distance", config.esp.dangerZone[selected].distance);
				ImGui::Text("");
				ImGui::SliderFloat("Max Distance", &config.esp.dangerZone[selected].maxDistance, 0.0f, 200.0f, "%.2fm");
				break;
			}
			}

			ImGui::EndChild();
		}

		//ImGui::PopFont();

		if (!config.style.menuStyle)
			ImGui::End();
	}
}

void GUI::renderVisualsWindow() noexcept
{
	if (window.visuals) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 520.0f, 0.0f });
			ImGui::Begin("Visuals", &window.visuals, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		if (ImGui::Button("Main", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = true;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Chams", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = true;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Glow", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = true;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("ESP", ImVec2(146, 22)))
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = true;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}

		
		

		//ImGui::NewLine();

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnOffset(1, 210.0f);

		//ImGui::SameLine();
		ImGui::Checkbox("Disable Post-Processing", &config.visuals.disablePostProcessing);
		ImGui::Checkbox("No Fog", &config.visuals.noFog);
		//ImGui::Checkbox("No 3D Sky", &config.visuals.no3dSky);
		ImGui::Checkbox("No Aim Punch", &config.visuals.noAimPunch);
		//ImGui::Checkbox("No View Punch", &config.visuals.noViewPunch);
		ImGui::Checkbox("No Hands", &config.visuals.noHands);
		ImGui::Checkbox("No Sleeves", &config.visuals.noSleeves);
		ImGui::Checkbox("No Weapons", &config.visuals.noWeapons);
		ImGui::Checkbox("No Smoke", &config.visuals.noSmoke);
		ImGui::Checkbox("Wireframe Smoke", &config.visuals.wireframeSmoke);
		ImGui::Checkbox("No Blur", &config.visuals.noBlur);
		ImGui::Checkbox("No Scope overlay", &config.visuals.noScopeOverlay);
		//ImGui::Checkbox("No Grass", &config.visuals.noGrass);
		//ImGui::Checkbox("No Shadows", &config.visuals.noShadows);
		ImGui::NextColumn();
		ImGui::Checkbox("Zoom", &config.visuals.zoom);
		ImGui::SameLine();
		hotkey(config.visuals.zoomKey);
		ImGui::Checkbox("Thirdperson", &config.visuals.thirdperson);
		ImGui::SameLine();
		hotkey(config.visuals.thirdpersonKey);
		ImGui::PushItemWidth(290.0f);
		ImGui::PushID(0);
		ImGui::SliderInt("", &config.visuals.thirdpersonDistance, 0, 530, "Thirdperson Distance: %d");
		ImGui::PopID();
		ImGui::PushID(1);
		ImGui::SliderInt("", &config.visuals.viewmodelFov, -60, 60, "Viewmodel FOV: %d");
		ImGui::PopID();
		ImGui::PushID(2);
		ImGui::SliderInt("", &config.visuals.fov, -60, 60, "FOV: %d");
		ImGui::PopID();
		ImGui::PushID(3);
		ImGui::SliderInt("", &config.visuals.farZ, 0, 2000, "Far Z: %d");
		ImGui::PopID();
		ImGui::PushID(4);
		ImGui::SliderInt("", &config.visuals.flashReduction, 0, 100, "Flash Reduction: %d%%");
		ImGui::PopID();
		ImGui::PushID(5);
		ImGui::SliderFloat("", &config.visuals.brightness, 0.0f, 1.0f, "Brightness: %.2f");
		ImGui::PopID();
		ImGui::PopItemWidth();
		ImGui::Combo("Skybox", &config.visuals.skybox, "Default\0cs_baggage_skybox_\0cs_tibet\0embassy\0italy\0jungle\0nukeblank\0office\0sky_cs15_daylight01_hdr\0sky_cs15_daylight02_hdr\0sky_cs15_daylight03_hdr\0sky_cs15_daylight04_hdr\0sky_csgo_cloudy01\0sky_csgo_night_flat\0sky_csgo_night02\0sky_day02_05_hdr\0sky_day02_05\0sky_dust\0sky_l4d_rural02_ldr\0sky_venice\0vertigo_hdr\0vertigo\0vertigoblue_hdr\0vietnam\0");
		ImGuiCustom::colorPicker("World Color", config.visuals.world.color, nullptr, &config.visuals.world.rainbow, &config.visuals.world.rainbowSpeed);
		//ImGui::Checkbox("Deagle Spinner", &config.visuals.deaglespinner);
		//ImGui::Combo("Screen Effect", &config.visuals.screenEffect, "None\0Drone Cam\0Drone Cam (Noise)\0Underwater\0Healthboost\0Dangerzone\0");
		ImGui::Combo("Hit Marker", &config.visuals.hitMarker, "None\0Drone Cam\0Drone Cam (Noise)\0Underwater\0Healthboost\0Dangerzone\0");
		ImGui::SliderFloat("Hit Marker Time", &config.visuals.hitMarkerTime, 0.1f, 1.5f, "%.2fs");
		ImGui::Columns(1);

		//ImGui::PopFont();

		if (!config.style.menuStyle)
			ImGui::End();
	}	
}

void GUI::renderSkinChangerWindow() noexcept
{
	if (window.skinChanger) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 700.0f, 0.0f });
			ImGui::Begin("Skins", &window.skinChanger, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		static auto itemIndex = 0;

		ImGui::PushItemWidth(110.0f);
		ImGui::Combo("##1", &itemIndex, [](void* data, int idx, const char** out_text) {
			*out_text = game_data::weapon_names[idx].name;
			return true;
			}, nullptr, IM_ARRAYSIZE(game_data::weapon_names), 5);
		ImGui::PopItemWidth();

		auto& selected_entry = config.SkinChanger[itemIndex];
		selected_entry.definition_vector_index = itemIndex;

		{
			ImGui::SameLine();
			ImGui::Checkbox("Enabled", &selected_entry.enabled);
			ImGui::NewLine();
			ImGui::Columns(2, nullptr, false);
			ImGui::InputInt("Seed", &selected_entry.seed);
			ImGui::InputInt("StatTrak", &selected_entry.stat_trak);
			ImGui::Text("");
			ImGui::SliderFloat("Wear", &selected_entry.wear, FLT_MIN, 1.f, "%.10f", 5);

			ImGui::NextColumn();

			ImGui::Combo("Paint Kit", &selected_entry.paint_kit_vector_index, [](void* data, int idx, const char** out_text) {
				*out_text = (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits)[idx].name.c_str();
				return true;
				}, nullptr, (itemIndex == 1 ? SkinChanger::gloveKits : SkinChanger::skinKits).size(), 10);

			ImGui::Combo("Quality", &selected_entry.entity_quality_vector_index, [](void* data, int idx, const char** out_text) {
				*out_text = game_data::quality_names[idx].name;
				return true;
				}, nullptr, IM_ARRAYSIZE(game_data::quality_names), 5);

			if (itemIndex == 0) {
				ImGui::Combo("Knife", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
					*out_text = game_data::knife_names[idx].name;
					return true;
					}, nullptr, IM_ARRAYSIZE(game_data::knife_names), 5);
			}
			else if (itemIndex == 1) {
				ImGui::Combo("Glove", &selected_entry.definition_override_vector_index, [](void* data, int idx, const char** out_text) {
					*out_text = game_data::glove_names[idx].name;
					return true;
					}, nullptr, IM_ARRAYSIZE(game_data::glove_names), 5);
			}
			else {
				static auto unused_value = 0;
				selected_entry.definition_override_vector_index = 0;
				ImGui::Combo("Unavailable", &unused_value, "For knives or gloves\0");
			}

			ImGui::InputText("Name Tag", selected_entry.custom_name, 32);
		}

		selected_entry.update();

		ImGui::Columns(1);


		if (ImGui::Button("Update", { 130.0f, 30.0f }))
			SkinChanger::scheduleHudUpdate();

		//ImGui::PopFont();

		if (!config.style.menuStyle)
			ImGui::End();
	}
}


void GUI::renderMiscWindow() noexcept
{
	if (window.misc) {
		if (!config.style.menuStyle) {
			ImGui::SetNextWindowSize({ 580.0f, 0.0f });
			ImGui::Begin("Misc", &window.misc, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnOffset(1, 230.0f);
		//ImGui::TextUnformatted("Menu Key");
		//ImGui::SameLine();
		//hotkey(config.misc.menuKey);

		ImGui::Checkbox("Auto Strafe", &config.misc.autoStrafe);
		ImGui::Checkbox("Bunny Hop", &config.misc.fastmovementjump);
		ImGui::SliderInt("Hitchance", &config.misc.fastmovementjum_hitchance, 0, 100);
		ImGui::SliderInt("Max Hops", &config.misc.fastmovementjum_maxhops, 0, 20);
		ImGui::SliderInt("Min Hops", &config.misc.fastmovementjum_minhops, 0, 20);
		ImGui::SliderInt("Restricted", &config.misc.fastmovementjum_restricted, 0, 5);
		ImGui::Checkbox("Infinate Duck", &config.misc.fastDuck);
		ImGui::Checkbox("Slide Walk", &config.misc.spacemove);
		ImGui::Checkbox("Sniper Crosshair", &config.misc.scopegunpoint);
		ImGui::Checkbox("Recoil Crosshair", &config.misc.gunmovepoint);
		ImGui::Checkbox("Auto Pistol", &config.misc.autoPistol);
		ImGui::Checkbox("Auto Reload", &config.misc.autoReload);
		ImGui::Checkbox("Auto Accept", &config.misc.autoAccept);
		ImGui::NextColumn();
		ImGui::Checkbox("Reveal Radar", &config.misc.radarHack);
		ImGui::Checkbox("Reveal Ranks", &config.misc.revealRanks);
		ImGui::Checkbox("Reveal Money", &config.misc.revealMoney);
		ImGui::Checkbox("Watermark", &config.misc.watermark);
		ImGui::Checkbox("Disable Model Occlusion", &config.misc.disableModelOcclusion);
		ImGui::Checkbox("Disable HUD Blur", &config.misc.disablePanoramablur);
		ImGui::Checkbox("Clan Tag", &config.misc.useClanTag);
		/*
		ImGui::Checkbox("Custom Clantag", &config.misc.customClanTag);						//todo:	FIX CLANTAGS
		ImGui::SameLine();
		ImGui::PushItemWidth(120.0f);
		ImGui::PushID(0);
		if (ImGui::InputText("", config.misc.clanTag, IM_ARRAYSIZE(config.misc.clanTag)))  // Half high while coding
			Misc::updateClanTag(true);*/
		ImGui::PopID();
		ImGui::Checkbox("Kill Say", &config.misc.killMessage);
		ImGui::SameLine();
		ImGui::PushItemWidth(120.0f);
		ImGui::PushID(1);
		ImGui::InputText("", config.misc.killMessageString, IM_ARRAYSIZE(config.misc.killMessageString));
		ImGui::PopID();
		ImGui::PushID(2);
		ImGui::PopID();
		//ImGui::SameLine();
		
		//ImGui::Checkbox("Fast Plant", &config.misc.fastPlant);
		ImGuiCustom::colorPicker("Bomb Timer", config.misc.bombTimer);
		ImGui::Combo("Hit Sound", &config.misc.hitSound, "None\0Metal\0Gamesense\0Bell\0Glass\0");
		ImGui::SetNextItemWidth(90.0f);
		//ImGui::InputInt("Choked Packets", &config.misc.chokedPackets, 1, 5);
		//config.misc.chokedPackets = std::clamp(config.misc.chokedPackets, 0, 64);
		//ImGui::SameLine();
		//hotkey(config.misc.chokedPacketsKey);
		ImGui::Checkbox("Grenade Prediction", &config.misc.nadePredict);
		ImGui::Checkbox("Grenade Trajectory", &config.misc.nadeTrajectory);
		ImGui::SetNextItemWidth(120.0f);

		if (ImGui::Button("Unhook"))
			hooks.restore();

		//ImGui::PopFont();

		ImGui::Columns(1);
		if (!config.style.menuStyle)
			ImGui::End();
	}
}

void GUI::renderConfigWindow() noexcept
{
	if (window.config) {
		const auto menuStyle{ config.style.menuStyle };
		if (true == false) {
			ImGui::SetNextWindowSize({ 290.0f, 190.0f });
			ImGui::Begin("Config", &window.config, windowFlags);
		}

		//ImGui::PushFont(fonts.tahoma);

		ImGui::Columns(2, nullptr, false);
		ImGui::SetColumnOffset(1, 170.0f);

		ImGui::PushItemWidth(160.0f);

		constexpr auto& configItems = config.getConfigs();
		static int currentConfig = -1;

		if (static_cast<size_t>(currentConfig) >= configItems.size())
			currentConfig = -1;

		static char buffer[16];

		if (ImGui::ListBox("", &currentConfig, [](void* data, int idx, const char** out_text) {
			auto& vector = *static_cast<std::vector<std::string>*>(data);
			*out_text = vector[idx].c_str();
			return true;
			}, &configItems, configItems.size(), 5) && currentConfig != -1)
			strcpy(buffer, configItems[currentConfig].c_str());

			ImGui::PushID(0);
			if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
				if (currentConfig != -1)
					config.rename(currentConfig, buffer);
			}
			ImGui::PopID();
			ImGui::NextColumn();

			ImGui::PushItemWidth(100.0f);

			if (ImGui::Button("New", { 100.0f, 25.0f }))
				config.add(buffer);

			if (currentConfig != -1) {
				if (ImGui::Button("Load", { 100.0f, 25.0f })) {
					config.load(currentConfig);
					updateColors();
					SkinChanger::scheduleHudUpdate();
					Misc::updateClanTag(true);
				}
				if (ImGui::Button("Save", { 100.0f, 25.0f }))
					config.save(currentConfig);
				ImGui::Text("");
				if (ImGui::Button("Delete", { 100.0f, 25.0f }))
					config.remove(currentConfig);
			}
			ImGui::Columns(1);

			//ImGui::PopFont();

			if (menuStyle == 0)
				ImGui::End();
	}
}

void GUI::renderGuiStyle2() noexcept
{
	ImGui::SetNextWindowSize({ 625.0f, 495.0f });
	ImGui::Begin("Brisk", nullptr, windowFlags | ImGuiWindowFlags_NoTitleBar);
	{
		
		
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x / 2) - (ImGui::CalcTextSize("Brisk.club").x / 2)); //Center the text
		ImGui::Text("Brisk.club");
		ImGui::Spacing();
		ImGui::Separator();

		ImGui::PushFont(fonts.tabfont);

		if (ImGui::Button("A", ImVec2(95, 38))) //Aimbot //A
		{
			window.aimhelper = true;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("F", ImVec2(95, 38))) //Trigger //F
		{
			window.aimhelper = 0;
			window.fastshotbot = true;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("B",ImVec2(95, 38))) //Visuals //B
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = true;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("M", ImVec2(95, 38))) //Skins //M
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = true;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("C", ImVec2(95, 38))) //Misc //C
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = true;
			window.skinChanger = 0;
			window.config = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("D", ImVec2(95, 38))) //Config //D
		{
			window.aimhelper = 0;
			window.fastshotbot = 0;
			window.visuals = 0;
			window.colouredbody = 0;
			window.bodyoutline = 0;
			window.esp = 0;
			window.misc = 0;
			window.skinChanger = 0;
			window.config = true;
		}
		ImGui::Separator();
		ImGui::PopFont();

		//ImGui::PushFont(fonts.tahoma);


		//if (ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_Reorderable)) {

			/*if (ImGui::BeginTabItem("Triggerbot")) {
				window = { };
				window.fastshotbot = true;
				ImGui::EndTabItem();
			}*/
			/*if (ImGui::BeginTabItem("Glow")) {
				window = { };
				window.bodyoutline = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Chams")) {
				window = { };
				window.colouredbody = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("ESP")) {
				window = { };
				window.esp = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Visuals")) {
				window = { };
				window.visuals = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Skins")) {
				window = { };
				window.skinChanger = true;
				ImGui::EndTabItem();
			}
			/*if (ImGui::BeginTabItem("Style")) {
				window = { };
				window.style = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Misc")) {
				window = { };
				window.misc = true;
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Config")) {
				window = { };
				window.config = true;
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();*/
			//}

		

		renderAimbotWindow();
		renderfastshotbotWindow();
		renderbodyoutlineWindow();
		rendercolouredbodyWindow();
		renderespWindow();
		renderVisualsWindow();
		renderSkinChangerWindow();
		renderMiscWindow();
		renderConfigWindow();

		ImGui::End();
	}

	
}
