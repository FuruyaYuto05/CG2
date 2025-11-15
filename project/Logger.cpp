#include "Logger.h"
#include <Windows.h> // OutputDebugStringAを使用するため

namespace Logger
{
    void Log(const std::string& message)
    {
        OutputDebugStringA(message.c_str());
    }
}