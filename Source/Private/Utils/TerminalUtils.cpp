#include "Utils/TerminalUtils.h"
#include "NovaCore.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#endif

using namespace Core;

namespace Utils
{
    bool TerminalUtils::SetTerminalSize(int width, int height)
    {
        NOVA_LOG(("Setting terminal size to " + std::to_string(width) + "x" + std::to_string(height)).c_str(), LogType::Debug);
        
#ifdef _WIN32
        // Windows implementation
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE)
            return false;

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
            return false;

        // Set window size
        SMALL_RECT windowSize = { 0, 0, static_cast<SHORT>(width - 1), static_cast<SHORT>(height - 1) };
        if (!SetConsoleWindowInfo(hConsole, TRUE, &windowSize))
            return false;

        // Set buffer size
        COORD bufferSize = { static_cast<SHORT>(width), static_cast<SHORT>(height) };
        if (!SetConsoleScreenBufferSize(hConsole, bufferSize))
            return false;
        
        NOVA_LOG("Terminal size set successfully on Windows", LogType::Debug);
        return true;
#elif defined(__APPLE__) || defined(__linux__)
        // macOS and Linux implementation
        // Use ANSI escape codes to resize terminal
        printf("\033[8;%d;%dt", height, width);
        fflush(stdout);
        NOVA_LOG("Terminal size set using ANSI escape codes on Unix-like system", LogType::Debug);
        return true;
#else
        NOVA_LOG("Terminal size setting not supported on this platform", LogType::Warning);
        return false;
#endif
    }

    bool TerminalUtils::GetTerminalSize(int& width, int& height)
    {
#ifdef _WIN32
        // Windows implementation
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole == INVALID_HANDLE_VALUE)
            return false;

        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
            return false;

        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        return true;
#else
        // Unix-like systems (macOS, Linux)
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
            return false;
            
        width = ws.ws_col;
        height = ws.ws_row;
        return true;
#endif
    }
}
