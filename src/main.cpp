#include <array>
#include <cstring>
#include <string_view>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fileapi.h>
#include <sys/stat.h>
#pragma comment(lib, "User32.lib")

using namespace std::string_literals;

constexpr std::wstring_view junction(L"├── "), bar(L"│   "), angle(L"└── "), space(L"    ");

std::vector<wchar_t> indent;

void LogSystemError(unsigned long error) {
	std::array<wchar_t, 1024> buffer; // Stack allocated buffer
	auto msglen = FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		buffer.data(),
		(DWORD)buffer.size(),
		NULL
		);
	fwprintf_s(stderr, L"%s", buffer.data());
}
void LogSystemError() {
	auto error = GetLastError();
	LogSystemError(error);
}

int GetTabStopWidth() {
    int tab_stop_width = -1;
    HANDLE h = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD n;
        if (WriteConsoleW(h, L"\r\t", 2, &n, NULL)) {
            CONSOLE_SCREEN_BUFFER_INFO info;
            if (GetConsoleScreenBufferInfo(h, &info))
                tab_stop_width = info.dwCursorPosition.X;
        }
        CloseHandle(h);
    }
    return tab_stop_width;
}

void WCharToUTF8String(const wchar_t* widestr, const unsigned long widestrsize, char* utfstr, const unsigned long utfstrsize) {
	int defaultCharUsed = false;
	unsigned long size = WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, widestr, widestrsize, NULL, 0, NULL, &defaultCharUsed);
	size = (size < utfstrsize) ? size : utfstrsize;
	if (size == 0) {
		LogSystemError();
		return;
	}
	WideCharToMultiByte(CP_ACP, WC_COMPOSITECHECK, widestr, widestrsize, utfstr, size, NULL, &defaultCharUsed);
}

int WalkDirectory(std::wstring current_path) {
	current_path.append(L"\\*");
	//fwprintf_s(stderr, L"Walking %s\n", current_path.c_str());
	WIN32_FIND_DATAW current = {};
	HANDLE handle = FindFirstFileW(current_path.data(), &current);
	if (handle == INVALID_HANDLE_VALUE) {
		LogSystemError();
		return 1;
	}
	// Flush out '.' and '..' directories.
	FindNextFileW(handle, &current);

	for (int valid = FindNextFileW(handle, &current); valid != 0; valid = FindNextFileW(handle, &current)) {
		if (current.cFileName == L"..") {
			continue;
		}
		fwprintf_s(stderr, L"%s%s\n", indent.data(), current.cFileName);
		if (current.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			std::wstring enter_path = current_path;
			enter_path.pop_back();
			enter_path.append(current.cFileName);
			indent.back() = L'\t';
			indent.push_back(L'\0');
			WalkDirectory(std::move(enter_path));
			indent.pop_back();
			indent.back() = L'\0';
		}
	}
	auto error = GetLastError();
	if (error != ERROR_NO_MORE_FILES) {
		LogSystemError(error);
		return 1;
	}
	FindClose(handle);
	return 0;
}

int wmain(int argc, wchar_t *argv[]) {
	indent = std::vector<wchar_t>();
	indent.push_back(L'\0');
	std::wstring root(1024, '\0');
	unsigned long len = GetCurrentDirectoryW((unsigned long)root.size(), root.data());
	if (len == 0) {
		LogSystemError();
		return 1;
	}
	root.resize(len);
	WalkDirectory(std::move(root));

	return 0;
}