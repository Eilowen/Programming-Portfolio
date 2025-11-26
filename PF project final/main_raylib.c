#include "raylib.h"
#include <stdio.h>
#include <time.h>

float GetCPUUsage();
float GetMemoryUsage();
float GetBatteryPercentage();

typedef struct {
    float memoryUsage;
    float batteryPercentage;
    float cpuUsage;
    int fps;
} SystemStats;

void UpdateStats(SystemStats * stats) {
    stats->memoryUsage = GetMemoryUsage();
    stats->batteryPercentage = GetBatteryPercentage();
    stats->cpuUsage = GetCPUUsage();
    stats->fps = GetFPS();
}

void DrawProgressBar (int x, int y, int width, int height, float percentage, Color fillColor, Color bgColor) {
    DrawRectangle(x,y,width,height,bgColor);
    int filled = (int)(width * (percentage / 100.0f));
    if (filled > width) {
        filled = width;
    }
    if (filled < 0) {
        filled = 0;
    }
    DrawRectangle(x,y,filled,height,fillColor);
    DrawRectangleLines(x,y,width, height, DARKGRAY);
    char text[32];
    snprintf(text, sizeof(text), "%.1f%%", percentage);
    DrawText(text, x + width + 10, y + (height / 2) - 10, 20, BLACK);
}

void DrawBatteryIcon(int x, int y, float percentage) {
    if (percentage < 0.0f) {
        DrawText("N/A", x,y,20,GRAY);
        return;
    }
    Color batteryColor;
    int segments;
    if (percentage > 75.0f) {
        batteryColor = GREEN;
        segments = 4;
    }
    else if (percentage > 50.0f) {
        batteryColor = YELLOW;
        segments = 3;
    }
    else if (percentage > 25.0f) {
        batteryColor = ORANGE;
        segments = 2; 
    }
    else {
        batteryColor = RED; 
        segments = 1; 
    }

    DrawRectangleLines(x,y,60,30,BLACK);
    DrawRectangle(x+60, y+10, 5,10, BLACK);
    int segmentWidth = 12;
    for (int i = 0; i < segments; i++) {
        DrawRectangle(x + 5 + (i * 14), y + 5, segmentWidth, 20, batteryColor);
    }
    char text[32];
    snprintf(text, sizeof(text), "%.1f%%", percentage);
    DrawText(text, x + 70, y + 5, 20, BLACK);
}

int main(){
    const int screenW = 800, screenH = 600;
    InitWindow(screenW, screenH, "System Resource Monitor - Raylib");
    SetTargetFPS(60);

    SystemStats stats = {0};
    UpdateStats(&stats);
    float updateTimer = 0.0f;
    
    while(!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        updateTimer = updateTimer + deltaTime;

        if (updateTimer >= 1.0f) {
            UpdateStats(&stats);
            updateTimer = 0.0f;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("SYSTEM RESOURCE MONITOR", 180, 20, 35, DARKBLUE);
        time_t now = time(NULL);
        struct tm *timeinfo = localtime(&now);
        char timeStr[32];
        strftime(timeStr, sizeof(timeStr), "Time: %H:%M:%S", timeinfo);
        DrawText(timeStr, 300, 70, 18, GRAY);

        DrawText("MEMORY USAGE: ", 50, 130, 24, BLACK);
        DrawProgressBar(50, 170, 500, 40, stats.memoryUsage, BLUE, LIGHTGRAY);

        DrawText("BATTERY STATUS:", 50, 230, 24, BLACK);
        DrawBatteryIcon(50, 270, stats.batteryPercentage);

        DrawText("CPU USAGE:", 50, 330, 24, BLACK);
        DrawProgressBar(50, 370, 500, 40, stats.cpuUsage, RED, LIGHTGRAY);

        DrawText("FPS:", 50, 430, 24, BLACK);
        char fpsText[32];
        snprintf(fpsText, sizeof(fpsText), "%d", stats.fps);
        DrawText(fpsText, 50, 470, 40, DARKGREEN);

        DrawText("Press ESC to exit", 300, 540, 20, GRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
