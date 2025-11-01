#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "Headers/Hashtable.h"
#include "Headers/User.h"
#include "Headers/Resource.h"
#include "Headers/Lab.h"
#include "Headers/Bus.h"
#include "Headers/LectureHall.h"
#include "Headers/textfiles.h"
#include "Headers/Slot.h"

using namespace std;

User* currentUser = nullptr;
int next_user_id = 1;
int next_resource_id = 1;

// Function Prototypes
static void printMenu();
void initialize_resources(map<int, Resource*>& resources_map);
void print_all_resources(const map<int, Resource*>& resources_map);
void cleanup_resources(map<int, Resource*>& resources_map);

int main() {
    
    HashTable user_db(10);
    
    map<int, Resource*> resources_table;
    initialize_resources(resources_table);

    user_db.insert(next_user_id++, "Thapelo", "adminpass", "Admin");
    load_resources(resources_table);
    load_users(user_db);

    int choice;
    while (true) {
        printMenu();
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: { // Login
                string username, password;
                cout << "\nEnter username: "; getline(cin, username);
                cout << "Enter password: "; getline(cin, password);

                User* user = user_db.login(username, password);
                if (user) {
                    currentUser = user;
                    cout << "\nLogin successful! Welcome, " << currentUser->getName() << " (" << currentUser->getType() << ").\n";
                } else {
                    cout << "\nLogin failed. Invalid username or password.\n";
                    currentUser = nullptr;
                }
                break;
            }

            case 2: { // Sign Up
                string name, password;
                cout << "\nEnter new username: "; getline(cin, name);
                
                if (user_db.get(name) != nullptr) {
                    cout << "\nUsername already exists. Please choose another.\n";
                    break;
                }

                cout << "Enter password: "; getline(cin, password);
                user_db.insert(next_user_id++, name, password, "Regular");
                cout << "Account created for " << name << ". Please log in.\n";
                break;
            }

            case 3: { // View All Resources
                print_all_resources(resources_table);
                break;
            }

            case 4: { // View My Bookings
                if (!currentUser) { cout << "\nPlease login first.\n"; break; }
                currentUser->viewMyBookings();
                break;
            }
            
            case 5: { // Display All Users
                if (!currentUser || currentUser->getType() != "Admin") { cout << "Access denied. Admin privileges required.\n"; break; }
                user_db.display();
                break;
            }

            case 6: { // Add booking
                if (!currentUser) { cout << "\nPlease login first.\n"; break; }
                
                print_all_resources(resources_table);
                int rid;
                cout << "\nEnter Resource ID to book: ";
                if (!(cin >> rid)) {
                    cout << "\nInvalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (resources_table.count(rid) == 0) {
                    cout << "\nResource ID not found.\n"; break;
                }

                Resource* resourceB = resources_table.at(rid);
                
                if (resourceB->getType() == "LAB" || resourceB->getType() == "LECTUREHALL") {
                    Lab* resource = dynamic_cast<Lab*>(resourceB);
                    int sid;
                    cout << "\nChoose from the available slots:\n";
                    resource->viewAvailableSlots();
                    
                    cout << "\nEnter slot ID to book: ";
                    if (!(cin >> sid)) {
                        cout << "\nInvalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (resource->bookSlot(sid)) {
                        currentUser->addBooking(resource);
                        cout << "? Successfully booked slot " << sid << " for resource ID " << rid << ".\n";
                    } else {
                        // Slot already booked or not found.,Prompt waitlist.
                        cout << "\nSlot is either already booked or ID is invalid.\n";
                        cout << "\nWould you like to join the waitlist for this resource (ID " << rid << ")? (y/n): ";
                        char join;
                        cin >> join;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (tolower(join) == 'y') {
                            currentUser->addToResourceWaitlist(resource);
                        }
                    }
                } else if (resourceB->getType() == "BUS") {
                    currentUser->addBooking(resourceB);
                    cout << "\nSuccessfully booked Bus ID " << rid << ".\n";
                } else {
                    cout << "\nBooking not supported for this resource type.\n";
                }
                break;
            }
            
            case 7: { // Remove Booking and Process Waitlist
                if (!currentUser) { cout << "\nPlease login first.\n"; break; }

                currentUser->viewMyBookings();
                int rid;
                cout << "\nEnter Resource ID to cancel: ";
                if (!(cin >> rid)) {
                    cout << "\nInvalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (resources_table.count(rid) == 0) {
                    cout << "\nResource ID not found.\n"; break;
                }

                Resource* resourceB = resources_table.at(rid);

                if (resourceB->getType() == "LAB" || resourceB->getType() == "LECTUREHALL") {
                    Lab* resource = dynamic_cast<Lab*>(resourceB);
                    int sid;
                    cout << "\nResource is slotted. Enter Slot ID to cancel: ";
                    if (!(cin >> sid)) {
                        cout << "\nInvalid input.\n"; cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); break;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    // Cancel slot and process waitlist if successful
                    if (resource->cancelSlotBooking(sid)) {
                        currentUser->removeBooking(rid);
                    } else {
                        cout << "\nSlot not found or was not booked.\n";
                    }
                } else {
                    currentUser->removeBooking(rid);
                }
                break;
            }

            case 8: { // Quit
                save_resources(resources_table);
                save_users(user_table);
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
    //cleanup_resources(resources_table);
    return 0;
}

// Functions Implementations
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
    resources_map[next_resource_id] = new LectureHall(next_resource_id, "ETF2", "LECTUREHALL", Location("ETF Building"), true);
    next_resource_id++;
    resources_map[next_resource_id] = new LectureHall(next_resource_id, "CMP105", "LECTUREHALL", Location("CMP Building"), true);
    next_resource_id++;
    resources_map[next_resource_id] = new LectureHall(next_resource_id, "SCILT", "LECTUREHALL", Location("Old Science Building"), true);
    next_resource_id++;

    // 3. BUS Resources 
    Bus* campus_bus = new Bus(next_resource_id, "NUL Bus 1", "BUS", Location("ISAS Building"), true);
    campus_bus->setFromDate("01-01-2025");
    campus_bus->setToDate("31-12-2025");
    resources_map[next_resource_id] = campus_bus;
    next_resource_id++;

    //cout << "\nSystem initialized with " << resources_map.size() << " resources.\n";
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

void save

