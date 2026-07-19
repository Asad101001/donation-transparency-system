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

int my_strlen(const char* str) {
    int i = 0;
    while(str[i] != '\0') i++;
    return i;
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

enum AppScreen { DASHBOARD, DONORS, PROJECTS, DONATIONS, ALLOCATIONS, EXPENSES, REPORT };
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
        if (!clickedInside) activeTextBox = NULL; // click outside deselects
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
    if (color.r == bgDark.r && color.g == bgDark.g && color.b == bgDark.b) textColor = textWhite; // outline button
    
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

// Global Forms
TextBox tDonorName = { { 0, 0, 400, 50 }, "", 0, "Donor Name" };
TextBox tDonorCnic = { { 0, 0, 400, 50 }, "", 0, "Donor CNIC" };
TextBox tProjName = { { 0, 0, 400, 50 }, "", 0, "Project Name" };
TextBox tProjDesc = { { 0, 0, 400, 50 }, "", 0, "Project Description" };
TextBox tDonAmt = { { 0, 0, 400, 50 }, "", 0, "Amount (Rs)" };
TextBox tDonDate = { { 0, 0, 400, 50 }, "", 0, "Date (DD/MM/YYYY)" };
TextBox tExpAmt = { { 0, 0, 400, 50 }, "", 0, "Expense Amount" };
TextBox tExpDesc = { { 0, 0, 400, 50 }, "", 0, "Expense Desc" };
TextBox tExpDate = { { 0, 0, 400, 50 }, "", 0, "Date" };

TextBox* allTextBoxes[] = { &tDonorName, &tDonorCnic, &tProjName, &tProjDesc, &tDonAmt, &tDonDate, &tExpAmt, &tExpDesc, &tExpDate };

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
    DrawTextEx(customFontBold, "DT System", (Vector2){30, 40}, 44, 1, accentCyan);
    DrawLine(30, 100, 250, 100, panelLight);
    
    const char* menus[] = { "Dashboard", "Donors", "Projects", "Donations", "Allocations", "Expenses", "Report", "EXIT APP" };
    AppScreen screens[] = { DASHBOARD, DONORS, PROJECTS, DONATIONS, ALLOCATIONS, EXPENSES, REPORT, REPORT };
    
    for(int i = 0; i < 8; i++) {
        Rectangle btn = { 0, (float)130 + i * 60, 280, 50 };
        bool hovered = CheckCollisionPointRec(GetMousePosition(), btn);
        bool active = (currentScreen == screens[i] && i != 7);
        
        if (hovered || active) {
            DrawRectangleRec(btn, active ? accentCyanDark : (i == 7 ? errorRed : panelLight));
            DrawRectangle(0, btn.y, 6, btn.height, active ? textWhite : bgDark); // Indicator line
        }
        DrawTextEx(customFont, menus[i], (Vector2){40, btn.y + 12}, 24, 1, active ? bgDark : textWhite);
        
        if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (i == 7) wantsToExit = true; 
            else { 
                currentScreen = screens[i]; 
                activeTextBox = NULL; 
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
    ShowPopup("Dummy Data Seeded!", successGreen);
}

// Renders the tree projects onto an array for display
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
        float contentX = 320; 
        
        if (currentScreen == DASHBOARD) {
            DrawTextEx(customFontBold, "Dashboard Overview", (Vector2){contentX, 50}, 48, 1, textWhite);
            
            Rectangle stat1 = { contentX, 150, 300, 150 };
            DrawRectangleRec(stat1, panelDark);
            DrawTextEx(customFont, "Largest Donation", (Vector2){stat1.x + 20, stat1.y + 20}, 20, 1, textMuted);
            char buf[100]; my_sprintf_int(buf, "Rs ", (int)getMax(sys->maxDonations));
            DrawTextEx(customFontBold, buf, (Vector2){stat1.x + 20, stat1.y + 60}, 36, 1, accentCyan);
            
            Rectangle stat2 = { contentX + 330, 150, 300, 150 };
            DrawRectangleRec(stat2, panelDark);
            DrawTextEx(customFont, "Smallest Donation", (Vector2){stat2.x + 20, stat2.y + 20}, 20, 1, textMuted);
            my_sprintf_int(buf, "Rs ", (int)getMin(sys->minDonations));
            DrawTextEx(customFontBold, buf, (Vector2){stat2.x + 20, stat2.y + 60}, 36, 1, accentCyan);
            
            int count = 0; double total = 0;
            Donation* temp = sys->donationRecords->head;
            while(temp != NULL) { count++; total += temp->amount; temp = temp->next; }
            
            Rectangle stat3 = { contentX, 330, 300, 150 };
            DrawRectangleRec(stat3, panelDark);
            DrawTextEx(customFont, "Total Received", (Vector2){stat3.x + 20, stat3.y + 20}, 20, 1, textMuted);
            my_sprintf_int(buf, "", count);
            DrawTextEx(customFontBold, buf, (Vector2){stat3.x + 20, stat3.y + 60}, 36, 1, textWhite);
            
            Rectangle stat4 = { contentX + 330, 330, 300, 150 };
            DrawRectangleRec(stat4, panelDark);
            DrawTextEx(customFont, "Average Donation", (Vector2){stat4.x + 20, stat4.y + 20}, 20, 1, textMuted);
            my_sprintf_int(buf, "Rs ", count > 0 ? (int)(total / count) : 0);
            DrawTextEx(customFontBold, buf, (Vector2){stat4.x + 20, stat4.y + 60}, 36, 1, textWhite);
            
            if (DrawButton((Rectangle){contentX, 510, 630, 60}, "Seed Interactive Dummy Data", panelLight)) {
                seedDummyData(sys);
            }
            
        } else if (currentScreen == DONORS) {
            DrawTextEx(customFontBold, "Register Donor", (Vector2){contentX, 50}, 36, 1, textWhite);
            tDonorName.bounds = { contentX, 110, 400, 50 };
            tDonorCnic.bounds = { contentX, 180, 400, 50 };
            DrawTextBox(tDonorName); DrawTextBox(tDonorCnic);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Register Donor", successGreen)) {
                if (tDonorName.letterCount > 0) {
                    registerDonor(sys, tDonorName.text, tDonorCnic.text);
                    ShowPopup("Donor Registered Successfully!", successGreen);
                    ResetTextBox(tDonorName); ResetTextBox(tDonorCnic);
                }
            }
            
            DrawTextEx(customFontBold, "Select Donor for Donation:", (Vector2){contentX + 450, 50}, 30, 1, textMuted);
            int yOff = 100;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) {
                    Rectangle card = { contentX + 450, (float)yOff, 500, 60 };
                    bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                    DrawRectangleRec(card, hover ? panelLight : panelDark);
                    
                    char buf[200]; my_sprintf_int(buf, "ID ", cur->donor.donorID);
                    DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 18}, 24, 1, accentCyan);
                    DrawTextEx(customFont, cur->donor.name, (Vector2){card.x + 100, card.y + 18}, 24, 1, textWhite);
                    
                    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedDonorID = cur->donor.donorID;
                        currentScreen = DONATIONS; // Jump directly to make donation
                    }
                    yOff += 70;
                    cur = cur->next;
                }
            }
            
        } else if (currentScreen == PROJECTS) {
            DrawTextEx(customFontBold, "Create Welfare Project", (Vector2){contentX, 50}, 36, 1, textWhite);
            tProjName.bounds = { contentX, 110, 400, 50 };
            tProjDesc.bounds = { contentX, 180, 400, 50 };
            DrawTextBox(tProjName); DrawTextBox(tProjDesc);
            
            if (DrawButton((Rectangle){contentX, 250, 200, 50}, "Create Project", successGreen)) {
                if (tProjName.letterCount > 0) {
                    createProject(sys, tProjName.text, tProjDesc.text);
                    ShowPopup("Project Created Successfully!", successGreen);
                    ResetTextBox(tProjName); ResetTextBox(tProjDesc);
                }
            }
            
            DrawTextEx(customFontBold, "Active Projects:", (Vector2){contentX + 450, 50}, 30, 1, textMuted);
            projCount = 0;
            flattenProjects(sys->projects->root);
            
            int yOff = 100;
            for(int i = 0; i < projCount; i++) {
                Rectangle card = { contentX + 450, (float)yOff, 500, 60 };
                DrawRectangleRec(card, panelDark);
                char buf[200]; my_sprintf_int(buf, "Proj ID: ", projArray[i].projectID);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 18}, 24, 1, accentCyan);
                DrawTextEx(customFont, projArray[i].name, (Vector2){card.x + 150, card.y + 18}, 24, 1, textWhite);
                yOff += 70;
            }
            
        } else if (currentScreen == DONATIONS) {
            DrawTextEx(customFontBold, "Make Donation", (Vector2){contentX, 50}, 36, 1, textWhite);
            
            if (selectedDonorID != -1) {
                char buf[100]; my_sprintf_int(buf, "Selected Donor ID: ", selectedDonorID);
                DrawTextEx(customFontBold, buf, (Vector2){contentX, 110}, 24, 1, accentCyan);
                if (DrawButton((Rectangle){contentX + 300, 105, 100, 35}, "Change", panelDark)) {
                    selectedDonorID = -1;
                    currentScreen = DONORS;
                }
            } else {
                DrawTextEx(customFont, "Please select a donor from the Donors tab first.", (Vector2){contentX, 110}, 20, 1, errorRed);
                if (DrawButton((Rectangle){contentX, 150, 200, 40}, "Go to Donors", panelDark)) currentScreen = DONORS;
            }
            
            tDonAmt.bounds = { contentX, 220, 400, 50 };
            tDonDate.bounds = { contentX, 290, 400, 50 };
            DrawTextBox(tDonAmt); DrawTextBox(tDonDate);
            
            if (DrawButton((Rectangle){contentX, 360, 200, 50}, "Process Donation", successGreen)) {
                if (selectedDonorID != -1 && tDonAmt.letterCount > 0) {
                    makeDonation(sys, selectedDonorID, my_atof(tDonAmt.text), tDonDate.text);
                    ShowPopup("Donation Successfully Queued!", successGreen);
                    ResetTextBox(tDonAmt); ResetTextBox(tDonDate);
                    selectedDonorID = -1;
                } else if (selectedDonorID == -1) {
                    ShowPopup("Select a Donor first!", errorRed);
                }
            }
            
            DrawTextEx(customFontBold, "Recent Donations", (Vector2){contentX + 450, 50}, 30, 1, textMuted);
            int yOff = 100;
            Donation* cur = sys->donationRecords->head;
            while(cur != NULL && yOff < sh - 100) {
                Rectangle card = { contentX + 450, (float)yOff, 500, 60 };
                DrawRectangleRec(card, panelDark);
                char buf[200]; my_sprintf_int(buf, "Don ID ", cur->donationID);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 18}, 24, 1, textWhite);
                my_sprintf_int(buf, "Rs ", (int)cur->amount);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 160, card.y + 18}, 24, 1, accentCyan);
                
                const char* stat = "Pending";
                if(cur->currentStatus == STATUS_ALLOCATED) stat = "Allocated";
                else if(cur->currentStatus == STATUS_IN_PROGRESS) stat = "In Progress";
                else if(cur->currentStatus == STATUS_COMPLETED) stat = "Completed";
                DrawTextEx(customFont, stat, (Vector2){card.x + 350, card.y + 18}, 20, 1, textMuted);
                
                yOff += 70;
                cur = cur->next;
            }
            
        } else if (currentScreen == ALLOCATIONS) {
            DrawTextEx(customFontBold, "Pending Allocations Queue", (Vector2){contentX, 50}, 36, 1, textWhite);
            
            if (isQueueEmpty(sys->allocationQueue)) {
                DrawRectangle(contentX, 110, 400, 100, panelDark);
                DrawTextEx(customFont, "Queue is completely empty.", (Vector2){contentX + 20, 145}, 24, 1, textMuted);
            } else {
                Allocation nextAlloc = sys->allocationQueue->front->data;
                Rectangle qCard = { contentX, 110, 400, 120 };
                DrawRectangleRec(qCard, panelDark);
                DrawRectangleLinesEx(qCard, 2, accentCyan);
                DrawTextEx(customFontBold, "NEXT IN LINE", (Vector2){contentX + 20, 125}, 20, 1, accentCyan);
                char buf[100]; my_sprintf_int(buf, "Donation ID: ", nextAlloc.donationID);
                DrawTextEx(customFontBold, buf, (Vector2){contentX + 20, 160}, 28, 1, textWhite);
                
                DrawTextEx(customFontBold, "Click a project below to allocate:", (Vector2){contentX, 260}, 24, 1, textMuted);
            }
            
            DrawTextEx(customFontBold, "Welfare Projects", (Vector2){contentX + 450, 50}, 30, 1, textWhite);
            projCount = 0; flattenProjects(sys->projects->root);
            
            int yOff = 110;
            for(int i = 0; i < projCount; i++) {
                Rectangle card = { contentX + 450, (float)yOff, 500, 80 };
                bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                DrawRectangleRec(card, hover ? panelLight : panelDark);
                
                char buf[200]; my_sprintf_int(buf, "Proj ID: ", projArray[i].projectID);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 15}, 24, 1, textWhite);
                DrawTextEx(customFont, projArray[i].name, (Vector2){card.x + 15, card.y + 45}, 20, 1, textMuted);
                
                if (!isQueueEmpty(sys->allocationQueue)) {
                    if (DrawButton((Rectangle){card.x + 320, card.y + 20, 160, 40}, "Allocate Here", accentCyan)) {
                        allocateDonation(sys, projArray[i].projectID);
                        ShowPopup("Allocation Successful!", successGreen);
                    }
                }
                
                yOff += 90;
            }
            
        } else if (currentScreen == EXPENSES) {
            DrawTextEx(customFontBold, "Add Project Expense", (Vector2){contentX, 50}, 36, 1, textWhite);
            
            if (selectedProjectID != -1) {
                char buf[100]; my_sprintf_int(buf, "Selected Project ID: ", selectedProjectID);
                DrawTextEx(customFontBold, buf, (Vector2){contentX, 110}, 24, 1, accentCyan);
                if (DrawButton((Rectangle){contentX + 320, 105, 100, 35}, "Change", panelDark)) {
                    selectedProjectID = -1;
                }
            } else {
                DrawTextEx(customFont, "Please select a Project from the right.", (Vector2){contentX, 110}, 20, 1, errorRed);
            }
            
            tExpAmt.bounds = { contentX, 170, 400, 50 };
            tExpDesc.bounds = { contentX, 240, 400, 50 };
            tExpDate.bounds = { contentX, 310, 400, 50 };
            DrawTextBox(tExpAmt); DrawTextBox(tExpDesc); DrawTextBox(tExpDate);
            
            if (DrawButton((Rectangle){contentX, 390, 200, 50}, "Record Expense", successGreen)) {
                if (selectedProjectID != -1 && tExpAmt.letterCount > 0) {
                    addExpense(sys, selectedProjectID, my_atof(tExpAmt.text), tExpDesc.text, tExpDate.text);
                    ShowPopup("Expense Recorded Successfully!", successGreen);
                    ResetTextBox(tExpAmt); ResetTextBox(tExpDesc); ResetTextBox(tExpDate);
                    selectedProjectID = -1;
                } else if (selectedProjectID == -1) {
                    ShowPopup("Select a Project first!", errorRed);
                }
            }
            
            DrawTextEx(customFontBold, "Select Project:", (Vector2){contentX + 450, 50}, 30, 1, textMuted);
            projCount = 0; flattenProjects(sys->projects->root);
            int yOff = 100;
            for(int i = 0; i < projCount; i++) {
                Rectangle card = { contentX + 450, (float)yOff, 500, 60 };
                bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                DrawRectangleRec(card, hover ? panelLight : panelDark);
                char buf[200]; my_sprintf_int(buf, "Proj ID ", projArray[i].projectID);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 18}, 24, 1, textWhite);
                DrawTextEx(customFont, projArray[i].name, (Vector2){card.x + 150, card.y + 18}, 20, 1, textMuted);
                
                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedProjectID = projArray[i].projectID;
                }
                yOff += 70;
            }
            
        } else if (currentScreen == REPORT) {
            DrawTextEx(customFontBold, "Transparency Flow Report", (Vector2){contentX, 50}, 36, 1, textWhite);
            
            if (selectedReportDonationID != -1) {
                Donation* d = searchDonation(sys->donationRecords, selectedReportDonationID);
                if (d != NULL) {
                    Rectangle panel = { contentX, 120, 500, 400 };
                    DrawRectangleRec(panel, panelDark);
                    
                    char buf[100];
                    my_sprintf_int(buf, "TRACKING DONATION #", d->donationID);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 30}, 24, 1, textMuted);
                    
                    my_sprintf_int(buf, "Rs ", (int)d->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 80}, 48, 1, accentCyan);
                    DrawTextEx(customFontBold, "Initial Total Received", (Vector2){panel.x + 30, panel.y + 140}, 20, 1, textMuted);
                    
                    DrawLineEx((Vector2){panel.x + 30, panel.y + 190}, (Vector2){panel.x + 470, panel.y + 190}, 2, panelLight);
                    
                    my_sprintf_int(buf, "Rs ", (int)d->remainingAmount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 220}, 48, 1, textWhite);
                    DrawTextEx(customFontBold, "Current Remaining Balance", (Vector2){panel.x + 30, panel.y + 280}, 20, 1, textMuted);
                    
                    const char* stat = "Status: Pending Allocation";
                    Color statCol = textMuted;
                    if(d->currentStatus == STATUS_ALLOCATED) { stat = "Status: Allocated, Pending Expense"; statCol = accentCyan; }
                    else if(d->currentStatus == STATUS_IN_PROGRESS) { stat = "Status: In Progress"; statCol = successGreen; }
                    else if(d->currentStatus == STATUS_COMPLETED) { stat = "Status: 100% Fully Utilized"; statCol = successGreen; }
                    
                    DrawRectangle(panel.x + 30, panel.y + 330, 440, 40, panelLight);
                    DrawTextEx(customFontBold, stat, (Vector2){panel.x + 40, panel.y + 338}, 24, 1, statCol);
                    
                }
            } else {
                DrawTextEx(customFont, "Click a donation from the right to view its transparency report.", (Vector2){contentX, 150}, 20, 1, textMuted);
            }
            
            DrawTextEx(customFontBold, "Select Donation to Audit:", (Vector2){contentX + 550, 50}, 30, 1, textMuted);
            int yOff = 100;
            Donation* cur = sys->donationRecords->head;
            while(cur != NULL && yOff < sh - 100) {
                Rectangle card = { contentX + 550, (float)yOff, 400, 60 };
                bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                DrawRectangleRec(card, hover ? panelLight : panelDark);
                char buf[200]; my_sprintf_int(buf, "Don ID ", cur->donationID);
                DrawTextEx(customFontBold, buf, (Vector2){card.x + 15, card.y + 18}, 24, 1, textWhite);
                my_sprintf_int(buf, "Rs ", (int)cur->amount);
                DrawTextEx(customFont, buf, (Vector2){card.x + 150, card.y + 18}, 24, 1, textMuted);
                
                if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    selectedReportDonationID = cur->donationID;
                }
                
                yOff += 70;
                cur = cur->next;
            }
        }
        
        // Popup Animation
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
