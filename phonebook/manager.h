#pragma once
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

	Contact* get_contact(int index); //получить доступ к контакту по индексу
	vector<Contact*> find_contacts(const string& search_query); //вектор указателей на найденные контакты

	void sort_by_field(const string& field); //сортировка отображаемых данных по указанному полю

	int get_contact_count() const; //получаем количество контактов в телефонной книге
	void print_all() const; //выврдим все контакты
};
