#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional> // Required for std::hash
#include <algorithm>  // Required for std::remove_if
#include "User.h"

using namespace std;

class HashTable {
private:
    // Use a vector of lists to hold User objects (Chaining)
    vector<list<User>> table;
    int size;

    // A utility to compute the index from the key (username)
    int _hash(const string&) const;

public:
    // Constructor
    HashTable(int);

    // 2. The sign-up function: Accepts user data and stores the User object
    // Note: Type parameter is optional here to match the User ADT constructor
    void insert(int, const string&, const string&, string);
        
    // 3. Retrieves a pointer to the User object based on the name (key)
    User* get(const string&);

    // Authenticate a user by username and password. Returns pointer to User on success, nullptr otherwise.
    User* login(const string& username, const string& password);

    // Display the hash table structure
    void display() const;
};


// A utility to compute the index from the key (username)
int HashTable::_hash(const string& key) const {
    hash<string> hash_fn;
    return hash_fn(key) % size;
}

// Constructor
HashTable::HashTable(int table_size) : size(table_size) {
    table.resize(size);
}

// 2. The sign-up function: Accepts user data and stores the User object
// Note: Type parameter is optional here to match the User ADT constructor
void HashTable::insert(int id, const string& name, const string& passwordHash, const string type) {
    // Use 'name' as the key for hashing
    int index = _hash(name);
    list<User>& bucket = table[index];

    // 2. Check for existing user (collision/update handling)
    for (User& user : bucket) {
        if (user.getName() == name) {
            // Key found: User already exists, update password hash
            cout << "User '" << name << "' already exists!" << endl;
            return;
        }
    }

    // 3. Key is new: Add the new User object to the bucket (sign up)
    bucket.emplace_back(id, name, passwordHash, type);
    cout << "✅ Success! Signed up user: '" << name << "' (Stored in Bucket " << index << ")" << endl;
}

// 3. Retrieves a pointer to the User object based on the name (key)
User* HashTable::get(const string& name) {
    int index = _hash(name);
    list<User>& bucket = table[index];

    // Search the bucket for the key
    for (User& user : bucket) {
        if (user.getName() == name) {
            return &user; // Key found, return pointer to the user object
        }
    }

    return nullptr; // Key not found
}

// Authenticate user by username and password (simple plaintext compare for demo)
User* HashTable::login(const string& username, const string& password) {
    int index = _hash(username);
    list<User>& bucket = table[index];
    for (User& user : bucket) {
        if (user.getName() == username) {
            if (user.getPasswordHash() == password && user.getType().size() >= 0) {
                return &user;
            }
            return nullptr;
        }
    }
    return nullptr;
}

// Display the hash table structure
void HashTable::display() const {
    cout << "\n--- Current User Database (Hash Table Structure) ---" << endl;
    for (int i = 0; i < size; ++i) {
        cout << "Bucket " << i << ": [";
        for (const User& user : table[i]) {
            // Displaying key attributes
            cout << " (ID:" << user.getId() << ", NAME:" << user.getName() << ", TYPE: " << user.getType() << ") ,";
        }
        cout << "]" << endl;
    }
    cout << "--------------------------------------------------" << endl;
}

#endif //HASHTABLE_H
