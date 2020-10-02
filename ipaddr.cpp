#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <process.h>
#include <stdio.h>
#include <assert.h>

static const TCHAR s_szName[] = TEXT("katahiromz ipaddr");

static HWND s_hwndMain;
static HWND s_hwndButton;
static HWND s_hwndIPAddress;

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    s_hwndMain = hwnd;

    DWORD style;

    style = WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;
    s_hwndButton = CreateWindowEx(0, TEXT("BUTTON"), TEXT("BUTTON"), style,
        0, 0, 80, 20, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
    if (!s_hwndButton)
        return FALSE;

    style = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    s_hwndIPAddress = CreateWindowEx(0, WC_IPADDRESS, NULL, style,
        80, 0, 90, 20, hwnd, (HMENU)1, GetModuleHandle(NULL), NULL);
    if (!s_hwndIPAddress)
        return FALSE;

    PostMessage(hwnd, WM_COMMAND, 999, 0);
    return TRUE;
}

void PressTab(BOOL bShift = FALSE)
{
    if (bShift)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_LSHIFT;
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(input));
    }
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_TAB;
        input.ki.dwFlags = 0;
        SendInput(1, &input, sizeof(input));
    }
    Sleep(100);
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_TAB;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(input));
    }
    if (bShift)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = VK_LSHIFT;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(input));
    }
    Sleep(100);
}

HWND s_hEdit1, s_hEdit2, s_hEdit3, s_hEdit4;

unsigned __stdcall thread_proc(void *arg)
{
    HWND hwnd = (HWND)arg;
    s_hEdit4 = GetTopWindow(s_hwndIPAddress);
    s_hEdit3 = GetNextWindow(s_hEdit4, GW_HWNDNEXT);
    s_hEdit2 = GetNextWindow(s_hEdit3, GW_HWNDNEXT);
    s_hEdit1 = GetNextWindow(s_hEdit2, GW_HWNDNEXT);
    printf("%p %p %p %p %p %p %p\n", hwnd, s_hwndButton, s_hwndIPAddress,
        s_hEdit1, s_hEdit2, s_hEdit3, s_hEdit4);
    PostMessage(hwnd, WM_COMMAND, 888, 0);

    PressTab();
    Sleep(500);
    PostMessage(hwnd, WM_COMMAND, 777, 0);

    PressTab();
    Sleep(500);
    PostMessage(hwnd, WM_COMMAND, 666, 0);

    PressTab();
    Sleep(500);
    PostMessage(hwnd, WM_COMMAND, 555, 0);
    return 0;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case 999:
        CloseHandle((HANDLE)_beginthreadex(NULL, 0, thread_proc, hwnd, 0, NULL));
        break;
    case 888:
        printf("%p\n", GetFocus());
        assert(GetFocus() == hwnd);
        break;
    case 777:
        printf("%p\n", GetFocus());
        assert(GetFocus() == s_hwndButton);
        break;
    case 666:
        printf("%p\n", GetFocus());
        assert(GetFocus() == s_hEdit1);
        break;
    case 555:
        printf("%p\n", GetFocus());
        assert(GetFocus() == s_hwndButton);
        MessageBox(hwnd, TEXT("SUCCESS"), TEXT("SUCCESS"), MB_ICONINFORMATION);
        DestroyWindow(hwnd);
        break;
    }
}

void OnDestroy(HWND hwnd)
{   
    PostQuitMessage(0);
}

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int main(void)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    InitCommonControls();

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wc.lpszClassName = s_szName;
    if (!RegisterClass(&wc))
    {
        MessageBox(NULL, TEXT("RegisterClass"), NULL, MB_ICONERROR);
        return -1;
    }

    HWND hwnd = CreateWindowEx(0, s_szName, s_szName, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 80, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL)
    {
        MessageBox(NULL, TEXT("CreateWindowEx"), NULL, MB_ICONERROR);
        return -2;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (IsDialogMessage(hwnd, &msg))
            continue;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
