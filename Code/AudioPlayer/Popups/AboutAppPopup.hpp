#pragma once

class AboutPopup
{
public:
	static void Open();
	static void Draw();

	AboutPopup() = delete;
	AboutPopup(const AboutPopup&) = delete;
	AboutPopup(AboutPopup&&) = delete;
	~AboutPopup() = delete;

private:
	inline static bool ShouldOpen = false;
};