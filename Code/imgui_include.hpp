#pragma once

#include "windows_include.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_stdlib.h>

#include "Utils/Console.hpp"

#include <functional>
#include <stdio.h>

namespace ImGui
{
	constexpr ImGuiInputTextFlags DefaultAdvancedTextboxFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo;

	template<typename T>
	inline static bool CheckboxEx(const char* label, T& type)
	{
		return ImGui::Checkbox(label, reinterpret_cast<bool*>(&type));
	}

	template<typename T>
	inline constexpr ImGuiDataType GetImGuiType()
	{
		static_assert(std::is_arithmetic_v<T> && sizeof(T) >= 1 && sizeof(T) <= 8, "Template type must be arithmetic! And be of size [1,8]");

		if constexpr (std::is_same_v<T, float>) {
			return ImGuiDataType_Float;
		} else if constexpr (std::is_same_v<T, double>) {
			return ImGuiDataType_Double;
		} else {
			if constexpr (std::is_unsigned_v<T>) {
				if constexpr (sizeof(T) == 1) {
					return ImGuiDataType_U8;
				} else if constexpr (sizeof(T) == 2) {
					return ImGuiDataType_U16;
				} else if constexpr (sizeof(T) == 4) {
					return ImGuiDataType_U32;
				} else if constexpr (sizeof(T) == 8) {
					return ImGuiDataType_U64;
				}
			} else {
				if constexpr (sizeof(T) == 1) {
					return ImGuiDataType_S8;
				} else if constexpr (sizeof(T) == 2) {
					return ImGuiDataType_S16;
				} else if constexpr (sizeof(T) == 4) {
					return ImGuiDataType_S32;
				} else if constexpr (sizeof(T) == 8) {
					return ImGuiDataType_S64;
				}
			}
		}
	}

	template<typename t_function>
	inline void BetterSelectable(const char* label, bool is_selected, const t_function& func_ptr)
	{
		if (ImGui::Selectable(label, &is_selected))
			func_ptr();

		if (is_selected)
			ImGui::SetItemDefaultFocus();
	}

	template<typename T>
	inline bool BetterInput(const char* label, T& value, const T& step, const T& step_fast, const char* fmt, ImGuiInputTextFlags flags = 0)
	{
		return ImGui::InputScalar(label, ImGui::GetImGuiType<T>(), &value, &step, &step_fast, fmt, flags);
	}

	template<typename T, int t_num_items = 1>
	inline bool BetterSlider(const char* name, T& value, const T& min_value, const T& max_value, const char* format, ImGuiSliderFlags flags = 0)
	{
		return ImGui::SliderScalarN(name, ImGui::GetImGuiType<T>(), &value, t_num_items, &min_value, &max_value, format, flags);
	}

	struct AdvancedInputBoxData
	{
		inline static int Selection = -1;
		inline static int Counter = 0;
	};

	inline static void NewFrameCustom()
	{
		AdvancedInputBoxData::Counter = 0;
		ImGui::NewFrame();
	}

	template<size_t t_buffer_size>
	static void DrawAdvancedTextInput(
		const char* current_value,
		const bool is_active,
		const ImGuiInputTextFlags& flags, 
		const std::function<void (const std::string&)>& v_enter_callback)
	{
		const std::uintptr_t v_custom_addr = reinterpret_cast<std::uintptr_t>(&v_enter_callback) + AdvancedInputBoxData::Counter;
		ImGui::PushID(reinterpret_cast<const void*>(v_custom_addr));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));

		if (is_active)
		{
			static char v_text_input_buffer_cpy[t_buffer_size + 1] = "0";
			if (AdvancedInputBoxData::Counter == AdvancedInputBoxData::Selection)
			{
				ImGui::SetKeyboardFocusHere(0);

				if (flags & ImGuiInputTextFlags_NoHorizontalScroll)
					ImGui::SetNextItemWidth(ImGui::CalcTextSize(v_text_input_buffer_cpy).x);

				ImGui::InputText(
					"##adv_txt_input",
					v_text_input_buffer_cpy,
					sizeof(v_text_input_buffer_cpy),
					flags
				);

				if (ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					if (v_enter_callback != nullptr)
					{
						if ((flags & ImGuiInputTextFlags_ReadOnly) == 0)
							v_enter_callback(std::string(v_text_input_buffer_cpy));
					}

					AdvancedInputBoxData::Selection = -1;
				}

				if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					AdvancedInputBoxData::Selection = -1;

				if (ImGui::IsKeyPressed(ImGuiKey_Escape))
					AdvancedInputBoxData::Selection = -1;
			}
			else
			{
				ImGui::Text("%s", current_value);
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					AdvancedInputBoxData::Selection = AdvancedInputBoxData::Counter;

					const std::size_t v_buff_sz = std::min(std::strlen(current_value), t_buffer_size);
					std::memcpy(v_text_input_buffer_cpy, current_value, v_buff_sz);
					v_text_input_buffer_cpy[v_buff_sz] = 0;
				}
			}
		}
		else
		{
			ImGui::Text("%s", current_value);
		}

		ImGui::PopStyleVar(2);
		ImGui::PopID();

		AdvancedInputBoxData::Counter++;
	}

	template<size_t t_buffer_size>
	static void DrawAdvancedTextInputColored(
		const ImVec4& v_color,
		const char* current_value,
		const bool is_active,
		const ImGuiInputTextFlags& flags,
		const std::function<void (const std::string&)>& v_enter_callback)
	{
		const std::uintptr_t v_custom_addr = reinterpret_cast<std::uintptr_t>(&v_enter_callback) + AdvancedInputBoxData::Counter;
		ImGui::PushID(reinterpret_cast<const void*>(v_custom_addr));

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, ImGui::GetStyle().ItemSpacing.y));

		if (is_active)
		{
			static char v_text_input_buffer_cpy[t_buffer_size + 1] = "0";
			if (AdvancedInputBoxData::Counter == AdvancedInputBoxData::Selection)
			{
				ImGui::SetKeyboardFocusHere(0);

				if (flags & ImGuiInputTextFlags_NoHorizontalScroll)
					ImGui::SetNextItemWidth(ImGui::CalcTextSize(v_text_input_buffer_cpy).x);

				ImGui::InputText(
					"##adv_txt_input",
					v_text_input_buffer_cpy,
					sizeof(v_text_input_buffer_cpy),
					flags
				);

				if (ImGui::IsKeyPressed(ImGuiKey_Enter))
				{
					if (v_enter_callback != nullptr)
					{
						if ((flags & ImGuiInputTextFlags_ReadOnly) == 0)
							v_enter_callback(std::string(v_text_input_buffer_cpy));
					}

					AdvancedInputBoxData::Selection = -1;
				}

				if (!ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
					AdvancedInputBoxData::Selection = -1;

				if (ImGui::IsKeyPressed(ImGuiKey_Escape))
					AdvancedInputBoxData::Selection = -1;
			}
			else
			{
				ImGui::TextColored(v_color, "%s", current_value);
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
				{
					AdvancedInputBoxData::Selection = AdvancedInputBoxData::Counter;

					const std::size_t v_buff_sz = std::min(std::strlen(current_value), t_buffer_size);
					std::memcpy(v_text_input_buffer_cpy, current_value, v_buff_sz);
					v_text_input_buffer_cpy[v_buff_sz] = 0;
				}
			}
		}
		else
		{
			ImGui::TextColored(v_color, "%s", current_value);
		}

		ImGui::PopStyleVar(2);
		ImGui::PopID();

		AdvancedInputBoxData::Counter++;
	}

	//List box helper functions

	void FollowListBoxSelection(const std::size_t& v_cur_idx, const std::size_t& v_num_items);
	bool UpdateCurrentListBoxIndex(std::size_t& v_lbox_idx, const std::size_t& v_item_count);
}