#include "imgui_include.hpp"

void ImGui::FollowListBoxSelection(const std::size_t& v_cur_idx, const std::size_t& v_num_items)
{
	if (v_cur_idx == -1)
		return;

	const std::size_t v_item_count_m1 = v_num_items - 1;

	const float v_single_item_height = ImGui::GetTextLineHeightWithSpacing();
	const float v_total_item_height = v_single_item_height * v_num_items;

	const float v_cur_window_min = ImGui::GetScrollY();
	const float v_cur_window_max = v_cur_window_min + v_total_item_height;

	const std::size_t v_min_idx = static_cast<std::size_t>(v_cur_window_min / v_single_item_height);
	const std::size_t v_max_idx = static_cast<std::size_t>(v_cur_window_max / v_single_item_height);

	if (v_cur_idx < v_min_idx)
	{
		ImGui::SetScrollY(static_cast<float>(v_cur_idx) * v_single_item_height);
	}
	else if (v_cur_idx >= v_max_idx && v_cur_idx >= v_item_count_m1)
	{
		const std::size_t v_new_idx = v_cur_idx - v_item_count_m1;
		ImGui::SetScrollY(static_cast<float>(v_new_idx) * v_single_item_height);
	}
}

bool ImGui::UpdateCurrentListBoxIndex(std::size_t& v_lbox_idx, const std::size_t& v_item_count)
{
	const bool is_up_pressed = ImGui::IsKeyPressed(ImGuiKey_UpArrow);
	const bool is_down_pressed = ImGui::IsKeyPressed(ImGuiKey_DownArrow);

	if (!is_up_pressed && !is_down_pressed)
		return false;

	const bool is_not_empty = (v_item_count > 0);

	if (is_not_empty && v_lbox_idx == -1)
	{
		v_lbox_idx = 0;
		return true;
	}
	else if (is_up_pressed && v_lbox_idx > 0)
	{
		v_lbox_idx--;
		return true;
	}
	else if (is_down_pressed && is_not_empty)
	{
		if (v_lbox_idx < (v_item_count - 1))
		{
			v_lbox_idx++;
			return true;
		}
	}

	return false;
}