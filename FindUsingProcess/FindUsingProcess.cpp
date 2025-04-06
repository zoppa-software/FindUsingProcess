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

/// <summary>エントリポイント。</summary>
/// <param name="args">アプリケーション引数。</param>
/// <returns>戻り値。</returns>
int main(array<String ^> ^args)
{
	Console::Out->WriteLine(L"Find using Process v1.0.0");
	Console::Out->WriteLine(L"    by zoppa-software");

	try {
		// コマンドライン引数を取得
		if (args->Length == 0) {
			throw gcnew FindUsingProcessException(L"ファイル名を指定してください。");
		}

		// 対象ファイルを取得してループ
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

/// <summary>指定されたパスからファイル情報を収集します。</summary>
/// <param name="targetPath">対象パス。</param>
/// <returns>ファイル情報のリスト。</returns>
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
			throw gcnew FindUsingProcessException(L"指定されたパスは存在しません。");
		}
	}
}

/// <summary>指定されたファイルを使用しているプロセスをチェックします。</summary>
/// <param name="targetPath">対象ファイルのパス。</param>
/// <exception cref="FindUsingProcessException">エラーが発生した場合。</exception>
void CheckUsing(String^ targetPath)
{
	Console::Out->WriteLine(L"対象ファイル:{0}", targetPath);

	// セッションの開始
	DWORD session;
	WCHAR session_key[CCH_RM_SESSION_KEY + 1]{};
	DWORD sessionErr = RmStartSession(&session, 0, session_key);
	if (sessionErr != ERROR_SUCCESS) {
		throw gcnew FindUsingProcessException(L"セッションの開始に失敗: {0}", FindUsingProcessHelper::GetErrorMessage(sessionErr));
	}

	LPCWSTR targetFilePtr = nullptr;
	UINT n_proc_info_needed = 0;
	UINT n_proc_info = 256;
	RM_PROCESS_INFO* rgpi = new RM_PROCESS_INFO[n_proc_info];
	ZeroMemory(rgpi, sizeof(RM_PROCESS_INFO) * n_proc_info);
	try {
		// 対象ファイルのパスをネイティブ文字列に変換
		targetFilePtr = (const LPCWSTR)(Marshal::StringToHGlobalUni(targetPath)).ToPointer();

		// 対象ファイルを登録
		const int files_n = 1;
		DWORD resErr = RmRegisterResources(session, files_n, &targetFilePtr, 0, NULL, 0, NULL);
		if (resErr != ERROR_SUCCESS) {
			throw gcnew FindUsingProcessException(L"ファイルの登録に失敗: {0}", FindUsingProcessHelper::GetErrorMessage(resErr));
		}

		// そのファイルを使用しているプロセスの一覧を取得
		DWORD reason;
		while (true) {
			DWORD listError = RmGetList(session, &n_proc_info_needed, &n_proc_info, rgpi, &reason);
			switch (listError)
			{
			case ERROR_SUCCESS:
				goto LOOP_END;

			case ERROR_MORE_DATA:
				// 取得するプロセス情報の数が足りない場合、再度取得
				delete[] rgpi;
				n_proc_info = n_proc_info_needed;
				rgpi = new RM_PROCESS_INFO[n_proc_info];
				ZeroMemory(rgpi, sizeof(RM_PROCESS_INFO) * n_proc_info);
				break;

			default:
				throw gcnew FindUsingProcessException(L"プロセス情報の取得に失敗: {0}", FindUsingProcessHelper::GetErrorMessage(listError));
			}
		}

	LOOP_END:;
		// 結果を表示
		Console::Out->WriteLine(L"このファイルを使用しているプロセスは {0} 件です。", n_proc_info_needed);
		for (UINT i = 0; i < n_proc_info_needed && i < n_proc_info; i++) {
			Console::Out->WriteLine(L"名前:{0} (Process ID:{1})", gcnew String(rgpi[i].strAppName), rgpi[i].Process.dwProcessId);
		}
	}
	catch (Exception^ e) {
		Console::Error->WriteLine(e->Message);
	}
	finally {
		delete[] rgpi;

		// メモリを解放
		if (targetFilePtr != nullptr) {
			Marshal::FreeHGlobal(IntPtr((void*)targetFilePtr));
		}

		// セッションの終了
		RmEndSession(session);
	}
}