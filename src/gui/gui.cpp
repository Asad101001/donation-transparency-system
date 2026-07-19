#include "gui.h"
#include "raylib.h"
#include "../core/Utils.h"
#include <ctime>
#include <cstdio>

// String and Math utilities
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

inline int my_min(int a, int b) { return (a < b) ? a : b; }

// GUI Premium Theme Colors
Color bgDark = { 11, 11, 20, 255 };          // Sleek dark background
Color panelDark = { 22, 22, 37, 255 };       // Dark gray-blue panel
Color panelLight = { 35, 36, 57, 255 };      // Soft interactive/highlight panel
Color accentCyan = { 0, 184, 217, 255 };     // Vibrant neon cyan
Color accentCyanDark = { 0, 107, 128, 255 }; // Deep selection cyan
Color textWhite = { 242, 246, 250, 255 };    // Clean off-white text
Color textMuted = { 145, 154, 179, 255 };    // Muted slate gray
Color errorRed = { 255, 86, 111, 255 };      // Pastel rose-red
Color successGreen = { 54, 197, 150, 255 };  // Emerald green
Color goldAccent = { 255, 171, 0, 255 };     // Warm gold for status/warnings
Color orangeAlert = { 255, 127, 80, 255 };   // Soft coral orange for progress

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
    
    // Draw modern rounded textbox panel
    DrawRectangleRounded(tb.bounds, 0.2f, 4, isActive ? panelLight : panelDark);
    DrawRectangleRoundedLines(tb.bounds, 0.2f, 4, isActive ? accentCyan : textMuted);
    
    if (tb.letterCount == 0 && !isActive) {
        DrawTextEx(customFont, tb.placeholder, (Vector2){tb.bounds.x + 15, tb.bounds.y + tb.bounds.height/2 - 12}, 22, 1, textMuted);
    } else {
        DrawTextEx(customFont, tb.text, (Vector2){tb.bounds.x + 15, tb.bounds.y + tb.bounds.height/2 - 12}, 22, 1, textWhite);
    }
    
    // Cursor rendering
    if (isActive && ((int)(GetTime() * 2) % 2 == 0)) {
        Vector2 textSize = MeasureTextEx(customFont, tb.text, 22, 1);
        DrawLine(tb.bounds.x + 17 + textSize.x, tb.bounds.y + 12, tb.bounds.x + 17 + textSize.x, tb.bounds.y + tb.bounds.height - 12, accentCyan);
    }
}

void UpdateTextBoxes(TextBox** textboxes, int count) {
    // 1. Mouse Click Collision Detection
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool clickedInside = false;
        for(int i = 0; i < count; i++) {
            if (textboxes[i]->bounds.width > 0 && CheckCollisionPointRec(GetMousePosition(), textboxes[i]->bounds)) {
                activeTextBox = textboxes[i];
                clickedInside = true;
                break;
            }
        }
        if (!clickedInside) activeTextBox = NULL;
    }

    // 2. Keyboard TAB Cycle Navigation
    if (IsKeyPressed(KEY_TAB)) {
        if (currentScreen == CONTRIBUTORS) {
            if (activeTextBox == textboxes[0]) activeTextBox = textboxes[1];      // tDonorName -> tDonorCnic
            else if (activeTextBox == textboxes[1]) activeTextBox = textboxes[0]; // tDonorCnic -> tDonorName
            else if (activeTextBox == textboxes[2]) activeTextBox = textboxes[2]; // Keep focus on tDonAmt
            else activeTextBox = textboxes[0];
        } else if (currentScreen == CAUSES) {
            if (activeTextBox == textboxes[3]) activeTextBox = textboxes[4];      // tProjName -> tProjDesc
            else if (activeTextBox == textboxes[4]) activeTextBox = textboxes[3]; // tProjDesc -> tProjName
            else if (activeTextBox == textboxes[5]) activeTextBox = textboxes[6]; // tExpAmt -> tExpDesc
            else if (activeTextBox == textboxes[6]) activeTextBox = textboxes[5]; // tExpDesc -> tExpAmt
            else activeTextBox = textboxes[3];
        }
    }

    // 3. Character and Clipboard Inputs
    if (activeTextBox != NULL) {
        // Clipboard Paste Shortcut (Ctrl+V)
        if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
            const char* clipboard = GetClipboardText();
            if (clipboard != NULL) {
                int k = 0;
                while (clipboard[k] != '\0' && activeTextBox->letterCount < 99) {
                    char key = clipboard[k];
                    bool valid = false;
                    if (activeTextBox->isNumeric) {
                        if ((key >= '0' && key <= '9') || key == '.' || key == '-') valid = true;
                    } else {
                        if (key >= 32 && key <= 125) valid = true;
                    }
                    if (valid) {
                        activeTextBox->text[activeTextBox->letterCount] = key;
                        activeTextBox->text[activeTextBox->letterCount + 1] = '\0';
                        activeTextBox->letterCount++;
                    }
                    k++;
                }
            }
        }

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
    
    // Smooth color change on hover
    Color currentBtnColor = color;
    if (hovered) {
        currentBtnColor.r = (unsigned char)my_min(255, color.r + 30);
        currentBtnColor.g = (unsigned char)my_min(255, color.g + 30);
        currentBtnColor.b = (unsigned char)my_min(255, color.b + 30);
    }
    
    DrawRectangleRounded(bounds, 0.25f, 4, currentBtnColor);
    DrawRectangleRoundedLines(bounds, 0.25f, 4, hovered ? textWhite : panelLight);
    
    // Auto-calculate text color based on button color brightness
    Color textColor = (color.r == panelLight.r && color.g == panelLight.g && color.b == panelLight.b) ? textWhite : bgDark;
    if (hovered) textColor = textWhite;
    
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
    registerDonor(sys, "Alice Smith", "35201-1234567-1");
    registerDonor(sys, "Bob Johnson", "35202-9876543-2");
    createProject(sys, "Water Well - Thar", "Clean drinking water facility");
    createProject(sys, "Solar School - Swat", "Solar panels installation for primary school");
    makeDonation(sys, 1, 25000, "20/07/2026");
    makeDonation(sys, 2, 8000, "21/07/2026");
    allocateDonation(sys, 10);
    addExpense(sys, 10, 8000, "Rigging and Pipes Purchase", "22/07/2026");
    ShowPopup("Example Data Seeded!", successGreen);
}

int projCount = 0;
Project projArray[100];
void flattenProjects(Project* root) {
    if (root == NULL || projCount >= 100) return;
    flattenProjects(root->left);
    projArray[projCount++] = *root;
    flattenProjects(root->right);
}

// Dynamically calculates text box bounds BEFORE input handling to keep clicks frame-accurate
void LayoutTextBoxes(SystemManager* sys, float contentX, int sw, int sh) {
    // Reset all bounds first
    for (int i = 0; i < 7; i++) allTextBoxes[i]->bounds = {0,0,0,0};
    
    if (currentScreen == CONTRIBUTORS) {
        tDonorName.bounds = { contentX, 110, 400, 50 };
        tDonorCnic.bounds = { contentX, 180, 400, 50 };
        
        if (selectedDonorID != -1) {
            float yOff = 140 - scrollOffset;
            int donorIdx = 0;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) {
                    float cardH = (selectedDonorID == cur->donor.donorID) ? 280.0f : 60.0f;
                    if (selectedDonorID == cur->donor.donorID) {
                        tDonAmt.bounds = { contentX + 520, yOff + 190, 300, 40 };
                    }
                    yOff += cardH + 10;
                    cur = cur->next;
                    donorIdx++;
                }
            }
        }
    } else if (currentScreen == CAUSES) {
        tProjName.bounds = { contentX, 110, 400, 50 };
        tProjDesc.bounds = { contentX, 180, 400, 50 };
        
        if (selectedProjectID != -1) {
            projCount = 0; flattenProjects(sys->projects->root);
            float yOff = 150 - scrollOffset;
            for(int i = 0; i < projCount; i++) {
                float cardH = (selectedProjectID == projArray[i].projectID) ? 300.0f : 80.0f;
                if (selectedProjectID == projArray[i].projectID) {
                    tExpAmt.bounds = { contentX + 520, yOff + 165, 460, 35 };
                    tExpDesc.bounds = { contentX + 520, yOff + 210, 460, 35 };
                }
                yOff += cardH + 10;
            }
        }
    }
}

void runGUI(SystemManager* sys) {
    char currentDate[20];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(currentDate, "%02d/%02d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
    
    while (!WindowShouldClose() && !wantsToExit) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        float contentX = 330; 
        
        // 1. Calculate Layout Bounds first (Syncs scrolling instantly)
        LayoutTextBoxes(sys, contentX, sw, sh);
        
        // 2. Process Input
        UpdateTextBoxes(allTextBoxes, 7);
        
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollOffset -= wheel * 40.0f;
            if (scrollOffset < 0) scrollOffset = 0;
        }
        
        // 3. Draw Frame
        BeginDrawing();
        ClearBackground(bgDark);
        DrawSidebar();
        
        if (currentScreen == HOME) {
            DrawTextEx(customFontBold, "Welcome to the DT System", (Vector2){contentX, 50}, 50, 1, textWhite);
            DrawTextEx(customFont, "Manage and track funds with complete transparency.", (Vector2){contentX, 110}, 24, 1, textMuted);
            
            // Calculate system metrics in real-time
            int donorCount = 0;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) { donorCount++; cur = cur->next; }
            }
            
            int donationCount = 0;
            double donationTotal = 0.0;
            Donation* dTemp = sys->donationRecords->head;
            while(dTemp != NULL) { donationCount++; donationTotal += dTemp->amount; dTemp = dTemp->next; }
            
            projCount = 0; flattenProjects(sys->projects->root);
            
            double pendingTotal = 0.0;
            int pendingCount = 0;
            AllocationNode* qCur = sys->allocationQueue->front;
            while(qCur != NULL) { pendingCount++; pendingTotal += qCur->data.allocatedAmount; qCur = qCur->next; }
            
            double totalSpent = 0.0;
            Expense* eCur = sys->expenses->head;
            while(eCur != NULL) { totalSpent += eCur->amount; eCur = eCur->next; }

            // Workflow visualizer
            Rectangle guide = { contentX, 160, 800, 290 };
            DrawRectangleRounded(guide, 0.05f, 4, panelDark);
            DrawRectangleRoundedLines(guide, 0.05f, 4, panelLight);
            
            DrawTextEx(customFontBold, "DT System Status & Workflow:", (Vector2){guide.x + 20, guide.y + 20}, 28, 1, accentCyan);
            
            // Render interactive metric badges for steps
            char metrics1[100], metrics2[100], metrics3[100], metrics4[100];
            sprintf(metrics1, "1. Receive Money: %d donors registered, Total: Rs %d", donorCount, (int)donationTotal);
            sprintf(metrics2, "2. Create Causes: %d welfare projects active", projCount);
            sprintf(metrics3, "3. Assign Funds: Rs %d in allocation queue (%d pending)", (int)pendingTotal, pendingCount);
            sprintf(metrics4, "4. Spend Funds: Rs %d utilized for cause expenses", (int)totalSpent);
            const char* step5 = "5. Track Transparency: Status history stack tracks every single rupee!";
            
            DrawTextEx(customFont, metrics1, (Vector2){guide.x + 20, guide.y + 70}, 20, 1, textWhite);
            DrawTextEx(customFont, metrics2, (Vector2){guide.x + 20, guide.y + 110}, 20, 1, textWhite);
            DrawTextEx(customFont, metrics3, (Vector2){guide.x + 20, guide.y + 150}, 20, 1, (pendingCount > 0) ? goldAccent : textWhite);
            DrawTextEx(customFont, metrics4, (Vector2){guide.x + 20, guide.y + 190}, 20, 1, textWhite);
            DrawTextEx(customFont, step5, (Vector2){guide.x + 20, guide.y + 230}, 20, 1, successGreen);
            
            // Largest Donation Received Card
            Rectangle stat1 = { contentX, 475, 380, 120 };
            DrawRectangleRounded(stat1, 0.1f, 4, panelDark);
            DrawRectangleRoundedLines(stat1, 0.1f, 4, panelLight);
            DrawRectangle((int)stat1.x, (int)stat1.y, 6, (int)stat1.height, accentCyan); // accent left bar
            DrawTextEx(customFont, "Largest Donation Received", (Vector2){stat1.x + 20, stat1.y + 20}, 20, 1, textMuted);
            char buf[100]; my_sprintf_int(buf, "Rs ", (int)getMax(sys->maxDonations));
            DrawTextEx(customFontBold, buf, (Vector2){stat1.x + 20, stat1.y + 60}, 36, 1, accentCyan);
            
            // Total Contributions count Card
            Rectangle stat2 = { contentX + 420, 475, 380, 120 };
            DrawRectangleRounded(stat2, 0.1f, 4, panelDark);
            DrawRectangleRoundedLines(stat2, 0.1f, 4, panelLight);
            DrawRectangle((int)stat2.x, (int)stat2.y, 6, (int)stat2.height, successGreen); // accent left bar
            DrawTextEx(customFont, "Total Donations Logged", (Vector2){stat2.x + 20, stat2.y + 20}, 20, 1, textMuted);
            my_sprintf_int(buf, "", donationCount);
            DrawTextEx(customFontBold, buf, (Vector2){stat2.x + 20, stat2.y + 60}, 36, 1, textWhite);
            
            if (DrawButton((Rectangle){contentX, 615, 800, 60}, "Seed Sample Project Data (Auto-Workflow)", panelLight)) {
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
            DrawTextEx(customFont, "Click someone to receive money or view history.", (Vector2){contentX + 500, 100}, 20, 1, textMuted);
            
            BeginScissorMode(contentX + 500, 140, sw - (contentX + 500), sh - 140);
            float yOff = 140 - scrollOffset;
            int donorIdx = 0;
            for(int i = 0; i < sys->donorDB->size; i++) {
                HashNode* cur = sys->donorDB->table[i];
                while(cur != NULL) {
                    float targetH = selectedDonorID == cur->donor.donorID ? 280.0f : 60.0f;
                    if (donorAnimHeights[donorIdx] < 60.0f) donorAnimHeights[donorIdx] = 60.0f;
                    donorAnimHeights[donorIdx] += (targetH - donorAnimHeights[donorIdx]) * 0.15f;
                    
                    Rectangle card = { contentX + 500, yOff, 500, donorAnimHeights[donorIdx] };
                    
                    if (yOff + card.height > 140 && yOff < sh) {
                        bool hover = CheckCollisionPointRec(GetMousePosition(), {card.x, card.y, card.width, 60});
                        
                        DrawRectangleRounded(card, 0.05f, 4, (hover || selectedDonorID == cur->donor.donorID) ? panelLight : panelDark);
                        DrawRectangleRoundedLines(card, 0.05f, 4, (selectedDonorID == cur->donor.donorID) ? accentCyan : panelLight);
                        
                        DrawTextEx(customFontBold, cur->donor.name, (Vector2){card.x + 20, card.y + 18}, 24, 1, textWhite);
                        
                        char cnicText[100];
                        sprintf(cnicText, "CNIC: %s", cur->donor.cnic);
                        DrawTextEx(customFont, cnicText, (Vector2){card.x + 220, card.y + 21}, 18, 1, textMuted);
                        
                        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            selectedDonorID = (selectedDonorID == cur->donor.donorID) ? -1 : cur->donor.donorID;
                            activeTextBox = NULL;
                        }
                        
                        // Expanded Card: Add donation & view last 3 donations by this contributor
                        if (selectedDonorID == cur->donor.donorID && donorAnimHeights[donorIdx] > 260.0f) {
                            DrawLine(card.x + 20, card.y + 60, card.x + 480, card.y + 60, panelDark);
                            DrawTextEx(customFontBold, "Contributor History:", (Vector2){card.x + 20, card.y + 75}, 18, 1, accentCyan);
                            
                            int donCount = 0;
                            Donation* dCur = sys->donationRecords->head;
                            while(dCur != NULL) {
                                if (dCur->donorID == cur->donor.donorID) {
                                    donCount++;
                                    if (donCount <= 3) {
                                        char donInfo[100];
                                        sprintf(donInfo, "• Rs %d on %s", (int)dCur->amount, dCur->date);
                                        DrawTextEx(customFont, donInfo, (Vector2){card.x + 20, card.y + 100 + (donCount - 1) * 25}, 18, 1, textWhite);
                                    }
                                }
                                dCur = dCur->next;
                            }
                            if (donCount == 0) {
                                DrawTextEx(customFont, "No previous contributions.", (Vector2){card.x + 20, card.y + 100}, 18, 1, textMuted);
                            }
                            
                            tDonAmt.bounds = { card.x + 20, card.y + 190, 300, 40 };
                            DrawTextBox(tDonAmt);
                            
                            char autoDateMsg[100];
                            sprintf(autoDateMsg, "Date: %s", currentDate);
                            DrawTextEx(customFont, autoDateMsg, (Vector2){card.x + 20, card.y + 245}, 16, 1, textMuted);
                            
                            if (DrawButton((Rectangle){card.x + 340, card.y + 190, 140, 40}, "Receive", successGreen)) {
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
                DrawTextEx(customFontBold, buf, (Vector2){contentX + 500, 100}, 24, 1, goldAccent);
            } else {
                DrawTextEx(customFont, "No funds currently waiting to be assigned.", (Vector2){contentX + 500, 100}, 20, 1, textMuted);
            }
            
            projCount = 0; flattenProjects(sys->projects->root);
            
            BeginScissorMode(contentX + 500, 150, sw - (contentX + 500), sh - 150);
            float yOff = 150 - scrollOffset;
            for(int i = 0; i < projCount; i++) {
                float targetH = selectedProjectID == projArray[i].projectID ? 300.0f : 80.0f;
                if (projAnimHeights[i] < 80.0f) projAnimHeights[i] = 80.0f;
                projAnimHeights[i] += (targetH - projAnimHeights[i]) * 0.15f;
                
                Rectangle card = { contentX + 500, yOff, 500, projAnimHeights[i] };
                
                if (yOff + card.height > 150 && yOff < sh) {
                    bool hover = CheckCollisionPointRec(GetMousePosition(), {card.x, card.y, card.width, 80});
                    
                    DrawRectangleRounded(card, 0.05f, 4, (hover || selectedProjectID == projArray[i].projectID) ? panelLight : panelDark);
                    DrawRectangleRoundedLines(card, 0.05f, 4, (selectedProjectID == projArray[i].projectID) ? accentCyan : panelLight);
                    
                    DrawTextEx(customFontBold, projArray[i].name, (Vector2){card.x + 20, card.y + 15}, 26, 1, textWhite);
                    DrawTextEx(customFont, projArray[i].description, (Vector2){card.x + 20, card.y + 45}, 16, 1, textMuted);
                    
                    if (!isQueueEmpty(sys->allocationQueue)) {
                        if (DrawButton((Rectangle){card.x + 340, card.y + 20, 140, 40}, "Assign Funds", accentCyan)) {
                            allocateDonation(sys, projArray[i].projectID);
                            ShowPopup("Funds Assigned!", successGreen);
                        }
                    }
                    
                    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedProjectID = (selectedProjectID == projArray[i].projectID) ? -1 : projArray[i].projectID;
                        activeTextBox = NULL;
                    }
                    
                    // Expanded Cause Card: list expenses and allow logging a new expense
                    if (selectedProjectID == projArray[i].projectID && projAnimHeights[i] > 280.0f) {
                        DrawLine(card.x + 20, card.y + 80, card.x + 480, card.y + 80, panelDark);
                        DrawTextEx(customFontBold, "Logged Expenses:", (Vector2){card.x + 20, card.y + 90}, 16, 1, errorRed);
                        
                        int expCount = 0;
                        Expense* eCur = sys->expenses->head;
                        while(eCur != NULL) {
                            if (eCur->projectID == projArray[i].projectID) {
                                expCount++;
                                if (expCount <= 3) {
                                    char expInfo[120];
                                    sprintf(expInfo, "• Rs %d: %s (%s)", (int)eCur->amount, eCur->description, eCur->date);
                                    DrawTextEx(customFont, expInfo, (Vector2){card.x + 20, card.y + 110 + (expCount - 1) * 22}, 16, 1, textWhite);
                                }
                            }
                            eCur = eCur->next;
                        }
                        if (expCount == 0) {
                            DrawTextEx(customFont, "No logged expenses yet.", (Vector2){card.x + 20, card.y + 110}, 16, 1, textMuted);
                        }
                        
                        tExpAmt.bounds = { card.x + 20, card.y + 185, 460, 35 };
                        tExpDesc.bounds = { card.x + 20, card.y + 225, 460, 35 };
                        DrawTextBox(tExpAmt); DrawTextBox(tExpDesc);
                        
                        if (DrawButton((Rectangle){card.x + 20, card.y + 265, 200, 30}, "Log Expense", errorRed)) {
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
                    Rectangle panel = { contentX, 120, 480, 500 };
                    DrawRectangleRounded(panel, 0.05f, 4, panelDark);
                    DrawRectangleRoundedLines(panel, 0.05f, 4, panelLight);
                    
                    char buf[100];
                    my_sprintf_int(buf, "Funds Received: Rs ", (int)d->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 35}, 30, 1, accentCyan);
                    
                    DrawLine(panel.x + 30, panel.y + 85, panel.x + 450, panel.y + 85, panelLight);
                    
                    my_sprintf_int(buf, "Unspent Balance: Rs ", (int)d->remainingAmount);
                    DrawTextEx(customFontBold, buf, (Vector2){panel.x + 30, panel.y + 110}, 28, 1, textWhite);
                    
                    const char* stat = "Status: Waiting for Cause";
                    Color statCol = textMuted;
                    if(d->currentStatus == STATUS_ALLOCATED) { stat = "Status: Assigned to Cause"; statCol = accentCyan; }
                    else if(d->currentStatus == STATUS_IN_PROGRESS) { stat = "Status: Being Spent"; statCol = orangeAlert; }
                    else if(d->currentStatus == STATUS_COMPLETED) { stat = "Status: 100% Utilized"; statCol = successGreen; }
                    
                    DrawRectangleRounded((Rectangle){panel.x + 30, panel.y + 160, 420, 45}, 0.2f, 4, panelLight);
                    DrawTextEx(customFontBold, stat, (Vector2){panel.x + 45, panel.y + 172}, 22, 1, statCol);
                    
                    // Connected vertical step timeline
                    DrawTextEx(customFontBold, "Transparency Flow History:", (Vector2){panel.x + 30, panel.y + 230}, 20, 1, textMuted);
                    
                    int timelineX = panel.x + 50;
                    int timelineY = panel.y + 280;
                    
                    // We iterate through the Stack from oldest to newest status by temporarily printing or drawing
                    // Let's reverse the stack visualization to draw logically from oldest at top to newest at bottom, or vice versa.
                    // A vertical flow makes the DSA Stack visualization premium.
                    StatusNode* hCur = d->historyTop;
                    int step = 0;
                    while (hCur != NULL) {
                        if (hCur->next != NULL) {
                            DrawLineEx((Vector2){(float)timelineX, (float)timelineY + 12}, (Vector2){(float)timelineX, (float)timelineY + 48}, 2.0f, panelLight);
                        }
                        
                        Color circleCol = textMuted;
                        const char* hStat = "Received";
                        if (hCur->status == STATUS_RECEIVED) { circleCol = accentCyan; hStat = "Donation Received"; }
                        else if (hCur->status == STATUS_ALLOCATED) { circleCol = accentCyan; hStat = "Assigned to Cause"; }
                        else if (hCur->status == STATUS_IN_PROGRESS) { circleCol = orangeAlert; hStat = "Funds Being Spent"; }
                        else if (hCur->status == STATUS_COMPLETED) { circleCol = successGreen; hStat = "100% Utilized"; }
                        
                        DrawCircle(timelineX, timelineY, 7, circleCol);
                        DrawCircleLines(timelineX, timelineY, 10, textWhite);
                        
                        DrawTextEx(customFontBold, hStat, (Vector2){(float)timelineX + 25, (float)timelineY - 10}, 18, 1, textWhite);
                        
                        timelineY += 48;
                        hCur = hCur->next;
                        step++;
                    }
                }
            } else {
                DrawTextEx(customFont, "Select a contribution card on the right to track funds.", (Vector2){contentX, 120}, 22, 1, textMuted);
            }
            
            DrawLine(contentX + 500, 50, contentX + 500, sh - 50, panelLight);
            DrawTextEx(customFontBold, "Contributions List:", (Vector2){contentX + 540, 50}, 30, 1, textWhite);
            
            BeginScissorMode(contentX + 540, 100, sw - (contentX + 540), sh - 100);
            float yOff = 100 - scrollOffset;
            Donation* cur = sys->donationRecords->head;
            while(cur != NULL) {
                Rectangle card = { contentX + 540, yOff, 400, 60 };
                
                if (yOff + card.height > 100 && yOff < sh) {
                    bool hover = CheckCollisionPointRec(GetMousePosition(), card);
                    
                    DrawRectangleRounded(card, 0.15f, 4, (hover || selectedReportDonationID == cur->donationID) ? panelLight : panelDark);
                    DrawRectangleRoundedLines(card, 0.15f, 4, (selectedReportDonationID == cur->donationID) ? accentCyan : panelLight);
                    
                    char buf[200]; my_sprintf_int(buf, "Rs ", (int)cur->amount);
                    DrawTextEx(customFontBold, buf, (Vector2){card.x + 20, card.y + 18}, 26, 1, textWhite);
                    
                    const char* stat = "Pending";
                    Color statC = goldAccent;
                    if(cur->currentStatus == STATUS_ALLOCATED) { stat = "Assigned"; statC = accentCyan; }
                    else if(cur->currentStatus == STATUS_IN_PROGRESS) { stat = "Spending"; statC = orangeAlert; }
                    else if(cur->currentStatus == STATUS_COMPLETED) { stat = "Utilized"; statC = successGreen; }
                    
                    DrawTextEx(customFontBold, stat, (Vector2){card.x + 280, card.y + 20}, 18, 1, statC);
                    
                    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedReportDonationID = cur->donationID;
                    }
                }
                
                yOff += 70;
                cur = cur->next;
            }
            EndScissorMode();
        }
        
        // Notifications Popup Draw
        if (popupTimer > 0) {
            popupY += (0 - popupY) * 0.15f; 
            DrawRectangle(280, popupY, sw - 280, 70, popupColor);
            Vector2 ts = MeasureTextEx(customFontBold, popupMsg, 28, 1);
            DrawTextEx(customFontBold, popupMsg, (Vector2){280 + (sw - 280 - ts.x)/2, popupY + 20}, 28, 1, bgDark);
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
