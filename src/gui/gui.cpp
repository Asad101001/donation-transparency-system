#include "gui.h"
#include "raylib.h"
#include "../core/Utils.h"

// Define to bypass standard string functions
int my_sprintf_int(char* buf, const char* prefix, int val) {
    char temp[20];
    int i = 0;
    if (val == 0) { temp[i++] = '0'; }
    while (val > 0) { temp[i++] = (val % 10) + '0'; val /= 10; }
    
    int k = 0;
    while(prefix[k] != '\0') { buf[k] = prefix[k]; k++; }
    
    while (i > 0) { buf[k++] = temp[--i]; }
    buf[k] = '\0';
    return k;
}

int my_sprintf_double(char* buf, const char* prefix, double val) {
    return my_sprintf_int(buf, prefix, (int)val); // simplify for UI
}

using namespace std;

Color bgDark = { 18, 18, 18, 255 };
Color panelDark = { 30, 30, 30, 255 };
Color accentCyan = { 0, 229, 255, 255 };
Color accentCyanDark = { 0, 180, 200, 255 };
Color textWhite = { 240, 240, 240, 255 };
Color textMuted = { 150, 150, 150, 255 };
Color errorRed = { 255, 80, 80, 255 };

Font customFont;
Font customFontBold;

enum AppScreen { DASHBOARD, DONORS, PROJECTS, DONATIONS, REPORT };
AppScreen currentScreen = DASHBOARD;

struct TextBox {
    Rectangle bounds;
    char text[100];
    int letterCount;
    bool editMode;
    const char* placeholder;
};

void DrawTextBox(TextBox& tb) {
    DrawRectangleRec(tb.bounds, tb.editMode ? panelDark : bgDark);
    DrawRectangleLinesEx(tb.bounds, 2, tb.editMode ? accentCyan : textMuted);
    if (tb.letterCount == 0 && !tb.editMode) {
        DrawTextEx(customFont, tb.placeholder, (Vector2){tb.bounds.x + 10, tb.bounds.y + 10}, 20, 1, textMuted);
    } else {
        DrawTextEx(customFont, tb.text, (Vector2){tb.bounds.x + 10, tb.bounds.y + 10}, 20, 1, textWhite);
    }
    if (tb.editMode && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(customFont, tb.text, 20, 1);
        DrawLine(tb.bounds.x + 12 + textSize.x, tb.bounds.y + 10, tb.bounds.x + 12 + textSize.x, tb.bounds.y + 30, accentCyan);
    }
}

void UpdateTextBox(TextBox& tb) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        tb.editMode = CheckCollisionPointRec(GetMousePosition(), tb.bounds);
    }
    if (tb.editMode) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (tb.letterCount < 99)) {
                tb.text[tb.letterCount] = (char)key;
                tb.text[tb.letterCount + 1] = '\0';
                tb.letterCount++;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            tb.letterCount--;
            if (tb.letterCount < 0) tb.letterCount = 0;
            tb.text[tb.letterCount] = '\0';
        }
    }
}

bool DrawButton(Rectangle bounds, const char* text) {
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    DrawRectangleRec(bounds, hovered ? accentCyanDark : accentCyan);
    Vector2 tSize = MeasureTextEx(customFontBold, text, 20, 1);
    DrawTextEx(customFontBold, text, (Vector2){bounds.x + (bounds.width - tSize.x)/2, bounds.y + (bounds.height - tSize.y)/2}, 20, 1, bgDark);
    return clicked;
}

char popupMsg[200] = "";
float popupTimer = 0.0f;
void ShowPopup(const char* msg) {
    my_strcpy(popupMsg, msg);
    popupTimer = 3.0f;
}

TextBox tDonorName = { { 300, 150, 400, 40 }, "", 0, false, "Donor Name" };
TextBox tDonorCnic = { { 300, 210, 400, 40 }, "", 0, false, "Donor CNIC" };

TextBox tProjName = { { 300, 150, 400, 40 }, "", 0, false, "Project Name" };
TextBox tProjDesc = { { 300, 210, 400, 40 }, "", 0, false, "Project Description" };

TextBox tDonID = { { 300, 150, 400, 40 }, "", 0, false, "Donor ID" };
TextBox tDonAmt = { { 300, 210, 400, 40 }, "", 0, false, "Amount (Rs)" };
TextBox tDonDate = { { 300, 270, 400, 40 }, "", 0, false, "Date (DD/MM/YYYY)" };

TextBox tRepID = { { 300, 150, 400, 40 }, "", 0, false, "Donation ID" };

void ResetTextBox(TextBox& tb) { tb.text[0] = '\0'; tb.letterCount = 0; tb.editMode = false; }
int my_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) { if(str[i] >= '0' && str[i] <= '9') res = res * 10 + str[i] - '0'; }
    return res;
}
double my_atof(const char* str) { return (double)my_atoi(str); }

void initGUI() {
    InitWindow(1280, 720, "Donation Transparency System");
    SetTargetFPS(60);
    customFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 32, 0, 250);
    customFontBold = LoadFontEx("C:\\Windows\\Fonts\\segoeuib.ttf", 32, 0, 250);
    if (customFont.baseSize == 0) customFont = GetFontDefault();
    if (customFontBold.baseSize == 0) customFontBold = GetFontDefault();
}

void DrawSidebar() {
    DrawRectangle(0, 0, 250, 720, panelDark);
    DrawTextEx(customFontBold, "DT System", (Vector2){20, 30}, 36, 1, accentCyan);
    const char* menus[] = { "Dashboard", "Donors", "Projects", "Donations", "Report" };
    AppScreen screens[] = { DASHBOARD, DONORS, PROJECTS, DONATIONS, REPORT };
    for(int i = 0; i < 5; i++) {
        Rectangle btn = { 0, (float)120 + i * 60, 250, 50 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
        bool active = (currentScreen == screens[i]);
        if (hovered || active) DrawRectangleRec(btn, active ? accentCyanDark : bgDark);
        DrawTextEx(customFont, menus[i], (Vector2){30, btn.y + 15}, 20, 1, active ? bgDark : textWhite);
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentScreen = screens[i];
    }
}

void DrawDashboard(SystemManager* sys) {
    DrawTextEx(customFontBold, "Dashboard", (Vector2){300, 50}, 40, 1, textWhite);
    char buf[100];
    my_sprintf_int(buf, "Max Donation: Rs ", (int)getMax(sys->maxDonations));
    DrawTextEx(customFont, buf, (Vector2){300, 150}, 24, 1, textWhite);
    my_sprintf_int(buf, "Min Donation: Rs ", (int)getMin(sys->minDonations));
    DrawTextEx(customFont, buf, (Vector2){300, 200}, 24, 1, textWhite);
}

void runGUI(SystemManager* sys) {
    while (!WindowShouldClose()) {
        UpdateTextBox(tDonorName); UpdateTextBox(tDonorCnic);
        UpdateTextBox(tProjName); UpdateTextBox(tProjDesc);
        UpdateTextBox(tDonID); UpdateTextBox(tDonAmt); UpdateTextBox(tDonDate);
        UpdateTextBox(tRepID);

        BeginDrawing();
        ClearBackground(bgDark);
        DrawSidebar();
        
        if (currentScreen == DASHBOARD) {
            DrawDashboard(sys);
        } else if (currentScreen == DONORS) {
            DrawTextEx(customFontBold, "Register Donor", (Vector2){300, 50}, 40, 1, textWhite);
            DrawTextBox(tDonorName);
            DrawTextBox(tDonorCnic);
            if (DrawButton((Rectangle){300, 270, 200, 40}, "Register")) {
                if (tDonorName.letterCount > 0) {
                    registerDonor(sys, tDonorName.text, tDonorCnic.text);
                    ShowPopup("Donor Registered Successfully!");
                    ResetTextBox(tDonorName); ResetTextBox(tDonorCnic);
                }
            }
        } else if (currentScreen == PROJECTS) {
            DrawTextEx(customFontBold, "Create Project", (Vector2){300, 50}, 40, 1, textWhite);
            DrawTextBox(tProjName);
            DrawTextBox(tProjDesc);
            if (DrawButton((Rectangle){300, 270, 200, 40}, "Create")) {
                if (tProjName.letterCount > 0) {
                    createProject(sys, tProjName.text, tProjDesc.text);
                    ShowPopup("Project Created Successfully!");
                    ResetTextBox(tProjName); ResetTextBox(tProjDesc);
                }
            }
        } else if (currentScreen == DONATIONS) {
            DrawTextEx(customFontBold, "Make Donation", (Vector2){300, 50}, 40, 1, textWhite);
            DrawTextBox(tDonID); DrawTextBox(tDonAmt); DrawTextBox(tDonDate);
            if (DrawButton((Rectangle){300, 330, 200, 40}, "Donate")) {
                if (tDonAmt.letterCount > 0) {
                    makeDonation(sys, my_atoi(tDonID.text), my_atof(tDonAmt.text), tDonDate.text);
                    ShowPopup("Donation Processed!");
                    ResetTextBox(tDonID); ResetTextBox(tDonAmt); ResetTextBox(tDonDate);
                }
            }
        } else if (currentScreen == REPORT) {
            DrawTextEx(customFontBold, "Transparency Report", (Vector2){300, 50}, 40, 1, textWhite);
            DrawTextBox(tRepID);
            if (tRepID.letterCount > 0) {
                int donID = my_atoi(tRepID.text);
                Donation* d = searchDonation(sys->donationRecords, donID);
                if (d != NULL) {
                    DrawRectangle(300, 210, 600, 300, panelDark);
                    char buf[100];
                    my_sprintf_int(buf, "Initial Donation: Rs ", (int)d->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){320, 230}, 24, 1, accentCyan);
                    DrawLine(320, 270, 320, 300, textMuted);
                    my_sprintf_int(buf, "Remaining: Rs ", (int)d->remainingAmount);
                    DrawTextEx(customFontBold, buf, (Vector2){320, 310}, 24, 1, textWhite);
                } else {
                    DrawTextEx(customFont, "Donation Not Found.", (Vector2){300, 210}, 24, 1, errorRed);
                }
            }
        }
        
        if (popupTimer > 0) {
            DrawRectangle(0, 0, 1280, 50, accentCyanDark);
            Vector2 ts = MeasureTextEx(customFontBold, popupMsg, 24, 1);
            DrawTextEx(customFontBold, popupMsg, (Vector2){(1280 - ts.x)/2, 13}, 24, 1, bgDark);
            popupTimer -= GetFrameTime();
        }
        
        EndDrawing();
    }
}

void closeGUI() {
    CloseWindow();
}
