#pragma once

/// <summary>共通ヘルパークラス。</summary>
ref class FindUsingProcessHelper sealed
{
public:
	/// <summary>エラーコードをメッセージ化します。</summary>
	/// <param name="errorCode">エラーコード。</param>
	/// <returns>メッセージ。</returns>
	static System::String^ GetErrorMessage(DWORD errorCode);
};