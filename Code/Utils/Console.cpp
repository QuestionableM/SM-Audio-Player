#include "Console.hpp"

#ifdef _DEBUG
namespace Engine
{
	HANDLE Console::Handle = NULL;
	__ConsoleOutputHandler Console::Out = __ConsoleOutputHandler();

	bool Console::CreateEngineConsole(const wchar_t* title)
	{
		if (Console::Handle == NULL)
		{
			if (AllocConsole())
			{
				SetConsoleOutputCP(CP_UTF8);
				SetConsoleTitleW(title);

				Console::Handle = GetStdHandle(STD_OUTPUT_HANDLE);

				return true;
			}
		}

		return false;
	}

	void Console::DestroyConsole()
	{
		if (Console::Handle == NULL) return;

		FreeConsole();
		Console::Handle = NULL;
	}
}
#endif