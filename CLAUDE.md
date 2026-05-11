# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
gcc pomodoro.c -o pomodoro.exe -mwindows
./pomodoro.exe
```

Requires MinGW-w64 compiler (e.g., from `C:/llvm-mingw`).

## Architecture

Single-file Win32 desktop application. No external dependencies beyond Windows SDK.

**State machine:**
- `isWork` flag toggles between work/break phases
- `isRunning` controls timer active state
- Timer fires `WM_TIMER` every 1000ms to decrement `timeLeft`
- Phase sequence: Work (25min) → Short Break (5min) → Work → Short Break → Work → Short Break → Work → Long Break (15min) → repeat
- Long break triggers every 4 pomodoros (`POMODOROS_TO_LONG_BREAK = 4`)

**Key identifiers:**
- Window class: `"PomodoroTimer"`
- Timer ID: `ID_TIMER` (1)
- Button IDs: 1=Start/Pause, 2=Reset, 3=Skip