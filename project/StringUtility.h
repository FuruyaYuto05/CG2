#pragma once
#include <string> // std::string, std::wstringを使用するため

namespace StringUtility
{
    // stringをwstringに変換
    std::wstring ConvertString(const std::string& str);

    // wstringをstringに変換
    std::string ConvertString(const std::wstring& str);
}
