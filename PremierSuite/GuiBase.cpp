﻿#include "pch.h"
#include "PremierSuite.h"
#include "imgui_custom.hpp"

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

std::string PremierSuite::GetPluginName()
{
	return "PremierSuite";
}

std::string PremierSuite::GetMenuName()
{
	return "PremierSuite";
}

std::string PremierSuite::GetMenuTitle()
{
	return menuTitle_;
}

void PremierSuite::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

bool PremierSuite::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool PremierSuite::IsActiveOverlay()
{
	return true;
}

void PremierSuite::OnOpen()
{
	isWindowOpen_ = true;
}

void PremierSuite::OnClose()
{
	isWindowOpen_ = false;
}

void PremierSuite::RenderSettings()
{
	ImGui::BeginTabBar("#TabBarSettings", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_FittingPolicyResizeDown);
	ImGui::Indent(5);
	ImGui::Spacing();
	renderSettingsTab();
	renderKeybindsTab();
	ImGui::EndTabBar();
}

void PremierSuite::Render()
{
	ImGui::SetNextWindowSize(ImVec2(450, 555));
	if (ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize)) {

		if (ImGui::BeginMenuBar()) {
			renderMenu();
			ImGui::EndMenuBar();
		}

		ImGui::BeginTabBar("#TabBarWindow", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_FittingPolicyResizeDown);
		ImGui::Indent(5);
		ImGui::Spacing();
		renderSettingsTab();
		renderKeybindsTab();
		ImGui::EndTabBar();
		ImGui::End();
	}
	ImGui::End();

	if (!isWindowOpen_) {
		cvarManager->executeCommand("togglemenu PremierSuite");
		return;
	}
}

/// <summary> Renders main menu. </summary>
void PremierSuite::renderMenu()
{
	static bool show_about_window = false;

	// Dear ImGui Apps (accessible from the "Tools" menu)
	static bool show_app_metrics = false;
	static bool show_app_style_editor = false;
	static bool show_app_about = false;

	ImGuiStyle& style = ImGui::GetStyle();
	if (show_app_about) { renderAboutWindow(&show_app_about); }
	if (ImGui::BeginMenu("About"))
	{
		ImGui::MenuItem("About PremierSuite", NULL, &show_app_about);
		ImGui::EndMenu();
	}
	ImGui::Indent(5);
	ImGui::Spacing();
}

/// <summary> Renders keybinds tab for changing GUI keybinds. </summary>
void PremierSuite::renderKeybindsTab()
{
	if (ImGui::BeginTabItem("Keybinds")) {

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			ImGui::Text(
				"Easily Change keybinds to whatever button you'd like.\n"
			);

			ImGui::Separator();
			ImGui::Indent(5);

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			ImGui::SetNextItemWidth(long_width / 2);

			char keybindInput[128] = "";
			if (ImGui::InputTextWithHint("##KeybindInputText", "Keybind", keybindInput, IM_ARRAYSIZE(keybindInput), ImGuiInputTextFlags_CharsNoBlank)) {
				*keybindHolder = keybindInput;
			}

			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Type out desired keybind.\n"
					"Use capital first letter, lowercase next letters, PascalCase for compound words. \n\n"
					"i.e. Home, End, NumPadFive, NumPadOne, LeftAlt. \n\n"
					"NumPad +, -, * are just Add, Subtract, Multiply. "
				);

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			ImGui::Text("Open GUI");
			ImGui::SameLine(150);

			if (ImGui::Button("set##PSGUISetter"))
			{
				std::string unbind = "unbind " + *gui_keybind + " " + "ps_gui";
				std::string bind = "bind " + *keybindHolder + " " + "ps_gui";

				_globalCvarManager->executeCommand(unbind);
				_globalCvarManager->executeCommand(bind);
				setNewGUIKeybind(*keybindHolder);
				_globalCvarManager->executeCommand("writeconfig", true);
			}

			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Set keybind to open/close the GUI.\n"
					"Avoid F2 (Bakkesmod) and F6 (Bakkesmod Console."
				);

			ImGui::SameLine();
			ImGui::Text("Bound key: % s\n", *gui_keybind);

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			if (*plugin_keybind == "") {
				setNewPluginKeybind("Unset");
			}

			ImGui::Text("Toggle Plugin");
			ImGui::SameLine(150);
			if (ImGui::Button("set##CHANGEENABLEDSETTER"))
			{
				_globalCvarManager->executeCommand("unbind " + *plugin_keybind + " change_ps_enabled");
				_globalCvarManager->executeCommand("bind " + *keybindHolder + " change_ps_enabled");
				_globalCvarManager->executeCommand("writeconfig", true);
				setNewPluginKeybind(*keybindHolder);
			}
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Set keybind to immediately enable/disable the plugin.\n"
					"Avoid F2 (Bakkesmod) and F6 (Bakkesmod Console)."
				);
			ImGui::SameLine();
			ImGui::Text("Bound key: % s\n", *plugin_keybind);

			ImGui::Dummy(ImVec2(0.0f, 10.0f));

			ImGui::EndTabItem();
	}
}

/// <summary> Renders main GUI settings. </summary>
void PremierSuite::renderSettingsTab()
{
	if (ImGui::BeginTabItem("Settings"))
	{
		ImGui::Spacing();
		ImGui::Text(
			"PremierSuite:\n"
			"Instant Access to Auto-Queue, Freeplay, and Custom Training."
		);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		//-----------------------------------------------------------------------------
		// Plugin Options
		//-----------------------------------------------------------------------------
		{
			ImGui::Text("Plugin Options");
			ImGui::SameLine();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (ImGui::ToggleButton("##ToggleButtonPlugin", &*enabled))
			{
				setEnablePlugin(enabled);
				_globalCvarManager->executeCommand("writeconfig", false);
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Enable/Disable the plugin and all of its functionality.\n"
					"You can also bind this to a key in the Keybinds tab!.\n");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (!*enabled) {
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
			}

			if (ImGui::Checkbox("Disable Plugin for Private Matches", &*disablePrivate)) {
				setDisablePrivate(disablePrivate);
				_globalCvarManager->executeCommand("writeconfig", false);
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("When this box is checked, the plugin will deactivate during private matches.");
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		//-----------------------------------------------------------------------------
		// Auto-Queue Options
		//-----------------------------------------------------------------------------
		{
			ImGui::Text("Auto-Queue");
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Auto queue immediately, or with delay.\n"
					"This will queue the same game-mode or modes you were previously queueing."
				);

			ImGui::SameLine(150);

			ImGui::SetNextItemWidth(long_width);
			if (ImGui::SliderFloat("##DelayQueueFloatSlider", &*delayQueue, 0.0f, 10.0f, "Delay: %.1f s")) {
				setDelayQueue(delayQueue);
				_globalCvarManager->executeCommand("writeconfig", false);
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Set a delay for the auto-queue.");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (ImGui::Checkbox("Enable##EnableQueue", &*enableQueue)) {
				setEnableQueue(enableQueue);
				_globalCvarManager->executeCommand("writeconfig", false);
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Automatically queue when an online match has ended.");
			
			if (ImGui::Checkbox("Disable for Casual##DisableQueueCasual", &*disableQueueCasual)) {
				setDisableQueueCasual(disableQueueCasual);
				_globalCvarManager->executeCommand("writeconfig", false);
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Disable the automatic queue for casual games.");
		}

		ImGui::Dummy(ImVec2(0.0f, 5.0f));
		ImGui::Separator();
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		//-----------------------------------------------------------------------------
		// Auto-Exit Options
		//-----------------------------------------------------------------------------
		{
			ImGui::Text("Auto-Exit");
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Exit to desired location immediately, or with delay.\n"
					"Current options: Main-Menu, Freeplay, Custom Training Pack.\n"
					"Workshops, private game modes and specialty game modes are on the way.\n\n"
					"*If multiple exit-options are enabled, the uppmost most selection will be executed.");


			ImGui::SameLine(150);
			ImGui::SetNextItemWidth(long_width);
			if (ImGui::SliderFloat("##exitDelayTimeID", &*delayExit, 0.0f, 10.0f, "Delay: %.1f s")) {
				setDelayExit(delayExit);
			}
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Set a delay for auto-exit options (options are below).");

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			/// Freeplay ----------------------------------------
			{
				if (ImGui::Checkbox("Freeplay##FreeplayCheckbox", &*freeplayEnabled)) {
					setEnableFreeplay(freeplayEnabled);
					_globalCvarManager->executeCommand("writeconfig", false);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Instant-exit to Freeplay.\n");
				ImGui::SameLine(150);

				std::string currMap = *freeplayMap;
				const char* currMapChr = currMap.c_str();
				int index = std::distance(freeplayMaps.begin(), std::find(freeplayMaps.begin(), freeplayMaps.end(), currMap));
			
				ImGui::SetNextItemWidth(long_width);
				if (ImGui::SearchableCombo("##FPCombo", &index, freeplayMaps, "no maps found", "type to search"))
				{
					setFreeplayMap(freeplayMaps[index]);
					ImGui::EndCombo();
				}
	
			}
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			/// Custom Training ----------------------------------------
			{
				if (ImGui::Checkbox("Custom Training##CustomTrainingCheckbox", &*customEnabled)) {
					setEnableCustomTraining(customEnabled);
					_globalCvarManager->executeCommand("writeconfig", false);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Instant-exit to Custom Training.\n");
				ImGui::SameLine(150);

				std::string str0 = std::string();
				std::string c = *customCode;
				ImGuiInputTextFlags ctflags = ImGuiInputTextFlags_CharsUppercase | ImGuiInputTextFlags_EnterReturnsTrue;
				ImGui::SetNextItemWidth(long_width);

				bool ctInput = ImGui::InputTextWithHint("##CustomTrainingInputID", c.c_str(), &str0, ctflags);

				if (ctInput) {
					setCustomTrainingCode(str0);
					str0 = std::string();
					_globalCvarManager->executeCommand("writeconfig", false);
					ImGui::SetItemDefaultFocus();
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Training pack code here, ENTER to apply.\n"
						"The default is A0FE-F860-967D-E628: GC Balls to Master.\n"
						"This is one I made.  :) \n\n"
						"...its hard"
					);
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			/// Workshop Maps ----------------------------------------
			{
				if (!*isOnSteam) {
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Checkbox("Workshop##WorkshopEnabledCheckbox", &*workshopEnabled)) {
					setEnableWorkshop(workshopEnabled);
					_globalCvarManager->executeCommand("writeconfig", false);
				}

				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Instantly load into a workshop map.\n");
				ImGui::SameLine(150);

				std::string currentWorkshopMap = *workshopMap;
				const char* workshop_selection = currentWorkshopMap.c_str();
				int workshop_index = std::distance(workshopMapNames.begin(), std::find(workshopMapNames.begin(), workshopMapNames.end(), currentWorkshopMap));
				
				ImGui::SetNextItemWidth(long_width);
				if (ImGui::SearchableCombo("##WorkshopComboID", &workshop_index, workshopMapNames, "no maps selected", "type to search"))
				{
					setWorkshopMap(workshopMapNames[workshop_index]);
					setEnableWorkshop(workshopEnabled);
					ImGui::EndCombo();
				}

				if (!*isOnSteam) {
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Workshops incoming for epic.\n");
				}
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			/// Main Menu ----------------------------------------
			{
				if (ImGui::Checkbox("Main Menu", &*exitEnabled)) {
					setEnableExit(exitEnabled);
					_globalCvarManager->executeCommand("writeconfig", false);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Instant-exit to Main-Menu.\n");
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			/// Disable Exit for Casual
			{
				if (ImGui::Checkbox("Disable for Casual##DisableExitCasual", &*disableExitCasual)) {
					setDisableExitCasual(disableExitCasual);
					_globalCvarManager->executeCommand("writeconfig", false);
				}
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Disable the all automatic-exit settings for casual matches..");
			}
			ImGui::Dummy(ImVec2(0.0f, 5.0f));

			if (!*enabled) {
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
		}
		ImGui::EndTabItem();
	}
}

/// <summary> Renders about window. </summary>
void PremierSuite::renderAboutWindow(bool* open)
{
	if (!ImGui::Begin("About PremierSuite", open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}
	ImGui::Separator();
	ImGui::Text("PremierSuite: Made by Neuro with help Xenobyte and many BakkesMod Plugin Developers.");
	ImGui::Text("Contact me with bugs or ideas on discord: Neuro#0618");
	ImGui::Text("This plugin was started by and an extension of instantsuite @iamxenobyte: https://xenobyte.dev/ ");
	ImGui::Text("PremierSuite is licensed under the MIT License, see LICENSE for more information.");
	ImGui::Text("Source code for this plugin is located at: https://github.com/NeuroPyPy/PremierSuite");
	ImGui::End();
}
