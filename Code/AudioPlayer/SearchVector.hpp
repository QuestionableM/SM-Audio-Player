#pragma once

#include "Utils/String.hpp"
#include "imgui_include.hpp"

#include <string>
#include <vector>

template<typename T>
class SearchVector
{
public:
	std::string search_str;
	std::vector<T*> storage;
	std::vector<T*> search_results;

	inline void RenderInputBox(const char* label)
	{
		const std::size_t v_search_sz_before = search_str.size();
		if (ImGui::InputText(label, &search_str))
			this->UpdateSearchResults(v_search_sz_before);
	}

	inline void UpdateSearchResults(const std::size_t& v_prev_str_length)
	{
		const std::string v_lower_search = String::ToLower(search_str);

		if (v_lower_search.size() < v_prev_str_length || v_lower_search.size() == 0 || v_prev_str_length == 0)
		{
			search_results.clear();

			for (auto& v_cur_item : storage)
				if (v_cur_item->m_lower_name.find(v_lower_search) != std::string::npos)
					search_results.push_back(v_cur_item);
		}
		else
		{
			std::size_t v_new_search_sz = 0;

			for (auto& v_cur_item : search_results)
				if (v_cur_item->m_lower_name.find(v_lower_search) != std::string::npos)
					search_results[v_new_search_sz++] = v_cur_item;

			search_results.resize(v_new_search_sz);
		}
	}

	std::vector<T*>& GetVector()
	{
		return search_str.empty() ? storage : search_results;
	}

	void ClearSearch() { search_results.clear(); }
	void ClearMemory()
	{
		for (auto& v_cur_item : storage)
			delete v_cur_item;

		storage.clear();
		search_results.clear();
	}

	SearchVector() = default;
	~SearchVector() { this->ClearMemory(); }
};