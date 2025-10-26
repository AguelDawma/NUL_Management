#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include "Headers/Hashtable.h"
#include "Headers/User.h"
#include "Headers/Map.h"

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
    cout << "8) Navigate NUL Campus\n";
    cout << "9) Quit\n";
    cout << "Choose an option: ";
}

// Function to display the menu options
void display_menu() {
    cout << "\n============================================\n";
    cout << "          🗺️ NUL Campus Navigation Menu 🗺️\n";
    cout << "============================================\n";
    cout << "1. Find Shortest Path between two buildings\n";
    cout << "2. List all available buildings/locations\n";
    cout << "3. Exit Program\n";
    cout << "Enter your choice (1, 2, or 3): ";
}

// Function to initialize the map data
void initialize_map(NULMapGraph& map) {
    map.add_path("Science Building", "Library", 3.0);
    map.add_path("Library", "Moshoeshoe Building", 2.5);
    map.add_path("Moshoeshoe Building", "ETF", 1.0);
    map.add_path("ETF", "DTF", 1.5);
    map.add_path("DTF", "FTF", 2.0);
    map.add_path("Science Building", "Toilets", 1.0);
    map.add_path("Toilets", "ETF", 3.5);
    map.add_path("FTF", "Library", 4.0);
}

// Function to list all nodes
void list_buildings(const NULMapGraph& map) {
    cout << "\n🏛️ **Available Buildings and Locations** 🏛️\n";
    vector<string> nodes = map.get_nodes();
    for (size_t i = 0; i < nodes.size(); ++i) {
        cout << i + 1 << ". " << nodes[i] << "\n";
    }
    cout << "------------------------------------------\n";
}

// Function to get and validate user input
string get_valid_input(const NULMapGraph& map, const string& prompt) {
    string input;
    vector<string> valid_nodes = map.get_nodes();
    
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Check if the input is a valid building name
        if (find(valid_nodes.begin(), valid_nodes.end(), input) != valid_nodes.end()) {
            return input;
        }
        
        cout << "🚫 Invalid location name. Please enter one of the buildings listed in option 2.\n";
    }
}

// Function to handle the navigation logic
void execute_navigation(const NULMapGraph& nul_map) {
    // Note: It's best practice to validate input against the map nodes.
    // We'll use the validation function to ensure correct names are used.
    
    string start_loc = get_valid_input(nul_map, "Enter the starting building: ");
    string end_loc = get_valid_input(nul_map, "Enter the destination building: ");
    
    // --- Run Dijkstra's Algorithm ---
    map<string, double> distances;
    map<string, string> previous = nul_map.dijkstra(start_loc, distances);

    // --- Display Result ---
    if (distances.count(end_loc) && distances.at(end_loc) != INF) {
        // Assuming the print_shortest_path function is defined or you print manually here.
        // For simplicity, we'll output the result directly:
        vector<string> path;
        string current = end_loc;
        while (current != start_loc) {
            path.push_back(current);
            current = previous.at(current);
        }
        path.push_back(start_loc);
        reverse(path.begin(), path.end());

        cout << "\n✅ **Shortest Path Found**\n";
        cout << "Total Estimated Walk Time (Minutes): **" << fixed << setprecision(1) << distances.at(end_loc) << "**\n";
        cout << "Route: ";
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << "\n------------------------------------------\n";
        print_shortest_path(start_loc, end_loc, previous, distances.at(end_loc), nul_map);

    } else {
        cout << "\n❌ Path not found from **" << start_loc << "** to **" << end_loc << "**.\n";
    }
}

int main() {

    NULMapGraph nul_campus;
    initialize_map(nul_campus);
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

            case 8: { // Navigation Menu
                display_menu();
                int nav_choice = 0;
                if (!(cin >> nav_choice)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Returning to main menu.\n";
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // eat rest of line

                switch (nav_choice) {
                    case 1:
                        execute_navigation(nul_campus);
                        break;
                    case 2:
                        list_buildings(nul_campus);
                        break;
                    case 3:
                        cout << "Exiting navigation menu.\n";
                        break;
                    default:
                        cout << "Unknown option in navigation menu.\n";
                        break;
                }
                break;
            }

            case 9: {
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
