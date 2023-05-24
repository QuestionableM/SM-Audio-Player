#include "AboutAppPopup.hpp"

#include "imgui_include.hpp"

#define AboutPopup_Name "about_app_popup"

void AboutPopup::Open()
{
	if (ImGui::IsPopupOpen(AboutPopup_Name))
		return;

	AboutPopup::ShouldOpen = true;
}

void AboutPopup::Draw()
{
	if (AboutPopup::ShouldOpen)
	{
		AboutPopup::ShouldOpen = false;
		ImGui::OpenPopup(AboutPopup_Name, ImGuiPopupFlags_NoOpenOverExistingPopup);
	}

	if (!ImGui::BeginPopup(AboutPopup_Name))
		return;

	ImGui::Text("Created by: Questionable Mark");
	ImGui::Text("Powered by: FMOD");
	ImGui::Text("Build Version: " __TIMESTAMP__);

	ImGui::EndPopup();
}