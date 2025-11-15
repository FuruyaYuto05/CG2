#include "StringUtility.h"
#include <Windows.h> // MultiByteToWideChar, WideCharToMultiByteを使用するため
#include <cstdint>   // static_castにcstdintを使用する場合（ここでは不要だが慣例として）

namespace StringUtility
{
    // stringをwstringに変換 (main.cppのConvertString関数)
    std::wstring ConvertString(const std::string& str)
    {
        if (str.empty()) {
            return std::wstring();
        }

        // 必要なバッファサイズを計算
        auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0
        );
        if (sizeNeeded == 0) {
            return std::wstring();
        }
        std::wstring result(sizeNeeded, 0);

        // 変換を実行
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
        return result;
    }

    // wstringをstringに変換 (main.cppのConverString関数)
    std::string ConvertString(const std::wstring& str)
    {
        if (str.empty()) {
            return std::string();
        }

        // 必要なバッファサイズを計算
        auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
        if (sizeNeeded == 0) {
            return std::string();
        }
        std::string result(sizeNeeded, 0);

        // 変換を実行
        WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
        return result;
    }
}