#include "pch.h"
#include <Windows.h>
#include <RestartManager.h>
#include <vcruntime.h>
#include "FindUsingProcessException.h"
#include "FindUsingProcessHelper.h"

using namespace System;
using namespace System::Text::RegularExpressions;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace System::Collections::Generic;

#pragma comment(lib, "Rstrtmgr.lib")

List<FileInfo^>^ CollectTargetFiles(String^ targetPath);
void CheckUsing(String^ targetPath);

/// <summary>�G���g���|�C���g�B</summary>
/// <param name="args">�A�v���P�[�V���������B</param>
/// <returns>�߂�l�B</returns>
int main(array<String ^> ^args)
{
	Console::Out->WriteLine(L"Find using Process v1.0.0");
	Console::Out->WriteLine(L"    by zoppa-software");

	try {
		// �R�}���h���C���������擾
		if (args->Length == 0) {
			throw gcnew FindUsingProcessException(L"�t�@�C�������w�肵�Ă��������B");
		}

		// �Ώۃt�@�C�����擾���ă��[�v
		for each (FileInfo ^ fpath in CollectTargetFiles(args[0])) {
			CheckUsing(fpath->FullName);
		}
	}
	catch (Exception^ e) {
		Console::Error->WriteLine(e->Message);
		return 1;
	}

    return 0;
}

/// <summary>�w�肳�ꂽ�p�X����t�@�C���������W���܂��B</summary>
/// <param name="targetPath">�Ώۃp�X�B</param>
/// <returns>�t�@�C�����̃��X�g�B</returns>
List<FileInfo^>^ CollectTargetFiles(String^ targetPath)
{
	if (!Regex::IsMatch(targetPath, L"^[a-zA-z]:\\\\")) {
		targetPath = System::Environment::CurrentDirectory + L"\\" + targetPath;
	}
	Console::Out->WriteLine(targetPath);

	if (File::Exists(targetPath)) {
		List<FileInfo^>^ res = gcnew List<FileInfo^>();
		res->Add(gcnew FileInfo(targetPath));
		return res;
	}
	else if (Directory::Exists(targetPath)) {
		DirectoryInfo^ dirInfo = gcnew DirectoryInfo(targetPath);
		return gcnew List<FileInfo^>(dirInfo->EnumerateFiles("*", System::IO::SearchOption::AllDirectories));
	}
	else {
		String^ fname = Path::GetFileName(targetPath);
		DirectoryInfo^ dirInfo = gcnew DirectoryInfo(Path::GetDirectoryName(targetPath));
		if (dirInfo->Exists) {
			return gcnew List<FileInfo^>(dirInfo->EnumerateFiles(fname, System::IO::SearchOption::AllDirectories));
		}
		else {
			throw gcnew FindUsingProcessException(L"�w�肳�ꂽ�p�X�͑��݂��܂���B");
		}
	}
}

/// <summary>�w�肳�ꂽ�t�@�C�����g�p���Ă���v���Z�X���`�F�b�N���܂��B</summary>
/// <param name="targetPath">�Ώۃt�@�C���̃p�X�B</param>
/// <exception cref="FindUsingProcessException">�G���[�����������ꍇ�B</exception>
void CheckUsing(String^ targetPath)
{
	Console::Out->WriteLine(L"�Ώۃt�@�C��:{0}", targetPath);

	// �Z�b�V�����̊J�n
	DWORD session;
	WCHAR session_key[CCH_RM_SESSION_KEY + 1]{};
	DWORD sessionErr = RmStartSession(&session, 0, session_key);
	if (sessionErr != ERROR_SUCCESS) {
		throw gcnew FindUsingProcessException(L"�Z�b�V�����̊J�n�Ɏ��s: {0}", FindUsingProcessHelper::GetErrorMessage(sessionErr));
	}

	LPCWSTR targetFilePtr = nullptr;
	UINT n_proc_info_needed = 0;
	UINT n_proc_info = 256;
	RM_PROCESS_INFO* rgpi = new RM_PROCESS_INFO[n_proc_info];
	ZeroMemory(rgpi, sizeof(RM_PROCESS_INFO) * n_proc_info);
	try {
		// �Ώۃt�@�C���̃p�X���l�C�e�B�u������ɕϊ�
		targetFilePtr = (const LPCWSTR)(Marshal::StringToHGlobalUni(targetPath)).ToPointer();

		// �Ώۃt�@�C����o�^
		const int files_n = 1;
		DWORD resErr = RmRegisterResources(session, files_n, &targetFilePtr, 0, NULL, 0, NULL);
		if (resErr != ERROR_SUCCESS) {
			throw gcnew FindUsingProcessException(L"�t�@�C���̓o�^�Ɏ��s: {0}", FindUsingProcessHelper::GetErrorMessage(resErr));
		}

		// ���̃t�@�C�����g�p���Ă���v���Z�X�̈ꗗ���擾
		DWORD reason;
		while (true) {
			DWORD listError = RmGetList(session, &n_proc_info_needed, &n_proc_info, rgpi, &reason);
			switch (listError)
			{
			case ERROR_SUCCESS:
				goto LOOP_END;

			case ERROR_MORE_DATA:
				// �擾����v���Z�X���̐�������Ȃ��ꍇ�A�ēx�擾
				delete[] rgpi;
				n_proc_info = n_proc_info_needed;
				rgpi = new RM_PROCESS_INFO[n_proc_info];
				ZeroMemory(rgpi, sizeof(RM_PROCESS_INFO) * n_proc_info);
				break;

			default:
				throw gcnew FindUsingProcessException(L"�v���Z�X���̎擾�Ɏ��s: {0}", FindUsingProcessHelper::GetErrorMessage(listError));
			}
		}

	LOOP_END:;
		// ���ʂ�\��
		Console::Out->WriteLine(L"���̃t�@�C�����g�p���Ă���v���Z�X�� {0} ���ł��B", n_proc_info_needed);
		for (UINT i = 0; i < n_proc_info_needed && i < n_proc_info; i++) {
			Console::Out->WriteLine(L"���O:{0} (Process ID:{1})", gcnew String(rgpi[i].strAppName), rgpi[i].Process.dwProcessId);
		}
	}
	catch (Exception^ e) {
		Console::Error->WriteLine(e->Message);
	}
	finally {
		delete[] rgpi;

		// �����������
		if (targetFilePtr != nullptr) {
			Marshal::FreeHGlobal(IntPtr((void*)targetFilePtr));
		}

		// �Z�b�V�����̏I��
		RmEndSession(session);
	}
}