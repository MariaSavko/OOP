#pragma once
#include <string>
#include <vector>

#include "validator.h"

using namespace std;

class Contact {
private:
	string first_name;
	string last_name;
	string surname;
	string address;
	string birthday;
	string email;
	vector<string> phoneNumbers;
	//string phoneNumbers[MAX_PHONES]; //массив для номеров телефонов
	//int countPhone; //текущее количество номеров телефонов
	
	
	//void shift_phone(int index); //Вспомогательный метод для сдвига элементов массива при удалении
	

public:
	//конструкторы
	Contact();
	Contact(const string& first_name, const string& last_name, const string &email, const string &phone);
	Contact(const Contact& c);
	Contact& operator=(const Contact& c); //чтобы работало присваивание контактов
	~Contact();

	//геттеры
	string get_fname() const;
	string get_lname() const;
	string get_surname() const;
	string get_address() const;
	string get_email() const;
	string get_birthday() const; //строка в формате  ДД.ММ.ГГГГ
	string get_phoneNumber(int i) const; //возвращает номер по индексу
	int get_countPhone() const; //возвращает количество телефонов

	//сеттеры
	void set_fname(const string& fname); //передаем по ссылке, чтобы избежать лишнего копирования
	void set_lname(const string& lname);
	void set_surname(const string& sname);
	void set_address(const string& addr);
	void set_email(const string& mail);
	void set_birthday(const string& birth);
	 
	//для телефонов
	bool add_phone(const string& phone); //возвращает true если добавление успешно, false если массив полон
	bool remove_phone(int i); //возвращает true если удаление успешно, false если неверный индекс
	
	

	string to_file_string() const; //упаковываем все поля объекта в одну строку для записи в файл
	static Contact from_file_string(const string& data); //превращает сохранённую строку из файла обратно в объект
	bool required_fields() const; //проверка обязательных полей
};




