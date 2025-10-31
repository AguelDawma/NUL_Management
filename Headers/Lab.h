#ifndef LAB_H
#define LAB_H

#include <iostream>
#include <string>
#include <vector>
#include <queue> // Required for Waitlist Queue
#include <limits> // Required for numeric_limits in main.cpp, added here for completeness
#include "Resource.h"
#include "Slot.h"

using namespace std;

// ----------------------------------------------------
// ?? Slot BST Node Structure for Scheduling/Searching
// Trees are excellent for quick searching and sorting of keys (slot.id)
// ----------------------------------------------------
struct SlotNode {
    Slot slot;
    SlotNode* left;
    SlotNode* right;

    // Constructor initializes the node with a Slot and null children
    SlotNode(const Slot& s) : slot(s), left(nullptr), right(nullptr) {}
};

class Lab: public Resource {
protected:
    // ?? Root of the Binary Search Tree (BST) to manage slots
    SlotNode* slots_tree = nullptr; 

    // ? Queue for Waitlist/Booking Queue
    // Stores User IDs (int) of those waiting for a slot
    queue<int> waitlist; 

    // --- BST Helper Functions (Private Implementation) ---
    // Inserts a new slot into the BST based on its ID
    SlotNode* insertSlotNode(SlotNode* node, const Slot& slot);
    // Searches for a slot node based on its ID
    SlotNode* findSlotNode(SlotNode* node, int slotId) const;
    // Traverses the tree (in-order) to collect all slots into a vector
    void inorderTraversal(SlotNode* node, vector<Slot>& result) const;
    // Traverses the tree (in-order) to print only available slots
    void viewAvailableSlotsRecursive(SlotNode* node) const;
    // Recursively deletes the entire tree (used in destructor)
    void deleteTree(SlotNode* node); 

public:
    // Setters
    void addSlot(const Slot& slot); // Updated to use BST insert

    // Getters / actions
    void viewAvailableSlots() const; // Updated to use BST traversal
    bool bookSlot(int slotId); // Updated to use BST search
    bool cancelSlotBooking(int slotId); // Updated to use BST search
    void addLabSlots(); // Initializes default slots
    Slot getSlot(int id) const; // Updated to use BST search
    vector<Slot> getSlots() const; // Updated to use BST traversal

    // ? Waitlist/Queue operations
    void addToWaitlist(int userId); // Adds a user to the back of the queue (FIFO)
    int processWaitlist(); // Removes the user from the front of the queue and returns their ID

    // Constructors and Destructor
    Lab();
    Lab(int id, const string& name, const string& type, Location location, bool available);
    ~Lab();
};

// =======================================================================
//                        LAB IMPLEMENTATION
// =======================================================================

// --- BST Helper Implementations ---

SlotNode* Lab::insertSlotNode(SlotNode* node, const Slot& slot) {
    if (node == nullptr) {
        return new SlotNode(slot);
    }
    // Tree key is the Slot ID
    if (slot.id < node->slot.id) {
        node->left = insertSlotNode(node->left, slot);
    } else if (slot.id > node->slot.id) {
        node->right = insertSlotNode(node->right, slot);
    }
    // Ignore duplicate IDs (no action needed for equality)
    return node;
}

SlotNode* Lab::findSlotNode(SlotNode* node, int slotId) const {
    if (node == nullptr || node->slot.id == slotId) {
        return node;
    }
    if (slotId < node->slot.id) {
        return findSlotNode(node->left, slotId);
    } else {
        return findSlotNode(node->right, slotId);
    }
}

void Lab::inorderTraversal(SlotNode* node, vector<Slot>& result) const {
    // In-order traversal gives slots sorted by ID (the key)
    if (node) {
        inorderTraversal(node->left, result);
        result.push_back(node->slot);
        inorderTraversal(node->right, result);
    }
}

void Lab::viewAvailableSlotsRecursive(SlotNode* node) const {
    if (node) {
        viewAvailableSlotsRecursive(node->left);
        if (!node->slot.isBooked) {
            cout << "  Slot id=" << node->slot.id << " " << node->slot.day << " " << node->slot.startTime << " - " << node->slot.endTime << "\n";
        }
        viewAvailableSlotsRecursive(node->right);
    }
}

void Lab::deleteTree(SlotNode* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

// --- Lab Constructors/Destructor ---

Lab::Lab() {
    setId(0);
    setName("");
    setType("");
    setLocation(Location());
    setAvailability(true);
    addLabSlots(); // Add slots using the new BST structure
}

Lab::Lab(int id, const string& name, const string& type, Location location, bool available) {
    setId(id);
    setName(name);
    setType(type);
    setLocation(location);
    setAvailability(available);
    addLabSlots(); // Add slots using the new BST structure
}

Lab::~Lab() {
    deleteTree(slots_tree); // ?? Clean up the dynamically allocated BST nodes
}

// --- Lab Method Updates (using BST) ---

Slot Lab::getSlot(int id) const {
    SlotNode* node = findSlotNode(slots_tree, id);
    if (node) {
        return node->slot;
    }
    cout << "\nSlot not found." << endl;
    return Slot(); // Return a default/empty slot
}

void Lab::addSlot(const Slot& slot) {
    slots_tree = insertSlotNode(slots_tree, slot);
}

void Lab::viewAvailableSlots() const {
    cout << "Available slots for Lab '" << getName() << "' (id=" << getId() << "):\n";
    if (slots_tree == nullptr) {
        cout << "  (no slots defined)\n";
    } else {
        viewAvailableSlotsRecursive(slots_tree);
    }
}

bool Lab::bookSlot(int slotId) {
    SlotNode* node = findSlotNode(slots_tree, slotId);
    if (node && !node->slot.isBooked) {
        node->slot.isBooked = true;
        return true;
    }
    // Note: If booking fails, main.cpp handles the prompt to join the waitlist
    return false;
}

bool Lab::cancelSlotBooking(int slotId) {
    SlotNode* node = findSlotNode(slots_tree, slotId);
    if (node && node->slot.isBooked) {
        node->slot.isBooked = false;
        
        // ? After cancellation, check and process the waitlist
        if (!waitlist.empty()) {
            int next_user_id = processWaitlist();
            cout << "? Slot " << slotId << " canceled and freed up.\n";
            cout << "?? Waitlist notification: User ID " << next_user_id << " is next in line for this resource.\n";
        } else {
            cout << "? Slot " << slotId << " canceled and freed up. Waitlist is empty.\n";
        }
        return true;
    }
    return false;
}

vector<Slot> Lab::getSlots() const {
    vector<Slot> all_slots;
    inorderTraversal(slots_tree, all_slots);
    return all_slots;
}

void Lab::addLabSlots(){
    // Using a simple set of slots. IDs must be unique for the BST to work correctly.
    // Slots are added using the BST insertion function `addSlot`.
    Slot monday1(1,"Monday","08:00","10:00");
    Slot monday2(2,"Monday","10:00","12:00");
    Slot monday3(3,"Monday","12:00","14:00");
    Slot monday4(4,"Monday","14:00","16:00");
    Slot tuesday1(5,"Tuesday","08:00","10:00");
    Slot tuesday2(6,"Tuesday","10:00","12:00");
    Slot wednesday1(7,"Wednesday","14:00","16:00");
    
    addSlot(monday1);
    addSlot(monday2);
    addSlot(monday3);
    addSlot(monday4);
    addSlot(tuesday1);
    addSlot(tuesday2);
    addSlot(wednesday1);
}

// --- Queue Implementations ---

void Lab::addToWaitlist(int userId) {
    // ? Queues are FIFO (First-In, First-Out)
    waitlist.push(userId);
    cout << "? User ID " << userId << " added to waitlist for " << getName() << ". You are #" << waitlist.size() << " in line.\n";
}

int Lab::processWaitlist() {
    if (waitlist.empty()) {
        return 0; // No user in the waitlist
    }
    int next_user_id = waitlist.front(); // Get the first user
    waitlist.pop(); // Remove them from the queue
    return next_user_id;
}


#endif // LAB_H

