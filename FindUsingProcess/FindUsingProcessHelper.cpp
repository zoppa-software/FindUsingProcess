#include "pch.h"
#include <Windows.h>
#include "FindUsingProcessHelper.h"

using namespace System;
using namespace System::Runtime::InteropServices;

String^ FindUsingProcessHelper::GetErrorMessage(DWORD errorCode)
{
    LPWSTR messageBuffer = nullptr;
    try {
		// エラーコードをメッセージに変換する
        size_t size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer,
            0,
            NULL
        );

		// メッセージが取得できなかった場合、エラーコードをそのまま返す
		if (size == 0) {
			return String::Format(L"解析できないエラーコード: {0}", errorCode);
		}

		// 取得したメッセージを文字列に変換して返す
        return Marshal::PtrToStringUni((IntPtr)messageBuffer);
    }
    catch (Exception^ e) {
        return String::Format(L"不明な例外:{0}", e->Message);
    }
    finally {
        LocalFree(messageBuffer);
    }
}