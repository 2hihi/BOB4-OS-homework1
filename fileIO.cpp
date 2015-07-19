//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <strsafe.h>
#include <crtdbg.h>
#include <stdint.h>

bool is_file_existsW(_In_ const wchar_t* file_path) {
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH)))
		return false;

	WIN32_FILE_ATTRIBUTE_DATA info = { 0 };

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info) == 0)
		return false;
	else
		return true;
}

bool create_bob_txt() {
	wchar_t *buf = NULL;
	uint32_t buflen = 0;
	buflen = GetCurrentDirectoryW(buflen, buf);

	if (buflen == 0) {
		printf("[error] GetCurrentDirectoryW() failed!!! gle = 0x%08x\n", GetLastError());
		return false;
	}

	buf = (PWSTR)malloc(sizeof(WCHAR)*buflen);

	if (GetCurrentDirectoryW(buflen, buf) == 0) {
		printf("[error] GetCurrentDirectoryW() failed!!! gle = 0x%08x\n", GetLastError());
		free(buf);
		return false;
	}

	// current directory\\bob.txt 생성
	wchar_t file_name[260];
	if (!SUCCEEDED(StringCbPrintfW(
		file_name,
		sizeof(file_name),
		L"%ws\\bob.txt",
		buf))) {
		printf("[error] can not create bob.txt\n");
		free(buf);
		return false;
	}
	free(buf);
	buf = NULL;

	if (is_file_existsW(file_name)) {
		DeleteFileW(file_name);
	}

	// 파일 생성
	HANDLE file_handle = CreateFileW(
		file_name,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	DWORD numberOfBytesWritten;
	int result;
	unsigned char mark[3];
	
	mark[0] = 0xEF;
	mark[1] = 0xBB;
	mark[2] = 0xBF;	// UTF-8

	wchar_t strUni[256] = L"HiHello안녕안녕";
	char strUtf8[256] = { 0, };

	if (file_handle == INVALID_HANDLE_VALUE) {
		printf("[error] can not CreateFile, gle=0x%08x\n", GetLastError());
		return false;
	}

	int nlen = WideCharToMultiByte(CP_UTF8, 0, strUni, lstrlenW(strUni), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, strUni, lstrlenW(strUni), strUtf8, nlen, NULL, NULL);

	result = WriteFile(file_handle, &mark, 3, &numberOfBytesWritten, NULL);
	result = WriteFile(file_handle, strUtf8, strlen(strUtf8), &numberOfBytesWritten, NULL);

	// bob2.txt 경로
	wchar_t *buf2 = NULL;
	wchar_t file_name2[260];

	uint32_t buflen2 = 0;
	buflen2 = GetCurrentDirectoryW(buflen2, buf2);

	if (buflen2 == 0) {
		printf("[error] GetCurrentDirectoryW() failed!!! gle = 0x%08x\n", GetLastError());
		return false;
	}

	buf2 = (PWSTR)malloc(sizeof(WCHAR)*buflen2);

	if (GetCurrentDirectoryW(buflen2, buf2) == 0) {
		printf("[error] GetCurrentDirectoryW() failed!!! gle = 0x%08x\n", GetLastError());
		free(buf2);
		return false;
	}

	if (!SUCCEEDED(StringCbPrintfW(
		file_name2,
		sizeof(file_name2),
		L"%ws\\bob2.txt",
		buf2))) {
		printf("[error] can not create bob.txt\n");
		free(buf2);
		return false;
	}
	free(buf2);
	buf2 = NULL;

	// bob.txt -> bob2.txt 파일 복사
	CopyFile(file_name, file_name2, false);

	
	// bob2.txt 파일 내용 읽기
	char rBuf[256] = { 0, };
	DWORD dwRead=0;
	BOOL readOK;
	HANDLE file_handle2 = CreateFileW(file_name2, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file_handle2 == INVALID_HANDLE_VALUE) {
		printf("[error] can not file open, gle=0x%08x\n", GetLastError());
	}

	readOK = ReadFile(file_handle2, rBuf, 256, &dwRead, NULL);	// 파일 내용 읽어서 rBuf에 저장

	if (readOK && dwRead == 0)
		printf("[error] can not read file, gle=0x%08x\n", GetLastError());

	int len = MultiByteToWideChar(CP_UTF8, 0, rBuf, strlen(rBuf), NULL, NULL);	// rBuf <- utf8
	wchar_t pMultiByte[256] = { 0, };
	MultiByteToWideChar(CP_UTF8, 0, rBuf, strlen(rBuf), pMultiByte, len); // rBuf(utf8) -> pMultibyte(unicode)

	char strMultiByte[256] = { 0, };
	len = WideCharToMultiByte(CP_ACP, 0, pMultiByte, -1, NULL, 0, NULL, NULL);	// unicode -> multibyte
	WideCharToMultiByte(CP_ACP, 0, pMultiByte, -1, strMultiByte, len, NULL, NULL); // pMultibute -> strMultiByte(multibyte)
	
	printf("%s\n", strMultiByte+1);	// header 다음부터 출력

	CloseHandle(file_handle);
	CloseHandle(file_handle2);

	DeleteFile(file_name);
	DeleteFile(file_name2);

	return true;
}

void main() {
	create_bob_txt();
}