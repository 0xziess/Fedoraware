#include "Menu.h"

#include "../Vars.h"
#include "../Camera/CameraWindow.h"
#include "../Items/AttributeChanger/AttributeChanger.h"
#include "../Radar/Radar.h"
#include "../Misc/Misc.h"
#include "../Chams/DMEChams.h"
#include "../Glow/Glow.h"
#include "../Killsay/Killsay.h"
#include "../Discord/Discord.h"
#include "../NoSpread/NoSpread.h"

#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx9.h>
#include <ImGui/imgui_stdlib.h>


#include "Fonts/IconsMaterialDesign.h"
#include "Playerlist/Playerlist.h"
#include "MaterialEditor/MaterialEditor.h"
#include "Pong/Pong.h"

#include "Components.hpp"
#include "ConfigManager/ConfigManager.h"

#include <mutex>


#pragma warning (disable : 4309)



int unuPrimary = 0;
int unuSecondary = 0;

constexpr auto SIZE_FULL_WIDTH = ImVec2(-1, 20);

/* The main menu */
void CMenu::DrawMenu()
{
	ImGui::SetNextWindowSize(ImVec2(700, 700), ImGuiCond_FirstUseEver);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 700, 500 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

	if (ImGui::Begin("MainWindow", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar))
	{
		const auto drawList = ImGui::GetWindowDrawList();
		const auto windowSize = ImGui::GetWindowSize();
		const auto windowPos = ImGui::GetWindowPos();

		// Title gradient setup
		{
			TitleGradient.ClearMarks();
			TitleGradient.AddMark(0.f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(0.25f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(0.5f, ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value));
			TitleGradient.AddMark(0.75f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
			TitleGradient.AddMark(1.0f, ImGui::ColorToVec(Color_t{ 0,0,0,0 }));
		}
		ImGui::GradientRect(&TitleGradient, { windowPos.x, windowPos.y }, windowSize.x, 3.f);
		ImGui::Dummy({ 0, 2 });

		// Title Text
		{
			ImGui::PushFont(TitleFont);
			const auto titleWidth = ImGui::CalcTextSize(Vars::Menu::CheatName.Value.c_str()).x;
			drawList->AddText(TitleFont, TitleFont->FontSize, { windowPos.x + (windowSize.x / 2) - (titleWidth / 2), windowPos.y }, Accent, Vars::Menu::CheatName.Value.c_str());
			ImGui::PopFont();
		}

		// Icons
		{
			float currentX = windowSize.x;

			// Settings Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_SETTINGS))
			{
				ShowSettings = !ShowSettings;
			}
			ImGui::HelpMarker("Settings");

			// Playerlist Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_PEOPLE))
			{
				Vars::Menu::ShowPlayerlist.Value = !Vars::Menu::ShowPlayerlist.Value;
			}
			ImGui::HelpMarker("Playerlist");

			// Keybinds Icon
			ImGui::SetCursorPos({ currentX -= 25, 0 });
			if (ImGui::IconButton(ICON_MD_KEYBOARD))
			{
				Vars::Menu::ShowKeybinds.Value = !Vars::Menu::ShowKeybinds.Value;
			}
			ImGui::HelpMarker("Keybinds");

		}

		// Tabbar
		ImGui::SetCursorPos({ 0, TitleHeight });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundLight.Value);
		if (ImGui::BeginChild("Tabbar", { windowSize.x + 5, TabHeight + SubTabHeight }, false, ImGuiWindowFlags_NoScrollWithMouse))
		{
			DrawTabbar();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();

		// Main content
		ImGui::SetCursorPos({ 0, TitleHeight + TabHeight + SubTabHeight });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8.f, 10.f });
		ImGui::PushStyleColor(ImGuiCol_ChildBg, BackgroundDark.Value);
		if (ImGui::BeginChild("Content", { windowSize.x, windowSize.y - (TitleHeight + TabHeight + SubTabHeight) }, false, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar))
		{
			ImGui::PushFont(Verdana);
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 3.f, 2.f });

			switch (CurrentTab)
			{
			case MenuTab::Aimbot: { MenuAimbot(); break; }
			case MenuTab::Trigger: { MenuTrigger(); break; }
			case MenuTab::Visuals: { MenuVisuals(); break; }
			case MenuTab::HvH: { MenuHvH(); break; }
			case MenuTab::Misc: { MenuMisc(); break; }
			}

			ImGui::PopStyleVar();
			ImGui::PopFont();
		}
		ImGui::EndChild();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		// Footer
		{
			if (!Vars::Menu::ModernDesign.Value)
			{
				const auto hintHeight = ImGui::CalcTextSize(FeatureHint.c_str()).y;
				drawList->AddText(Verdana, Verdana->FontSize, { windowPos.x + 10, windowPos.y + windowSize.y - (hintHeight + ImGui::GetStyle().ItemInnerSpacing.y) }, TextLight, FeatureHint.c_str());
			}
		}

		// End
		ImGui::End();
	}

	ImGui::PopStyleVar(2);
}

void CMenu::DrawTabbar()
{
	ImGui::PushFont(SectionFont);
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

	if (ImGui::BeginTable("TabbarTable", 5))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
		ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
		if (ImGui::TabButton("Aimbot", CurrentTab == MenuTab::Aimbot))
		{
			CurrentTab = MenuTab::Aimbot;
		}

		if (ImGui::TabButton("Triggerbot", CurrentTab == MenuTab::Trigger))
		{
			CurrentTab = MenuTab::Trigger;
		}

		if (ImGui::TabButton("Visuals", CurrentTab == MenuTab::Visuals))
		{
			CurrentTab = MenuTab::Visuals;
		}

		if (ImGui::TabButton("HvH", CurrentTab == MenuTab::HvH))
		{
			CurrentTab = MenuTab::HvH;
		}

		if (ImGui::TabButton("Misc", CurrentTab == MenuTab::Misc))
		{
			CurrentTab = MenuTab::Misc;
		}

		ImGui::PopStyleColor(2);
		ImGui::EndTable();
	}

	ImGui::SetCursorPosY(TabHeight);
	if (CurrentTab == MenuTab::Visuals)
	{
		SubTabHeight = 30.f;

		if (ImGui::BeginTable("SubbarTable", 6))
		{
			ImGui::PushStyleColor(ImGuiCol_Button, BackgroundLight.Value);
			ImGui::PushStyleColor(ImGuiCol_Text, TextLight.Value);
			if (ImGui::TabButton("Players", CurrentVisualsTab == VisualsTab::Players))
			{
				CurrentVisualsTab = VisualsTab::Players;
			}

			if (ImGui::TabButton("Buildings", CurrentVisualsTab == VisualsTab::Buildings))
			{
				CurrentVisualsTab = VisualsTab::Buildings;
			}

			if (ImGui::TabButton("World", CurrentVisualsTab == VisualsTab::World))
			{
				CurrentVisualsTab = VisualsTab::World;
			}

			if (ImGui::TabButton("Fonts", CurrentVisualsTab == VisualsTab::Font))
			{
				CurrentVisualsTab = VisualsTab::Font;
			}

			if (ImGui::TabButton("Misc", CurrentVisualsTab == VisualsTab::Misc))
			{
				CurrentVisualsTab = VisualsTab::Misc;
			}

			ImGui::PopStyleColor(2);
			ImGui::EndTable();
		}
	}
	else
	{
		SubTabHeight = 0.f;
	}

	ImGui::PopStyleVar(3);
	ImGui::PopFont();
}

#pragma region Tabs
/* Tab: Aimbot */
void CMenu::MenuAimbot()
{
	using namespace ImGui;


	if (BeginTable("AimbotTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("AimbotCol1"))
		{

			SectionTitle("Global");
			WToggle("Aimbot", &Vars::Aimbot::Global::Active.Value); HelpMarker("Aimbot master switch");
			ColorPickerL("Target", Vars::Colours::Target.Value);
			InputKeybind("Aimbot key", Vars::Aimbot::Global::AimKey); HelpMarker("The key to enable aimbot");
			WSlider("Aimbot FoV####AimbotFoV", &Vars::Aimbot::Global::AimFOV.Value, 0.f, 180.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ColorPickerL("Aimbot FOV circle", Vars::Colours::FOVCircle.Value);
			WToggle("Autoshoot###AimbotAutoshoot", &Vars::Aimbot::Global::AutoShoot.Value); HelpMarker("Automatically shoot when a target is found");
			MultiCombo({ "Players", "Buildings", "Stickies", "NPCs", "Bombs" }, { &Vars::Aimbot::Global::AimPlayers.Value, &Vars::Aimbot::Global::AimBuildings.Value, &Vars::Aimbot::Global::AimStickies.Value, &Vars::Aimbot::Global::AimNPC.Value, &Vars::Aimbot::Global::AimBombs.Value }, "Aim targets");
			HelpMarker("Choose which targets the Aimbot should aim at");
			{
				static std::vector flagNames{ "Invulnerable", "Cloaked", "Dead Ringer", "Friends", "Taunting", "Vaccinator", "Unsimulated Players", "Disguised" };
				static std::vector flagValues{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7 };
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Global::IgnoreOptions.Value, "Ignored targets###AimbotIgnoredTargets");
				HelpMarker("Choose which targets should be ignored");
			}
			ColorPickerL("Invulnerable colour", Vars::Colours::Invuln.Value);

			if (Vars::Aimbot::Global::IgnoreOptions.Value & (1 << 6))
			{
				WSlider("Tick Tolerance###AimbotUnsimulatedTolerance", &Vars::Aimbot::Global::TickTolerance.Value, 0, 21, "%d", ImGuiSliderFlags_AlwaysClamp);
			}

			WToggle("Dont wait for shot###AimbotWaitForValidShot", &Vars::Aimbot::Global::DontWaitForShot.Value); HelpMarker("Prevents fps drops by only running aimbot while we are able to shoot");


			SectionTitle("Crits");
			WToggle("Crit hack", &Vars::CritHack::Active.Value);  HelpMarker("Enables the crit hack (BETA)");
			MultiCombo({ "Indicators", "Avoid Random", "Always Melee", "Auto Melee Crit" }, { &Vars::CritHack::Indicators.Value, &Vars::CritHack::AvoidRandom.Value, &Vars::CritHack::AlwaysMelee.Value, &Vars::CritHack::AutoMeleeCrit.Value }, "Misc###CrithackMiscOptions");
			HelpMarker("Misc options for crithack");
			InputKeybind("Crit key", Vars::CritHack::CritKey); HelpMarker("Will try to force crits when the key is held");

			SectionTitle("Backtrack");
			WToggle("Active", &Vars::Backtrack::Enabled.Value); HelpMarker("If you shoot at the backtrack manually it will attempt to hit it");
			WCombo("Backtrack Method###HitscanBacktrackMethod", &Vars::Aimbot::Hitscan::BackTrackMethod.Value, { "All", "Last", "Prefer OnShot" });
			WSlider("Amount of latency###BTLatency", &Vars::Backtrack::Latency.Value, 0, 800, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("This won't work on local servers");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("AimbotCol2"))
		{
			SectionTitle("Hitscan");
			WCombo("Sort method###HitscanSortMethod", &Vars::Aimbot::Hitscan::SortMethod.Value, { "FOV", "Distance" }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
			if (Vars::Aimbot::Hitscan::SortMethod.Value == 1)
			{
				WToggle("Respect FOV", &Vars::Aimbot::Hitscan::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
			}
			WCombo("Aim method###HitscanAimMethod", &Vars::Aimbot::Hitscan::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			WCombo("Preferred Hitbox###HitscanHitbox", &Vars::Aimbot::Hitscan::AimHitbox.Value, { "Head", "Body", "Auto" }); // this could probably be removed entirely since it actually does nothing.
			WCombo("Tapfire###HitscanTapfire", &Vars::Aimbot::Hitscan::TapFire.Value, { "Off", "Distance", "Always" }); HelpMarker("How/If the aimbot chooses to tapfire enemies.");
			if (Vars::Aimbot::Hitscan::TapFire.Value == 1) {
				WSlider("Tap Fire Distance###HitscanTapfireDistance", &Vars::Aimbot::Hitscan::TapFireDist.Value, 64.f, 4096.f, "%.0f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The distance at which tapfire will activate.");
				WToggle("Check for NoSpread state", &Vars::Aimbot::Hitscan::TapFireCheckForNSS.Value); HelpMarker("Turns off Tapfire if NoSpread is synced");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::ScanHitboxes.Value, "Scan Hitboxes###AimbotHitboxScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::MultiHitboxes.Value, "Multipoint Hitboxes###AimbotMultipointScanning");
			}
			{
				static std::vector flagNames{ "Head", "Body", "Pelvis", "Arms", "Legs" };
				static std::vector flagValues{ 0x00000001, 0x00000004, 0x00000002, 0x00000008, 0x00000010 }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
				MultiFlags(flagNames, flagValues, &Vars::Aimbot::Hitscan::StaticHitboxes.Value, "Static Hitboxes###AimbotStaticOnlyScanning");
			}
			WSlider("Point Scale###HitscanMultipointScale", &Vars::Aimbot::Hitscan::PointScale.Value, 0.5f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			WToggle("Preserve Target", &Vars::Aimbot::Hitscan::PreserveTarget.Value); HelpMarker("Prioritises the target that you most recently aimed at.");
			if (Vars::Aimbot::Hitscan::PreserveTarget.Value) {
				WToggle("Preserved Target Ignores FoV", &Vars::Aimbot::Hitscan::IgnorePreservedFoV.Value); HelpMarker("Ignores FoV check for preserved target.");
			}
			WToggle("Wait for headshot", &Vars::Aimbot::Hitscan::WaitForHeadshot.Value); HelpMarker("The aimbot will wait until it can headshot (if applicable)");
			WToggle("Wait for charge", &Vars::Aimbot::Hitscan::WaitForCharge.Value); HelpMarker("The aimbot will wait until the rifle has charged long enough to kill in one shot");
			WToggle("Smooth if spectated", &Vars::Aimbot::Hitscan::SpectatedSmooth.Value); HelpMarker("The aimbot will switch to the smooth method if being spectated");
			WToggle("Scoped only", &Vars::Aimbot::Hitscan::ScopedOnly.Value); HelpMarker("The aimbot will only shoot if scoped");
			WToggle("Auto scope", &Vars::Aimbot::Hitscan::AutoScope.Value); HelpMarker("The aimbot will automatically scope in to shoot");
			WToggle("Auto rev minigun", &Vars::Aimbot::Hitscan::AutoRev.Value); HelpMarker("Will rev heavy's minigun regardless of if aimbot has a target");
			WToggle("Bodyaim if lethal", &Vars::Aimbot::Global::BAimLethal.Value); HelpMarker("The aimbot will aim for body when damage is lethal to it");


			SectionTitle("Melee");
			{
				WToggle("Require Aimbot Bind", &Vars::Aimbot::Melee::RequireBind.Value); HelpMarker("Requires the aimbot keybind to be held in order to run, otherwise it will remain on at all times.");
				WCombo("Sort method###MeleeSortMethod", &Vars::Aimbot::Melee::SortMethod.Value, { "FOV", "Distance", }); HelpMarker("Which method the aimbot uses to decide which target to aim at");
				if (Vars::Aimbot::Melee::SortMethod.Value == 1)
				{
					WToggle("Respect FOV", &Vars::Aimbot::Melee::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###MeleeAimMethod", &Vars::Aimbot::Melee::AimMethod.Value, { "Plain", "Smooth", "Silent" }); HelpMarker("Which method the aimbot uses to aim at the target");
			}
			WSlider("Smooth factor###MeleeSmoothing", &Vars::Aimbot::Melee::SmoothingAmount.Value, 0, 20, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How smooth the aimbot should be");
			WToggle("Range check", &Vars::Aimbot::Melee::RangeCheck.Value); HelpMarker("Only aim at target if within melee range");
			WToggle("Swing prediction", &Vars::Aimbot::Melee::PredictSwing.Value); HelpMarker("Aimbot will attack preemptively, predicting you will be in range of the target");
			//WToggle("Wait for hit", &Vars::Aimbot::Projectile::WaitForHit.Value); HelpMarker("Will avoid shooting until the last shot hits");
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("AimbotCol3"))
		{
			SectionTitle("Projectile");
			WSlider("Prediction Time", &Vars::Aimbot::Projectile::PredictionTime.Value, 0.1f, 10.f, "%.1f");

			{
				WCombo("Sort method###ProjectileSortMethod", &Vars::Aimbot::Projectile::SortMethod.Value, { "FOV", "Distance" });
				if (Vars::Aimbot::Projectile::SortMethod.Value == 1)
				{
					WToggle("Respect FOV", &Vars::Aimbot::Projectile::RespectFOV.Value); HelpMarker("Respect the Aim FOV set when using distance sorting.");
				}
				WCombo("Aim method###ProjectileAimMethod", &Vars::Aimbot::Projectile::AimMethod.Value, { "Plain", "Silent" });
				WCombo("Priority Hitbox###ProjectileHitboxPriority", &Vars::Aimbot::Projectile::AimPosition.Value, { "Head", "Body", "Feet", "Auto" });
				{
					static std::vector flagNames{ "Head", "Body", "Feet" };
					static std::vector flagValues{ (1 << 0), (1 << 1), (1 << 2) }; // 1<<1 and 1<<2 are swapped because the enum for hitboxes is weird.
					MultiFlags(flagNames, flagValues, &Vars::Aimbot::Projectile::AllowedHitboxes.Value, "Allowed Hitboxes###ProjectileHitboxScanning"); HelpMarker("Controls what hitboxes the cheat is allowed to consider shooting at.");
				}
				WSlider("Point VisTest Limit", &Vars::Aimbot::Projectile::VisTestPoints.Value, 3, 15, "%d", ImGuiSliderFlags_AlwaysClamp);	HelpMarker("Controls how many points the cheat is allowed to consider.");
				WSlider("Point Scan Limit", &Vars::Aimbot::Projectile::ScanPoints.Value, 3, Vars::Aimbot::Projectile::VisTestPoints.Value, "%d", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls how many visible points the cheat needs to find before it picks one to aim at.");
				WSlider("Point Scale", &Vars::Aimbot::Projectile::ScanScale.Value, 0.7f, 1.f, "%.2f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("Controls the size of the hitbox as it's given to the cheat.");

				SectionTitle("Preferences");
				WToggle("Predict Obscured Enemies", &Vars::Aimbot::Projectile::PredictObscured.Value); HelpMarker("Will predict enemies that cannot yet be targetted because of a wall etc and shoot if they are predicted to peek (FPS)");
				WToggle("Feet aim on ground", &Vars::Aimbot::Projectile::FeetAimIfOnGround.Value); HelpMarker("Will aim at targets feet if they're on the ground in order to launch them into the air");

				SectionTitle("Strafe Prediction");
				MultiCombo({ "Air", "Ground" }, { &Vars::Aimbot::Projectile::StrafePredictionAir.Value, &Vars::Aimbot::Projectile::StrafePredictionGround.Value }, "Strafe Prediction");
				WSlider("Velocity samples", &Vars::Aimbot::Projectile::StrafePredictionSamples.Value, 1, 20); HelpMarker("How many ticks to keep velocity records of");
				WSlider("Minimum deviation", &Vars::Aimbot::Projectile::StrafePredictionMinDifference.Value, 0, 180); HelpMarker("How big the angle difference of the predicted strafe has to be to apply");
				WSlider("Maximum distance", &Vars::Aimbot::Projectile::StrafePredictionMaxDistance.Value, 100.f, 10000.f); HelpMarker("Max distance to apply strafe prediction (lower is better)");
			}

		} EndChild();

		/* End */
		EndTable();
	}
}

/* Tab: Trigger */
void CMenu::MenuTrigger()
{
	using namespace ImGui;
	if (BeginTable("TriggerTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("TriggerCol1"))
		{
			SectionTitle("Autostab");
			WToggle("Auto backstab###TriggerAutostab", &Vars::Triggerbot::Stab::Active.Value); HelpMarker("Auto backstab will attempt to backstab the target if possible");
			WToggle("Rage mode", &Vars::Triggerbot::Stab::RageMode.Value); HelpMarker("Stabs whenever possible by aiming toward the back");
			WToggle("Silent", &Vars::Triggerbot::Stab::Silent.Value); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			WToggle("Disguise on kill", &Vars::Triggerbot::Stab::Disguise.Value); HelpMarker("Will apply the previous disguise after stabbing");
			WToggle("Ignore razorback", &Vars::Triggerbot::Stab::IgnRazor.Value); HelpMarker("Will not attempt to backstab snipers wearing the razorback");
			WSlider("Stab range###StabRange", &Vars::Triggerbot::Stab::Range.Value, 0.0f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The range at which auto backstab will attempt to stab");

			SectionTitle("Auto Detonate");
			WToggle("Autodetonate###TriggerDet", &Vars::Triggerbot::Detonate::Active.Value);
			{
				static std::vector Names{ "Players", "Buildings", "NPCs", "Bombs", "Stickies" };
				static std::vector Values{ 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4 };

				MultiFlags(Names, Values, &Vars::Triggerbot::Detonate::DetonateTargets.Value, "Targets###TriggerbotDetonateTargets");
			}
			WToggle("Explode stickies###TriggerSticky", &Vars::Triggerbot::Detonate::Stickies.Value); HelpMarker("Detonate sticky bombs when a player is in range");
			WToggle("Detonate flares###TriggerFlares", &Vars::Triggerbot::Detonate::Flares.Value); HelpMarker("Detonate detonator flares when a player is in range");
			WSlider("Detonation radius###TriggerDetRadius", &Vars::Triggerbot::Detonate::RadiusScale.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("The radius around the projectile that it will detonate if a player is in");
		} EndChild();

		/* Column 2 */
		if (TableColumnChild("TriggerCol2 "))
		{
			SectionTitle("Autoblast");
			WToggle("Autoblast###Triggreairblast", &Vars::Triggerbot::Blast::Active.Value); HelpMarker("Auto airblast master switch");
			WToggle("Rage airblast###TriggerAirRage", &Vars::Triggerbot::Blast::Rage.Value); HelpMarker("Will airblast whenever possible, regardless of FoV");
			WToggle("Silent###triggerblastsilent", &Vars::Triggerbot::Blast::Silent.Value); HelpMarker("Aim changes made by the rage mode setting aren't visible");
			WToggle("Extinguish Players###TriggerExtinguishPlayers", &Vars::Triggerbot::Blast::ExtinguishPlayers.Value); HelpMarker("Will automatically extinguish burning players");
			WToggle("Disable on Attack###TriggerDisableOnAttack", &Vars::Triggerbot::Blast::DisableOnAttack.Value); HelpMarker("Will not air blast while attacking");
			WSlider("FOV####AirBlastFov", &Vars::Triggerbot::Blast::Fov.Value, 0, 90, "%d", ImGuiSliderFlags_AlwaysClamp);

			SectionTitle("Autouber");
			WToggle("Autouber###Triggeruber", &Vars::Triggerbot::Uber::Active.Value); HelpMarker("Auto uber master switch");
			WToggle("Only uber friends", &Vars::Triggerbot::Uber::OnlyFriends.Value); HelpMarker("Auto uber will only activate if healing steam friends");
			WToggle("Preserve self", &Vars::Triggerbot::Uber::PopLocal.Value); HelpMarker("Auto uber will activate if local player's health falls below the percentage");
			WToggle("Vaccinator resistances", &Vars::Triggerbot::Uber::AutoVacc.Value); HelpMarker("Auto uber will automatically find the best resistance and pop when needed (This doesn't work properly)");
			if (Vars::Triggerbot::Uber::AutoVacc.Value)
			{
				MultiCombo({ "Bullet", "Blast", "Fire" }, { &Vars::Triggerbot::Uber::BulletRes.Value, &Vars::Triggerbot::Uber::BlastRes.Value, &Vars::Triggerbot::Uber::FireRes.Value }, "Allowed Resistances");
			}
			{
				static std::vector Names{ "Scout", "Soldier", "Pyro", "Heavy", "Engineer", "Sniper", "Spy" };
				static std::vector Values{ 1 << 0, 1 << 1, 1 << 2, 1 << 4, 1 << 5, 1 << 7, 1 << 8 };

				MultiFlags(Names, Values, &Vars::Triggerbot::Uber::ReactClasses.Value, "Hitscan React Classes###TriggerbotAutoVaccClasses");
			}
			WSlider("Health left (%)###TriggerUberHealthLeft", &Vars::Triggerbot::Uber::HealthLeft.Value, 1.f, 99.f, "%.0f%%", 1.0f); HelpMarker("The amount of health the heal target must be below to actiavte");
			WSlider("Reaction FoV###TriggerUberReactFoV", &Vars::Triggerbot::Uber::ReactFoV.Value, 0, 90, "%d", 1); HelpMarker("Checks whether you are within a certain FoV from legit players before auto ubering.");
			WToggle("Activate charge trigger", &Vars::Triggerbot::Uber::VoiceCommand.Value); HelpMarker("Will ubercharge regardless of anything if your target says activate charge");
		} EndChild();

		EndTable();
	}
}

/* Tab: Visuals */
void CMenu::MenuVisuals()
{
	using namespace ImGui;

	switch (CurrentVisualsTab)
	{
		// Visuals: Players
	case VisualsTab::Players:
	{
		if (BeginTable("VisualsPlayersTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsPlayersCol1"))
			{
				SectionTitle("ESP Main");
				WToggle("ESP###EnableESP", &Vars::ESP::Main::Active.Value); HelpMarker("Global ESP master switch");
				WToggle("Outlined health bars", &Vars::ESP::Main::Outlinedbar.Value); HelpMarker("Will outline the health bars");
				WToggle("Relative colours", &Vars::ESP::Main::EnableTeamEnemyColors.Value); HelpMarker("Chooses colors relative to your team (team/enemy)");
				if (Vars::ESP::Main::EnableTeamEnemyColors.Value)
				{
					ColorPickerL("Enemy color", Vars::Colours::Enemy.Value);
					ColorPickerL("Team color", Vars::Colours::Friendly.Value, 1);
				}
				else
				{
					ColorPickerL("RED Team color", Vars::Colours::TeamRed.Value);
					ColorPickerL("BLU Team color", Vars::Colours::TeamBlu.Value, 1);
				}

				SectionTitle("Player ESP");
				WToggle("Player ESP###EnablePlayerESP", &Vars::ESP::Players::Active.Value); HelpMarker("Will draw useful information/indicators on players");
				WToggle("Name ESP###PlayerNameESP", &Vars::ESP::Players::Name.Value); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Players::NameCustom.Value); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Players::NameCustom.Value)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Players::NameColor.Value);
				}
				WToggle("Name ESP box###PlayerNameESPBox", &Vars::ESP::Players::NameBox.Value); HelpMarker("Will draw a box around players name to make it stand out");
				WToggle("Self ESP###SelfESP", &Vars::ESP::Players::ShowLocal.Value); HelpMarker("Will draw ESP on local player (thirdperson)");
				ColorPickerL("Local colour", Vars::Colours::Local.Value);
				MultiFlags({ "Friends", "Teammates", "Enemies" },
					{ (1 << 0), (1 << 1), (1 << 2) },
					&Vars::ESP::Players::IgnoreFlags.Value,
					"Ignore Flags###IgnoreFlagsESP"
				); HelpMarker("Which groups the ESP will ignore.");
				ColorPickerL("Friend colour", Vars::Colours::Friend.Value);
				WCombo("Ignore cloaked###IgnoreCloakESPp", &Vars::ESP::Players::IgnoreCloaked.Value, { "Off", "All", "Only enemies" }); HelpMarker("Which cloaked spies the ESP will ignore drawing on");
				ColorPickerL("Cloaked colour", Vars::Colours::Cloak.Value);
				WCombo("Ubercharge###PlayerUber", &Vars::ESP::Players::Uber.Value, { "Off", "Text", "Bar" }); HelpMarker("Will draw how much ubercharge a medic has");
				ColorPickerL("Ubercharge colour", Vars::Colours::UberColor.Value);
				WToggle("Health bar###ESPPlayerHealthBar", &Vars::ESP::Players::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the player has");
				if (Vars::ESP::Players::HealthBarStyle.Value == 0)
				{
					ColorPickerL("Health Bar Top", Vars::Colours::GradientHealthBar.Value.startColour);
					ColorPickerL("Health Bar Bottom", Vars::Colours::GradientHealthBar.Value.endColour, 1);
				}

				WCombo("Health bar style", &Vars::ESP::Players::HealthBarStyle.Value, { "Gradient", "Old" }); HelpMarker("How to draw the health bar");
				if (Vars::ESP::Players::HealthBarStyle.Value == 0)
				{
					ColorPickerL("Overheal Bar Top", Vars::Colours::GradientOverhealBar.Value.startColour);
					ColorPickerL("Overheal Bar Bottom", Vars::Colours::GradientOverhealBar.Value.endColour, 1);
				}
				if (Vars::ESP::Players::HealthBarStyle.Value == 1)
				{
					ColorPickerL("Overheal Colour", Vars::Colours::Overheal.Value);
				}
				WCombo("Health Text###ESPPlayerHealthText", &Vars::ESP::Players::HealthText.Value, { "Off", "Default", "Bar" }); HelpMarker("Draws the player health as a text");
				WToggle("Condition", &Vars::ESP::Players::Cond.Value); HelpMarker("Will draw what conditions the player is under");
				ColorPickerL("Condition colour", Vars::Colours::Cond.Value);
				WToggle("Cheater Detection", &Vars::ESP::Players::CheaterDetection.Value); HelpMarker("Attempts to automatically mark cheaters.");
				WCombo("Box###PlayerBoxESP", &Vars::ESP::Players::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on players");
				WToggle("Lines###Playerlines", &Vars::ESP::Players::Lines.Value); HelpMarker("Draws lines from the local players position to enemies position");
				WSlider("ESP alpha###PlayerESPAlpha", &Vars::ESP::Players::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WToggle("Sniper sightlines", &Vars::ESP::Players::SniperSightlines.Value);
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsPlayersCol2"))
			{
				SectionTitle("Chams Main");
				WToggle("Chams###ChamsMasterSwitch", &Vars::Chams::Main::Active.Value); HelpMarker("Chams master switch");

				static std::vector chamOptions{
					"Local",
					"FakeAngles",
					"Friends",
					"Enemies",
					"Teammates",
					"Target",
					"Ragdolls",
					"ViewModel",
					"VM Weapon"
				};

				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; // 0.local 1.friends 2.enemies 3.team 4.target 5.ragdolls 6.hands 7.weapon
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected)
						{
						case 0:
						{
							return Vars::Chams::Players::Local.Value;
						}
						case 1:
						{
							return Vars::Chams::Players::FakeAng.Value;
						}
						case 2:
						{
							return Vars::Chams::Players::Friend.Value;
						}
						case 3:
						{
							return Vars::Chams::Players::Enemy.Value;
						}
						case 4:
						{
							return Vars::Chams::Players::Team.Value;
						}
						case 5:
						{
							return Vars::Chams::Players::Target.Value;
						}
						case 6:
						{
							return Vars::Chams::Players::Ragdoll.Value;
						}
						case 7:
						{
							return Vars::Chams::DME::Hands.Value;
						}
						case 8:
						{
							return Vars::Chams::DME::Weapon.Value;
						}
						}

						return Vars::Chams::Players::Local.Value;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel" };

				//WToggle("Player chams###PlayerChamsBox", &Vars::Chams::Players::Active.Value); HelpMarker("Player chams master switch");

				MultiCombo({ "Render Wearable", "Render Weapon", "Fadeout Own Team" }, { &Vars::Chams::Players::Wearables.Value, &Vars::Chams::Players::Weapons.Value, &Vars::Chams::Players::FadeoutTeammates.Value }, "Flags");
				HelpMarker("Customize Chams");
				WCombo("Config", &currentSelected, chamOptions);
				{
					ColorPickerL("Colour", currentStruct.colour, 1);
					MultiCombo({ "Active", "Fixed" }, { &currentStruct.chamsActive, &currentStruct.showObstructed }, "Options");

					WCombo("Material", &currentStruct.drawMaterial, DMEChamMaterials); HelpMarker("Which material the chams will apply to the player");
					if (currentStruct.drawMaterial == 7)
					{
						ColorPickerL("Fresnel base colour", currentStruct.fresnelBase, 1);
					}
					ColorPickerL("Glow Colour", currentStruct.overlayColour, 1);
					WToggle("Rainbow Glow", &currentStruct.overlayRainbow);
					WToggle("Pulse Glow", &currentStruct.overlayPulse);
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

				}

				SectionTitle("Chams Misc");

				SectionTitle("Backtrack chams");
				WToggle("Backtrack chams", &Vars::Backtrack::BtChams::Enabled.Value); HelpMarker("Draws chams to show where a player is");
				ColorPickerL("Backtrack colour", Vars::Backtrack::BtChams::BacktrackColor.Value);
				ColorPickerL("Backtrack colour 2", Vars::Backtrack::BtChams::BacktrackColor2.Value, 1);
				WToggle("Only draw last tick", &Vars::Backtrack::BtChams::LastOnly.Value); HelpMarker("Only draws the last tick (can save FPS)");
				if (!Vars::Backtrack::BtChams::LastOnly.Value)
				{
					WToggle("Gradient###BTChams", &Vars::Backtrack::BtChams::Gradient.Value); HelpMarker("Will draw a gradient on the backtrack chams");
				}
				WToggle("Enemy only", &Vars::Backtrack::BtChams::EnemyOnly.Value); HelpMarker("You CAN backtrack your teammates. (Whip, medigun)");

				static std::vector backtrackMaterial{
					"Original",
					"Shaded",
					"Shiny",
					"Flat",
					"Wireframe shaded",
					"Wireframe shiny",
					"Wireframe flat",
					"Fresnel",
				};
				WCombo("Backtrack material", &Vars::Backtrack::BtChams::Material.Value, backtrackMaterial);
				WToggle("Ignore Z###BtIgnoreZ", &Vars::Backtrack::BtChams::IgnoreZ.Value); HelpMarker("Draws them through walls");
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsPlayersCol3"))
			{
				SectionTitle("Glow Main");
				WToggle("Glow", &Vars::Glow::Main::Active.Value);
				WCombo("Glow Type###GlowTypeSelect", &Vars::Glow::Main::Type.Value, { "Blur", "Stencil", "FPStencil", "Wireframe" }); HelpMarker("Method in which glow will be rendered");
				if (Vars::Glow::Main::Type.Value != 1) { WSlider("Glow scale", &Vars::Glow::Main::Scale.Value, 1, 10, "%d", ImGuiSliderFlags_AlwaysClamp); }

				SectionTitle("Player Glow");
				WToggle("Player glow###PlayerGlowButton", &Vars::Glow::Players::Active.Value); HelpMarker("Player glow master switch");
				WToggle("Self glow###SelfGlow", &Vars::Glow::Players::ShowLocal.Value); HelpMarker("Draw glow on the local player");
				WToggle("Self rainbow glow###SelfGlowRainbow", &Vars::Glow::Players::LocalRainbow.Value); HelpMarker("Homosapien");
				WCombo("Ignore team###IgnoreTeamGlowp", &Vars::Glow::Players::IgnoreTeammates.Value, { "Off", "All", "Only friends" }); HelpMarker("Which teammates the glow will ignore drawing on");
				WToggle("Wearable glow###PlayerWearableGlow", &Vars::Glow::Players::Wearables.Value); HelpMarker("Will draw glow on player cosmetics");
				WToggle("Weapon glow###PlayerWeaponGlow", &Vars::Glow::Players::Weapons.Value); HelpMarker("Will draw glow on player weapons");
				WSlider("Glow alpha###PlayerGlowAlpha", &Vars::Glow::Players::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColour", &Vars::Glow::Players::Color.Value, { "Team", "Health" }); HelpMarker("Which colour the glow will draw");

				SectionTitle("Misc Glow");
				WToggle("Prediction glow", &Vars::Glow::Misc::MovementSimLine.Value);
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: Building
	case VisualsTab::Buildings:
	{
		if (BeginTable("VisualsBuildingsTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsBuildingsCol1"))
			{
				SectionTitle("Building ESP");
				WToggle("Building ESP###BuildinGESPSwioifas", &Vars::ESP::Buildings::Active.Value); HelpMarker("Will draw useful information/indicators on buildings");
				WToggle("Ignore team buildings###BuildingESPIgnoreTeammates", &Vars::ESP::Buildings::IgnoreTeammates.Value); HelpMarker("Whether or not to draw ESP on your teams buildings");
				WToggle("Name ESP###BuildingNameESP", &Vars::ESP::Buildings::Name.Value); HelpMarker("Will draw the players name");
				WToggle("Custom Name Color", &Vars::ESP::Buildings::NameCustom.Value); HelpMarker("Custom color for name esp");
				if (Vars::ESP::Buildings::NameCustom.Value)
				{
					ColorPickerL("Name ESP Color", Vars::ESP::Buildings::NameColor.Value);
				}
				WToggle("Name ESP box###BuildingNameESPBox", &Vars::ESP::Buildings::NameBox.Value); HelpMarker("Will draw a box around the buildings name to make it stand out");
				WToggle("Health bar###Buildinghelathbar", &Vars::ESP::Buildings::HealthBar.Value); HelpMarker("Will draw a bar visualizing how much health the building has");
				WToggle("Health text###buildinghealth", &Vars::ESP::Buildings::Health.Value); HelpMarker("Will draw the building's health, as well as its max health");
				WToggle("Distance", &Vars::ESP::Buildings::Distance.Value); HelpMarker("Shows the distance from you to the building in meters");
				WToggle("Building owner###Buildingowner", &Vars::ESP::Buildings::Owner.Value); HelpMarker("Shows who built the building");
				WToggle("Building level###Buildinglevel", &Vars::ESP::Buildings::Level.Value); HelpMarker("Will draw what level the building is");
				WToggle("Building condition###Buildingconditions", &Vars::ESP::Buildings::Cond.Value); HelpMarker("Will draw what conditions the building is under");
				WToggle("Teleporter exit direction###Buildingteleexitdir", &Vars::ESP::Buildings::TeleExitDir.Value); HelpMarker("Show teleporter exit direction arrow");
				ColorPickerL("Teleporter exit direction arrow color", Vars::ESP::Buildings::TeleExitDirColor.Value);
				WToggle("Lines###buildinglines", &Vars::ESP::Buildings::Lines.Value); HelpMarker("Draws lines from the local players position to the buildings position");
				WCombo("Box###PBuildingBoxESP", &Vars::ESP::Buildings::Box.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on buildings");
				WToggle("Dlights###PlayerDlights", &Vars::ESP::Buildings::Dlights.Value); HelpMarker("Will make buildings emit a dynamic light around them, although buildings can't move some I'm not sure that the lights are actually dynamic here...");
				WSlider("Dlight radius###PlayerDlightRadius", &Vars::ESP::Buildings::DlightRadius.Value, 0.f, 500.f, "%.f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How far the Dlight will illuminate");
				WSlider("ESP alpha###BuildingESPAlpha", &Vars::ESP::Buildings::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the ESP should be");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsBuildingsCol2"))
			{
				SectionTitle("Building Chams");
				WToggle("Building chams###BuildingChamsBox", &Vars::Chams::Buildings::Active.Value); HelpMarker("Building chams master switch");

				static std::vector chamOptions{
					"Local",
					"Friends",
					"Enemies",
					"Teammates",
					"Target"
				};
				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; //
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected)
						{
						case 0:
						{
							return Vars::Chams::Buildings::Local.Value;
						}
						case 1:
						{
							return Vars::Chams::Buildings::Friend.Value;
						}
						case 2:
						{
							return Vars::Chams::Buildings::Enemy.Value;
						}
						case 3:
						{
							return Vars::Chams::Buildings::Team.Value;
						}
						case 4:
						{
							return Vars::Chams::Buildings::Target.Value;
						}
						}

						return Vars::Chams::Buildings::Local.Value;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel" };

				WCombo("Config", &currentSelected, chamOptions);
				{
					ColorPickerL("Colour", currentStruct.colour, 1);
					MultiCombo({ "Active", "Fixed" }, { &currentStruct.chamsActive, &currentStruct.showObstructed }, "Options");

					WCombo("Material", &currentStruct.drawMaterial, DMEChamMaterials); HelpMarker("Which material the chams will apply to the player");
					if (currentStruct.drawMaterial == 7)
					{
						ColorPickerL("Fresnel base colour", currentStruct.fresnelBase, 1);
					}
					WCombo("Glow Overlay", &currentStruct.overlayType, dmeGlowMaterial);
					ColorPickerL("Glow Colour", currentStruct.overlayColour, 1);
					WToggle("Rainbow Glow", &currentStruct.overlayRainbow);
					WToggle("Pulse Glow", &currentStruct.overlayPulse);
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsBuildingsCol3"))
			{
				SectionTitle("Building Glow");
				WToggle("Building glow###BuildiongGlowButton", &Vars::Glow::Buildings::Active.Value);
				WToggle("Ignore team buildings###buildingglowignoreteams", &Vars::Glow::Buildings::IgnoreTeammates.Value);
				WSlider("Glow alpha###BuildingGlowAlpha", &Vars::Glow::Buildings::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				WCombo("Glow colour###GlowColourBuildings", &Vars::Glow::Buildings::Color.Value, { "Team", "Health" });
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: World
	case VisualsTab::World:
	{
		if (BeginTable("VisualsWorldTable", 3))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsWorldCol1"))
			{
				SectionTitle("World ESP");

				WToggle("World ESP###WorldESPActive", &Vars::ESP::World::Active.Value); HelpMarker("World ESP master switch");
				WSlider("ESP alpha###WordlESPAlpha", &Vars::ESP::World::Alpha.Value, 0.01f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp); HelpMarker("How transparent the world ESP should be");

				SectionTitle("Healthpack");
				WToggle("Name###WorldESPHealthpackName", &Vars::ESP::World::HealthName.Value); HelpMarker("Will draw ESP on healthpacks");
				WToggle("Line###WorldESPHealthpackLine", &Vars::ESP::World::HealthLine.Value); HelpMarker("Will draw a line to healthpacks");
				WCombo("Box###WorldESPHealthpackBox", &Vars::ESP::World::HealthBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on healthpacks");
				WToggle("Health Distance", &Vars::ESP::World::HealthDistance.Value); HelpMarker("Shows the distance from you to the health pack in meters");
				ColorPickerL("Healthpack colour", Vars::Colours::Health.Value); HelpMarker("Color for healthpack ESP");

				SectionTitle("Ammopack");
				WToggle("Name###WorldESPAmmopackName", &Vars::ESP::World::AmmoName.Value); HelpMarker("Will draw ESP on ammopacks");
				WToggle("Line###WorldESPAmmopackLine", &Vars::ESP::World::AmmoLine.Value); HelpMarker("Will draw a line to ammopacks");
				WCombo("Box###WorldESPAmmopackBox", &Vars::ESP::World::AmmoBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on ammopacks");
				WToggle("Ammo Distance", &Vars::ESP::World::AmmoDistance.Value); HelpMarker("Shows the distance from you to the ammo box in meters");
				ColorPickerL("Ammopack colour", Vars::Colours::Ammo.Value); HelpMarker("Color for ammopack ESP");

				SectionTitle("NPC");
				WToggle("Name###WorldESPNPCName", &Vars::ESP::World::NPCName.Value); HelpMarker("Will draw ESP on NPCs");
				WToggle("Line###WorldESPNPCLine", &Vars::ESP::World::NPCLine.Value); HelpMarker("Will draw a line to NPCs");
				WCombo("Box###WorldESPNPCBox", &Vars::ESP::World::NPCBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on NPCs");
				WToggle("NPC Distance", &Vars::ESP::World::NPCDistance.Value); HelpMarker("Shows the distance from you to the NPC in meters");
				ColorPickerL("NPC colour", Vars::Colours::NPC.Value); HelpMarker("Color for NPC ESP");

				SectionTitle("Bombs");
				WToggle("Name###WorldESPBombName", &Vars::ESP::World::BombName.Value); HelpMarker("Will draw ESP on bombs");
				WToggle("Line###WorldESPBombLine", &Vars::ESP::World::BombLine.Value); HelpMarker("Will draw a line to bombs");
				WCombo("Box###WorldESPBombBox", &Vars::ESP::World::BombBox.Value, { "Off", "Bounding", "Cornered", "3D" }); HelpMarker("What sort of box to draw on bombs");
				WToggle("Bomb Distance", &Vars::ESP::World::BombDistance.Value); HelpMarker("Shows the distance from you to the bomb in meters");
				ColorPickerL("Bomb Colour", Vars::Colours::Bomb.Value); HelpMarker("Color for bomb ESP");

			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsWorldCol2"))
			{
				SectionTitle("World Chams");
				WToggle("World chams###woldchamsbut", &Vars::Chams::World::Active.Value);

				static std::vector chamOptions{
					"Healthpacks",
					"Ammopacks",
					"Projectiles"
				};
				static std::vector dmeGlowMaterial{
					"None",
					"Fresnel Glow",
					"Wireframe Glow"
				};

				static int currentSelected = 0; //
				Chams_t& currentStruct = ([&]() -> Chams_t&
					{
						switch (currentSelected)
						{
						case 0:
						{
							return Vars::Chams::World::Health.Value;
						}
						case 1:
						{
							return Vars::Chams::World::Ammo.Value;
						}
						case 2:
						{
							return Vars::Chams::World::Projectiles.Value;
						}
						}

						return Vars::Chams::World::Health.Value;
					}());
				static std::vector DMEChamMaterials{ "Original", "Shaded", "Shiny", "Flat", "Wireframe shaded", "Wireframe shiny", "Wireframe flat", "Fresnel" };

				WCombo("Config", &currentSelected, chamOptions);
				{
					ColorPickerL("Colour", currentStruct.colour, 1);
					MultiCombo({ "Active", "Obstructed" }, { &currentStruct.chamsActive, &currentStruct.showObstructed }, "Options");

					WCombo("Material", &currentStruct.drawMaterial, DMEChamMaterials); HelpMarker("Which material the chams will apply to the player");
					if (currentStruct.drawMaterial == 7)
					{
						ColorPickerL("Fresnel base colour", currentStruct.fresnelBase, 1);
					}
					WCombo("Glow Overlay", &currentStruct.overlayType, dmeGlowMaterial);
					ColorPickerL("Glow Colour", currentStruct.overlayColour, 1);
					WToggle("Rainbow Glow", &currentStruct.overlayRainbow);
					WToggle("Pulse Glow", &currentStruct.overlayPulse);
					WSlider("Glow Reduction", &currentStruct.overlayIntensity, 150.f, 0.1f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
				}
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsWorldCol3"))
			{
				SectionTitle("World Glow");
				WToggle("World glow###Worldglowbutton", &Vars::Glow::World::Active.Value);
				WToggle("Healthpacks###worldhealthpackglow", &Vars::Glow::World::Health.Value);
				WToggle("Ammopacks###worldammopackglow", &Vars::Glow::World::Ammo.Value);
				WToggle("NPCs###worldnpcs", &Vars::Glow::World::NPCs.Value);
				WToggle("Bombs###worldbombglow", &Vars::Glow::World::Bombs.Value);
				WCombo("Projectile glow###teamprojectileglow", &Vars::Glow::World::Projectiles.Value, { "Off", "All", "Only enemies" });
				WSlider("Glow alpha###WorldGlowAlpha", &Vars::Glow::World::Alpha.Value, 0.f, 1.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: Font
	case VisualsTab::Font:
	{
		if (BeginTable("VisualsFontTable", 3))
		{
			static std::vector fontFlagNames{ "Italic", "Underline", "Strikeout", "Symbol", "Antialias", "Gaussian", "Rotary", "Dropshadow", "Additive", "Outline", "Custom" };
			static std::vector fontFlagValues{ 0x001, 0x002, 0x004, 0x008, 0x010, 0x020, 0x040, 0x080, 0x100, 0x200, 0x400 };

			/* Column 1 */
			if (TableColumnChild("VisualsFontCol1"))
			{
				SectionTitle("ESP Font");
				WInputText("Font name###espfontname", &Vars::Fonts::FONT_ESP::szName.Value);
				WInputInt("Font height###espfontheight", &Vars::Fonts::FONT_ESP::nTall.Value);
				WInputInt("Font weight###espfontweight", &Vars::Fonts::FONT_ESP::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP::nFlags.Value, "Font flags###FONT_ESP");

				SectionTitle("Name Font");
				WInputText("Font name###espfontnamename", &Vars::Fonts::FONT_ESP_NAME::szName.Value);
				WInputInt("Font height###espfontnameheight", &Vars::Fonts::FONT_ESP_NAME::nTall.Value);
				WInputInt("Font weight###espfontnameweight", &Vars::Fonts::FONT_ESP_NAME::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_NAME::nFlags.Value, "Font flags###FONT_ESP_NAME");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsFontCol2"))
			{
				SectionTitle("Condition Font");
				WInputText("Font name###espfontcondname", &Vars::Fonts::FONT_ESP_COND::szName.Value);
				WInputInt("Font height###espfontcondheight", &Vars::Fonts::FONT_ESP_COND::nTall.Value);
				WInputInt("Font weight###espfontcondweight", &Vars::Fonts::FONT_ESP_COND::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_COND::nFlags.Value, "Font flags###FONT_ESP_COND");

				SectionTitle("Pickup Font");
				WInputText("Font name###espfontpickupsname", &Vars::Fonts::FONT_ESP_PICKUPS::szName.Value);
				WInputInt("Font height###espfontpickupsheight", &Vars::Fonts::FONT_ESP_PICKUPS::nTall.Value);
				WInputInt("Font weight###espfontpickupsweight", &Vars::Fonts::FONT_ESP_PICKUPS::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_ESP_PICKUPS::nFlags.Value, "Font flags###FONT_ESP_PICKUPS");
			} EndChild();

			/* Column 3 */
			if (TableColumnChild("VisualsFontCol3"))
			{
				SectionTitle("Menu Font");
				WInputText("Font name###espfontnamenameneby", &Vars::Fonts::FONT_MENU::szName.Value);
				WInputInt("Font height###espfontnameheightafsdfads", &Vars::Fonts::FONT_MENU::nTall.Value);
				WInputInt("Font weight###espfontnameweightasfdafsd", &Vars::Fonts::FONT_MENU::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_MENU::nFlags.Value, "Font flags###FONT_MENU");

				SectionTitle("Indicator Font");
				WInputText("Font name###espfontindicatorname", &Vars::Fonts::FONT_INDICATORS::szName.Value);
				WInputInt("Font height###espfontindicatorheight", &Vars::Fonts::FONT_INDICATORS::nTall.Value);
				WInputInt("Font weight###espfontindicatorweight", &Vars::Fonts::FONT_INDICATORS::nWeight.Value);
				MultiFlags(fontFlagNames, fontFlagValues, &Vars::Fonts::FONT_INDICATORS::nFlags.Value, "Font flags###FONT_INDICATORS");

				if (Button("Apply settings###fontapply"))
				{
					g_Draw.RemakeFonts();
				}
			} EndChild();

			EndTable();
		}
		break;
	}

	// Visuals: Misc
	case VisualsTab::Misc:
	{
		if (BeginTable("VisualsMiscTable", 2))
		{
			/* Column 1 */
			if (TableColumnChild("VisualsMiscCol1"))
			{
				SectionTitle("World & UI");
				WSlider("Field of view", &Vars::Visuals::FieldOfView.Value, 70, 150, "%d"); HelpMarker("How many degrees of field of vision you would like");
				WCombo("Vision modifiers", &Vars::Visuals::VisionModifier.Value, { "Off", "Pyrovision", "Halloween", "Romevision" }); HelpMarker("Vision modifiers");
				MultiCombo({ "World", "Sky", "Prop Wireframe" }, { &Vars::Visuals::WorldModulation.Value, &Vars::Visuals::SkyModulation.Value, &Vars::Visuals::PropWireframe.Value }, "Modulations");
				HelpMarker("Select which types of modulation you want to enable");
				if (ColorPickerL("World modulation colour", Vars::Colours::WorldModulation.Value) ||
					ColorPickerL("Sky modulation colour", Vars::Colours::SkyModulation.Value, 1) ||
					ColorPickerL("Prop modulation colour", Vars::Colours::StaticPropModulation.Value, 2))
				{
					G::ShouldUpdateMaterialCache = true;
				}
				MultiCombo({ "Scope", "Zoom", "Disguises", "Taunts", "Interpolation", "Input Delay", "View Punch", "MOTD", "Angle Forcing", "Ragdolls", "Screen Overlays", "DSP", "Convar Queries" }, { &Vars::Visuals::RemoveScope.Value, &Vars::Visuals::RemoveZoom.Value, &Vars::Visuals::RemoveDisguises.Value, &Vars::Visuals::RemoveTaunts.Value, &Vars::Misc::DisableInterpolation.Value, &Vars::Misc::FixInputDelay.Value, &Vars::Visuals::RemovePunch.Value, &Vars::Visuals::RemoveMOTD.Value, &Vars::Visuals::RemoveScreenEffects.Value, &Vars::Visuals::PreventForcedAngles.Value, &Vars::Visuals::RemoveRagdolls.Value, &Vars::Visuals::RemoveScreenOverlays.Value, &Vars::Visuals::RemoveDSP.Value, &Vars::Visuals::RemoveConvarQueries.Value }, "Removals");
				ColorPickerL("Particle Color", Vars::Colours::ParticleColor.Value);
				if (Vars::Visuals::HalloweenSpellFootsteps.Value)
				{
					if (!Vars::Visuals::ParticleColors.Value) //Particle colors overrides the color picker
					{
						WCombo("Color Type", &Vars::Visuals::ColorType.Value, { "Color Picker", "Rainbow" });
						if (Vars::Visuals::ColorType.Value == 0)
						{
							ColorPickerL("Footstep Color", Vars::Colours::FeetColor.Value);
						}
					}
					WToggle("Dash only", &Vars::Visuals::DashOnly.Value);
				}
				WToggle("Clean Screenshots", &Vars::Visuals::CleanScreenshots.Value);
				WToggle("Viewmodel aim position", &Vars::Visuals::AimbotViewmodel.Value);
				WToggle("Bullet tracers", &Vars::Visuals::BulletTracer.Value);
				ColorPickerL("Bullet tracer colour", Vars::Colours::BulletTracer.Value);
				WToggle("Viewmodel sway", &Vars::Visuals::ViewmodelSway.Value);
				if (Vars::Visuals::ViewmodelSway.Value)
				{
					WSlider("Viewmodel Sway Scale", &Vars::Visuals::ViewmodelSwayScale.Value, 0.01, 5, "%.2f");
				}
				MultiCombo({ "Line", "Seperators" }, { &Vars::Visuals::MoveSimLine.Value, &Vars::Visuals::MoveSimSeperators.Value }, "Proj Aim Lines");
				ColorPickerL("Prediction Line Color", Vars::Aimbot::Projectile::PredictionColor.Value);
				if (Vars::Visuals::MoveSimSeperators.Value)
				{
					WSlider("Seperator Length", &Vars::Visuals::SeperatorLength.Value, 2, 16, "%d", ImGuiSliderFlags_Logarithmic);
					WSlider("Seperator Spacing", &Vars::Visuals::SeperatorSpacing.Value, 1, 64, "%d", ImGuiSliderFlags_Logarithmic);
				}
				{
					static std::vector flagNames{ "Text", "Console", "Chat", "Party", "Verbose" };
					static std::vector flagValues{ 1, 2, 4, 8, 32 };
					MultiFlags(flagNames, flagValues, &Vars::Misc::VotingOptions.Value, "Vote Logger###VoteLoggingOptions");
				}
				WCombo("Particle tracer", &Vars::Visuals::ParticleTracer.Value, { "Off", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus ZAP", "Merasmus ZAP Beam 2", "Big Nasty", "Distortion Trail", "Black Ink", "Custom" });
				WToggle("Pickup Timers", &Vars::Visuals::PickupTimers.Value); HelpMarker("Displays the respawn time of health and ammopacks");
				WToggle("Draw Hitboxes", &Vars::Aimbot::Global::showHitboxes.Value); HelpMarker("Shows client hitboxes for enemies once they are attacked (not bbox)");
				ColorPickerL("Hitbox matrix face colour", Vars::Colours::HitboxFace.Value);
				ColorPickerL("Hitbox matrix edge colour", Vars::Colours::HitboxEdge.Value, 1);
				WToggle("Clear Hitboxes", &Vars::Aimbot::Global::ClearPreviousHitbox.Value); HelpMarker("Removes previous drawn hitboxes to mitigate clutter");
				WSlider("Hitbox Draw Time", &Vars::Aimbot::Global::HitboxLifetime.Value, 1, 5); HelpMarker("Removes previous drawn hitboxes after n seconds");
				WCombo("Spectator list", &Vars::Visuals::SpectatorList.Value, { "Off", "Draggable", "Static", "Static + Avatars" });
				WToggle("Killstreak weapon", &Vars::Misc::KillstreakWeapon.Value); HelpMarker("Enables the killstreak counter on any weapon");


				SectionTitle("Viewmodel Offset");
				WSlider("X", &Vars::Visuals::VMOffsets.Value.x, -45.f, 45.f);
				WSlider("Y", &Vars::Visuals::VMOffsets.Value.y, -45.f, 45.f);
				WSlider("Z", &Vars::Visuals::VMOffsets.Value.z, -45.f, 45.f);

				SectionTitle("DT Bar");
				WCombo("DT indicator style", &Vars::Misc::CL_Move::DTBarStyle.Value, { "Off", "Nitro", "Rijin" }); HelpMarker("What style the bar should draw in.");
				Text("Charging Gradient");
				ColorPickerL("DT charging right", Vars::Colours::DTBarIndicatorsCharging.Value.endColour);
				ColorPickerL("DT charging left", Vars::Colours::DTBarIndicatorsCharging.Value.startColour, 1);
				Text("Charged Gradient");
				ColorPickerL("DT charged right", Vars::Colours::DTBarIndicatorsCharged.Value.endColour);
				ColorPickerL("DT charged left", Vars::Colours::DTBarIndicatorsCharged.Value.startColour, 1);

				SectionTitle("Ragdoll effects");
				WToggle("Enemy only###RagdollEnemyOnly", &Vars::Visuals::RagdollEffects::EnemyOnly.Value); HelpMarker("Only runs it on enemies");
				MultiCombo({ "Burning", "Electrocuted", "Ash", "Dissolve" }, { &Vars::Visuals::RagdollEffects::Burning.Value, &Vars::Visuals::RagdollEffects::Electrocuted.Value, &Vars::Visuals::RagdollEffects::BecomeAsh.Value, &Vars::Visuals::RagdollEffects::Dissolve.Value }, "Effects###RagdollEffects");
				HelpMarker("Ragdoll particle effects");
				WCombo("Ragdoll model", &Vars::Visuals::RagdollEffects::RagdollType.Value, { "None", "Gold", "Ice" }); HelpMarker("Which ragdoll model should be used");

				SectionTitle("Projectile Camera");
				InputKeybind("Proj Cam Key", Vars::Visuals::ProjectileCameraKey, true, false, "None");  HelpMarker("Makes your camera snap to the projectile you most recently fired.");
			} EndChild();

			/* Column 2 */
			if (TableColumnChild("VisualsMiscCol2"))
			{
				SectionTitle("Skybox & Textures");
				static std::vector skyNames{
					"Custom",
					"sky_tf2_04",
					"sky_upward",
					"sky_dustbowl_01",
					"sky_goldrush_01",
					"sky_granary_01",
					"sky_well_01",
					"sky_gravel_01",
					"sky_badlands_01",
					"sky_hydro_01",
					"sky_night_01",
					"sky_nightfall_01",
					"sky_trainyard_01",
					"sky_stormfront_01",
					"sky_morningsnow_01",
					"sky_alpinestorm_01",
					"sky_harvest_01",
					"sky_harvest_night_01",
					"sky_halloween",
					"sky_halloween_night_01",
					"sky_halloween_night2014_01",
					"sky_island_01",
					"sky_rainbow_01"
				};
				WToggle("Skybox changer", &Vars::Visuals::SkyboxChanger.Value); HelpMarker("Will change the skybox, either to a base TF2 one or a custom one");
				WCombo("Skybox", &Vars::Skybox::SkyboxNum.Value, skyNames);
				if (Vars::Skybox::SkyboxNum.Value == 0)
				{
					WInputText("Custom skybox name", &Vars::Skybox::SkyboxName.Value); HelpMarker("Name of the skybox you want to you (tf/materials/skybox)");
				}
				WToggle("Bypass sv_pure", &Vars::Misc::BypassPure.Value); HelpMarker("Allows you to load any custom files, even if disallowed by the sv_pure setting");

				SectionTitle("Thirdperson");
				WToggle("Thirdperson", &Vars::Visuals::ThirdPerson.Value); HelpMarker("Will move your camera to be in a thirdperson view");
				InputKeybind("Thirdperson key", Vars::Visuals::ThirdPersonKey, true, false, "None"); HelpMarker("What key to toggle thirdperson, press ESC if no bind is desired");
				WToggle("Show real angles###tpRealAngles", &Vars::Visuals::ThirdPersonSilentAngles.Value); HelpMarker("Will show your real angles on thirdperson (not what others see)");
				WToggle("Instant yaw###tpInstantYaw", &Vars::Visuals::ThirdPersonInstantYaw.Value); HelpMarker("Will set your yaw instantly in thirdperson, showing your actual angle, instead of what others see");
				WToggle("Show server hitboxes (localhost only)###tpShowServer", &Vars::Visuals::ThirdPersonServerHitbox.Value); HelpMarker("Will show the server angles in thirdperson");

				WToggle("Thirdperson offsets", &Vars::Visuals::ThirdpersonOffset.Value); HelpMarker("These will mess you up if you use a small FoV");
				WSlider("Thirdperson distance", &Vars::Visuals::ThirdpersonDist.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson right", &Vars::Visuals::ThirdpersonRight.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WSlider("Thirdperson up", &Vars::Visuals::ThirdpersonUp.Value, -500.f, 500.f, "%.1f", ImGuiSliderFlags_None);
				WToggle("Thirdperson crosshair", &Vars::Visuals::ThirdpersonCrosshair.Value);
				WToggle("Offset with arrow keys", &Vars::Visuals::ThirdpersonOffsetWithArrows.Value);
				InputKeybind("Move offset key", Vars::Visuals::ThirdpersonArrowOffsetKey, false);

				SectionTitle("Out of FOV arrows");
				WToggle("Active###fovar", &Vars::Visuals::OutOfFOVArrows.Value); HelpMarker("Will draw arrows to players who are outside of the range of your FoV");
				WToggle("Outline arrows###OutlinedArrows", &Vars::Visuals::OutOfFOVArrowsOutline.Value); HelpMarker("16 missed calls");
				WSlider("Arrow length", &Vars::Visuals::ArrowLength.Value, 5.f, 50.f, "%.2f"); HelpMarker("How long the arrows are");
				WSlider("Arrow angle", &Vars::Visuals::ArrowAngle.Value, 5.f, 180.f, "%.2f"); HelpMarker("The angle of the arrow");
				WSlider("Distance from center", &Vars::Visuals::FovArrowsDist.Value, 0.01f, 0.2f, "%.3f"); HelpMarker("How far from the center of the screen the arrows will draw");
				WSlider("Max distance", &Vars::Visuals::MaxDist.Value, 0.f, 4000.f, "%.2f"); HelpMarker("How far until the arrows will not show");
				WSlider("Min distance", &Vars::Visuals::MinDist.Value, 0.f, 1000.f, "%.2f"); HelpMarker("How close until the arrows will be fully opaque");
			} EndChild();

			EndTable();
		}
		break;
	}


	}
}

/* Tab: HvH */
void CMenu::MenuHvH()
{
	using namespace ImGui;
	if (BeginTable("HvHTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("HvHCol1"))
		{
			/* Section: Tickbase Exploits */
			SectionTitle("Tickbase Exploits");
			WToggle("Enable Tickbase Exploits", &Vars::Misc::CL_Move::Enabled.Value); HelpMarker("Allows tickbase shifting");
			ColorPickerL("DT bar outline colour", Vars::Colours::DtOutline.Value);
			InputKeybind("Recharge key", Vars::Misc::CL_Move::RechargeKey, true, false, "None"); HelpMarker("Recharges ticks for shifting");
			InputKeybind("Teleport key", Vars::Misc::CL_Move::TeleportKey, true, false, "None"); HelpMarker("Shifts ticks to warp");
			if (Vars::Misc::CL_Move::DTMode.Value == 0 || Vars::Misc::CL_Move::DTMode.Value == 2)
			{
				InputKeybind("Doubletap key", Vars::Misc::CL_Move::DoubletapKey, true, false, "None"); HelpMarker("Only doubletap when the key is pressed. Leave as (None) for always active.");
			}

			WCombo("Teleport Mode", &Vars::Misc::CL_Move::TeleportMode.Value, { "Plain", "Smooth", "Streamed Smooth" }); HelpMarker("How the teleport should be done");
			if (Vars::Misc::CL_Move::TeleportMode.Value)
			{
				WSlider("Smooth Teleport Factor", &Vars::Misc::CL_Move::TeleportFactor.Value, 2, 6, "%d");
			}
			MultiCombo({ "Recharge While Dead", "Auto Recharge", "Wait for DT", "Anti-warp", "Avoid airborne", "Retain Fakelag", "Safe Tick", "Safe Tick Airborne", "Auto Retain" }, { &Vars::Misc::CL_Move::RechargeWhileDead.Value, &Vars::Misc::CL_Move::AutoRecharge.Value, &Vars::Misc::CL_Move::WaitForDT.Value, &Vars::Misc::CL_Move::AntiWarp.Value, &Vars::Misc::CL_Move::NotInAir.Value, &Vars::Misc::CL_Move::RetainFakelag.Value, &Vars::Misc::CL_Move::SafeTick.Value, &Vars::Misc::CL_Move::SafeTickAirOverride.Value, &Vars::Misc::CL_Move::AutoRetain.Value }, "Options");
			HelpMarker("Enable various features regarding tickbase exploits");
			WCombo("Doubletap Mode", &Vars::Misc::CL_Move::DTMode.Value, { "On key", "Always", "Disable on key", "Disabled" }); HelpMarker("How should DT behave");
			const int ticksMax = g_ConVars.sv_maxusrcmdprocessticks->GetInt() - 3;	// remove the 2 backup cmd's and the cmd that we will send when we dt.
			WSlider("Ticks to shift", &Vars::Misc::CL_Move::DTTicks.Value, 1, ticksMax ? ticksMax : 21, "%d"); HelpMarker("19 - 21 ticks recommended");

			/* Section: Fakelag */
			SectionTitle("Fakelag");
			WToggle("Enable Fakelag", &Vars::Misc::CL_Move::Fakelag.Value);
			MultiCombo({ "While Moving", "On Key", "While Visible", "Predict Visibility", "While Unducking", "While Airborne" }, { &Vars::Misc::CL_Move::WhileMoving.Value, &Vars::Misc::CL_Move::FakelagOnKey.Value, &Vars::Misc::CL_Move::WhileVisible.Value, &Vars::Misc::CL_Move::PredictVisibility.Value, &Vars::Misc::CL_Move::WhileUnducking.Value, &Vars::Misc::CL_Move::WhileInAir.Value }, "Flags###FakeLagFlags");
			if (Vars::Misc::CL_Move::FakelagOnKey.Value)
			{
				InputKeybind("Fakelag key", Vars::Misc::CL_Move::FakelagKey, true, false, "None"); HelpMarker("The key to activate fakelag as long as it's held");
			}
			WCombo("Fakelag Mode###FLmode", &Vars::Misc::CL_Move::FakelagMode.Value, { "Plain", "Random", "Adaptive" }); HelpMarker("Controls how fakelag will be controlled.");

			switch (Vars::Misc::CL_Move::FakelagMode.Value)
			{
			case 0: { WSlider("Fakelag value", &Vars::Misc::CL_Move::FakelagValue.Value, 1, 22, "%d"); HelpMarker("How much lag you should fake(?)"); break; }
			case 1:
			{
				WSlider("Random max###flRandMax", &Vars::Misc::CL_Move::FakelagMax.Value, Vars::Misc::CL_Move::FakelagMin.Value + 1, 22, "%d"); HelpMarker("Maximum random fakelag value");
				WSlider("Random min###flRandMin", &Vars::Misc::CL_Move::FakelagMin.Value, 1, Vars::Misc::CL_Move::FakelagMax.Value - 1, "%d"); HelpMarker("Minimum random fakelag value");
				break;
			}
			}	//	add more here if you add your own fakelag modes :D
			WToggle("Retain BlastJump", &Vars::Misc::CL_Move::RetainBlastJump.Value); HelpMarker("Will attempt to retain the blast jumping condition as soldier and runs independently of fakelag.");
			WToggle("Unchoke On Attack", &Vars::Misc::CL_Move::UnchokeOnAttack.Value); HelpMarker("Will exit a fakelag cycle if you are attacking.");

		} EndChild();

		if (TableColumnChild("HvHCol2"))
		{
			SectionTitle("Resolver");
			WToggle("Enable Resolver", &Vars::AntiHack::Resolver::Resolver.Value); HelpMarker("Enables the anti-aim resolver.");
			if (Vars::AntiHack::Resolver::Resolver.Value)
			{
				WToggle("Ignore in-air", &Vars::AntiHack::Resolver::IgnoreAirborne.Value); HelpMarker("Doesn't resolve players who are in the air.");
			}
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("HvHCol3"))
		{
			/* Section: Anti Aim */
			SectionTitle("Anti Aim");
			WToggle("Enable Anti-aim", &Vars::AntiHack::AntiAim::Active.Value);
			InputKeybind("Anti-aim Key", Vars::AntiHack::AntiAim::ToggleKey, true, false, "None"); HelpMarker("The key to toggle anti aim");
			WCombo("Pitch", &Vars::AntiHack::AntiAim::Pitch.Value, { "None", "Zero", "Up", "Down", "Fake up", "Fake down", "Random", "Half Up", "Jitter", "Fake Up Custom", "Fake Down Custom" }); HelpMarker("Which way to look up/down");
			WCombo("Base Yaw", &Vars::AntiHack::AntiAim::BaseYawMode.Value, { "Offset", "FOV Player", "FOV Player + Offset" });
			WCombo("Real yaw", &Vars::AntiHack::AntiAim::YawReal.Value, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt", "Custom", "Invert", "Jitter", "Jitter Random", "Jitter Flip", "Manual" }); HelpMarker("Which way to look horizontally");
			WCombo("Fake yaw", &Vars::AntiHack::AntiAim::YawFake.Value, { "None", "Forward", "Left", "Right", "Backwards", "Random", "Spin", "Edge", "On Hurt", "Custom", "Invert", "Jitter", "Jitter Random", "Jitter Flip", "Manual" }); HelpMarker("Which way to appear to look horizontally");
			if (Vars::AntiHack::AntiAim::Pitch.Value == 9 || Vars::AntiHack::AntiAim::Pitch.Value == 10)
			{
				WSlider("Custom Real Pitch", &Vars::AntiHack::AntiAim::CustomRealPitch.Value, -89.f, 89.f, "%.1f", 0);
			}
			if (Vars::AntiHack::AntiAim::Pitch.Value == 6 || Vars::AntiHack::AntiAim::YawFake.Value == 6 || Vars::AntiHack::AntiAim::YawReal.Value == 6)
			{
				WSlider("Spin Speed", &Vars::AntiHack::AntiAim::SpinSpeed.Value, -30.f, 30.f, "%.1f", 0); HelpMarker("You spin me right 'round, baby, right 'round");
			}
			if (Vars::AntiHack::AntiAim::Pitch.Value == 6 || Vars::AntiHack::AntiAim::YawFake.Value == 5 || Vars::AntiHack::AntiAim::YawReal.Value == 5)
			{
				WSlider("Random Interval", &Vars::AntiHack::AntiAim::RandInterval.Value, 0, 100, "%d"); HelpMarker("How often the random Anti-Aim should update");
			}
			if (Vars::AntiHack::AntiAim::BaseYawMode.Value != 1)
			{
				WSlider("Base Yaw Offset", &Vars::AntiHack::AntiAim::BaseYawOffset.Value, -180, 180);
			}
			if (Vars::AntiHack::AntiAim::YawFake.Value == 10 || Vars::AntiHack::AntiAim::YawReal.Value == 10)
			{
				InputKeybind("Invert Key", Vars::AntiHack::AntiAim::InvertKey, true, false, "None");
			}
			if (Vars::AntiHack::AntiAim::YawFake.Value == 14 || Vars::AntiHack::AntiAim::YawReal.Value == 14)
			{
				InputKeybind("Manual Key", Vars::AntiHack::AntiAim::ManualKey, true, false, "None");
			}
			switch (Vars::AntiHack::AntiAim::YawFake.Value)
			{
			case 9: { WSlider("Custom fake yaw", &Vars::AntiHack::AntiAim::CustomFakeYaw.Value, -180, 180); break; }
			case 11:
			case 12:
			case 13: { WSlider("Fake Jitter Amt", &Vars::AntiHack::AntiAim::FakeJitter.Value, -180, 180); break; }
			}
			switch (Vars::AntiHack::AntiAim::YawReal.Value)
			{
			case 9: { WSlider("Custom Real yaw", &Vars::AntiHack::AntiAim::CustomRealYaw.Value, -180, 180); break; }
			case 11:
			case 12:
			case 13: { WSlider("Real Jitter Amt", &Vars::AntiHack::AntiAim::RealJitter.Value, -180, 180); break; }
			}
			MultiCombo({ "AntiOverlap", "Jitter Legs", "HidePitchOnShot", "Anti-Backstab", "Rehide Angle Post-Shot" }, { &Vars::AntiHack::AntiAim::AntiOverlap.Value, &Vars::AntiHack::AntiAim::LegJitter.Value, &Vars::AntiHack::AntiAim::InvalidShootPitch.Value, &Vars::AntiHack::AntiAim::AntiBackstab.Value, &Vars::AntiHack::AntiAim::RehideAntiAimPostShot.Value }, "Misc.");

		} EndChild();

		EndTable();
	}
}

/* Tab: Misc */
void CMenu::MenuMisc()
{
	using namespace ImGui;
	if (BeginTable("MiscTable", 3))
	{
		/* Column 1 */
		if (TableColumnChild("MiscCol1"))
		{
			SectionTitle("Movement");
			WCombo("No Push###MovementNoPush", &Vars::Misc::NoPush.Value, { "Off", "Always", "While Moving" });
			WCombo("Fast Stop", &Vars::Misc::AccurateMovement.Value, { "Off", "Instant" });

			MultiFlags({ "Fast Strafe", "Fast Accel", "Crouch Speed" }, { 8, 4, 2, 1 }, &Vars::Misc::AltMovement.Value, "Movement");
			WToggle("Bunnyhop", &Vars::Misc::AutoJump.Value); HelpMarker("Will jump as soon as you touch the ground again, keeping speed between jumps");
			WCombo("Autostrafe", &Vars::Misc::AutoStrafe.Value, { "Off", "Directional" }); HelpMarker("Will strafe for you in air automatically so that you gain speed");
			if (Vars::Misc::AutoStrafe.Value == 2)
			{
				WToggle("Only on movement key", &Vars::Misc::DirectionalOnlyOnMove.Value); HelpMarker("This makes it so that you dont always go forward if u just hold space");
			}

			SectionTitle("Automation");
			WToggle("Anti-AFK", &Vars::Misc::AntiAFK.Value); HelpMarker("Will make you jump every now and then so you don't get kicked for idling");
			WToggle("Auto Vote", &Vars::Misc::AutoVote.Value); HelpMarker("Automatically votes yes/no depending on the target");
			WToggle("Taunt slide", &Vars::Misc::TauntSlide.Value); HelpMarker("Allows you to input in taunts");
			WToggle("Taunt spin", &Vars::Misc::TauntSpin.Value);
			InputKeybind("Taunt spin key", Vars::Misc::TauntSpinKey, true, false, "None");	//	why was this set to disallow none?
			WSlider("Taunt spin speed", &Vars::Misc::TauntSpinSpeed.Value, 0.1f, 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

			WCombo("Pick Class", &Vars::Misc::AutoJoin.Value, { "Off", "Scout", "Soldier", "Pyro", "Demoman", "Heavy", "Engineer", "Medic", "Sniper", "Spy" }); HelpMarker("Automatically joins the given class");
			WToggle("Rage retry", &Vars::Misc::RageRetry.Value); HelpMarker("Will automatically reconnect when your health is low");
			if (Vars::Misc::RageRetry.Value)
			{
				WSlider("Rage Retry health", &Vars::Misc::RageRetryHealth.Value, 1, 99, "%d%%"); HelpMarker("Minimum health percentage that will cause a retry");
			}
			WToggle("Infinite Spectator time", &Vars::Misc::ExtendFreeze.Value); HelpMarker("spams extendfreeze in console");
			WToggle("Auto accept item drops", &Vars::Misc::AutoAcceptItemDrops.Value); HelpMarker("Automatically accepts all item drops");

			SectionTitle("Queueing");
			WToggle("Region selector", &Vars::Misc::RegionChanger.Value);

			MultiFlags({ "Europe",	"North America",	"South America",	"Asia",		"Africa",	"Australia",	"Unknown" },
				{ (1 << 0),	(1 << 1),			(1 << 2),			(1 << 3),	(1 << 4),	(1 << 5),		(1 << 6) },
				&Vars::Misc::RegionsAllowed.Value,
				"Regions"
			);
			WCombo("Match accept notification", &Vars::Misc::InstantAccept.Value, { "Default", "Instant join", "Freeze timer" }); HelpMarker("Will skip the 10 second delay before joining a match or let you never join");
			WCombo("Auto casual queue", &Vars::Misc::AutoCasualQueue.Value, { "Off", "In menu", "Always" }); HelpMarker("Automatically starts queueuing for casual");
			WToggle("Auto VAC Join", &Vars::Misc::AntiVAC.Value); HelpMarker("Tries to join a server while having a VAC ban");

			SectionTitle("Sound");
			MultiFlags({ "Footsteps", "Noisemaker" }, { 1 << 0, 1 << 1 }, &Vars::Misc::SoundBlock.Value, "Block Sounds###SoundRemovals");

		} EndChild();

		/* Column 2 */
		if (TableColumnChild("MiscCol2"))
		{
			SectionTitle("Chat");

			WToggle("Chat Flags", &Vars::Misc::ChatFlags.Value); HelpMarker("Adds advanced prefixes to chat messages");

			SectionTitle("Exploits");
			WToggle("Anti Autobalance", &Vars::Misc::AntiAutobal.Value); HelpMarker("Prevents auto balance by reconnecting to the server");
			WToggle("sv_cheats Bypass", &Vars::Misc::CheatsBypass.Value); HelpMarker("Allows you to use some sv_cheats commands (clientside)");
			WToggle("Noisemaker Spam", &Vars::Misc::NoisemakerSpam.Value); HelpMarker("Spams the noisemaker without reducing it's charges");
			WToggle("Ping reducer", &Vars::Misc::PingReducer.Value); HelpMarker("Reduces your ping on the scoreboard");
			if (Vars::Misc::PingReducer.Value)
			{
				WSlider("Target ping", &Vars::Misc::PingTarget.Value, 0, 200); HelpMarker("Target ping that should be reached");
			}

			SectionTitle("Followbot");
			WToggle("Enable Followbot###FollowbotEnable", &Vars::Misc::Followbot::Enabled.Value); HelpMarker("Follows a player around.");
			WToggle("Friends only###FollowbotFriends", &Vars::Misc::Followbot::FriendsOnly.Value); HelpMarker("Only follow friends");
			WSlider("Follow Distance###FollowbotDistance", &Vars::Misc::Followbot::Distance.Value, 50.f, 400.f, "%.0f"); HelpMarker("How close we should follow the target");

			/*SectionTitle("Leaderboard");
			WToggle("Send statistics", &Vars::Misc::StoreStatistics.Value); HelpMarker("Will send your steamid/kills/deaths/highest killstreak whenever you leave the server");

			if (Button("Open leaderboards", { GetWindowSize().x - 2 * GetStyle().WindowPadding.x, 20 }))
			{
				ShellExecute(0, 0, L"http://198.244.189.210:4077/leaderboard", 0, 0, SW_SHOW);
			}*/
		} EndChild();

		/* Column 3 */
		if (TableColumnChild("MiscCol3"))
		{


			SectionTitle("Utilities");
			if (Button("Full update", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("cl_fullupdate");
			if (Button("Reload HUD", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("hud_reloadscheme");
			if (Button("Restart sound", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("snd_restart");
			if (Button("Resync NoSpread", SIZE_FULL_WIDTH))
				F::NoSpread.Reset();
			if (Button("Stop sound", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("stopsound");
			if (Button("Status", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("status");
			if (Button("Ping", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("ping");
			if (Button("Pong", SIZE_FULL_WIDTH))
				F::Pong.IsOpen = !F::Pong.IsOpen;
			if (Button("Retry", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("retry");
			if (Button("Exit", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("exit");
			if (Button("Console", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("showconsole");
			if (Button("Demo playback", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("demoui");
			if (Button("Demo trackbar", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("demoui2");
			if (Button("Itemtest", SIZE_FULL_WIDTH))
				I::EngineClient->ClientCmd_Unrestricted("itemtest");
			if (Button("Fix Chams", SIZE_FULL_WIDTH))
			{
				F::DMEChams.CreateMaterials();
				F::Glow.CreateMaterials();
			}
			if (Button("Print Hashes", SIZE_FULL_WIDTH))
			{
				Hash::PrintHash();
			}
			//if (Button("CPrint", ImVec2(btnWidth, 20))){
			//	I::CenterPrint->Print((char*)"niggaz");
			//}


			if (!I::EngineClient->IsConnected())
			{
				if (Button("Unlock all achievements", SIZE_FULL_WIDTH))
				{
					F::Misc.UnlockAchievements();
				}
				if (Button("Lock all achievements", SIZE_FULL_WIDTH))
				{
					F::Misc.LockAchievements();
				}
			}
		} EndChild();

		EndTable();
	}
}
#pragma endregion

/* Settings Window */
void CMenu::SettingsWindow()
{
	using namespace ImGui;
	if (!ShowSettings) { return; }

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
	PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(10, 10));
	SetNextWindowSize(ImVec2(230.f, 0.f));

	if (Begin("Settings", &ShowSettings, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		PushItemWidth(-1);

		/* General Menu Settings */
		if (CollapsingHeader("Menu Settings"))
		{
			if (Checkbox("Old Design", &Vars::Menu::ModernDesign.Value)) { LoadStyle(); }
			Checkbox("Close Menu on Unfocus", &Vars::Menu::CloseOnUnfocus.Value);

			SetNextItemWidth(100);
			InputKeybind("Extra Menu key", Vars::Menu::MenuKey, true, true, "None");
		}

		Dummy({ 0, 5 });

		/* Config Tool Buttons */
		if (Button("Open configs folder", SIZE_FULL_WIDTH))
		{
			ShellExecuteA(nullptr, nullptr, g_CFG.GetConfigPath().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		}
		if (Button("Open visuals folder", SIZE_FULL_WIDTH))
		{
			ShellExecuteA(nullptr, nullptr, g_CFG.GetVisualsPath().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		}

		Dummy({ 0, 5 });

		/* Config Tabs */
		PushFont(SectionFont);
		PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
		PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
		if (BeginTable("ConfigTable", 2))
		{
			PushStyleColor(ImGuiCol_Text, TextLight.Value);
			if (TabButton("General", CurrentConfigTab == ConfigTab::General))
			{
				CurrentConfigTab = ConfigTab::General;
			}

			if (TabButton("Visuals", CurrentConfigTab == ConfigTab::Visuals))
			{
				CurrentConfigTab = ConfigTab::Visuals;
			}

			PopStyleColor(1);
			EndTable();
		}
		PopStyleVar(2);
		PopFont();

		static std::string selected;
		static std::string selectedvis;

		/* Config list*/
		if (CurrentConfigTab == ConfigTab::General)
		{
			// Current config
			const std::string cfgText = "Loaded: " + g_CFG.GetCurrentConfig();
			Text(cfgText.c_str());

			// Config name field
			std::string newConfigName = {};
			if (InputTextWithHint("###configname", "New config name", &newConfigName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::filesystem::exists(g_CFG.GetConfigPath() + "\\" + newConfigName))
				{
					g_CFG.SaveConfig(newConfigName);
				}
			}

			// Config list
			for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetConfigPath()))
			{
				if (!entry.is_regular_file()) { continue; }
				if (entry.path().extension() != g_CFG.ConfigExtension) { continue; }

				std::string configName = entry.path().filename().string();
				configName.erase(configName.end() - g_CFG.ConfigExtension.size(), configName.end());

				if (configName == selected)
				{
					const ImGuiStyle* style2 = &GetStyle();
					const ImVec4* colors2 = style2->Colors;
					ImVec4 buttonColor = colors2[ImGuiCol_Button];
					buttonColor.w *= .5f;
					PushStyleColor(ImGuiCol_Button, buttonColor);

					// Config name button
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();

					if (BeginTable("ConfigActions", 3))
					{
						// Save config button
						TableNextColumn();
						if (Button("Save", SIZE_FULL_WIDTH))
						{
							if (configName != g_CFG.GetCurrentConfig())
							{
								OpenPopup("Save config?");
							}
							else
							{
								g_CFG.SaveConfig(selected);
								selected.clear();
							}
						}

						// Load config button
						TableNextColumn();
						if (Button("Load", SIZE_FULL_WIDTH))
						{
							g_CFG.LoadConfig(selected);
							selected.clear();
							LoadStyle();
						}

						// Remove config button
						TableNextColumn();
						if (Button("Remove", SIZE_FULL_WIDTH))
						{
							OpenPopup("Remove config?");
						}

						// Dialogs
						{
							// Save config dialog
							if (BeginPopupModal("Save config?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to override this config?");

								Separator();
								if (Button("Yes, override!", ImVec2(150, 0)))
								{
									g_CFG.SaveConfig(selected);
									selected.clear();
									CloseCurrentPopup();
								}

								SameLine();
								if (Button("No", ImVec2(120, 0)))
								{
									CloseCurrentPopup();
								}
								EndPopup();
							}

							// Delete config dialog
							if (BeginPopupModal("Remove config?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to delete this config?");

								Separator();
								if (Button("Yes, remove!", ImVec2(150, 0)))
								{
									g_CFG.RemoveConfig(selected);
									selected.clear();
									CloseCurrentPopup();
								}
								SameLine();
								if (Button("No", ImVec2(150, 0)))
								{
									CloseCurrentPopup();
								}
								EndPopup();
							}
						}

						EndTable();
					}
				}
				else if (configName == g_CFG.GetCurrentConfig())
				{
					PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_ButtonActive]);
					std::string buttonText = "> " + configName + " <";
					if (Button(buttonText.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();
				}
				else
				{
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
				}
			}

		}
		else if (CurrentConfigTab == ConfigTab::Visuals)
		{
			// Current config
			const std::string cfgText = "Loaded: " + g_CFG.GetCurrentVisuals();
			Text(cfgText.c_str());

			// Config name field
			std::string newConfigName = {};

			if (InputTextWithHint("###configname", "New config name", &newConfigName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				if (!std::filesystem::exists(g_CFG.GetVisualsPath() + "\\" + newConfigName))
				{
					g_CFG.SaveVisual(newConfigName);
				}
			}

			// Visuals list
			for (const auto& entry : std::filesystem::directory_iterator(g_CFG.GetVisualsPath()))
			{
				if (!entry.is_regular_file()) { continue; }
				if (entry.path().extension() != g_CFG.ConfigExtension) { continue; }

				std::string configName = entry.path().filename().string();
				configName.erase(configName.end() - g_CFG.ConfigExtension.size(), configName.end());

				if (configName == selected)
				{
					const ImGuiStyle* style2 = &GetStyle();
					const ImVec4* colors2 = style2->Colors;
					ImVec4 buttonColor = colors2[ImGuiCol_Button];
					buttonColor.w *= .5f;
					PushStyleColor(ImGuiCol_Button, buttonColor);

					// Config name button
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();

					// Visuals action buttons
					if (BeginTable("ConfigActions", 3))
					{
						// Visuals config button
						TableNextColumn();
						if (Button("Save", SIZE_FULL_WIDTH))
						{
							if (configName != g_CFG.GetCurrentVisuals())
							{
								OpenPopup("Save visuals?");
							}
							else
							{
								g_CFG.SaveVisual(selected);
								selected.clear();
							}
						}

						// Load visuals button
						TableNextColumn();
						if (Button("Load", SIZE_FULL_WIDTH))
						{
							g_CFG.LoadVisual(selected);
							selected.clear();
							LoadStyle();
						}

						// Remove visual button
						TableNextColumn();
						if (Button("Remove", SIZE_FULL_WIDTH))
						{
							OpenPopup("Remove visuals?");
						}

						// Dialogs
						{
							// Save config dialog
							if (BeginPopupModal("Save visuals?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to override this config?");

								Separator();
								if (Button("Yes, override!", ImVec2(150, 0)))
								{
									g_CFG.SaveVisual(selected);
									selected.clear();
									CloseCurrentPopup();
								}

								SameLine();
								if (Button("No", ImVec2(120, 0)))
								{
									CloseCurrentPopup();
								}
								EndPopup();
							}

							// Delete config dialog
							if (BeginPopupModal("Remove visuals?", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
							{
								Text("Do you really want to delete this config?");

								Separator();
								if (Button("Yes, remove!", ImVec2(150, 0)))
								{
									g_CFG.RemoveVisual(selected);
									selected.clear();
									CloseCurrentPopup();
								}
								SameLine();
								if (Button("No", ImVec2(150, 0)))
								{
									CloseCurrentPopup();
								}
								EndPopup();
							}
						}

						EndTable();
					}
				}
				else if (configName == g_CFG.GetCurrentVisuals())
				{
					PushStyleColor(ImGuiCol_Button, GetStyle().Colors[ImGuiCol_ButtonActive]);
					std::string buttonText = "> " + configName + " <";
					if (Button(buttonText.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
					PopStyleColor();
				}
				else
				{
					if (Button(configName.c_str(), SIZE_FULL_WIDTH))
					{
						selected = configName;
					}
				}
			}
		}

		PopItemWidth();
		End();
	}

	PopStyleVar(2);
}




void CMenu::DrawCritDrag()
{
	if (Vars::CritHack::Indicators.Value)
	{

	}
}

void CMenu::AddDraggable(const char* szTitle, DragBox_t& info, bool bShouldDraw, bool setSize)
{
	constexpr int titlebarheight = 20;

	if (bShouldDraw)
	{
		if (info.update)
		{
			if (setSize)
			{
				ImGui::SetNextWindowSize({ static_cast<float>(info.w), static_cast<float>(info.h) + titlebarheight }, ImGuiCond_Always);
			}
			else
			{
				ImGui::SetNextWindowSize({ 80.f, 60.f }, ImGuiCond_Always);
			}
			ImGui::SetNextWindowPos({ static_cast<float>(info.x), static_cast<float>(info.y) - titlebarheight }, ImGuiCond_Always);
			info.update = false;
		}
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.1f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 50.f, 21.f });

		const auto bResize = setSize ? 0 : ImGuiWindowFlags_NoResize;
		if (ImGui::Begin(szTitle, nullptr, bResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus))
		{
			const ImVec2 winPos = ImGui::GetWindowPos();
			const ImVec2 winSize = ImGui::GetWindowSize();

			info.x = static_cast<int>(winPos.x);
			info.y = static_cast<int>(winPos.y + titlebarheight);	//	fix title bars
			if (setSize)
			{
				info.w = static_cast<int>(winSize.x);
				info.h = static_cast<int>(winSize.y - titlebarheight);	//	account for title bar fix
			}
			info.c = static_cast<int>(info.x + ((setSize ? info.w : 80.f) / 2));

			ImGui::End();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}
}

void CMenu::DrawKeybinds()
{
	if (!Vars::Menu::ShowKeybinds.Value) { return; }

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.35f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.f, 0.f, 0.f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4.f, 4.f });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
	ImGui::SetNextWindowSize({ 200.f, 0.f });

	if (ImGui::Begin("Keybinds", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
	{
		auto drawOption = [](const char* name, bool active)
			{
				ImGui::Text(name);
				ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::GetStyle().WindowPadding.x - ImGui::CalcTextSize(active ? "[On]" : "[Off]").x);
				ImGui::Text(active ? "[On]" : "[Off]");
			};

		auto isActive = [](bool active, bool needsKey, int key)
			{
				return active && (!needsKey || GetAsyncKeyState(key) & 0x8000);
			};

		drawOption("Aimbot", isActive(Vars::Aimbot::Global::Active.Value, Vars::Aimbot::Global::AimKey.Value, Vars::Aimbot::Global::AimKey.Value));
		drawOption("Auto Shoot", Vars::Aimbot::Global::AutoShoot.Value);
		drawOption("Double Tap", isActive(Vars::Misc::CL_Move::DTMode.Value != 3, Vars::Misc::CL_Move::DTMode.Value == 0, Vars::Misc::CL_Move::DoubletapKey.Value));
		drawOption("Anti Aim", Vars::AntiHack::AntiAim::Active.Value);
		drawOption("Fakelag", isActive(Vars::Misc::CL_Move::Fakelag.Value, Vars::Misc::CL_Move::FakelagOnKey.Value, Vars::Misc::CL_Move::FakelagKey.Value));
		drawOption("Triggerbot", isActive(Vars::Triggerbot::Global::Active.Value, Vars::Triggerbot::Global::TriggerKey.Value, Vars::Triggerbot::Global::TriggerKey.Value));
	}

	ImGui::End();
	ImGui::PopStyleVar(2);
	ImGui::PopStyleColor(3);
}

void CMenu::Render(IDirect3DDevice9* pDevice)
{
	if (!ConfigLoaded) { return; }

	static std::once_flag initFlag;
	std::call_once(initFlag, [&]
		{
			Init(pDevice);
		});

	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

	// Toggle menu (default is 'insert' can be changed in menu)
	static KeyHelper menuKey{ &Vars::Menu::MenuKey.Value };
	if (menuKey.Pressed() || GetAsyncKeyState(VK_INSERT) & 0x1)
	{
		F::Menu.IsOpen = !F::Menu.IsOpen;
		I::ViewRender->SetScreenOverlayMaterial(nullptr);
		I::VGuiSurface->SetCursorAlwaysVisible(F::Menu.IsOpen);
	}

	if (F::Menu.IsOpen && Vars::Menu::CloseOnUnfocus.Value)
	{
		if (!Utils::IsGameWindowInFocus())
		{
			F::Menu.IsOpen = false;
			I::ViewRender->SetScreenOverlayMaterial(nullptr);
			I::VGuiSurface->SetCursorAlwaysVisible(F::Menu.IsOpen);
		}
	}

	// Begin current frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::PushFont(Verdana);

	// Window that should always be visible
	DrawKeybinds();
	F::Radar.DrawWindow();

	if (IsOpen)
	{
		DrawMenu();
		AddDraggable("Conditions", Vars::Visuals::OnScreenConditions.Value, Vars::Visuals::DrawOnScreenConditions.Value, true);
		AddDraggable("Ping", Vars::Visuals::OnScreenPing.Value, Vars::Visuals::DrawOnScreenPing.Value, true);
		AddDraggable("DT Bar", Vars::Misc::CL_Move::DTIndicator.Value, Vars::Misc::CL_Move::DTBarStyle.Value, true);
		AddDraggable("Crits", Vars::CritHack::IndicatorPos.Value, Vars::CritHack::Indicators.Value, true);

		SettingsWindow();
		F::MaterialEditor.Render();
		F::PlayerList.Render();
		F::Pong.Render();
	}

	// End frame and render
	ImGui::PopFont();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
}

void CMenu::LoadStyle()
{
	// Style & Colors
	{



		ItemWidth = 120.f;

		Accent = ImGui::ColorToVec(Vars::Menu::Colors::MenuAccent.Value);
		AccentDark = ImColor(Accent.Value.x * 0.8f, Accent.Value.y * 0.8f, Accent.Value.z * 0.8f, Accent.Value.w);

		auto& style = ImGui::GetStyle();
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Center window title
		style.WindowMinSize = ImVec2(100, 100);
		style.WindowPadding = ImVec2(0, 0);
		style.WindowBorderSize = 1.f;
		style.ButtonTextAlign = ImVec2(0.5f, 0.4f); // Center button text
		style.FrameBorderSize = 1.f; // Old menu feeling
		style.FrameRounding = 6.f;
		style.ChildBorderSize = 1.f;
		style.ChildRounding = 0.f;
		style.GrabMinSize = 15.f;
		style.GrabRounding = 0.f;
		style.ScrollbarSize = 9.f;
		style.ScrollbarRounding = 6.f;
		style.ItemSpacing = ImVec2(12.f, 8.f);

		ImVec4* colors = style.Colors;
		style.Colors[ImGuiCol_Text] = ImVec4(238, 201, 116, 255);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);


		// Alternative Designs
		if (Vars::Menu::ModernDesign.Value)
		{


			ItemWidth = 120.f;

			style.WindowTitleAlign = ImVec2(0.5f, 0.5f); // Center window title
			style.WindowMinSize = ImVec2(100, 100);
			style.WindowPadding = ImVec2(0, 0);
			style.WindowBorderSize = 1.f;
			style.ButtonTextAlign = ImVec2(0.5f, 0.4f); // Center button text
			style.FrameBorderSize = 1.f; // Old menu feeling
			style.FrameRounding = 0.f;
			style.ChildBorderSize = 1.f;
			style.ChildRounding = 0.f;
			style.GrabMinSize = 15.f;
			style.GrabRounding = 0.f;
			style.ScrollbarSize = 4.f;
			style.ScrollbarRounding = 6.f;
			style.ItemSpacing = ImVec2(8.f, 5.f);

			ImVec4* colors = style.Colors;
			style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
			style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
			style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
			style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
			style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
			style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
			style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
			style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
			style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
			style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
			style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
			style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
			style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
			style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
			style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
			style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
			style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
		}
	}

	// Misc
	{
		//TitleGradient.ClearMarks();
		/*TitleGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.3f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(0.5f, Accent);
		TitleGradient.AddMark(0.7f, ImColor(0, 0, 0, 0));
		TitleGradient.AddMark(1.f, ImColor(0, 0, 0, 0));*/
	}

	{
		MainGradient.ClearMarks();
		MainGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(0.2f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(0.5f, Accent);
		MainGradient.AddMark(0.8f, ImColor(0, 0, 0, 0));
		MainGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}

	{
		TabGradient.ClearMarks();
		TabGradient.AddMark(0.f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(0.2f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(0.5f, ImColor(255, 255, 255));
		TabGradient.AddMark(0.8f, ImColor(0, 0, 0, 0));
		TabGradient.AddMark(1.f, ImColor(0, 0, 0, 0));
	}
}

void CMenu::Init(IDirect3DDevice9* pDevice)
{
	// Initialize ImGui and device
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(FindWindowA(nullptr, "Team Fortress 2"));
	ImGui_ImplDX9_Init(pDevice);

	// Fonts
	{
		const auto& io = ImGui::GetIO();

		auto fontConfig = ImFontConfig();
		fontConfig.OversampleH = 2;

		constexpr ImWchar fontRange[]{ 0x0020, 0x00FF,0x0400, 0x044F, 0 }; // Basic Latin, Latin Supplement and Cyrillic

		VerdanaSmall = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 12.0f, &fontConfig, fontRange);
		Verdana = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 14.0f, &fontConfig, fontRange);
		VerdanaBold = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdanab.ttf)", 18.0f, &fontConfig, fontRange);

		SectionFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdana.ttf)", 16.0f, &fontConfig, fontRange);
		TitleFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\verdanab.ttf)", 20.0f, &fontConfig, fontRange);

		constexpr ImWchar iconRange[]{ ICON_MIN_MD, ICON_MAX_MD, 0 };
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.PixelSnapH = true;
		IconFont = io.Fonts->AddFontFromMemoryCompressedTTF(MaterialFont_compressed_data, MaterialFont_compressed_size, 16.f, &iconConfig, iconRange);

		io.Fonts->Build();
	}

	LoadStyle();
	F::MaterialEditor.Init();
}
