#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

// Include necessary headers for the system structure
// Note: Assuming these files exist in the same directory or accessible path
#include "Headers/Hashtable.h"
#include "Headers/User.h"
#include "Headers/Resource.h"
#include "Headers/Lab.h"
#include "Headers/Bus.h"
#include "Headers/LectureHall.h"
#include "Headers/Slot.h" // Although included by Lab.h, explicitly including ensures clarity

using namespace std;

/*
// --- Minimal Definition for Location (Assumed from Resource.h dependency) ---
struct Location {
    string name;
    Location(string n = "Unknown") : name(n) {}
    string getName() const { return name; }
};
// ----------------------------------------------------------------------------
*/


// Global variables
User* currentUser = nullptr;
int next_user_id = 1;
int next_resource_id = 1;

// Function Prototypes
static void printMenu();
void initialize_resources(map<int, Resource*>& resources_map);
void print_all_resources(const map<int, Resource*>& resources_map);
void cleanup_resources(map<int, Resource*>& resources_map);


// =======================================================================
//                        MAIN APPLICATION LOGIC
// =======================================================================

int main() {
    // Initialize User Database (Hash Table)
    // Using a size of 10 for demonstration
    HashTable user_db(10);
    
    // Initialize Resources (Map<ID, Resource*>)
    map<int, Resource*> resources_table;
    initialize_resources(resources_table);

    // Initial users for demonstration (1 Admin, 1 Regular)
    user_db.insert(next_user_id++, "admin", "adminpass", "Admin");
    user_db.insert(next_user_id++, "alice", "alicepass", "Regular");
    user_db.insert(next_user_id++, "bob", "bobpass", "Regular");


    int choice;
    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the buffer

        switch (choice) {
            case 1: { // Login
                string username, password;
                cout << "\nEnter username: "; getline(cin, username);
                cout << "Enter password: "; getline(cin, password);

                User* user = user_db.login(username, password);
                if (user) {
                    currentUser = user;
                    cout << "? Login successful! Welcome, " << currentUser->getName() << " (" << currentUser->getType() << ").\n";
                } else {
                    cout << "? Login failed. Invalid username or password.\n";
                    currentUser = nullptr;
                }
                break;
            }

            case 2: { // Sign Up
                string name, password;
                cout << "\nEnter new username: "; getline(cin, name);
                
                if (user_db.get(name) != nullptr) {
                    cout << "? Username already exists. Please choose another.\n";
                    break;
                }

                cout << "Enter password: "; getline(cin, password);
                user_db.insert(next_user_id++, name, password, "Regular");
                cout << "? Account created for " << name << ". Please log in.\n";
                break;
            }

            case 3: { // View All Resources
                print_all_resources(resources_table);
                break;
            }

            case 4: { // View My Bookings
                if (!currentUser) { cout << "?? Please login first.\n"; break; }
                currentUser->viewMyBookings();
                break;
            }
            
            case 5: { // Display All Users (Admin only for security)
                if (!currentUser || currentUser->getType() != "Admin") { cout << "? Access denied. Admin privileges required.\n"; break; }
                user_db.display();
                break;
            }

            case 6: { // Add Booking - Includes Waitlist Logic
                if (!currentUser) { cout << "?? Please login first.\n"; break; }
                
                print_all_resources(resources_table);
                int rid;
                cout << "\nEnter Resource ID to book: ";
                if (!(cin >> rid)) {
                    cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (resources_table.count(rid) == 0) {
                    cout << "? Resource ID not found.\n"; break;
                }

                Resource* resourceB = resources_table.at(rid);
                
                if (resourceB->getType() == "LAB" || resourceB->getType() == "LECTUREHALL") {
                    Lab* resource = dynamic_cast<Lab*>(resourceB);
                    int sid;
                    cout << "\nChoose from the available slots:\n";
                    resource->viewAvailableSlots();
                    
                    cout << "\nEnter slot ID to book: ";
                    if (!(cin >> sid)) {
                        cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    // Attempt to book the slot
                    if (resource->bookSlot(sid)) {
                        currentUser->addBooking(resource);
                        cout << "? Successfully booked slot " << sid << " for resource ID " << rid << ".\n";
                    } else {
                        // ? Booking failed (slot already booked or not found). Prompt waitlist.
                        cout << "\nSlot is either already booked or ID is invalid.\n";
                        cout << "Would you like to join the waitlist for this resource (ID " << rid << ")? (y/n): ";
                        char join;
                        cin >> join;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (tolower(join) == 'y') {
                            currentUser->addToResourceWaitlist(resource);
                        }
                    }
                } else if (resourceB->getType() == "BUS") {
                    // Simple booking for Bus (no slots/waitlist logic needed)
                    currentUser->addBooking(resourceB);
                    cout << "? Successfully booked Bus ID " << rid << ".\n";
                } else {
                    cout << "? Booking not supported for this resource type.\n";
                }
                break;
            }
            
            case 7: { // Remove Booking and Process Waitlist
                if (!currentUser) { cout << "?? Please login first.\n"; break; }

                currentUser->viewMyBookings();
                int rid;
                cout << "\nEnter Resource ID to cancel: ";
                if (!(cin >> rid)) {
                    cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (resources_table.count(rid) == 0) {
                    cout << "? Resource ID not found.\n"; break;
                }

                Resource* resourceB = resources_table.at(rid);

                if (resourceB->getType() == "LAB" || resourceB->getType() == "LECTUREHALL") {
                    Lab* resource = dynamic_cast<Lab*>(resourceB);
                    int sid;
                    cout << "Resource is slotted. Enter Slot ID to cancel: ";
                    if (!(cin >> sid)) {
                        cout << "Invalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    // Cancel slot and process waitlist if successful
                    if (resource->cancelSlotBooking(sid)) {
                        currentUser->removeBooking(rid); // Assuming the user booked the resource to cancel the slot
                    } else {
                        cout << "? Slot not found or was not booked.\n";
                    }
                } else {
                    // Simple cancellation for non-slotted resources
                    currentUser->removeBooking(rid);
                }
                break;
            }

            case 8: { // Quit
                cleanup_resources(resources_table);
                cout << "Exiting application. Goodbye!\n";
                return 0;
            }
            
            default: {
                cout << "Unknown option. Please choose a number from the menu.\n";
                break;
            }
        }
    }

    // Cleanup (though case 8 handles it, good practice to put it here too)
    cleanup_resources(resources_table);
    return 0;
}


// =======================================================================
//                        HELPER FUNCTION DEFINITIONS
// =======================================================================

static void printMenu() {
    cout << "\n================================================\n";
    cout << "===      NUL Resource Management System      ===\n";
    cout << "================================================\n";
    cout << "Current User: " << (currentUser ? currentUser->getName() : "Guest") << "\n";
    cout << "------------------------------------------------\n";
    cout << "1)  Login\n";
    cout << "2)  Sign Up\n";
    cout << "3)  View All Resources\n";
    cout << "4)  View My Bookings\n";
    cout << "5)  Display All Users (Admin Only)\n";
    cout << "6)  Add Booking (Includes Waitlist)\n";
    cout << "7)  Remove Booking (Processes Waitlist)\n";
    cout << "8)  Quit\n";
    cout << "------------------------------------------------\n";
    cout << "Choose an option : ";
}

void initialize_resources(map<int, Resource*>& resources_map) {
    // 1. LAB Resources 
    resources_map[next_resource_id] = new Lab(next_resource_id, "ICT Lab", "LAB", Location("ICT Building"), true);
    next_resource_id++;
    resources_map[next_resource_id] = new Lab(next_resource_id, "SCN303", "LAB", Location("New Science Building"), true);
    next_resource_id++;

    // 2. LECTURE HALL Resources
    resources_map[next_resource_id] = new LectureHall(next_resource_id, "ETF1", "LECTUREHALL", Location("ETF Building"), true);
    next_resource_id++;

    // 3. BUS Resources 
    Bus* campus_bus = new Bus(next_resource_id, "NUL Bus 1", "BUS", Location("ISAS Building"), true);
    campus_bus->setFromDate("01-01-2025");
    campus_bus->setToDate("31-12-2025");
    resources_map[next_resource_id] = campus_bus;
    next_resource_id++;

    cout << "System initialized with " << resources_map.size() << " resources.\n";
}

void print_all_resources(const map<int, Resource*>& resources_map) {
    cout << "\n=== Available Resources ===\n";
    for (const auto& pair : resources_map) {
        Resource* r = pair.second;
        cout << "[" << r->getId() << "] " << r->getName() 
             << " (" << r->getType() << ") at " << r->getLocation().getName();
        
        if (r->getType() == "LAB" || r->getType() == "LECTUREHALL") {
            // For slotted resources, show available slots
            Lab* lab_ptr = dynamic_cast<Lab*>(r);
            if (lab_ptr) {
                cout << "\n--- Time Slots ---\n";
                lab_ptr->viewAvailableSlots();
                cout << "------------------\n";
            }
        } else if (r->getType() == "BUS") {
            Bus* bus_ptr = dynamic_cast<Bus*>(r);
            cout << " - Route Active: " << bus_ptr->getFromDate() << " to " << bus_ptr->getToDate();
        }
        cout << "\n";
    }
    cout << "===========================\n";
}

void cleanup_resources(map<int, Resource*>& resources_map) {
    for (auto& pair : resources_map) {
        delete pair.second;
    }
    resources_map.clear();
}

