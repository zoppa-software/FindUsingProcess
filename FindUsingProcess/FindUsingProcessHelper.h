#pragma once

/// <summary>���ʃw���p�[�N���X�B</summary>
ref class FindUsingProcessHelper sealed
{
public:
	/// <summary>�G���[�R�[�h�����b�Z�[�W�����܂��B</summary>
	/// <param name="errorCode">�G���[�R�[�h�B</param>
	/// <returns>���b�Z�[�W�B</returns>
	static System::String^ GetErrorMessage(DWORD errorCode);
};