#include <array>
#include <cstring>
#include <string_view>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fileapi.h>
#include <sys/stat.h>
//#pragma comment(lib, "User32.lib")

using namespace std::literals::string_view_literals;

constexpr std::wstring_view junction(L"|-- "), bar(L"|   "), angle(L"\\-- "), space(L"    ");
//constexpr std::wstring_view junction(L"├── "), bar(L"│   "), angle(L"└── "), space(L"    ");

int Walk(std::wstring& rootPath, std::vector<std::wstring_view>& headers);
int Walk(WIN32_FIND_DATAW thisEntry, std::wstring& thisParentPath, std::vector<std::wstring_view>& headers, bool isLast);

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

int Walk(WIN32_FIND_DATAW thisEntry, std::wstring& thisParentPath, std::vector<std::wstring_view>& headers, bool isLast) {
	if (L"."sv == thisEntry.cFileName) {
		return 0;
	}
	if (L".."sv == thisEntry.cFileName) {
		return 0;
	}
	for (const auto& entry : headers) {
		fwprintf(stdout, L"%s", entry.data());
	}
	fwprintf_s(stdout, L"%s\n", thisEntry.cFileName);
	if (thisEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		headers.back() = (isLast) ? space : bar;
		const size_t pathLen = thisParentPath.size();
		thisParentPath.append(thisEntry.cFileName);
		Walk(thisParentPath, headers);
		thisParentPath.resize(pathLen);
		headers.back() = junction;
	} else {
		headers.back() = junction;
	}
	return 0;
}

int Walk(std::wstring& rootPath, std::vector<std::wstring_view>& headers) {
	rootPath.append((rootPath.back() == L'\\') ? L"*" : L"\\*");
	WIN32_FIND_DATAW current = {};
	HANDLE handle = FindFirstFileW(rootPath.data(), &current);
	if (handle == INVALID_HANDLE_VALUE) {
		LogSystemError();
		return 1;
	}

	rootPath.pop_back(); // Remove '*'
	headers.push_back(junction);
	WIN32_FIND_DATAW recurseEntry = {}, recurseEntryPrev = {};
	FindNextFileW(handle, &recurseEntryPrev);
	bool hasNext = true;
	do {
		bool isLast = (FindNextFileW(handle, &recurseEntry) == 0);
		if (isLast) {
			hasNext = false;
			headers.back() = angle;
		}
		Walk(std::move(recurseEntryPrev), rootPath, headers, isLast);
		recurseEntryPrev = recurseEntry;
	} while (hasNext);
	headers.pop_back();
	return 0;
}

int wmain(int argc, wchar_t *argv[]) {
	std::wstring root;
	bool showHidden = false;
	if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			const wchar_t* arg = argv[i];
			if (L"-a"sv == arg || L"/a"sv == arg) {
				showHidden = true;
			}
		}
		// Final entry of args should always be the query directory.
		root.assign(argv[argc-1], ::wcslen(argv[argc-1]));
	} else {
		root.resize(512, '\0');
		unsigned long len = GetCurrentDirectoryW((unsigned long)root.size(), root.data());
		if (len == 0) {
			LogSystemError();
			return 1;
		}
		root.resize(len);
	}
	root.reserve(root.size() * 2);

	fwprintf_s(stdout, L"%s\n", root.c_str());
	std::vector<std::wstring_view> headers;
	Walk(std::move(root), headers);

	return 0;
}