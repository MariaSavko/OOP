#pragma once
#include <string>
#include <vector>

#include "validator.h"

using namespace std;

class Contact {
public:
    //
    enum PhoneType {
        HOME,
        WORK,
        OFFICE,
        MOBILE,
        OTHER
    };
private:
    string first_name;
    string last_name;
    string surname;
    string address;
    string birthday;
    string email;

    struct PhoneEntry {
        string number;
        PhoneType type;
        PhoneEntry(const string& num, PhoneType t) : number(num), type(t) {}
    };

    vector<PhoneEntry> phoneNumbers;


public:
    //
    Contact();
    Contact(const string& first_name, const string& last_name, const string &email, const string &phone);
    Contact(const Contact& c);
    Contact& operator=(const Contact& c); //
    ~Contact();

    //
    string get_fname() const;
    string get_lname() const;
    string get_surname() const;
    string get_address() const;
    string get_email() const;
    string get_birthday() const; //
    string get_phoneNumber(size_t i) const;
    size_t get_countPhone() const; //
    PhoneType get_phone_type(size_t i) const;


    //
    void set_fname(const string& fname); //
    void set_lname(const string& lname);
    void set_surname(const string& sname);
    void set_address(const string& addr);
    void set_email(const string& mail);
    void set_birthday(const string& birth);

    //
    bool add_phone(const string& phone, PhoneType type); //
    bool remove_phone(size_t i); //

    static string phoneTypeToString(PhoneType type) {
        switch (type) {
        case HOME: return "Home";
        case WORK: return "Work";
        case OFFICE: return "Office";
        case MOBILE: return "Mobile";
        case OTHER: return "Other";
        default: return "Unknown";
        }
    }

    string to_file_string() const; //
    static Contact from_file_string(const string& data); //
    bool required_fields() const; //
};
