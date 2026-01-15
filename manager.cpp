#include "manager.h"
#include <iostream>
#include <cctype> //для tolower (преобрзование символа в нижний регистр) и isdigit

using namespace std;

ManageContacts::ManageContacts() {
	filename = "phonebook.txt";
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
	for (int i = 0; i < low_str.length(); i++){
		low_str[i] = tolower(low_str[i]);
	}
	return low_str;
}
void ManageContacts::load_from_file() {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Не получилось открыть файл " << filename << endl;
		return;
	}
	string line;
	//обработка исключений
	while (getline(file, line)) {
		try {
			Contact contact = Contact::from_file_string(line);
			contacts.push_back(contact);
		}
		catch (const exception& e) {
			cout << "Ошибка: " << e.what() << endl;
		}
	}
	file.close();
}
void ManageContacts::save_to_file() {
	ofstream file(filename);
	if (!file.is_open()) {
		cout << "Не получилось сохранить в файл " << filename << endl;
		return;
	}
	for (int i = 0; i < contacts.size(); i++) {
		file << contacts[i].to_file_string() << endl; //преобразует контакт в строку
	}
	file.close();
}
bool ManageContacts::add_contact(const Contact& contact) {
	if (!contact.required_fields()) { //проверяем обязательные поля
		return false;
	}
	contacts.push_back(contact);
	save_to_file();
	return true;
}

bool ManageContacts::remove_contact(int index) {
	if (index < 0 or index >= contacts.size()) {
		return false;
	}
	contacts.erase(contacts.begin() + index); //удаление элемента из вектора по индексу
	save_to_file();
	return true;
}
bool ManageContacts::update_contact(int index, const Contact& contact) {
	if (index < 0 or index >= contacts.size() or !contact.required_fields()) {
		return false;
	}
	contacts[index] = contact;
	save_to_file();
	return true;
}

//доступ к конкретному контакту по его номеру в телефонной книге
Contact* ManageContacts::get_contact(int index) {
	if (index < 0 or index >= contacts.size()) {
		return nullptr;
	}
	return &contacts[index];
}
vector<Contact*>ManageContacts::find_contacts(const string& search_query) {
	vector<Contact*> results;
	string lower_query = to_lower(search_query);
	for (int i = 0; i < contacts.size(); i++) {
		string fname = to_lower(contacts[i].get_fname());
		string lname = to_lower(contacts[i].get_lname());
		string email = to_lower(contacts[i].get_email());

		if (fname.find(lower_query) != string::npos or lname.find(lower_query) != string::npos or email.find(lower_query)!=string::npos){
			results.push_back(&contacts[i]);
		}
	}
	return results;
}
int ManageContacts::get_contact_count() const {
	return contacts.size();
}
void ManageContacts::sort_by_field(const string& field) { //пузырьковая сортировка
	for (int i = 0; i < contacts.size() - 1; i++) {
		for (int j = 0; j < contacts.size() - i - 1; j++) {
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

			if (need_swap) {
				Contact temp = contacts[j];
				contacts[j] = contacts[j + 1];
				contacts[j + 1] = temp;
			}
		}
	}
}

void ManageContacts::print_all() const {
	cout << "ТЕЛЕФОННАЯ КНИГА" << endl;
	cout << "----------------" << endl;
	for (int i = 0; i < contacts.size(); i++) {
		cout << "Контакт " << i + 1 << ":" << endl;
		cout << "ФИО: " << contacts[i].get_fname() << " " << contacts[i].get_lname();
		if (!contacts[i].get_surname().empty()) {
			cout << " " << contacts[i].get_surname();
	}
	cout << endl;
	cout << "  Адрес: " << contacts[i].get_address() << endl;
	cout << "  Дата рождения: " << contacts[i].get_birthday() << endl;
	cout << "  Email: " << contacts[i].get_email() << endl;
	cout << "  Телефон(ы): ";
	for (int j = 0; j < contacts[i].get_countPhone(); j++) {
		cout << contacts[i].get_phoneNumber(j) << " ";
	}
	cout << endl << "------------------" << endl;
}
}
