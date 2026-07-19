#include "gui.h"
#include "raylib.h"
#include "../core/Utils.h"

// String format utilities
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
    return my_sprintf_int(buf, prefix, (int)val);
}

// GUI Theme
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
    const char* placeholder;
};

TextBox* activeTextBox = NULL;

void DrawTextBox(TextBox& tb) {
    bool isActive = (activeTextBox == &tb);
    
    DrawRectangleRec(tb.bounds, isActive ? panelDark : bgDark);
    DrawRectangleLinesEx(tb.bounds, isActive ? 3 : 2, isActive ? accentCyan : textMuted);
    
    if (tb.letterCount == 0 && !isActive) {
        DrawTextEx(customFont, tb.placeholder, (Vector2){tb.bounds.x + 15, tb.bounds.y + 15}, 24, 1, textMuted);
    } else {
        DrawTextEx(customFont, tb.text, (Vector2){tb.bounds.x + 15, tb.bounds.y + 15}, 24, 1, textWhite);
    }
    
    if (isActive && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(customFont, tb.text, 24, 1);
        DrawLine(tb.bounds.x + 17 + textSize.x, tb.bounds.y + 12, tb.bounds.x + 17 + textSize.x, tb.bounds.y + 38, accentCyan);
    }
}

void UpdateTextBoxes(TextBox** textboxes, int count) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeTextBox = NULL; // deselect all by default
        for(int i = 0; i < count; i++) {
            if (CheckCollisionPointRec(GetMousePosition(), textboxes[i]->bounds)) {
                activeTextBox = textboxes[i];
                break;
            }
        }
    }

    if (activeTextBox != NULL) {
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125) && (activeTextBox->letterCount < 99)) {
                activeTextBox->text[activeTextBox->letterCount] = (char)key;
                activeTextBox->text[activeTextBox->letterCount + 1] = '\0';
                activeTextBox->letterCount++;
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_BACKSPACE)) {
            activeTextBox->letterCount--;
            if (activeTextBox->letterCount < 0) activeTextBox->letterCount = 0;
            activeTextBox->text[activeTextBox->letterCount] = '\0';
        }
    }
}

bool DrawButton(Rectangle bounds, const char* text) {
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    DrawRectangleRec(bounds, hovered ? accentCyanDark : accentCyan);
    
    Vector2 tSize = MeasureTextEx(customFontBold, text, 24, 1);
    DrawTextEx(customFontBold, text, (Vector2){bounds.x + (bounds.width - tSize.x)/2, bounds.y + (bounds.height - tSize.y)/2}, 24, 1, bgDark);
    
    return clicked;
}

// Global Popups
char popupMsg[200] = "";
float popupTimer = 0.0f;
float popupY = -100;
void ShowPopup(const char* msg) {
    my_strcpy(popupMsg, msg);
    popupTimer = 3.0f;
    popupY = -100; // Slide in animation starting point
}

// Forms Data
TextBox tDonorName = { { 0, 0, 500, 50 }, "", 0, "Donor Name" };
TextBox tDonorCnic = { { 0, 0, 500, 50 }, "", 0, "Donor CNIC" };
TextBox tProjName = { { 0, 0, 500, 50 }, "", 0, "Project Name" };
TextBox tProjDesc = { { 0, 0, 500, 50 }, "", 0, "Project Description" };
TextBox tDonID = { { 0, 0, 500, 50 }, "", 0, "Donor ID" };
TextBox tDonAmt = { { 0, 0, 500, 50 }, "", 0, "Amount (Rs)" };
TextBox tDonDate = { { 0, 0, 500, 50 }, "", 0, "Date (DD/MM/YYYY)" };
TextBox tRepID = { { 0, 0, 500, 50 }, "", 0, "Donation ID" };

TextBox* allTextBoxes[] = { &tDonorName, &tDonorCnic, &tProjName, &tProjDesc, &tDonID, &tDonAmt, &tDonDate, &tRepID };

void ResetTextBox(TextBox& tb) { tb.text[0] = '\0'; tb.letterCount = 0; }
int my_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) { if(str[i] >= '0' && str[i] <= '9') res = res * 10 + str[i] - '0'; }
    return res;
}
double my_atof(const char* str) { return (double)my_atoi(str); }

void initGUI() {
    SetConfigFlags(FLAG_VSYNC_HINT); // | FLAG_WINDOW_RESIZABLE
    InitWindow(1280, 720, "Donation Transparency System");
    
    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();
    SetExitKey(KEY_NULL); // ESC won't exit
    
    customFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 64, 0, 250);
    customFontBold = LoadFontEx("C:\\Windows\\Fonts\\segoeuib.ttf", 64, 0, 250);
    
    if (customFont.baseSize == 0) customFont = GetFontDefault();
    if (customFontBold.baseSize == 0) customFontBold = GetFontDefault();
}

void DrawSidebar() {
    int sh = GetScreenHeight();
    
    DrawRectangle(0, 0, 300, sh, panelDark);
    DrawTextEx(customFontBold, "DT System", (Vector2){40, 50}, 48, 1, accentCyan);
    
    const char* menus[] = { "Dashboard", "Donors", "Projects", "Donations", "Report" };
    AppScreen screens[] = { DASHBOARD, DONORS, PROJECTS, DONATIONS, REPORT };
    
    for(int i = 0; i < 5; i++) {
        Rectangle btn = { 0, (float)180 + i * 80, 300, 70 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
        bool active = (currentScreen == screens[i]);
        
        if (hovered || active) DrawRectangleRec(btn, active ? accentCyanDark : bgDark);
        DrawTextEx(customFont, menus[i], (Vector2){40, btn.y + 20}, 28, 1, active ? bgDark : textWhite);
        
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = screens[i];
            activeTextBox = NULL; // Reset focus on tab change
        }
    }
}

// Function to seed dummy data internally from GUI
void seedDummyData(SystemManager* sys) {
    registerDonor(sys, "Alice", "12345");
    registerDonor(sys, "Bob", "67890");
    createProject(sys, "Water_Well", "Clean_Water");
    createProject(sys, "School", "Education_Fund");
    makeDonation(sys, 1, 15000, "20/07/2026");
    makeDonation(sys, 2, 5000, "21/07/2026");
    allocateDonation(sys, 10);
    addExpense(sys, 10, 5000, "Pipes", "22/07/2026");
    addExpense(sys, 10, 3000, "Labor", "23/07/2026");
    ShowPopup("Dummy Data Seeded!");
}

void DrawDashboard(SystemManager* sys) {
    int sh = GetScreenHeight();
    int startX = 350;
    
    DrawTextEx(customFontBold, "Dashboard", (Vector2){(float)startX, 50}, 60, 1, textWhite);
    
    char buf[100];
    my_sprintf_int(buf, "Max Donation: Rs ", (int)getMax(sys->maxDonations));
    DrawTextEx(customFont, buf, (Vector2){(float)startX, 200}, 36, 1, textWhite);
    
    my_sprintf_int(buf, "Min Donation: Rs ", (int)getMin(sys->minDonations));
    DrawTextEx(customFont, buf, (Vector2){(float)startX, 260}, 36, 1, textWhite);
    
    if (DrawButton((Rectangle){(float)startX, (float)sh - 150, 300, 60}, "Seed Dummy Mode Data")) {
        seedDummyData(sys);
    }
}

void runGUI(SystemManager* sys) {
    while (!WindowShouldClose()) {
        UpdateTextBoxes(allTextBoxes, 8);

        BeginDrawing();
        ClearBackground(bgDark);
        DrawSidebar();
        
        int sw = GetScreenWidth();
        float contentX = 300 + (sw - 300) / 2.0f; // Center of content area
        
        if (currentScreen == DASHBOARD) {
            DrawDashboard(sys);
        } else if (currentScreen == DONORS) {
            DrawTextEx(customFontBold, "Register Donor", (Vector2){contentX - 250, 150}, 50, 1, textWhite);
            tDonorName.bounds = { contentX - 250, 250, 500, 60 };
            tDonorCnic.bounds = { contentX - 250, 330, 500, 60 };
            DrawTextBox(tDonorName);
            DrawTextBox(tDonorCnic);
            
            if (DrawButton((Rectangle){contentX - 150, 430, 300, 60}, "Register")) {
                if (tDonorName.letterCount > 0) {
                    registerDonor(sys, tDonorName.text, tDonorCnic.text);
                    ShowPopup("Donor Registered Successfully!");
                    ResetTextBox(tDonorName); ResetTextBox(tDonorCnic);
                }
            }
        } else if (currentScreen == PROJECTS) {
            DrawTextEx(customFontBold, "Create Project", (Vector2){contentX - 250, 150}, 50, 1, textWhite);
            tProjName.bounds = { contentX - 250, 250, 500, 60 };
            tProjDesc.bounds = { contentX - 250, 330, 500, 60 };
            DrawTextBox(tProjName);
            DrawTextBox(tProjDesc);
            
            if (DrawButton((Rectangle){contentX - 150, 430, 300, 60}, "Create")) {
                if (tProjName.letterCount > 0) {
                    createProject(sys, tProjName.text, tProjDesc.text);
                    ShowPopup("Project Created Successfully!");
                    ResetTextBox(tProjName); ResetTextBox(tProjDesc);
                }
            }
        } else if (currentScreen == DONATIONS) {
            DrawTextEx(customFontBold, "Make Donation", (Vector2){contentX - 250, 100}, 50, 1, textWhite);
            tDonID.bounds = { contentX - 250, 200, 500, 60 };
            tDonAmt.bounds = { contentX - 250, 280, 500, 60 };
            tDonDate.bounds = { contentX - 250, 360, 500, 60 };
            DrawTextBox(tDonID); DrawTextBox(tDonAmt); DrawTextBox(tDonDate);
            
            if (DrawButton((Rectangle){contentX - 150, 460, 300, 60}, "Donate")) {
                if (tDonAmt.letterCount > 0) {
                    makeDonation(sys, my_atoi(tDonID.text), my_atof(tDonAmt.text), tDonDate.text);
                    ShowPopup("Donation Processed!");
                    ResetTextBox(tDonID); ResetTextBox(tDonAmt); ResetTextBox(tDonDate);
                }
            }
        } else if (currentScreen == REPORT) {
            DrawTextEx(customFontBold, "Transparency Report", (Vector2){contentX - 300, 100}, 50, 1, textWhite);
            tRepID.bounds = { contentX - 300, 180, 400, 60 };
            DrawTextBox(tRepID);
            
            if (tRepID.letterCount > 0) {
                int donID = my_atoi(tRepID.text);
                Donation* d = searchDonation(sys->donationRecords, donID);
                if (d != NULL) {
                    Rectangle panel = { contentX - 300, 280, 600, 350 };
                    DrawRectangleRec(panel, panelDark);
                    
                    char buf[100];
                    my_sprintf_int(buf, "Initial Donation: Rs ", (int)d->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 40}, 32, 1, accentCyan);
                    
                    DrawLine(panel.x + 30, panel.y + 110, panel.x + 570, panel.y + 110, textMuted);
                    
                    my_sprintf_int(buf, "Remaining: Rs ", (int)d->remainingAmount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 150}, 32, 1, textWhite);
                    
                    const char* stat = "Status: Pending";
                    if(d->currentStatus == STATUS_ALLOCATED) stat = "Status: Allocated";
                    else if(d->currentStatus == STATUS_IN_PROGRESS) stat = "Status: In Progress";
                    else if(d->currentStatus == STATUS_COMPLETED) stat = "Status: Completed";
                    DrawTextEx(customFont, stat, (Vector2){panel.x + 30, panel.y + 200}, 28, 1, textMuted);
                    
                } else {
                    DrawTextEx(customFont, "Donation Not Found.", (Vector2){contentX - 300, 280}, 32, 1, errorRed);
                }
            }
        }
        
        // Popup Animation
        if (popupTimer > 0) {
            popupY += (0 - popupY) * 0.1f; // slide in
            DrawRectangle(300, popupY, sw - 300, 80, accentCyanDark);
            Vector2 ts = MeasureTextEx(customFontBold, popupMsg, 36, 1);
            DrawTextEx(customFontBold, popupMsg, (Vector2){300 + (sw - 300 - ts.x)/2, popupY + 20}, 36, 1, bgDark);
            popupTimer -= GetFrameTime();
        } else {
            popupY = -100;
        }
        
        EndDrawing();
    }
}

void closeGUI() {
    CloseWindow();
}
