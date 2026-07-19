#include "../core/SystemManager.h"
#include <iostream>

using namespace std;

void clearInputBuffer() {
    // A simple flush without using <limits>
    char c;
    while ((c = cin.get()) != '\n' && c != EOF);
}

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
    int choice = 0;
    
    while (true) {
        cout << "\n==============================\n";
        cout << "Donation Transparency System\n";
        cout << "==============================\n";
        cout << "1. Register Donor\n";
        cout << "2. Make Donation\n";
        cout << "3. View Donors\n";
        cout << "4. Create Welfare Project\n";
        cout << "5. Allocate Donation\n";
        cout << "6. Add Expense\n";
        cout << "7. Search Donation\n";
        cout << "8. View Transparency Report\n";
        cout << "9. View Donation History\n";
        cout << "10. Statistics\n";
        cout << "11. Exit\n";
        cout << "Enter your choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            clearInputBuffer();
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        if (choice == 11) {
            cout << "Exiting...\n";
            break;
        }
        
        switch (choice) {
            case 1: {
                char name[100];
                char cnic[100];
                cout << "Enter Donor Name: ";
                cin >> name;
                cout << "Enter Donor CNIC: ";
                cin >> cnic;
                registerDonor(sys, name, cnic);
                break;
            }
            case 2: {
                int id;
                double amt;
                char date[100];
                cout << "Enter Donor ID: ";
                cin >> id;
                cout << "Enter Amount: ";
                cin >> amt;
                cout << "Enter Date (DD/MM/YYYY): ";
                cin >> date;
                makeDonation(sys, id, amt, date);
                break;
            }
            case 3: {
                viewDonors(sys);
                break;
            }
            case 4: {
                char name[100];
                char desc[100];
                cout << "Enter Project Name (no spaces): ";
                cin >> name;
                cout << "Enter Project Description (no spaces): ";
                cin >> desc;
                createProject(sys, name, desc);
                break;
            }
            case 5: {
                int pid;
                cout << "Enter Project ID to Allocate to: ";
                cin >> pid;
                allocateDonation(sys, pid);
                break;
            }
            case 6: {
                int pid;
                double amt;
                char desc[100];
                char date[100];
                cout << "Enter Project ID: ";
                cin >> pid;
                cout << "Enter Expense Amount: ";
                cin >> amt;
                cout << "Enter Expense Description (no spaces): ";
                cin >> desc;
                cout << "Enter Date: ";
                cin >> date;
                addExpense(sys, pid, amt, desc, date);
                break;
            }
            case 7: {
                int did;
                cout << "Enter Donation ID: ";
                cin >> did;
                searchDonation(sys, did);
                break;
            }
            case 8: {
                int did;
                cout << "Enter Donation ID for Report: ";
                cin >> did;
                generateTransparencyReport(sys, did);
                break;
            }
            case 9: {
                viewDonationHistory(sys);
                break;
            }
            case 10: {
                viewStatistics(sys);
                break;
            }
            default:
                cout << "Invalid choice!\n";
                break;
        }
    }
    
    return 0;
}
