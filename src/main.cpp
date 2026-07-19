#include "core/SystemManager.h"
#include "gui/gui.h"
#include <iostream>

using namespace std;

void seedDummyData(SystemManager* sys) {
    cout << "\n--- Seeding Dummy Data ---\n";
    registerDonor(sys, "Alice", "12345");
    registerDonor(sys, "Bob", "67890");
    createProject(sys, "Water_Well", "Clean_Water");
    createProject(sys, "School", "Education_Fund");
    
    makeDonation(sys, 1, 15000, "20/07/2026");
    makeDonation(sys, 2, 5000, "21/07/2026");
    
    allocateDonation(sys, 10); // Allocates Donation 100 to Water_Well
    
    addExpense(sys, 10, 5000, "Pipes", "22/07/2026");
    addExpense(sys, 10, 3000, "Labor", "23/07/2026");
    cout << "--- Seeding Complete ---\n\n";
}

int main() {
    SystemManager* sys = createSystemManager();
    seedDummyData(sys);
    
    initGUI();
    runGUI(sys);
    closeGUI();
    
    return 0;
}
