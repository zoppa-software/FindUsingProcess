#include "pch.h"
#include <Windows.h>
#include "FindUsingProcessHelper.h"

using namespace System;
using namespace System::Runtime::InteropServices;

String^ FindUsingProcessHelper::GetErrorMessage(DWORD errorCode)
{
    LPWSTR messageBuffer = nullptr;
    try {
		// �G���[�R�[�h�����b�Z�[�W�ɕϊ�����
        size_t size = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer,
            0,
            NULL
        );

		// ���b�Z�[�W���擾�ł��Ȃ������ꍇ�A�G���[�R�[�h�����̂܂ܕԂ�
		if (size == 0) {
			return String::Format(L"��͂ł��Ȃ��G���[�R�[�h: {0}", errorCode);
		}

		// �擾�������b�Z�[�W�𕶎���ɕϊ����ĕԂ�
        return Marshal::PtrToStringUni((IntPtr)messageBuffer);
    }
    catch (Exception^ e) {
        return String::Format(L"�s���ȗ�O:{0}", e->Message);
    }
    finally {
        LocalFree(messageBuffer);
    }
}