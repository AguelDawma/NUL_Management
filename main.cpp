#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include "Headers/Hashtable.h"
#include "Headers/User.h"
#include "Headers/Map.h"
#include "Headers/Resource.h"
#include "Headers/Location.h"
#include "Headers/Lab.h"
#include "Headers/Bus.h"
#include "Headers/LectureHall.h"

using namespace std;

static void printMenu();
void display_menu();
void add_new_resource(map<string, Resource*>& resources_map);
void print_all_resources(const std::map<std::string, Resource*>& resources_map);
void initialize_map(NULMapGraph& map);
void list_buildings(const NULMapGraph& map);
string get_valid_input(const NULMapGraph& map, const string& prompt);
void execute_navigation(const NULMapGraph& nul_map);
void addLabSlots();

int main() {

    map<string, Resource*> resources_table;
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
                if (resources_table.empty()) {
                    cout << "\nNo resources have been added yet.\n";
                    break;
                }

                cout << "\nChoose resource id amongst the below resources.\n"<<endl;

                int count = 1;
                // Iterate over the map of resources
                for (const auto& pair : resources_table) {
                    // Use the base class pointer to access common methods
                    const Resource* res = pair.second;
                    Location loc = res->getLocation(); // Get location details
                    
                    cout << "[" << count++ << "] ID: " << res->getId()
                            << " | Name: " << res->getName() << "" // Name is the map key
                            << " | Type: " << res->getType() // e.g., LAB, BUS
                            << " | Availability: " << (res->getAvailability() ? "Yes" : "No") << "\n";
                    
                    // Print Location details
                    std::cout << "    Location: Building " << loc.getBuilding()<<endl;
                }
                std::cout << "\n--------------------------------------------\n";

                int rid; string rname, rtype;
                cout << "\n Enter Resource id (integer): ";
                if (!(cin >> rid)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    cout << "Invalid id.\n"; 
                    break; 
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                for(const auto& pair : resources_table){
                    const Resource* resourceB = pair.second;
                    const Lab* resource = dynamic_cast<const Lab*>(resourceB);
                    if(rid == resource->getId() && resource->getType() == "LAB"){
                        int sid;
                        cout<<"\nChoose from the slots:"<<endl;
                        resource->viewAvailableSlots();
                        cout<< " Enter slot id: ";
                        cin>> sid;

                        resource->bookSlot(sid);
                        break;
                    }
                }
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
            	int nav_choice = 0;
            	do{
            		display_menu();
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
				}while(nav_choice!=3);
                break;
            }

            case 9: {
                add_new_resource(resources_table);
                break;
            }

            case 10: {
                print_all_resources(resources_table);
                break;
            }

            case 11: {
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

static void printMenu() {
    cout << "\n=== NUL Management Menu ===\n"<<endl;
    cout << "1)  Login\n";
    cout << "2)  Sign up user\n";
    cout << "3)  Logout\n";
    cout << "4)  List users\n";
    cout << "5)  View user bookings\n";
    cout << "6)  Add booking to (logged-in) user\n";
    cout << "7)  Remove booking from (logged-in) user\n";
    cout << "8)  Navigate NUL Campus\n";
    cout << "9)  Add New Resource\n";
    cout << "10) List All Resources\n";
    cout << "11) Quit\n"<<endl;
    cout << "\nChoose an option : ";
}

// Function to display the menu options
void display_menu() {
    cout << "\n============================================\n";
    cout << "          NUL Campus Navigation Menu�\n";
    cout << "============================================\n";
    cout << "1. Find Shortest Path between two buildings\n";
    cout << "2. List all available buildings/locations\n";
    cout << "3. Exit Program\n"<<endl;
    cout << "\nEnter your choice : ";
}

// A utility to simulate ID generation
static int next_resource_id = 1;

/**
 * @brief Prompts the user for resource details, creates the object, and stores it.
 * * @param resources_map The resource collection (simulating a Hash Table's main container).
 */
void add_new_resource(map<string, Resource*>& resources_map) {
    string type_input;
    cout << "\n--- Add New Resource ---\n";
    cout << "\nEnter resource type (LAB, BUS, LECTUREHALL): ";
    cin >> type_input;

    // Standardize input to uppercase for comparison
    transform(type_input.begin(), type_input.end(), type_input.begin(), ::toupper);

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Consume newline

    // --- Collect Common Resource Data ---
    string name, building, room;
    cout << "\nEnter resource name (e.g., Chem Lab 1): ";
    getline(cin, name);
    
    // Check for duplicates before gathering more info
    if (resources_map.count(name)) {
        cout << "Error: A resource named '" << name << "' already exists.\n";
        return;
    }

    cout << "\nEnter building location: ";
    getline(cin, building);
    cout << "\nEnter room: ";
    getline(cin, room);

    // Create a Location object
    Location loc(0, building, room); 

    Resource* new_resource = NULL;
    int current_id = next_resource_id++;

    // --- Create Specific Resource Type Polymorphically ---
    if (type_input == "LAB") {
        // Lab constructor: Lab(id, name, type, location, available)
        new_resource = new Lab(current_id, name, "LAB", loc, true);
        
    } else if (type_input == "BUS") {
        // Bus constructor: Bus() inherits from Resource()
        Bus* bus = new Bus();
        bus->setId(current_id);
        bus->setName(name);
        bus->setType("BUS");
        bus->setLocation(loc);
        bus->setAvailability(true);
        
        new_resource = bus;

    } else if (type_input == "LECTUREHALL") {
        // LectureHall constructor: LectureHall(id, name, type, location, available)
        new_resource = new LectureHall(current_id, name, "LECTUREHALL", loc, true);
        
    } else {
        cout << "\nInvalid resource type entered.\n";
        return;
    }

    // --- Store the Resource ---
    if (new_resource) {
        // This simulates the insertion into your HashTable, using the name as the key.
        resources_map[name] = new_resource; 
        
        cout << "\nResource Successfully added " << new_resource->getType() << " '" << name << "' (ID: " << current_id << ") to the resource manager.\n";
    }
}

/**
 * @brief Iterates through the resources map and prints key details for all stored resources.
 * @param resources_map The resource collection (std::map<std::string, Resource*>).
 */
void print_all_resources(const std::map<std::string, Resource*>& resources_map) {
    std::cout << "\n============================================\n";
    std::cout << "          Currently Stored Resources \n";
    std::cout << "============================================\n";

    if (resources_map.empty()) {
        std::cout << "\nNo resources have been added yet.\n";
        return;
    }

    int count = 1;
    // Iterate over the map of resources
    for (const auto& pair : resources_map) {
        // Use the base class pointer to access common methods
        const Resource* res = pair.second;
        Location loc = res->getLocation(); // Get location details
        
        std::cout << "[" << count++ << "] ID: " << res->getId()
                  << " | Name: " << res->getName() << "" // Name is the map key
                  << " | Type: " << res->getType() // e.g., LAB, BUS
                  << " | Availability: " << (res->getAvailability() ? "Yes" : "No") << "\n";
        
        // Print Location details
        std::cout << "    Location: Building " << loc.getBuilding()<<endl;
    }
    std::cout << "\n--------------------------------------------\n";
}

// Function to initialize the map data
void initialize_map(NULMapGraph& map) {
    map.add_path("Science Building", "Library", 2.0);
    map.add_path("Library", "Moshoeshoe Building", 2.0);
    map.add_path("Moshoeshoe Building", "ETF", 4.0);
    map.add_path("Moshoeshoe Building","Law Building", 1.0);
    map.add_path("Law Building", "Toilets", 1.0);
    map.add_path("Toilets", "BTM", 1.0);
    map.add_path("BTM", "DTF", 1.0);
    map.add_path("ETF", "DTF", 1.0);
    map.add_path("ETF", "FTF", 3.0);
    map.add_path("Science Building", "Toilets", 1.5);
    map.add_path("Toilets", "ETF", 3.5);
    map.add_path("ETF", "Economics Building", 1.0);
    map.add_path("Science Building", "Boitjaro", 1.0);
    map.add_path("Boitjaro", "toilets", 1.5);
    map.add_path("BTM", "CMP", 1.0);
    map.add_path("CMP", "Netherland Hall", 1.0);
    map.add_path("CMP", "Economics Building", 1.0);
    map.add_path("CMP", "ETF", 1.0);
    map.add_path("Netherlands Hall", "Economics Building", 2.0);
}

// Function to list all nodes
void list_buildings(const NULMapGraph& map) {
    cout << "\nAvailable Buildings and Locations:\n";
    vector<string> nodes = map.get_nodes();
    for (size_t i = 0; i < nodes.size(); ++i) {
        cout << i + 1 << ". " << nodes[i] << "\n";
    }
    cout << "\n------------------------------------------\n";
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
        
        cout << "\nInvalid location name. Please enter one of the buildings listed in option 2.\n";
    }
}

// Function to handle the navigation logic
void execute_navigation(const NULMapGraph& nul_map) {
    // Note: It's best practice to validate input against the map nodes.
    // We'll use the validation function to ensure correct names are used.
    
    string start_loc = get_valid_input(nul_map, "\nEnter the starting building: ");
    string end_loc = get_valid_input(nul_map, "\nEnter the destination building: ");
    
    map<string, double> distances;
    map<string, string> previous = nul_map.dijkstra(start_loc, distances);

    if (distances.count(end_loc) && distances.at(end_loc) != INF) {
        vector<string> path;
        string current = end_loc;
        while (current != start_loc) {
            path.push_back(current);
            current = previous.at(current);
        }
        path.push_back(start_loc);
        reverse(path.begin(), path.end());

        cout << "\nShortest Path Found\n";
        cout << "\nTotal Estimated Walk Time (Minutes): " << fixed << setprecision(1) << distances.at(end_loc) << "\n";
        cout << "\nRoute: ";
        
        for (size_t i = 0; i < path.size(); ++i) {
            cout << path[i];
            if (i < path.size() - 1) {
                cout << " -> ";
            }
        }
        
        cout << "\n------------------------------------------\n";
        print_shortest_path(start_loc, end_loc, previous, distances.at(end_loc), nul_map);

    } else {
        cout << "\nPath not found from " << start_loc << " to " << end_loc << ".\n";
    }
}


