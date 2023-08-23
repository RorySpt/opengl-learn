#include <Windows.h>
#include <string>

std::string WideCharToAnsi(const wchar_t* ws);
std::wstring AnsiToWideChar(const char* s);
std::u8string WideCharToUtf8(const wchar_t* ws);
std::wstring Utf8ToWideChar(const char8_t* u8s);
std::u8string AnsiToUtf8(const char* s);
std::string Utf8ToAnsi(const char8_t* u8s);


std::string WideCharToAnsi(const wchar_t* ws)
{
	const int length = WideCharToMultiByte(CP_ACP, 0, ws, -1, nullptr, 0, nullptr, nullptr);
	if (length == 0)
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::string s; s.resize(length);
	if (!WideCharToMultiByte(CP_ACP, 0, ws, -1, s.data(), length, nullptr, nullptr))
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return s;
}

std::wstring AnsiToWideChar(const char* s)
{
	const int length = MultiByteToWideChar(CP_ACP, 0, s, -1, nullptr, 0);
	if (length == 0)
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::wstring ws; ws.resize(length);
	if (!MultiByteToWideChar(CP_ACP, 0, s, -1, ws.data(), length))
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return ws;
}

std::u8string WideCharToUtf8(const wchar_t* ws)
{
	const int length = WideCharToMultiByte(CP_UTF8, 0, ws, -1, nullptr, 0, nullptr, nullptr);
	if (length == 0)
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::u8string u8s; u8s.resize(length);
	if (!WideCharToMultiByte(CP_UTF8, 0, ws, -1, reinterpret_cast<char*>(u8s.data()), length, nullptr, nullptr))
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return u8s;
}

std::wstring Utf8ToWideChar(const char8_t* u8s)
{
	const int length = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(u8s), -1, nullptr, 0);
	if (length == 0)
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	std::wstring ws; ws.resize(length);
	if (!MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(u8s), -1, ws.data(), length))
	{
		printf("Conversion failed. Error code: %lu\n", GetLastError());
		return {};
	}
	return ws;
}

std::u8string AnsiToUtf8(const char* s)
{
	return WideCharToUtf8(AnsiToWideChar(s).c_str());
}

std::string Utf8ToAnsi(const char8_t* u8s)
{
	return WideCharToAnsi(Utf8ToWideChar(u8s).c_str());
}

void unit_test()
{
	printf("unit test begin!\n");
	const std::wstring input_w = L"³É¹¦£¡";
	const std::u8string input_u8 = WideCharToUtf8(input_w.c_str());
	const std::string input = WideCharToAnsi(input_w.c_str());

	printf("Ansi<->WideChar: %s\n", WideCharToAnsi(AnsiToWideChar(input.c_str()).c_str()).c_str());
	printf("Utf8<->WideChar: %s\n", WideCharToAnsi(Utf8ToWideChar(input_u8.c_str()).c_str()).c_str());
	printf("Utf8<->Ansi: %s\n", Utf8ToAnsi(AnsiToUtf8(input.c_str()).c_str()).c_str());
}
int main()
{
	unit_test();
}

