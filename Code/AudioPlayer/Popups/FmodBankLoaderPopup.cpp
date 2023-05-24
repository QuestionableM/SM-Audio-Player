#include "FmodBankLoaderPopup.hpp"

#include "AudioPlayer/AudioPlayer.hpp"
#include "Utils/String.hpp"
#include "Utils/File.hpp"

#include "imgui_include.hpp"

#define BankLoaderPopup_Name "bank_loader_popup"

void BankLoaderPopup::CallOpenFileDialog(const wchar_t* title, std::string& output_str)
{
	const std::wstring v_path = File::OpenFileDialog(
		title,
		FOS_FILEMUSTEXIST,
		{ { L"FMOD Bank File", L"*.bank" } },
		APP_PTR()->m_window->GetHandle()
	);

	if (v_path.empty())
		return;

	output_str = String::ToUtf8(v_path);
}

void BankLoaderPopup::Open(const std::function<void(const std::string&, const std::string&)> v_callback_ptr)
{
	if (ImGui::IsPopupOpen(BankLoaderPopup_Name))
		return;

	BankLoaderPopup::ShouldOpen = true;
	BankLoaderPopup::Callback = v_callback_ptr;
}

void BankLoaderPopup::Draw()
{
	if (BankLoaderPopup::ShouldOpen)
	{
		BankLoaderPopup::ShouldOpen = false;
		ImGui::OpenPopup(BankLoaderPopup_Name, ImGuiPopupFlags_NoOpenOverExistingPopup);
	}

	if (!ImGui::BeginPopup(BankLoaderPopup_Name))
		return;

	ImGui::InputText("Bank Path", &BankLoaderPopup::BankPath);
	ImGui::SameLine();
	if (ImGui::Button("...###blp_path1"))
		BankLoaderPopup::CallOpenFileDialog(L"Select a FMOD Bank File", BankLoaderPopup::BankPath);

	ImGui::InputText("Bank Strings Path", &BankLoaderPopup::BankStringsPath);
	ImGui::SameLine();
	if (ImGui::Button("...###blp_path2"))
		BankLoaderPopup::CallOpenFileDialog(L"Select a FMOD Strings Bank File", BankLoaderPopup::BankStringsPath);

	ImGui::Separator();

	ImGui::BeginDisabled(BankLoaderPopup::BankPath.empty() || BankLoaderPopup::BankStringsPath.empty());

	if (ImGui::Button("Load"))
	{
		BankLoaderPopup::Callback(BankLoaderPopup::BankPath, BankLoaderPopup::BankStringsPath);
		ImGui::CloseCurrentPopup();
	}

	ImGui::EndDisabled();

	ImGui::SameLine();
	if (ImGui::Button("Cancel"))
		ImGui::CloseCurrentPopup();

	ImGui::EndPopup();
}