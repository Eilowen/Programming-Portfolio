#include <windows.h>

float GetCPUUsage() {
    static ULONGLONG lastIdle=0, lastKernel=0, lastUser=0;

    FILETIME idleFT, kernelFT, userFT;
    if (!GetSystemTimes(&idleFT, &kernelFT, &userFT)) {
        return 0.0f;
    }

    ULONGLONG idle = (((ULONGLONG)idleFT.dwHighDateTime) << 32) | idleFT.dwLowDateTime;
    ULONGLONG kernel = (((ULONGLONG)kernelFT.dwHighDateTime) << 32) | kernelFT.dwLowDateTime;
    ULONGLONG user   = (((ULONGLONG)userFT.dwHighDateTime)   << 32) | userFT.dwLowDateTime;

    ULONGLONG dIdle = idle - lastIdle;
    ULONGLONG dKernel = kernel - lastKernel;
    ULONGLONG dUser = user - lastUser;

    lastIdle = idle;
    lastKernel = kernel;
    lastUser = user;

    ULONGLONG total = dKernel + dUser;
    if (total == 0) {
        return 0.0f;
    }

    float cpu = (float)((total - dIdle) * 100.0 / total);
    if (cpu < 0) {
        cpu = 0;
    }
    if (cpu > 100) {
        cpu = 100;
    }
    return cpu;
}

float GetMemoryUsage() {
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof(MEMORYSTATUSEX);
    if (!GlobalMemoryStatusEx(&ms)) {
        return 0.0f;
    }

    float used = (float)(ms.ullTotalPhys - ms.ullAvailPhys);
    float total = (float)ms.ullTotalPhys;
    return used * 100.f / total;
}

float GetBatteryPercentage() {
        SYSTEM_POWER_STATUS sps;
        if (!GetSystemPowerStatus(&sps)) {
            return -1.0f;
        }
        return (float)sps.BatteryLifePercent;
}
