#include "AudioPlayer/AudioPlayer.hpp"
#include "AudioPlayer/AudioExceptions.hpp"

#include "Utils/Console.hpp"

int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	CreateDebugConsole(L"SM Audio Player Debug Console");

	try
	{
		AudioPlayer v_app{};
		if (!v_app.EnterLoop(L"SM Audio Player"))
			return -1;
	}
	catch (const AudioPlayerException& exc)
	{
		MessageBoxA(NULL, exc.message().c_str(), exc.title().c_str(), MB_OK);
	}
	catch (...)
	{
		MessageBoxA(NULL, "Unknown Exception", "Exception", MB_OK);
	}

	return 0;
}