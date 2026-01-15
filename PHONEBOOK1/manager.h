
#pragma once
#include <QString>
#include <QFile>
#include <QDebug>
#include "contact.h"
#include <string>
#include <vector>
#include <fstream>

using namespace std;

class ManageContacts {
private:
    vector<Contact> contacts;
    string filename;

    void load_from_file();
    void save_to_file();
    string to_lower(const string& str);
public:
    ManageContacts();
    ManageContacts(const string& file);
    ManageContacts(const ManageContacts& m);
    ~ManageContacts();

    bool add_contact(const Contact& contact);
    bool remove_contact(int index);
    bool update_contact(int index, const Contact& contact);

    Contact* get_contact(int index); //
    vector<Contact*> find_contacts(const string& search_query); //

    void sort_by_field(const string& field); //

    int get_contact_count() const; //
    void print_all() const; //
};
