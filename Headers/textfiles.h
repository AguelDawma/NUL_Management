#ifndef TEXTFILES_H
#define TEXTFILES_H

#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>

// Include all necessary data structure headers (needed for dynamic casting)
#include "Hashtable.h"
#include "User.h"
#include "Resource.h"
#include "Lab.h"
#include "Bus.h"
#include "LectureHall.h"
#include "Slot.h"

using namespace std;

// File constants
const std::string RESOURCE_FILE = "resources.txt";
const std::string USER_FILE = "users.txt";

// External variable declaration for ID counters (must be defined in main.cpp)
extern int next_user_id;
extern int next_resource_id;

// Global resource map (Needed for load_users to correctly link bookings)
extern map<int, Resource*> resources_table;

// --- Function Prototypes ---
void save_resources(const std::map<int, Resource*>& resources_map);
void load_resources(std::map<int, Resource*>& resources_map);
void save_users(const HashTable& user_table);
void load_users(HashTable& user_table);


// ----------------------------------------------------------------------------------
// --- Helper functions for data saving (Serialization) ---
// ----------------------------------------------------------------------------------

/**
 * @brief Serializes all resources (Labs, Buses, LectureHalls) and their slot/waitlist data to a text file.
 * Format (LAB/LECTUREHALL): ID|Type|Name|LocationName|Available|Slots:SId,Day,Start,End,Booked;...|Waitlist:UId,UId,...
 * Format (BUS): ID|BUS|Name|LocationName|Available|FromDate|ToDate
 */
void save_resources(const std::map<int, Resource*>& resources_map) {
    std::ofstream outfile(RESOURCE_FILE);
    if (!outfile.is_open()) {
        std::cerr << "? ERROR: Could not open " << RESOURCE_FILE << " for writing.\n";
        return;
    }

    for (const auto& pair : resources_map) {
        Resource* r = pair.second;
        // Base Resource data
        outfile << r->getId() << "|"
                << r->getType() << "|"
                << r->getName() << "|"
                << r->getLocation().getName() << "|"
                << r->getAvailability() << "|";

        if (r->getType() == "BUS") {
            Bus* bus_ptr = dynamic_cast<Bus*>(r);
            outfile << bus_ptr->getFromDate() << "|" << bus_ptr->getToDate();
        } else if (r->getType() == "LAB" || r->getType() == "LECTUREHALL") {
            Lab* lab_ptr = dynamic_cast<Lab*>(r);

            // 1. Slots (BST contents)
            outfile << "Slots:";
            vector<Slot> slots = lab_ptr->getSlots();
            for (const auto& s : slots) {
                // SId,Day,Start,End,Booked;
                outfile << s.id << "," << s.day << "," << s.startTime << "," << s.endTime << "," << s.isBooked << ";";
            }
            outfile << "|";

            // 2. Waitlist (Queue contents)
            outfile << "Waitlist:";
            vector<int> waitlist_users;
            lab_ptr->getWaitlist(waitlist_users); // Get the users from the queue
            for (int userId : waitlist_users) {
                outfile << userId << ",";
            }
        }
        outfile << "\n";
    }

    outfile.close();
    std::cout << "?? Saved all resources and their states to " << RESOURCE_FILE << ".\n";
}

/**
 * @brief Serializes all users and their booking/waitlist information to a text file.
 * Format: ID|Name|PasswordHash|Type|Bookings:RId,SId;RId,SId;...
 */
void save_users(const HashTable& user_table) {
    std::ofstream outfile(USER_FILE);
    if (!outfile.is_open()) {
        std::cerr << "? ERROR: Could not open " << USER_FILE << " for writing.\n";
        return;
    }

    // Iterate through all buckets in the hash table
    for (const auto& bucket : user_table.getAllUsers()) {
        for (const User& user : bucket) {
             // Base User data
             outfile << user.getId() << "|" 
                     << user.getName() << "|" 
                     << user.getPasswordHash() << "|" 
                     << user.getType() << "|";
            
             // User Bookings: RId,SId;RId,SId;...
             outfile << "Bookings:";
             for (const auto& booking : user.getBookings()) {
                 outfile << booking.first << "," << booking.second << ";";
             }
             outfile << "\n";
        }
    }
    
    outfile.close();
    std::cout << "?? Saved all users to " << USER_FILE << ".\n";
}


// ----------------------------------------------------------------------------------
// --- Helper functions for data loading (Deserialization) ---
// ----------------------------------------------------------------------------------

/**
 * @brief Loads resources from the text file and re-populates the resource map.
 */
void load_resources(std::map<int, Resource*>& resources_map) {
    std::ifstream infile(RESOURCE_FILE);
    if (!infile.is_open()) {
        std::cout << "Loading: " << RESOURCE_FILE << " not found. Starting with default resources.\n";
        return;
    }

    // Clear any default resources added during initialization
    for (auto& pair : resources_map) { delete pair.second; }
    resources_map.clear();
    next_resource_id = 1; // Reset counter, will be updated during loading

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

        if (parts.size() < 5) continue; // Basic check

        int id = std::stoi(parts[0]);
        std::string type = parts[1];
        std::string name = parts[2];
        Location location(parts[3]);
        bool available = (parts[4] == "1");

        Resource* new_resource = nullptr;

        if (type == "BUS" && parts.size() >= 7) {
            Bus* bus = new Bus(id, name, type, location, available);
            bus->setFromDate(parts[5]);
            bus->setToDate(parts[6]);
            new_resource = bus;
        } else if ((type == "LAB" || type == "LECTUREHALL") && parts.size() >= 7) {
            Lab* lab = (type == "LAB") 
                       ? new Lab(id, name, type, location, available, false) // false = do not add default slots
                       : new LectureHall(id, name, type, location, available, false);
            
            // 1. Load Slots (BST)
            std::string slots_data = parts[5].substr(parts[5].find(":") + 1);
            std::stringstream ss_slots(slots_data);
            std::string slot_segment;
            
            while (std::getline(ss_slots, slot_segment, ';')) {
                if (slot_segment.empty()) continue;
                std::stringstream ss_slot(slot_segment);
                std::string s_part;
                std::vector<std::string> s_parts;
                while (std::getline(ss_slot, s_part, ',')) { s_parts.push_back(s_part); }

                if (s_parts.size() == 5) {
                    Slot s(std::stoi(s_parts[0]), s_parts[1], s_parts[2], s_parts[3]);
                    s.isBooked = (s_parts[4] == "1");
                    lab->addSlot(s);
                }
            }
            
            // 2. Load Waitlist (Queue)
            std::string waitlist_data = parts[6].substr(parts[6].find(":") + 1);
            std::stringstream ss_waitlist(waitlist_data);
            std::string user_segment;
            
            while (std::getline(ss_waitlist, user_segment, ',')) {
                if (!user_segment.empty()) {
                    lab->loadWaitlist(std::stoi(user_segment));
                }
            }
            new_resource = lab;
        }

        if (new_resource) {
            resources_map[id] = new_resource;
            next_resource_id = std::max(next_resource_id, id + 1);
            loaded_count++;
        }
    }
    
    infile.close();
    std::cout << "? Loaded " << loaded_count << " resources from " << RESOURCE_FILE << ".\n";
}


/**
 * @brief Loads users from the text file and re-inserts them into the hash table,
 * and then links their bookings to the already loaded resources.
 */
void load_users(HashTable& user_table) {
    std::ifstream infile(USER_FILE);
    if (!infile.is_open()) {
        std::cout << "Loading: " << USER_FILE << " not found. Starting with default users.\n";
        return;
    }

    // Clear existing users
    user_table = HashTable(user_table.getSize()); // Re-initialize the hash table
    next_user_id = 1; // Reset counter

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

        if (parts.size() == 5) { // ID|Name|PasswordHash|Type|Bookings:...
            int id = std::stoi(parts[0]);
            std::string name = parts[1];
            std::string passwordHash = parts[2];
            std::string type = parts[3];
            std::string bookings_data = parts[4].substr(parts[4].find(":") + 1);

            // Re-insert user into the hash table
            user_table.insert(id, name, passwordHash, type);
            User* loaded_user = user_table.get(name);
            next_user_id = std::max(next_user_id, id + 1);

            // Load Bookings
            if (loaded_user) {
                std::stringstream ss_bookings(bookings_data);
                std::string booking_segment;
                while (std::getline(ss_bookings, booking_segment, ';')) {
                    if (booking_segment.empty()) continue;
                    
                    std::stringstream ss_booking(booking_segment);
                    std::string rid_str, sid_str;
                    
                    if (std::getline(ss_booking, rid_str, ',') && std::getline(ss_booking, sid_str, ',')) {
                        int rid = std::stoi(rid_str);
                        int sid = std::stoi(sid_str);
                        
                        // Check if the resource exists before loading the booking
                        if (resources_table.count(rid)) {
                            loaded_user->loadBooking(rid, sid);
                        }
                    }
                }
            }
            loaded_count++;
        }
    }
    
    infile.close();
    std::cout << "? Loaded " << loaded_count << " users from " << USER_FILE << ".\n";
}

#endif // TEXTFILES_H

