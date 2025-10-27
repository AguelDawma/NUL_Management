#ifndef TEXTFILES_H
#define TEXTFILES_H

#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <string>
#include <vector>

// Include all necessary data structure headers
#include "Hashtable.h"
#include "User.h"
#include "Map.h"
#include "Resource.h"
#include "Location.h"
#include "Lab.h"
#include "Bus.h"
#include "LectureHall.h"
#include "Slot.h"

using namespace std;

// File constants
const std::string RESOURCE_FILE = "resources.txt";
const std::string USER_FILE = "users.txt";

// External variable declaration for the resource ID counter (must be defined in main.cpp)
extern int next_resource_id;

// --- Function Prototypes ---
void save_resources(const std::map<std::string, Resource*>& resources_map);
void load_resources(std::map<std::string, Resource*>& resources_map);
void save_users(const HashTable& user_table);
void load_users(HashTable& user_table);

// ----------------------------------------------------------------------------------
// --- Resource Persistence (Saves Resources and Booking Status) ---
// ----------------------------------------------------------------------------------

/**
 * @brief Saves all resources (Labs, Buses, LectureHalls) and their slot data to a text file.
 * Format: ID|Type|Name|Building|Room|Available|Slots:ID,Day,Start,End,Booked;...
 */
void save_resources(const std::map<std::string, Resource*>& resources_map) {
    std::ofstream outfile(RESOURCE_FILE);
    if (!outfile.is_open()) {
        std::cerr << "❌ Error: Could not open " << RESOURCE_FILE << " for saving.\n";
        return;
    }

    for (const auto& pair : resources_map) {
        const Resource* res = pair.second;
        Location loc = res->getLocation();

        // 1. Write common Resource data
        outfile << res->getId() << "|" 
                << res->getType() << "|" 
                << res->getName() << "|" 
                << loc.getBuilding() << "|" 
                << loc.getRoom() << "|" 
                << (res->getAvailability() ? "1" : "0"); // Use 1/0 for boolean

        // 2. Write specific subclass data (Slots for Labs/LectureHalls)
        if (res->getType() == "LAB" || res->getType() == "LECTUREHALL") {
            const Lab* lab = dynamic_cast<const Lab*>(res);
            if (lab) {
                outfile << "|Slots:";
                
                // GetSlots() must be implemented in Lab.h
                for (const auto& slot : lab->getSlots()) { 
                    outfile << slot.id << "," 
                            << slot.day << "," 
                            << slot.startTime << "," 
                            << slot.endTime << "," 
                            << (slot.isBooked ? "1" : "0") << ";"; // Save booking status
                }
            }
        }
        
        // 3. Write final newline
        outfile << "\n";
    }

    outfile.close();
    std::cout << "💾 Successfully saved " << resources_map.size() << " resources to " << RESOURCE_FILE << ".\n";
}

/**
 * @brief Loads resources from the text file into the program's map, handling polymorphism and bookings.
 */
void load_resources(std::map<std::string, Resource*>& resources_map) {
    std::ifstream infile(RESOURCE_FILE);
    if (!infile.is_open()) {
        std::cout << "Loading: " << RESOURCE_FILE << " not found. Starting with empty resources.\n";
        return;
    }

    std::string line;
    int max_id = 0;
    
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> parts;
        
        // Split by the main delimiter '|'
        while (std::getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() < 6) continue;

        // Parse common data
        int id = std::stoi(parts[0]);
        std::string type = parts[1];
        std::string name = parts[2];
        Location loc(0, parts[3], parts[4]); 
        bool available = (parts[5] == "1");

        Resource* new_resource = nullptr;

        // Dynamic allocation based on type (Polymorphism)
        if (type == "LAB") {
            new_resource = new Lab(id, name, type, loc, available);
            // IMPORTANT: If Lab constructor calls addLabSlots(), we need to prevent duplication 
            // by clearing or checking the constructor logic. For loading, we assume the 
            // parameterized constructor does NOT call addLabSlots().
        } else if (type == "LECTUREHALL") {
            new_resource = new LectureHall(id, name, type, loc, available);
        } 
        
        if (new_resource) {
            Lab* lab = dynamic_cast<Lab*>(new_resource);

            // Handle Slot data for Labs/LectureHalls (parts[6] will contain "Slots:...")
            if (lab && parts.size() > 6 && parts[6].find("Slots:") == 0) {
                std::string slot_data = parts[6].substr(parts[6].find(':') + 1);
                std::stringstream slot_ss(slot_data);
                std::string slot_segment;
                
                // Split by the slot delimiter ';'
                while (std::getline(slot_ss, slot_segment, ';')) {
                    if (slot_segment.empty()) continue;

                    std::stringstream field_ss(slot_segment);
                    std::string field;
                    std::vector<std::string> fields;
                    
                    // Split by the field delimiter ','
                    while (std::getline(field_ss, field, ',')) {
                        fields.push_back(field);
                    }

                    if (fields.size() == 5) {
                        Slot loaded_slot(std::stoi(fields[0]), fields[1], fields[2], fields[3]);
                        if (fields[4] == "1") {
                            loaded_slot.isBooked = true; // Set booking status
                        }
                        lab->addSlot(loaded_slot);
                    }
                }
            }

            resources_map[name] = new_resource;
            if (id > max_id) max_id = id;
        }
    }

    // Update global ID counter for new insertions
    next_resource_id = max_id + 1;
    infile.close();
    std::cout << "✅ Loaded " << resources_map.size() << " resources from " << RESOURCE_FILE << ".\n";
}

/**
 * @brief Saves all users from the hash table to a text file.
 * Format: ID|Name|PasswordHash|Type
 */
void save_users(const HashTable& user_table) {
    std::ofstream outfile(USER_FILE);
    if (!outfile.is_open()) {
        std::cerr << "❌ Error: Could not open " << USER_FILE << " for saving.\n";
        return;
    }

    // Assuming HashTable::getAllUsers() returns vector<list<User>> table
    for (const auto& bucket : user_table.getAllUsers()) { 
        for (const User& user : bucket) {
             outfile << user.getId() << "|" 
                     << user.getName() << "|" 
                     << user.getPasswordHash() << "|" 
                     << user.getType() << "\n";
        }
    }
    
    outfile.close();
    std::cout << "💾 Saved all users to " << USER_FILE << ".\n";
}

/**
 * @brief Loads users from the text file and re-inserts them into the hash table.
 */
void load_users(HashTable& user_table) {
    std::ifstream infile(USER_FILE);
    if (!infile.is_open()) {
        std::cout << "Loading: " << USER_FILE << " not found. Starting with empty users.\n";
        return;
    }

    std::string line;
    int loaded_count = 0;
    
    while (std::getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> parts;
        
        while (std::getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() == 4) {
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            std::string passwordHash = parts[2];
            std::string type = parts[3];

            // Re-insert user into the hash table
            user_table.insert(id, name, passwordHash, type);
            loaded_count++;
        }
    }

    infile.close();
    std::cout << "✅ Loaded " << loaded_count << " users from " << USER_FILE << ".\n";
}

#endif // TEXTFILES_H