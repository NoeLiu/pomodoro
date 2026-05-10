/*
 * 番茄时钟 (Pomodoro Timer)
 * 使用 Win32 API 编写的 Windows 桌面应用
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/* 全局变量 */
#define ID_TIMER 1
#define WORK_TIME 25 * 60
#define SHORT_BREAK 5 * 60
#define LONG_BREAK 15 * 60
#define POMODOROS_TO_LONG_BREAK 4

HWND hwndMain;
HWND hwndTime;
HWND hwndStatus;
HWND hwndCount;
HWND hwndStartPause;
HWND hwndReset;
HWND hwndSkip;

int timeLeft = WORK_TIME;
int isRunning = 0;
int isWork = 1;
int pomodoroCount = 0;

/* 函数声明 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateDisplay(HWND hwnd);
void StartTimer(void);
void PauseTimer(void);
void ResetTimer(void);
void SkipPhase(void);
void OnTimerComplete(void);
void FormatTime(int seconds, char *buffer);

/* WinMain - 程序入口 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = {0};
    MSG msg;
    int screenWidth, screenHeight;

    /* 注册窗口类 */
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "PomodoroTimer";
    wc.hbrBackground = CreateSolidBrush(RGB(44, 62, 80));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "窗口注册失败!", "错误", MB_ICONERROR);
        return 1;
    }

    /* 获取屏幕尺寸居中窗口 */
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    /* 创建窗口 */
    hwndMain = CreateWindow(
        "PomodoroTimer",
        "番茄时钟",
        WS_OVERLAPPED | WS_CSYNC | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        (screenWidth - 400) / 2, (screenHeight - 500) / 2,
        400, 520,
        NULL, NULL, hInstance, NULL
    );

    if (!hwndMain) {
        MessageBox(NULL, "窗口创建失败!", "错误", MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwndMain, SW_SHOW);
    UpdateWindow(hwndMain);

    /* 消息循环 */
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

/* 窗口过程函数 */
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    HFONT hFont, hFontLarge;
    char timeStr[16];
    char statusStr[32];
    char countStr[64];

    switch (uMsg) {
        case WM_CREATE:
            /* 创建字体 */
            hFont = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft YaHei");
            hFontLarge = CreateFont(72, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Microsoft YaHei");

            /* 标题 */
            CreateWindow("STATIC", "🍅 番茄时钟",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                100, 20, 200, 40, hwnd, NULL, NULL, NULL);

            /* 状态标签 */
            hwndStatus = CreateWindow("STATIC", "专注工作",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                120, 75, 160, 30, hwnd, NULL, NULL, NULL);
            SendMessage(hwndStatus, WM_SETFONT, (WPARAM)hFont, TRUE);

            /* 计数标签 */
            hwndCount = CreateWindow("STATIC", "第 0 个番茄钟",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                120, 110, 160, 25, hwnd, NULL, NULL, NULL);
            SendMessage(hwndCount, WM_SETFONT, (WPARAM)hFont, TRUE);

            /* 时间显示 */
            FormatTime(timeLeft, timeStr);
            hwndTime = CreateWindow("STATIC", timeStr,
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                100, 160, 200, 100, hwnd, NULL, NULL, NULL);
            SendMessage(hwndTime, WM_SETFONT, (WPARAM)hFontLarge, TRUE);

            /* 按钮 */
            hwndStartPause = CreateWindow("BUTTON", "▶ 开始",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                40, 300, 100, 40, hwnd, (HMENU)1, NULL, NULL);
            SendMessage(hwndStartPause, WM_SETFONT, (WPARAM)hFont, TRUE);

            hwndReset = CreateWindow("BUTTON", "↻ 重置",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                150, 300, 100, 40, hwnd, (HMENU)2, NULL, NULL);
            SendMessage(hwndReset, WM_SETFONT, (WPARAM)hFont, TRUE);

            hwndSkip = CreateWindow("BUTTON", "⏭ 跳过",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                260, 300, 100, 40, hwnd, (HMENU)3, NULL, NULL);
            SendMessage(hwndSkip, WM_SETFONT, (WPARAM)hFont, TRUE);

            /* 说明 */
            CreateWindow("STATIC", "工作25分钟 → 休息5分钟\n每4个番茄钟后休息15分钟",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                50, 380, 300, 60, hwnd, NULL, NULL, NULL);

            SetTimer(hwnd, ID_TIMER, 1000, NULL);
            return 0;

        case WM_TIMER:
            if (wParam == ID_TIMER && isRunning && timeLeft > 0) {
                timeLeft--;
                UpdateDisplay(hwnd);
                if (timeLeft == 0) {
                    OnTimerComplete();
                }
            }
            return 0;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case 1: /* 开始/暂停 */
                    if (isRunning) {
                        PauseTimer();
                    } else {
                        StartTimer();
                    }
                    break;
                case 2: /* 重置 */
                    ResetTimer();
                    break;
                case 3: /* 跳过 */
                    SkipPhase();
                    break;
            }
            return 0;

        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, ID_TIMER);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/* 更新显示 */
void UpdateDisplay(HWND hwnd)
{
    char timeStr[16];
    char countStr[64];

    FormatTime(timeLeft, timeStr);
    SetWindowText(hwndTime, timeStr);

    sprintf(countStr, "第 %d 个番茄钟", pomodoroCount);
    SetWindowText(hwndCount, countStr);
}

/* 启动计时器 */
void StartTimer(void)
{
    isRunning = 1;
    SetWindowText(hwndStartPause, "⏸ 暂停");
}

/* 暂停计时器 */
void PauseTimer(void)
{
    isRunning = 0;
    SetWindowText(hwndStartPause, "▶ 继续");
}

/* 重置计时器 */
void ResetTimer(void)
{
    isRunning = 0;
    SetWindowText(hwndStartPause, "▶ 开始");

    if (isWork) {
        timeLeft = WORK_TIME;
    } else {
        int remainder = pomodoroCount % POMODOROS_TO_LONG_BREAK;
        timeLeft = (remainder == 0) ? LONG_BREAK : SHORT_BREAK;
    }

    UpdateDisplay(hwndMain);
}

/* 跳过当前阶段 */
void SkipPhase(void)
{
    isRunning = 0;
    SetWindowText(hwndStartPause, "▶ 开始");

    if (isWork) {
        isWork = 0;
        int remainder = (pomodoroCount + 1) % POMODOROS_TO_LONG_BREAK;
        timeLeft = (remainder == 0) ? LONG_BREAK : SHORT_BREAK;
        SetWindowText(hwndStatus, remainder == 0 ? "长休息" : "短休息");
    } else {
        isWork = 1;
        timeLeft = WORK_TIME;
        SetWindowText(hwndStatus, "专注工作");
    }

    UpdateDisplay(hwndMain);
}

/* 计时完成处理 */
void OnTimerComplete(void)
{
    isRunning = 0;
    SetWindowText(hwndStartPause, "▶ 开始");

    if (isWork) {
        pomodoroCount++;
        UpdateDisplay(hwndMain);

        char msg[128];
        if (pomodoroCount % POMODOROS_TO_LONG_BREAK == 0) {
            MessageBox(hwndMain, "工作时间结束！开始长休息吧！", "🍅 番茄时钟", MB_OK | MB_ICONINFORMATION);
            isWork = 0;
            timeLeft = LONG_BREAK;
            SetWindowText(hwndStatus, "长休息");
        } else {
            MessageBox(hwndMain, "工作时间结束！休息一下吧！", "🍅 番茄时钟", MB_OK | MB_ICONINFORMATION);
            isWork = 0;
            timeLeft = SHORT_BREAK;
            SetWindowText(hwndStatus, "短休息");
        }
    } else {
        MessageBox(hwndMain, "休息结束！开始新的专注时间！", "🍅 番茄时钟", MB_OK | MB_ICONINFORMATION);
        isWork = 1;
        timeLeft = WORK_TIME;
        SetWindowText(hwndStatus, "专注工作");
    }

    UpdateDisplay(hwndMain);
}

/* 格式化时间显示 */
void FormatTime(int seconds, char *buffer)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    sprintf(buffer, "%02d:%02d", minutes, secs);
}