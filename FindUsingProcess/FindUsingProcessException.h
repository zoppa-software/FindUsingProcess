#pragma once

/// <summary>������O�N���X�B</summary>
public ref class FindUsingProcessException :
	public System::Exception
{
public:
	/// <summary>�R���X�g���N�^�B</summary>
	/// <param name="format">���b�Z�[�W�t�H�[�}�b�g�B</param>
	/// <param name="args">�����B</param>
	FindUsingProcessException(System::String^ format, ... array<Object^>^ args) :
		Exception(System::String::Format(format, args))
	{
	}

	/// <summary>�R���X�g���N�^�B</summary>
	/// <param name="message">���b�Z�[�W�B</param>
	/// <param name="innerException">������O�B</param>
	FindUsingProcessException(System::String^ message, Exception^ innerException) :
		Exception(message, innerException)
	{
	}
};
