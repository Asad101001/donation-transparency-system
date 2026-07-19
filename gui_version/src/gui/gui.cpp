#include "gui.h"
#include "raylib.h"
#include "../core/Utils.h"

// String utilities
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
Color bgDark = { 15, 15, 20, 255 };
Color panelDark = { 25, 25, 35, 255 };
Color panelLight = { 40, 40, 55, 255 };
Color accentCyan = { 0, 200, 255, 255 };
Color accentCyanDark = { 0, 150, 200, 255 };
Color textWhite = { 245, 245, 250, 255 };
Color textMuted = { 140, 140, 160, 255 };
Color errorRed = { 255, 80, 80, 255 };
Color successGreen = { 80, 220, 120, 255 };

Font customFont;
Font customFontBold;

enum AppScreen { HOME, CONTRIBUTORS, CAUSES, TRACK };
AppScreen currentScreen = HOME;

struct TextBox {
    Rectangle bounds;
    char text[100];
    int letterCount;
    const char* placeholder;
};

TextBox* activeTextBox = NULL;

void DrawTextBox(TextBox& tb) {
    bool isActive = (activeTextBox == &tb);
    DrawRectangleRec(tb.bounds, isActive ? panelLight : panelDark);
    DrawRectangleLinesEx(tb.bounds, isActive ? 2 : 1, isActive ? accentCyan : textMuted);
    
    if (tb.letterCount == 0 && !isActive) {
        DrawTextEx(customFont, tb.placeholder, (Vector2){tb.bounds.x + 15, tb.bounds.y + tb.bounds.height/2 - 12}, 24, 1, textMuted);
    } else {
        DrawTextEx(customFont, tb.text, (Vector2){tb.bounds.x + 15, tb.bounds.y + tb.bounds.height/2 - 12}, 24, 1, textWhite);
    }
    
    if (isActive && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(customFont, tb.text, 24, 1);
        DrawLine(tb.bounds.x + 17 + textSize.x, tb.bounds.y + 12, tb.bounds.x + 17 + textSize.x, tb.bounds.y + tb.bounds.height - 12, accentCyan);
    }
}

void UpdateTextBoxes(TextBox** textboxes, int count) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool clickedInside = false;
        for(int i = 0; i < count; i++) {
            if (CheckCollisionPointRec(GetMousePosition(), textboxes[i]->bounds)) {
                activeTextBox = textboxes[i];
                clickedInside = true;
                break;
            }
        }
        if (!clickedInside) activeTextBox = NULL;
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
        
        if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
            activeTextBox->letterCount--;
            if (activeTextBox->letterCount < 0) activeTextBox->letterCount = 0;
            activeTextBox->text[activeTextBox->letterCount] = '\0';
        }
    }
}

bool DrawButton(Rectangle bounds, const char* text, Color color = accentCyan) {
    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    
    DrawRectangleRec(bounds, hovered ? panelLight : color);
    if (!hovered) DrawRectangleLinesEx(bounds, 1, textMuted);
    
    Color textColor = hovered ? textWhite : bgDark;
    if (color.r == bgDark.r && color.g == bgDark.g && color.b == bgDark.b) textColor = textWhite;
    
    Vector2 tSize = MeasureTextEx(customFontBold, text, 20, 1);
    DrawTextEx(customFontBold, text, (Vector2){bounds.x + (bounds.width - tSize.x)/2, bounds.y + (bounds.height - tSize.y)/2}, 20, 1, textColor);
    
    return clicked;
}

// Global Popups
char popupMsg[200] = "";
float popupTimer = 0.0f;
float popupY = -100;
Color popupColor = accentCyan;

void ShowPopup(const char* msg, Color col = accentCyanDark) {
    my_strcpy(popupMsg, msg);
    popupTimer = 3.0f;
    popupY = -100;
    popupColor = col;
}

// Forms Data
TextBox tDonorName = { { 0, 0, 400, 50 }, "", 0, "Name" };
TextBox tDonorCnic = { { 0, 0, 400, 50 }, "", 0, "National ID (CNIC)" };
TextBox tDonAmt = { { 0, 0, 400, 50 }, "", 0, "Amount (Rs)" };
TextBox tDonDate = { { 0, 0, 400, 50 }, "", 0, "Date (DD/MM/YYYY)" };

TextBox tProjName = { { 0, 0, 400, 50 }, "", 0, "Cause Name" };
TextBox tProjDesc = { { 0, 0, 400, 50 }, "", 0, "Short Description" };
TextBox tExpAmt = { { 0, 0, 400, 50 }, "", 0, "Expense Amount" };
TextBox tExpDesc = { { 0, 0, 400, 50 }, "", 0, "What was purchased?" };
TextBox tExpDate = { { 0, 0, 400, 50 }, "", 0, "Date" };

TextBox* allTextBoxes[] = { &tDonorName, &tDonorCnic, &tDonAmt, &tDonDate, &tProjName, &tProjDesc, &tExpAmt, &tExpDesc, &tExpDate };

int selectedDonorID = -1;
int selectedProjectID = -1;
int selectedReportDonationID = -1;

void ResetTextBox(TextBox& tb) { tb.text[0] = '\0'; tb.letterCount = 0; }
int my_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) { if(str[i] >= '0' && str[i] <= '9') res = res * 10 + str[i] - '0'; }
    return res;
}
double my_atof(const char* str) { return (double)my_atoi(str); }

void initGUI() {
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Donation Transparency System");
    SetExitKey(KEY_NULL); 
    
    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    ToggleFullscreen();
    
    customFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 64, 0, 250);
    customFontBold = LoadFontEx("C:\\Windows\\Fonts\\segoeuib.ttf", 64, 0, 250);
    if (customFont.baseSize == 0) customFont = GetFontDefault();
    if (customFontBold.baseSize == 0) customFontBold = GetFontDefault();
}

bool wantsToExit = false;

void DrawSidebar() {
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, 280, sh, panelDark);
    DrawTextEx(customFontBold, "Transparency", (Vector2){30, 40}, 36, 1, accentCyan);
    DrawLine(30, 100, 250, 100, panelLight);
    
    const char* menus[] = { "Home", "Contributors", "Causes", "Track Funds", "EXIT APP" };
    AppScreen screens[] = { HOME, CONTRIBUTORS, CAUSES, TRACK, TRACK };
    
    for(int i = 0; i < 5; i++) {
        Rectangle btn = { 0, (float)130 + i * 70, 280, 60 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
        bool active = (currentScreen == screens[i] && i != 4);
        
        if (hovered || active) {
            DrawRectangleRec(btn, active ? accentCyanDark : (i == 4 ? errorRed : panelLight));
            DrawRectangle(0, btn.y, 6, btn.height, active ? textWhite : bgDark); 
        }
        DrawTextEx(customFont, menus[i], (Vector2){40, btn.y + 18}, 26, 1, active ? bgDark : textWhite);
        
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (i == 4) wantsToExit = true; 
            else { 
                currentScreen = screens[i]; 
                activeTextBox = NULL; 
                selectedDonorID = -1;
                selectedProjectID = -1;
            }
        }
    }
}

void seedDummyData(SystemManager* sys) {
    registerDonor(sys, "Alice", "12345");
    registerDonor(sys, "Bob", "67890");
    createProject(sys, "Water_Well", "Clean_Water");
    createProject(sys, "School", "Education_Fund");
    makeDonation(sys, 1, 15000, "20/07/2026");
    makeDonation(sys, 2, 5000, "21/07/2026");
    allocateDonation(sys, 10);
    addExpense(sys, 10, 5000, "Pipes", "22/07/2026");
    ShowPopup("Example Data Added!", successGreen);
}

int projCount = 0;
Project projArray[100];
void flattenProjects(Project* root) {
    if (root == NULL || projCount >= 100) return;
    flattenProjects(root->left);
    projArray[projCount++] = *root;
    flattenProjects(root->right);
}

void runGUI(SystemManager* sys) {
    while (!WindowShouldClose() && !wantsToExit) {
        UpdateTextBoxes(allTextBoxes, 9);

        BeginDrawing();
        ClearBackground(bgDark);
        DrawSidebar();
        
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float contentX = 330; 
        
        if (currentScreen == HOME) {
            DrawTextEx(customFontBold, "Welcome", (Vector2){contentX, 60}, 60, 1, textWhite);
            DrawTextEx(customFont, "Manage and track funds with complete transparency.", (Vector2){contentX, 130}, 28, 1, textMuted);
            
            Rectangle stat1 = { contentX, 220, 350, 150 };
            DrawRectangleRec(stat1, panelDark);
            DrawTextEx(customFont, "Largest Received", (Vector2){stat1.x + 20, stat1.y + 20}, 24, 1, textMuted);
            char buf[100]; my_sprintf_int(buf, "Rs ", (int)getMax(sys->maxDonations));
            DrawTextEx(customFontBold, buf, (Vector2){stat1.x + 20, stat1.y + 70}, 40, 1, accentCyan);
            
            int count = 0; double total = 0;
            Donation* temp = sys->donationRecords->head;
            while(temp != NULL) { count++; total += temp->amount; temp = temp->next; }
            
            Rectangle stat3 = { contentX + 380, 220, 350, 150 };
            DrawRectangleRec(stat3, panelDark);
            DrawTextEx(customFont, "Total Contributions", (Vector2){stat3.x + 20, stat3.y + 20}, 24, 1, textMuted);
            my_sprintf_int(buf, "", count);
            DrawTextEx(customFontBold, buf, (Vector2){stat3.x + 20, stat3.y + 70}, 40, 1, textWhite);
            
            if (DrawButton((Rectangle){contentX, 420, 400, 60}, "Load Example Data", panelLight)) {
                seedDummyData(sys);
            }
            
        } else if (currentScreen == CONTRIBUTORS) {
            DrawTextEx(customFontBold, "Add New Contributor", (Vector2){contentX, 50}, 36, 1, textWhite);
            tDonorName.bounds = { contentX, 110, 400, 50 };
            tDonorCnic.bounds = { contentX, 180, 400, 50 };
            DrawTextBox(tDonorName); DrawTextBox(tDonorCnic);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Add Person", successGreen)) {
                if (tDonorName.letterCount > 0) {
                    registerDonor(sys, tDonorName.text, tDonorCnic.text);
                    ShowPopup("Contributor Added!", successGreen);
                    ResetTextBox(tDonorName); ResetTextBox(tDonorCnic);
                }
            }
            
            DrawLine(contentX + 450, 50, contentX + 450, sh - 50, panelLight);
            
            DrawTextEx(customFontBold, "Receive Funds", (Vector2){contentX + 500, 50}, 36, 1, textWhite);
            DrawTextEx(customFont, "Click someone to receive money from them.", (Vector2){contentX + 500, 100}, 20, 1, textMuted);
            
            int yOff = 140;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) {
                    Rectangle card = { contentX + 500, (float)yOff, 500, selectedDonorID == cur->donor.donorID ? 240.0f : 60.0f };
                    bool hover = CheckCollisionPointRec(GetMousePosition(), {card.x, card.y, card.width, 60});
                    DrawRectangleRec(card, (hover || selectedDonorID == cur->donor.donorID) ? panelLight : panelDark);
                    
                    DrawTextEx(customFontBold, cur->donor.name, (Vector2){card.x + 20, card.y + 18}, 24, 1, textWhite);
                    DrawTextEx(customFont, "Click to receive funds", (Vector2){card.x + 300, card.y + 20}, 18, 1, textMuted);
                    
                    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedDonorID = (selectedDonorID == cur->donor.donorID) ? -1 : cur->donor.donorID;
                    }
                    
                    if (selectedDonorID == cur->donor.donorID) {
                        tDonAmt.bounds = { card.x + 20, card.y + 70, 460, 45 };
                        tDonDate.bounds = { card.x + 20, card.y + 125, 460, 45 };
                        DrawTextBox(tDonAmt); DrawTextBox(tDonDate);
                        if (DrawButton((Rectangle){card.x + 20, card.y + 180, 200, 45}, "Confirm Funds", successGreen)) {
                            if (tDonAmt.letterCount > 0) {
                                makeDonation(sys, selectedDonorID, my_atof(tDonAmt.text), tDonDate.text);
                                ShowPopup("Funds Received!", successGreen);
                                ResetTextBox(tDonAmt); ResetTextBox(tDonDate);
                                selectedDonorID = -1;
                            }
                        }
                    }
                    
                    yOff += card.height + 10;
                    cur = cur->next;
                }
            }
            
        } else if (currentScreen == CAUSES) {
            DrawTextEx(customFontBold, "Start a New Cause", (Vector2){contentX, 50}, 36, 1, textWhite);
            tProjName.bounds = { contentX, 110, 400, 50 };
            tProjDesc.bounds = { contentX, 180, 400, 50 };
            DrawTextBox(tProjName); DrawTextBox(tProjDesc);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Create Cause", successGreen)) {
                if (tProjName.letterCount > 0) {
                    createProject(sys, tProjName.text, tProjDesc.text);
                    ShowPopup("Cause Started!", successGreen);
                    ResetTextBox(tProjName); ResetTextBox(tProjDesc);
                }
            }
            
            DrawLine(contentX + 450, 50, contentX + 450, sh - 50, panelLight);
            
            DrawTextEx(customFontBold, "Manage Causes", (Vector2){contentX + 500, 50}, 36, 1, textWhite);
            
            if (!isQueueEmpty(sys->allocationQueue)) {
                Allocation nextAlloc = sys->allocationQueue->front->data;
                char buf[100]; my_sprintf_int(buf, "Funds waiting to be assigned: Rs ", (int)nextAlloc.allocatedAmount);
                DrawTextEx(customFontBold, buf, (Vector2){contentX + 500, 100}, 24, 1, accentCyan);
            } else {
                DrawTextEx(customFont, "No funds currently waiting to be assigned.", (Vector2){contentX + 500, 100}, 20, 1, textMuted);
            }
            
            projCount = 0; flattenProjects(sys->projects->root);
            int yOff = 150;
            for(int i = 0; i < projCount; i++) {
                Rectangle card = { contentX + 500, (float)yOff, 500, selectedProjectID == projArray[i].projectID ? 260.0f : 80.0f };
                bool hover = CheckCollisionPointRec(GetMousePosition(), {card.x, card.y, card.width, 80});
                DrawRectangleRec(card, (hover || selectedProjectID == projArray[i].projectID) ? panelLight : panelDark);
                
                DrawTextEx(customFontBold, projArray[i].name, (Vector2){card.x + 20, card.y + 15}, 26, 1, textWhite);
                
                if (!isQueueEmpty(sys->allocationQueue)) {
                    if (DrawButton((Rectangle){card.x + 320, card.y + 20, 160, 40}, "Assign Funds", accentCyan)) {
                        allocateDonation(sys, projArray[i].projectID);
                        ShowPopup("Funds Assigned!", successGreen);
                    }
                }
                
                DrawTextEx(customFont, "Click to log an expense", (Vector2){card.x + 20, card.y + 50}, 16, 1, textMuted);
                
                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedProjectID = (selectedProjectID == projArray[i].projectID) ? -1 : projArray[i].projectID;
                }
                
                if (selectedProjectID == projArray[i].projectID) {
                    tExpAmt.bounds = { card.x + 20, card.y + 90, 460, 40 };
                    tExpDesc.bounds = { card.x + 20, card.y + 140, 460, 40 };
                    tExpDate.bounds = { card.x + 20, card.y + 190, 460, 40 };
                    DrawTextBox(tExpAmt); DrawTextBox(tExpDesc); DrawTextBox(tExpDate);
                    
                    if (DrawButton((Rectangle){card.x + 20, card.y + 240, 200, 40}, "Log Expense", errorRed)) {
                        if (tExpAmt.letterCount > 0) {
                            addExpense(sys, selectedProjectID, my_atof(tExpAmt.text), tExpDesc.text, tExpDate.text);
                            ShowPopup("Expense Logged!", successGreen);
                            ResetTextBox(tExpAmt); ResetTextBox(tExpDesc); ResetTextBox(tExpDate);
                            selectedProjectID = -1;
                        }
                    }
                }
                
                yOff += card.height + 10;
            }
            
        } else if (currentScreen == TRACK) {
            DrawTextEx(customFontBold, "Track Funds", (Vector2){contentX, 50}, 36, 1, textWhite);
            
            if (selectedReportDonationID != -1) {
                Donation* d = searchDonation(sys->donationRecords, selectedReportDonationID);
                if (d != NULL) {
                    Rectangle panel = { contentX, 120, 450, 400 };
                    DrawRectangleRec(panel, panelDark);
                    
                    char buf[100];
                    my_sprintf_int(buf, "Funds Received: Rs ", (int)d->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 40}, 32, 1, accentCyan);
                    
                    DrawLine(panel.x + 30, panel.y + 100, panel.x + 420, panel.y + 100, textMuted);
                    
                    my_sprintf_int(buf, "Unspent Balance: Rs ", (int)d->remainingAmount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 140}, 32, 1, textWhite);
                    
                    const char* stat = "Status: Waiting for Cause";
                    Color statCol = textMuted;
                    if(d->currentStatus == STATUS_ALLOCATED) { stat = "Status: Assigned to Cause"; statCol = accentCyan; }
                    else if(d->currentStatus == STATUS_IN_PROGRESS) { stat = "Status: Being Spent"; statCol = successGreen; }
                    else if(d->currentStatus == STATUS_COMPLETED) { stat = "Status: 100% Utilized"; statCol = successGreen; }
                    
                    DrawRectangle(panel.x + 30, panel.y + 220, 390, 50, panelLight);
                    DrawTextEx(customFontBold, stat, (Vector2){panel.x + 45, panel.y + 232}, 24, 1, statCol);
                }
            } else {
                DrawTextEx(customFont, "Click a contribution on the right to track it.", (Vector2){contentX, 120}, 22, 1, textMuted);
            }
            
            DrawLine(contentX + 500, 50, contentX + 500, sh - 50, panelLight);
            DrawTextEx(customFontBold, "Contributions:", (Vector2){contentX + 550, 50}, 30, 1, textMuted);
            
            int yOff = 100;
            Donation* cur = sys->donationRecords->head;
            while(cur != NULL && yOff < sh - 100) {
                Rectangle card = { contentX + 550, (float)yOff, 400, 60 };
                bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                DrawRectangleRec(card, (hover || selectedReportDonationID == cur->donationID) ? panelLight : panelDark);
                
                char buf[200]; my_sprintf_int(buf, "Rs ", (int)cur->amount);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 15}, 28, 1, textWhite);
                
                const char* stat = "Waiting";
                if(cur->currentStatus >= STATUS_ALLOCATED) stat = "Assigned";
                if(cur->currentStatus == STATUS_COMPLETED) stat = "Utilized";
                DrawTextEx(customFont, stat, (Vector2){card.x + 280, card.y + 20}, 20, 1, textMuted);
                
                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedReportDonationID = cur->donationID;
                }
                
                yOff += 70;
                cur = cur->next;
            }
        }
        
        if (popupTimer > 0) {
            popupY += (0 - popupY) * 0.15f; 
            DrawRectangle(320, popupY, sw - 320, 70, popupColor);
            Vector2 ts = MeasureTextEx(customFontBold, popupMsg, 28, 1);
            DrawTextEx(customFontBold, popupMsg, (Vector2){320 + (sw - 320 - ts.x)/2, popupY + 20}, 28, 1, popupColor.r == bgDark.r ? textWhite : bgDark);
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
