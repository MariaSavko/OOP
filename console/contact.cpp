#include "contact.h"
#include <stdexcept>     // для invalid_argument
#include <sstream> //для работы со строками


using namespace std;

//конструктор по умолчанию
Contact::Contact() {}

//конструктор с параметрами
Contact::Contact(const string& first_name, const string& last_name, const string& email, const string& phone) {
	set_fname(first_name);
	set_lname(last_name);
	set_email(email);
	add_phone(phone);
}

//конструктор копирования
Contact::Contact(const Contact& c) {
	first_name = c.first_name;
	last_name = c.last_name;
	surname = c.surname;
	address = c.address;
	birthday = c.birthday;
	email = c.email;
	phoneNumbers = c.phoneNumbers;
}

Contact& Contact::operator=(const Contact& c) { //перегружаем оператор = для класса Contact
	if (this != &c) { //защита от самоприсваивания
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

//сеттеры с проверкой
void Contact::set_fname(const string& fname) {
	string error; //пустая строка для хранения сообщения об ошибке
	if (!Validator::valid_name(fname, error)) {
		throw invalid_argument("Неккорректная запись имени: " + error); //исключение при ошибке
	}
	first_name = Validator::whitespace(fname);
}
void Contact::set_lname(const string& lname) {
	string error;
	if (!Validator::valid_name(lname, error)) {
		throw invalid_argument("Неккорректная запись имени: " + error);
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
		throw invalid_argument("Неккорректная запись имени: " + error);
	}
	surname = Validator::whitespace(sname);
}
void Contact::set_email(const string& mail) {
	string error;
	if (!Validator::valid_email(mail, error)) {
		throw invalid_argument("Некорректная запись email " + error);
	}
	email = Validator::whitespace(mail);
}
void Contact::set_birthday(const string& birth) {
	string error;
	if (!Validator::valid_birthday(birth, error)) {
		throw invalid_argument("Некорректная запись даты рождения: " + error);
	}
	birthday = Validator::whitespace(birth);
}
void Contact::set_address(const string& addr) { //нет требований к формату адреса, поэтому нет проверки
	address = Validator::whitespace(addr);
}

//геттеры
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
string Contact::get_phoneNumber(int i) const {
	if (i >= 0 and i < phoneNumbers.size()) { //если индекс валиден - возвращаем телефон, если нет - пустую строку
		return phoneNumbers[i];
	}
	else {
		return "";
	}
}
int Contact::get_countPhone() const { //возвращает текущее количество телефонных номеров у контакта
	return phoneNumbers.size();
}

//для телефонов
bool Contact::add_phone(const string& phone) {
	string error;
	if (!Validator::valid_phone(phone, error)) {
		return false;
	}
	string cleanedPhone = Validator::whitespace(phone); //очищает от пробелов
	phoneNumbers.push_back(cleanedPhone); //сохраняем в массив
	return true;
}

bool Contact::remove_phone(int i) {
	if (i < 0 or i >= phoneNumbers.size()) {
		return false;
	}
	phoneNumbers.erase(phoneNumbers.begin() + i);
	return true;
}

bool Contact::required_fields() const {
	return !(first_name == "") and !(last_name == "") and !(email == "") and !phoneNumbers.empty();
}


string Contact::to_file_string() const { //преобразуем данные контакта в одну строку
	string result = first_name + ";" + last_name + ";" + surname + ";" +
		address + ";" + birthday + ";" + email + ";" + to_string(phoneNumbers.size());

	for (int i = 0; i < phoneNumbers.size(); i++) { //для номеров телефонов
		result += ";" + phoneNumbers[i];
	}
	return result;
}
Contact Contact::from_file_string(const string& data) { //создает объект из строки файла
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
		throw invalid_argument("Некорректная запись данных контакта");
	}

	Contact contact;
	contact.first_name = fields[0];    
	contact.last_name = fields[1];    
	contact.surname = fields[2];    
	contact.address = fields[3];     
	contact.birthday = fields[4];      
	contact.email = fields[5];       
	
	int phone_count = stoi(fields[6]);
	for (int i = 0; i < phone_count and i + 7 < fields.size(); i++) {
		contact.phoneNumbers.push_back(fields[i+7]);
	}

	return contact;
}