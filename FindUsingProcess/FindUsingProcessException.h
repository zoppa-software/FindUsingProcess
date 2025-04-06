#pragma once

/// <summary>内部例外クラス。</summary>
public ref class FindUsingProcessException :
	public System::Exception
{
public:
	/// <summary>コンストラクタ。</summary>
	/// <param name="format">メッセージフォーマット。</param>
	/// <param name="args">引数。</param>
	FindUsingProcessException(System::String^ format, ... array<Object^>^ args) :
		Exception(System::String::Format(format, args))
	{
	}

	/// <summary>コンストラクタ。</summary>
	/// <param name="message">メッセージ。</param>
	/// <param name="innerException">内部例外。</param>
	FindUsingProcessException(System::String^ message, Exception^ innerException) :
		Exception(message, innerException)
	{
	}
};
