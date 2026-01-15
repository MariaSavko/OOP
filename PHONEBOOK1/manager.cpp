// TASK 2: Using QFile for file operations (replaces std::fstream from Task 1) 

#include "manager.h"
#include <iostream>
#include <cctype>
#include <QFile>
#include <QTextStream>
#include <QDebug>

using namespace std;

ManageContacts::ManageContacts() {
    filename = "phonebook.txt";  //
    load_from_file();
}

ManageContacts::ManageContacts(const string& file) {
    filename = file;
    load_from_file();
}

ManageContacts::ManageContacts(const ManageContacts& m) {
    filename = m.filename;
    contacts = m.contacts;
}

ManageContacts::~ManageContacts() {
    save_to_file();
}

string ManageContacts::to_lower(const string& str) {
    string low_str = str;
    for (size_t i = 0; i < low_str.length(); i++) {
        low_str[i] = tolower(low_str[i]);
    }
    return low_str;
}

void ManageContacts::load_from_file() {
    QFile file(QString::fromStdString(filename));
    if (!file.exists()) {
        qDebug() << "File" << QString::fromStdString(filename)
            << "does not exist. Creating empty phonebook.";
        return;
    }
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Cannot open file" << QString::fromStdString(filename)
            << "for reading:" << file.errorString();
        throw std::runtime_error("Failed to open contacts file");

    }

    QTextStream in(&file);
    QString line;
    int loadedCount = 0;
    while (!in.atEnd()) {
        line = in.readLine();
        try {
            Contact contact = Contact::from_file_string(line.toStdString());
            contacts.push_back(contact);
            loadedCount++;
        }
        catch (const exception& e) {
            qWarning() << "Failed to parse contact from line:" << line
                << "Error:" << e.what();
            continue;
        }
    }
    file.close();
    qDebug() << "Loaded" << loadedCount << "contacts from"
        << QString::fromStdString(filename);
}

void ManageContacts::save_to_file() {
    QFile file(QString::fromStdString(filename));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return;
    }
    QTextStream out(&file);

    for (size_t i = 0; i < contacts.size(); i++) {
        out << QString::fromStdString(contacts[i].to_file_string()) << "\n";
    }
    file.close();
}

bool ManageContacts::add_contact(const Contact& contact) {
    if (!contact.required_fields()) {
        return false;
    }
    contacts.push_back(contact);
    save_to_file();
    return true;
}

bool ManageContacts::remove_contact(int index) {
    if (index < 0 || (size_t)index >= contacts.size()) {
        return false;
    }

    contacts.erase(contacts.begin() + index);
    save_to_file();
    return true;
}

bool ManageContacts::update_contact(int index, const Contact& contact) {
    if (index < 0 || (size_t)index >= contacts.size() || !contact.required_fields()) {
        return false;
    }

    contacts[index] = contact;
    save_to_file();
    return true;
}

Contact* ManageContacts::get_contact(int index) {
    if (index < 0 || (size_t)index >= contacts.size()) {
        return nullptr;
    }
    return &contacts[index];
}

vector<Contact*> ManageContacts::find_contacts(const string& search_query) {
    vector<Contact*> results;
    string lower_query = to_lower(search_query);

    for (size_t i = 0; i < contacts.size(); i++) {
        string fname = to_lower(contacts[i].get_fname());
        string lname = to_lower(contacts[i].get_lname());
        string email = to_lower(contacts[i].get_email());

        if (fname.find(lower_query) != string::npos ||
            lname.find(lower_query) != string::npos ||
            email.find(lower_query) != string::npos) {
            results.push_back(&contacts[i]);
        }
    }
    return results;
}

int ManageContacts::get_contact_count() const {
    return (int)contacts.size();
}

void ManageContacts::sort_by_field(const string& field) {
    for (size_t i = 0; i < contacts.size() - 1; i++) {
        for (size_t j = 0; j < contacts.size() - i - 1; j++) {
            bool need_swap = false;

            if (field == "first_name") {
                need_swap = contacts[j].get_fname() > contacts[j + 1].get_fname();
            }
            else if (field == "last_name") {
                need_swap = contacts[j].get_lname() > contacts[j + 1].get_lname();
            }
            else if (field == "email") {
                need_swap = contacts[j].get_email() > contacts[j + 1].get_email();
            }
            else if (field == "address") {  //
                need_swap = contacts[j].get_address() > contacts[j + 1].get_address();
            }
            if (need_swap) {
                Contact temp = contacts[j];
                contacts[j] = contacts[j + 1];
                contacts[j + 1] = temp;
            }
        }
    }
}

void ManageContacts::print_all() const {
    cout << "Contacts: " << contacts.size() << endl;
    cout << "=====================" << endl;

    for (size_t i = 0; i < contacts.size(); i++) {
        cout << "Contact " << i + 1 << ":" << endl;
        cout << "  Name: " << contacts[i].get_fname() << " " << contacts[i].get_lname();

        if (!contacts[i].get_surname().empty()) {
            cout << " " << contacts[i].get_surname();
        }

        cout << endl;
        cout << "  Address: " << contacts[i].get_address() << endl;
        cout << "  Birth date: " << contacts[i].get_birthday() << endl;
        cout << "  Email: " << contacts[i].get_email() << endl;
        cout << "  Phone number(s): ";

        for (size_t j = 0; j < contacts[i].get_countPhone(); j++) {
            cout << contacts[i].get_phoneNumber((int)j) << " ";
        }

        cout << endl << "------------------" << endl;
    }
}
