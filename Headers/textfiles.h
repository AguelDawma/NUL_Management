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

#include "Hashtable.h"
#include "User.h"
#include "Resource.h"
#include "Lab.h"
#include "Bus.h"
#include "LectureHall.h"
#include "Slot.h"

using namespace std;

const string RESOURCE_FILE = "resources.txt";
const string USER_FILE = "users.txt";

extern int next_user_id;
extern int next_resource_id;

extern map<int, Resource*> resources_table;

// Function Prototypes
void save_resources(const map<int, Resource*>& resources_map);
void load_resources(map<int, Resource*>& resources_map);
void save_users(const HashTable& user_table);
void load_users(HashTable& user_table);

/**
 * @brief Serializes all resources (Labs, Buses, LectureHalls) and their slot/waitlist data to a text file.
 * Format (LAB/LECTUREHALL): ID|Type|Name|LocationName|Available|Slots:SId,Day,Start,End,Booked;...|Waitlist:UId,UId,...
 * Format (BUS): ID|BUS|Name|LocationName|Available|FromDate|ToDate
 */
void save_resources(const map<int, Resource*>& resources_map) {
    ofstream outfile(RESOURCE_FILE);
    if (!outfile.is_open()) {
        cerr << "\nERROR: Could not open " << RESOURCE_FILE << " for writing.\n";
        return;
    }

    for (const auto& pair : resources_map) {
        Resource* r = pair.second;

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

            // 1. Slots
            outfile << "Slots:";
            vector<Slot> slots = lab_ptr->getSlots();
            for (const auto& s : slots) {
                outfile << s.id << "," << s.day << "," << s.startTime << "," << s.endTime << "," << s.isBooked << ";";
            }
            outfile << "|";

            // 2. Waitlist
            outfile << "Waitlist:";
            vector<int> waitlist_users;
            lab_ptr->getWaitlist(waitlist_users);
            for (int userId : waitlist_users) {
                outfile << userId << ",";
            }
        }
        outfile << "\n";
    }

    outfile.close();
    cout << "\nSaved all resources and their states to " << RESOURCE_FILE << ".\n";
}

/**
 * @brief Serializes all users and their booking/waitlist information to a text file.
 * Format: ID|Name|PasswordHash|Type|Bookings:RId,SId;RId,SId;...
 */
void save_users(const HashTable& user_table) {
    ofstream outfile(USER_FILE);
    if (!outfile.is_open()) {
        cerr << "\nERROR: Could not open " << USER_FILE << " for writing.\n";
        return;
    }

    for (const auto& bucket : user_table.getAllUsers()) {
        for (const User& user : bucket) {
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
    cout << "\nSaved all users to " << USER_FILE << ".\n";
}

void load_resources(map<int, Resource*>& resources_map) {
    ifstream infile(RESOURCE_FILE);
    if (!infile.is_open()) {
        cout << "\nLoading: " << RESOURCE_FILE << " not found. Starting with default resources.\n";
        return;
    }

    // Clear any default resources added during initialization
    for (auto& pair : resources_map) { delete pair.second; }
    resources_map.clear();
    next_resource_id = 1;

    string line;
    int loaded_count = 0;
    
    while (getline(infile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        vector<string> parts;
        
        while (getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() < 5) continue;

        int id = stoi(parts[0]);
        string type = parts[1];
        string name = parts[2];
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
            
            // 1. Load Slots
            string slots_data = parts[5].substr(parts[5].find(":") + 1);
            stringstream ss_slots(slots_data);
            string slot_segment;
            
            while (getline(ss_slots, slot_segment, ';')) {
                if (slot_segment.empty()) continue;
                stringstream ss_slot(slot_segment);
                string s_part;
                vector<string> s_parts;
                while (getline(ss_slot, s_part, ',')) { s_parts.push_back(s_part); }

                if (s_parts.size() == 5) {
                    Slot s(stoi(s_parts[0]), s_parts[1], s_parts[2], s_parts[3]);
                    s.isBooked = (s_parts[4] == "1");
                    lab->addSlot(s);
                }
            }
            
            // 2. Load Waitlist
            string waitlist_data = parts[6].substr(parts[6].find(":") + 1);
            stringstream ss_waitlist(waitlist_data);
            string user_segment;
            
            while (getline(ss_waitlist, user_segment, ',')) {
                if (!user_segment.empty()) {
                    lab->loadWaitlist(stoi(user_segment));
                }
            }
            new_resource = lab;
        }

        if (new_resource) {
            resources_map[id] = new_resource;
            next_resource_id = max(next_resource_id, id + 1);
            loaded_count++;
        }
    }
    
    infile.close();
    cout << "\nLoaded " << loaded_count << " resources from " << RESOURCE_FILE << ".\n";
}

void load_users(HashTable& user_table) {
    ifstream infile(USER_FILE);
    if (!infile.is_open()) {
        cout << "\nLoading: " << USER_FILE << " not found. Starting with default users.\n";
        return;
    }

    // Clear existing users
    user_table = HashTable(user_table.getSize());
    next_user_id = 1;

    string line;
    int loaded_count = 0;
    
    while (getline(infile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string segment;
        vector<string> parts;
        
        while (getline(ss, segment, '|')) {
            parts.push_back(segment);
        }

        if (parts.size() == 5) {
            int id = stoi(parts[0]);
            string name = parts[1];
            string passwordHash = parts[2];
            string type = parts[3];
            string bookings_data = parts[4].substr(parts[4].find(":") + 1);

            // Re-insert user into the hash table
            user_table.insert(id, name, passwordHash, type);
            User* loaded_user = user_table.get(name);
            next_user_id = max(next_user_id, id + 1);

            // Load Bookings
            if (loaded_user) {
                stringstream ss_bookings(bookings_data);
                string booking_segment;
                while (getline(ss_bookings, booking_segment, ';')) {
                    if (booking_segment.empty()) continue;
                    
                    stringstream ss_booking(booking_segment);
                    string rid_str, sid_str;
                    
                    if (getline(ss_booking, rid_str, ',') && getline(ss_booking, sid_str, ',')) {
                        int rid = stoi(rid_str);
                        int sid = stoi(sid_str);
                        
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
    cout << "\nLoaded " << loaded_count << " users from " << USER_FILE << ".\n";
}

#endif // TEXTFILES_H

