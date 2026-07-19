#include "gui.h"
#include "raylib.h"
#include "../core/Utils.h"
#include <ctime>
#include <cstdio>

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
Color bgDark = { 10, 10, 15, 255 };
Color panelDark = { 20, 20, 28, 255 };
Color panelLight = { 35, 35, 45, 255 };
Color accentCyan = { 0, 180, 240, 255 };
Color accentCyanDark = { 0, 120, 180, 255 };
Color textWhite = { 245, 245, 250, 255 };
Color textMuted = { 130, 130, 150, 255 };
Color errorRed = { 240, 70, 70, 255 };
Color successGreen = { 60, 200, 110, 255 };

Font customFont;
Font customFontBold;

enum AppScreen { HOME, CONTRIBUTORS, CAUSES, TRACK };
AppScreen currentScreen = HOME;

struct TextBox {
    Rectangle bounds;
    char text[100];
    int letterCount;
    const char* placeholder;
    bool isNumeric;
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
            // Only check collision if bounds are actually valid (width > 0)
            if (textboxes[i]->bounds.width > 0 && CheckCollisionPointRec(GetMousePosition(), textboxes[i]->bounds)) {
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
            bool valid = false;
            if (activeTextBox->isNumeric) {
                if ((key >= '0' && key <= '9') || key == '.' || key == '-') valid = true;
            } else {
                if (key >= 32 && key <= 125) valid = true;
            }
            
            if (valid && (activeTextBox->letterCount < 99)) {
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
TextBox tDonorName = { { 0, 0, 0, 0 }, "", 0, "Name", false };
TextBox tDonorCnic = { { 0, 0, 0, 0 }, "", 0, "National ID (CNIC - Numbers Only)", true };
TextBox tDonAmt = { { 0, 0, 0, 0 }, "", 0, "Amount (Rs)", true };

TextBox tProjName = { { 0, 0, 0, 0 }, "", 0, "Cause Name", false };
TextBox tProjDesc = { { 0, 0, 0, 0 }, "", 0, "Short Description", false };
TextBox tExpAmt = { { 0, 0, 0, 0 }, "", 0, "Expense Amount", true };
TextBox tExpDesc = { { 0, 0, 0, 0 }, "", 0, "What was purchased?", false };

TextBox* allTextBoxes[] = { &tDonorName, &tDonorCnic, &tDonAmt, &tProjName, &tProjDesc, &tExpAmt, &tExpDesc };

int selectedDonorID = -1;
int selectedProjectID = -1;
int selectedReportDonationID = -1;

float donorAnimHeights[1000] = {0};
float projAnimHeights[1000] = {0};

void ResetTextBox(TextBox& tb) { tb.text[0] = '\0'; tb.letterCount = 0; }
int my_atoi(const char* str) {
    int res = 0;
    for (int i = 0; str[i] != '\0'; ++i) { if(str[i] >= '0' && str[i] <= '9') res = res * 10 + str[i] - '0'; }
    return res;
}
double my_atof(const char* str) { return (double)my_atoi(str); }

void initGUI() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Donation Transparency System");
    SetWindowMinSize(1024, 768);
    
    // Automatically make window maximize to screen bounds while retaining title bar
    SetWindowState(FLAG_WINDOW_MAXIMIZED);
    SetExitKey(KEY_NULL); // Prevent ESC from exiting the application
    
    customFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 64, 0, 250);
    customFontBold = LoadFontEx("C:\\Windows\\Fonts\\segoeuib.ttf", 64, 0, 250);
    if (customFont.baseSize == 0) customFont = GetFontDefault();
    if (customFontBold.baseSize == 0) customFontBold = GetFontDefault();
}

bool wantsToExit = false;
float scrollOffset = 0.0f;

void DrawSidebar() {
    int sh = GetScreenHeight();
    DrawRectangle(0, 0, 280, sh, panelDark);
    DrawTextEx(customFontBold, "Transparency", (Vector2){30, 40}, 36, 1, accentCyan);
    DrawLine(30, 100, 250, 100, panelLight);
    
    const char* menus[] = { "Home", "Contributors", "Causes", "Track Funds" };
    AppScreen screens[] = { HOME, CONTRIBUTORS, CAUSES, TRACK };
    
    for(int i = 0; i < 4; i++) {
        Rectangle btn = { 0, (float)130 + i * 70, 280, 60 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
        bool active = (currentScreen == screens[i]);
        
        if (hovered || active) {
            DrawRectangleRec(btn, active ? accentCyanDark : panelLight);
            DrawRectangle(0, btn.y, 6, btn.height, active ? textWhite : bgDark); 
        }
        DrawTextEx(customFont, menus[i], (Vector2){40, btn.y + 18}, 26, 1, active ? bgDark : textWhite);
        
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            currentScreen = screens[i]; 
            activeTextBox = NULL; 
            selectedDonorID = -1;
            selectedProjectID = -1;
            scrollOffset = 0.0f;
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
    char currentDate[20];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    
    while (!WindowShouldClose() && !wantsToExit) {
        
        // Update input using the bounds calculated and drawn from the PREVIOUS frame
        UpdateTextBoxes(allTextBoxes, 7);
        
        // Reset bounds of all text boxes to 0 so hidden ones don't steal clicks THIS frame
        for (int i = 0; i < 7; i++) allTextBoxes[i]->bounds = {0,0,0,0};
        
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollOffset -= wheel * 40.0f;
            if (scrollOffset < 0) scrollOffset = 0;
        }
        
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float contentX = 330; 
        
        // Setup bounds based on current screen ONLY
        if (currentScreen == CONTRIBUTORS) {
            tDonorName.bounds = { contentX, 110, 400, 50 };
            tDonorCnic.bounds = { contentX, 180, 400, 50 };
            // tDonAmt bounds will be set during draw if a card is expanded
        } else if (currentScreen == CAUSES) {
            tProjName.bounds = { contentX, 110, 400, 50 };
            tProjDesc.bounds = { contentX, 180, 400, 50 };
            // tExpAmt/Desc bounds set during draw
        }

        BeginDrawing();
        ClearBackground(bgDark);
        DrawSidebar();
        
        if (currentScreen == HOME) {
            DrawTextEx(customFontBold, "Welcome to the DT System", (Vector2){contentX, 50}, 50, 1, textWhite);
            DrawTextEx(customFont, "Manage and track funds with complete transparency.", (Vector2){contentX, 110}, 24, 1, textMuted);
            
            Rectangle guide = { contentX, 160, 800, 260 };
            DrawRectangleRec(guide, panelDark);
            DrawRectangleLinesEx(guide, 2, accentCyan);
            
            DrawTextEx(customFontBold, "How to use this system (The Workflow):", (Vector2){guide.x + 20, guide.y + 20}, 28, 1, accentCyan);
            
            const char* steps[] = {
                "1. Receive Money: Go to 'Contributors', add people, and receive funds from them.",
                "2. Create Causes: Go to 'Causes' and start welfare projects.",
                "3. Assign Funds: Inside 'Causes', click 'Assign Funds' to move pending money to a project.",
                "4. Spend Funds: Click on any Cause card to 'Log Expense' and utilize the money.",
                "5. Track It All: Go to 'Track Funds' to see exactly where every penny went!"
            };
            
            for(int i = 0; i < 5; i++) {
                DrawTextEx(customFont, steps[i], (Vector2){guide.x + 20, guide.y + 70 + (i * 35)}, 20, 1, textWhite);
            }
            
            int count = 0; double total = 0;
            Donation* temp = sys->donationRecords->head;
            while(temp != NULL) { count++; total += temp->amount; temp = temp->next; }
            
            Rectangle stat1 = { contentX, 450, 380, 120 };
            DrawRectangleRec(stat1, panelDark);
            DrawTextEx(customFont, "Largest Donation Received", (Vector2){stat1.x + 20, stat1.y + 20}, 20, 1, textMuted);
            char buf[100]; my_sprintf_int(buf, "Rs ", (int)getMax(sys->maxDonations));
            DrawTextEx(customFontBold, buf, (Vector2){stat1.x + 20, stat1.y + 60}, 36, 1, accentCyan);
            
            Rectangle stat2 = { contentX + 420, 450, 380, 120 };
            DrawRectangleRec(stat2, panelDark);
            DrawTextEx(customFont, "Total Contributions", (Vector2){stat2.x + 20, stat2.y + 20}, 20, 1, textMuted);
            my_sprintf_int(buf, "", count);
            DrawTextEx(customFontBold, buf, (Vector2){stat2.x + 20, stat2.y + 60}, 36, 1, textWhite);
            
            if (DrawButton((Rectangle){contentX, 600, 800, 60}, "Click Here to Load Example Data (Highly Recommended!)", panelLight)) {
                seedDummyData(sys);
            }
            
        } else if (currentScreen == CONTRIBUTORS) {
            DrawTextEx(customFontBold, "Add New Contributor", (Vector2){contentX, 50}, 36, 1, textWhite);
            DrawTextBox(tDonorName); DrawTextBox(tDonorCnic);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Add Person", successGreen)) {
                if (tDonorName.letterCount > 0) {
                    registerDonor(sys, tDonorName.text, tDonorCnic.text);
                    ShowPopup("Contributor Added!", successGreen);
                    ResetTextBox(tDonorName); ResetTextBox(tDonorCnic);
                } else {
                    ShowPopup("Please enter a name first.", errorRed);
                }
            }
            
            DrawLine(contentX + 450, 50, contentX + 450, sh - 50, panelLight);
            
            DrawTextEx(customFontBold, "Receive Funds", (Vector2){contentX + 500, 50}, 36, 1, textWhite);
            DrawTextEx(customFont, "Click someone to receive money from them.", (Vector2){contentX + 500, 100}, 20, 1, textMuted);
            
            BeginScissorMode(contentX + 500, 140, sw - (contentX + 500), sh - 140);
            float yOff = 140 - scrollOffset;
            int donorIdx = 0;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) {
                    float targetH = selectedDonorID == cur->donor.donorID ? 200.0f : 60.0f;
                    if (donorAnimHeights[donorIdx] < 60.0f) donorAnimHeights[donorIdx] = 60.0f;
                    donorAnimHeights[donorIdx] += (targetH - donorAnimHeights[donorIdx]) * 0.15f;
                    
                    Rectangle card = { contentX + 500, yOff, 500, donorAnimHeights[donorIdx] };
                    
                    if (yOff + card.height > 140 && yOff < sh) {
                        bool hover = CheckCollisionPointRec(GetMousePosition(), {card.x, card.y, card.width, 60});
                        DrawRectangleRec(card, (hover || selectedDonorID == cur->donor.donorID) ? panelLight : panelDark);
                        
                        DrawTextEx(customFontBold, cur->donor.name, (Vector2){card.x + 20, card.y + 18}, 24, 1, textWhite);
                        DrawTextEx(customFont, "Click to receive funds", (Vector2){card.x + 300, card.y + 20}, 18, 1, textMuted);
                        
                        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            selectedDonorID = (selectedDonorID == cur->donor.donorID) ? -1 : cur->donor.donorID;
                            activeTextBox = NULL;
                        }
                        
                        if (selectedDonorID == cur->donor.donorID && donorAnimHeights[donorIdx] > 180.0f) {
                            tDonAmt.bounds = { card.x + 20, card.y + 70, 460, 45 };
                            DrawTextBox(tDonAmt);
                            
                            char autoDateMsg[100];
                            sprintf(autoDateMsg, "Date: %s (Auto-filled)", currentDate);
                            DrawTextEx(customFont, autoDateMsg, (Vector2){card.x + 20, card.y + 130}, 18, 1, textMuted);
                            
                            if (DrawButton((Rectangle){card.x + 320, card.y + 130, 160, 45}, "Confirm", successGreen)) {
                                if (tDonAmt.letterCount > 0) {
                                    makeDonation(sys, selectedDonorID, my_atof(tDonAmt.text), currentDate);
                                    ShowPopup("Funds Received!", successGreen);
                                    ResetTextBox(tDonAmt);
                                    selectedDonorID = -1;
                                } else {
                                    ShowPopup("Please enter an amount.", errorRed);
                                }
                            }
                        }
                    }
                    
                    yOff += card.height + 10;
                    cur = cur->next;
                    donorIdx++;
                }
            }
            EndScissorMode();
            
        } else if (currentScreen == CAUSES) {
            DrawTextEx(customFontBold, "Start a New Cause", (Vector2){contentX, 50}, 36, 1, textWhite);
            DrawTextBox(tProjName); DrawTextBox(tProjDesc);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Create Cause", successGreen)) {
                if (tProjName.letterCount > 0) {
                    createProject(sys, tProjName.text, tProjDesc.text);
                    ShowPopup("Cause Started!", successGreen);
                    ResetTextBox(tProjName); ResetTextBox(tProjDesc);
                } else {
                    ShowPopup("Please enter a name.", errorRed);
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
            
            BeginScissorMode(contentX + 500, 150, sw - (contentX + 500), sh - 150);
            float yOff = 150 - scrollOffset;
            for(int i = 0; i < projCount; i++) {
                float targetH = selectedProjectID == projArray[i].projectID ? 220.0f : 80.0f;
                if (projAnimHeights[i] < 80.0f) projAnimHeights[i] = 80.0f;
                projAnimHeights[i] += (targetH - projAnimHeights[i]) * 0.15f;
                
                Rectangle card = { contentX + 500, yOff, 500, projAnimHeights[i] };
                
                if (yOff + card.height > 150 && yOff < sh) {
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
                        activeTextBox = NULL;
                    }
                    
                    if (selectedProjectID == projArray[i].projectID && projAnimHeights[i] > 200.0f) {
                        tExpAmt.bounds = { card.x + 20, card.y + 90, 460, 40 };
                        tExpDesc.bounds = { card.x + 20, card.y + 140, 460, 40 };
                        DrawTextBox(tExpAmt); DrawTextBox(tExpDesc);
                        
                        if (DrawButton((Rectangle){card.x + 20, card.y + 190, 200, 30}, "Log Expense", errorRed)) {
                            if (tExpAmt.letterCount > 0) {
                                addExpense(sys, selectedProjectID, my_atof(tExpAmt.text), tExpDesc.text, currentDate);
                                ShowPopup("Expense Logged!", successGreen);
                                ResetTextBox(tExpAmt); ResetTextBox(tExpDesc);
                                selectedProjectID = -1;
                            } else {
                                ShowPopup("Enter an expense amount.", errorRed);
                            }
                        }
                    }
                }
                
                yOff += card.height + 10;
            }
            EndScissorMode();
            
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
                    
                    // Render DSA Stack History
                    DrawTextEx(customFontBold, "Status History (DSA Stack):", (Vector2){panel.x + 30, panel.y + 300}, 20, 1, textMuted);
                    int histY = panel.y + 330;
                    StatusNode* hCur = d->historyTop;
                    while (hCur != NULL) {
                        const char* hStat = "Received";
                        if(hCur->status == STATUS_ALLOCATED) hStat = "Assigned";
                        if(hCur->status == STATUS_IN_PROGRESS) hStat = "In Progress";
                        if(hCur->status == STATUS_COMPLETED) hStat = "Completed";
                        
                        DrawTextEx(customFont, hStat, (Vector2){panel.x + 30, (float)histY}, 18, 1, textWhite);
                        if (hCur->next != NULL) {
                            DrawLine(panel.x + 130, histY + 10, panel.x + 150, histY + 10, textMuted);
                        }
                        histY += 25;
                        hCur = hCur->next;
                    }
                }
            } else {
                DrawTextEx(customFont, "Click a contribution on the right to track it.", (Vector2){contentX, 120}, 22, 1, textMuted);
            }
            
            DrawLine(contentX + 500, 50, contentX + 500, sh - 50, panelLight);
            DrawTextEx(customFontBold, "Contributions:", (Vector2){contentX + 550, 50}, 30, 1, textMuted);
            
            BeginScissorMode(contentX + 550, 100, sw - (contentX + 550), sh - 100);
            float yOff = 100 - scrollOffset;
            Donation* cur = sys->donationRecords->head;
            while(cur != NULL) {
                Rectangle card = { contentX + 550, yOff, 400, 60 };
                
                if (yOff + card.height > 100 && yOff < sh) {
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
                }
                
                yOff += 70;
                cur = cur->next;
            }
            EndScissorMode();
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
