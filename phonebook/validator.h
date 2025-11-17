#pragma once
#include <string>
#include <regex>  // для регулярных выражений
#include <ctime>

using namespace std;

class Validator {
private:

	//для проверки что дата рождения существует (проверяем, високосный ли год и тд)
	static bool valid_date(int year, int month, int day);
	static bool leap_year(int year); //високосный или нет
	static int days_in_month(int month, int year); //сколько дней в месяце
	static bool date_str(const string& date_str, int& day, int& month, int& year); //разбирает строку даты на компоненты
public:
	//проверка корректности данных
	static bool valid_name(const string& name, string& error_message); //ипсользуем static потому что правила валидации одинаковы для всех контактов
	static bool valid_email(const string& email, string& error_message);
	static bool valid_phone(const string& phone, string& error_message);
	static bool valid_birthday(const string& birth, string& error_message);

	static string whitespace(const string& str); //для удаления незначащих пробелов
};

