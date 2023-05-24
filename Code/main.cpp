#include "AudioPlayer/AudioPlayer.hpp"
#include "Utils/Console.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	CreateDebugConsole(L"ImReClass Debug Console");

	AudioPlayer v_app{};
	if (!v_app.EnterLoop(L"SM Audio Player"))
		return -1;

	return 0;
}