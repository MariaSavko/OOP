
#include "contact.h"
#include <stdexcept>     //
#include <sstream> //


using namespace std;

//
Contact::Contact() {}

//
Contact::Contact(const string& first_name, const string& last_name, const string& email, const string& phone) {
    set_fname(first_name);
    set_lname(last_name);
    set_email(email);
    add_phone(phone, MOBILE);
}

//
Contact::Contact(const Contact& c) {
    first_name = c.first_name;
    last_name = c.last_name;
    surname = c.surname;
    address = c.address;
    birthday = c.birthday;
    email = c.email;
    phoneNumbers = c.phoneNumbers;
}

Contact& Contact::operator=(const Contact& c) { //
    if (this != &c) { //
        first_name = c.first_name;
        last_name = c.last_name;
        surname = c.surname;
        address = c.address;
        birthday = c.birthday;
        email = c.email;
        phoneNumbers = c.phoneNumbers;
    }
    return *this;
}

Contact::~Contact() {}

//
void Contact::set_fname(const string& fname) {
    string error; //
    if (!Validator::valid_name(fname, error)) {
        throw invalid_argument("Invalid first name: " + error); //
    }
    first_name = Validator::whitespace(fname);
}
void Contact::set_lname(const string& lname) {
    string error;
    if (!Validator::valid_name(lname, error)) {
        throw invalid_argument("Invalid last name: " + error);
    }
    last_name = Validator::whitespace(lname);
}
void Contact::set_surname(const string& sname) {
    string error;
    if (sname == "") {
        surname = "";
        return;
    }
    if (!Validator::valid_name(sname, error)) {
        throw invalid_argument("Invalid surname: " + error);
    }
    surname = Validator::whitespace(sname);
}
void Contact::set_email(const string& mail) {
    string error;
    if (!Validator::valid_email(mail, error)) {
        throw invalid_argument("Invalid email " + error);
    }
    email = Validator::whitespace(mail);
}
void Contact::set_birthday(const string& birth) {
    //string error;
    //if (!Validator::valid_birthday(birth, error)) {
    //	throw invalid_argument("Invalid birth date: " + error);
    //}
    birthday = Validator::whitespace(birth);
}
void Contact::set_address(const string& addr) { //
    address = Validator::whitespace(addr);
}

//
string Contact::get_fname() const {
    return first_name;
}
string Contact::get_lname() const {
    return last_name;
}
string Contact::get_surname() const {
    return surname;
}
string Contact::get_address() const {
    return address;
}
string Contact::get_email() const {
    return email;
}
string Contact::get_birthday() const {
    return birthday;
}
string Contact::get_phoneNumber(size_t i) const {
    if (i < phoneNumbers.size()) { //
        return phoneNumbers[i].number;
    }
    else {
        return "";
    }
}
size_t Contact::get_countPhone() const { //
    return phoneNumbers.size();
}
Contact::PhoneType Contact::get_phone_type(size_t i) const {
    if (i < phoneNumbers.size()) {
        return phoneNumbers[i].type; //
    }
    return OTHER; //
}

//
bool Contact::add_phone(const string& phone,PhoneType type) {
    string error;
    if (!Validator::valid_phone(phone, error)) {
        return false;
    }
    string cleanedPhone = Validator::whitespace(phone); //
    phoneNumbers.push_back(PhoneEntry(cleanedPhone, type)); //
    return true;
}

bool Contact::remove_phone(size_t i) {
    if (i < phoneNumbers.size()) {
        phoneNumbers.erase(phoneNumbers.begin() + i);
        return true;
    }
    return false;
}

bool Contact::required_fields() const {
    return !(first_name == "") && !(last_name == "") && !(email == "") && !phoneNumbers.empty();
}


string Contact::to_file_string() const { //
    string result = first_name + ";" + last_name + ";" + surname + ";" +
        address + ";" + birthday + ";" + email + ";" + to_string(phoneNumbers.size());

    for (size_t i = 0; i < phoneNumbers.size(); i++) { //
        result += ";" + phoneNumbers[i].number + ";" + to_string(static_cast<int>(phoneNumbers[i].type));
    }
    return result;
}
Contact Contact::from_file_string(const string& data) { //
    vector<string> fields;
    string field = "";

    for (char c : data) {
        if (c == ';') {
            fields.push_back(field);
            field = "";
        }
        else {
            field += c;
        }
    }
    fields.push_back(field);

    if (fields.size() < 7) {
        throw invalid_argument("Invalid data format");
    }

    Contact contact;
    contact.first_name = fields[0];
    contact.last_name = fields[1];
    contact.surname = fields[2];
    contact.address = fields[3];
    contact.birthday = fields[4];
    contact.email = fields[5];

    size_t phone_count = static_cast<size_t>(stoi(fields[6]));

    for (size_t i = 0; i < phone_count; i++) {
        size_t number_index = 7 + i * 2;
        size_t type_index = number_index + 1;

        if (number_index < fields.size()) {
            string number = fields[number_index];
            PhoneType type = OTHER;

            if (type_index < fields.size()) {
                try {
                    int type_val = stoi(fields[type_index]);
                    if (type_val >= 0 && type_val <= 4) {
                        type = static_cast<PhoneType>(type_val);
                    }
                }
                catch (...) {
                    //
                }
            }

            contact.phoneNumbers.push_back(PhoneEntry(number, type));
        }
    }

    return contact;
}
