#include <iostream>
#include <string>
#include <limits>
#include "Headers/Hashtable.h"
#include "Headers/User.h"

using namespace std;

static void printMenu() {
    cout << "\n=== NUL Management — Menu ===\n";
    cout << "1) Login\n";
    cout << "2) Sign up user\n";
    cout << "3) Logout\n";
    cout << "4) List users\n";
    cout << "5) View user bookings\n";
    cout << "6) Add booking to (logged-in) user\n";
    cout << "7) Remove booking from (logged-in) user\n";
    cout << "8) Quit\n";
    cout << "Choose an option: ";
}

int main() {
    HashTable user_db(11);
    int nextId = 1001;
    User* currentUser = nullptr;

    while (true) {
        printMenu();
        int choice = 0;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // eat rest of line

        switch (choice) {
            case 1: { // Login
                string username, password;
                cout << "Username: "; getline(cin, username);
                cout << "Password: "; getline(cin, password);
                User* u = user_db.login(username, password);
                if (u) { currentUser = u; cout << "Login successful. Welcome, " << u->getName() << "\n"; }
                else { cout << "Login failed.\n"; }
                break;
            }

            case 2: { // Sign up
                string username, password, type;
                cout << "Username: "; getline(cin, username);
                cout << "Password: "; getline(cin, password);
                cout << "Type (Student/Lecturer/...): "; getline(cin, type);
                if (username.empty() || password.empty()) { cout << "Username and password cannot be empty.\n"; break; }
                user_db.insert(nextId++, username, password, type);
                cout << "User '" << username << "' signed up.\n";
                break;
            }

            case 3: { // Logout
                if (currentUser) { cout << "Logged out: " << currentUser->getName() << "\n"; currentUser = nullptr; }
                else cout << "Not logged in.\n";
                break;
            }

            case 4: {
                user_db.display();
                break;
            }

            case 5: { // View user bookings (own or others) - require login
                string username;
                cout << "Enter username (leave empty to view your bookings): ";
                getline(cin, username);
                User* u = nullptr;
                if (username.empty()) {
                    if (!currentUser) { cout << "Please login to view your bookings.\n"; break; }
                    u = currentUser;
                } else {
                    // require login to view other users' bookings
                    if (!currentUser) { cout << "Please login to view other users' bookings.\n"; break; }
                    u = user_db.get(username);
                }
                if (!u) { cout << "User not found.\n"; }
                else { u->viewMyBookings(); }
                break;
            }

            case 6: { // Add booking - requires login
                if (!currentUser) { cout << "Please login to add bookings.\n"; break; }
                int rid; string rname, rtype;
                cout << "Resource id (integer): ";
                if (!(cin >> rid)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Invalid id.\n"; break; }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Resource name: "; getline(cin, rname);
                cout << "Resource type: "; getline(cin, rtype);
                Resource r; r.setId(rid); r.setName(rname); r.setType(rtype);
                currentUser->addBooking(r);
                cout << "Booking added for " << currentUser->getName() << "\n";
                break;
            }

            case 7: { // Remove booking - requires login
                if (!currentUser) { cout << "Please login to remove bookings.\n"; break; }
                int rid;
                cout << "Resource id to remove: ";
                if (!(cin >> rid)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cout << "Invalid id.\n"; break; }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                currentUser->removeBooking(rid);
                cout << "If it existed, booking removed for " << currentUser->getName() << "\n";
                break;
            }

            case 8: {
                cout << "Exiting.\n";
                goto exit_loop;
            }

            default: {
                cout << "Unknown option.\n";
                break;
            }
        }
    }

exit_loop:
    return 0;
}
