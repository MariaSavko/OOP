#include "validator.h"
#include <cctype>
#include <locale>
using namespace std;
//устанавливаем локаль
namespace {
	struct LocaleInitializer {
		LocaleInitializer() {
			try {
				locale::global(locale("ru_RU.UTF-8"));
			}
			catch (...) {
				// если не получилось установить русскую локаль, используем системную по умолчанию
				locale::global(locale(""));
			}
		}
	} localeInitializer;
}
string Validator::whitespace(const string& str) {
	size_t start = str.find_first_not_of(" \t\n\r"); //" \t\n\r" это пробел, табуляция, перенос строки
	if (start == string::npos) //если все символы пробелы, возвращаем пустую строку
		return "";
	size_t end = str.find_last_not_of(" \t\n\r"); //ищет последниц символ которыц не пробел
	return str.substr(start, end - start + 1); //вырезаем нужную часть
}

bool Validator::valid_name(const string& name, string& error_message) {
	if (name == "") {
		error_message = "Имя не может быть пустым";
		return false;
	}
	string cleanedStr = whitespace(name);
	const string restrictions = "!@#$%^&*()+=[]{}|\?/<>,:;'~`";
	for (char c : cleanedStr) {
		if (restrictions.find(c) != string::npos) {
			error_message = "Недопустимые символы";
			return false;
		}
	}
	// дополнительные проверки
	if (cleanedStr[0] == '-' or cleanedStr[0] == ' ') {
		error_message = "Имя должно начинаться с буквы";
		return false;
	}

	if (cleanedStr.back() == '-' || cleanedStr.back() == ' ') {
		error_message = "Имя не может заканчиваться дефисом или пробелом";
		return false;
	}

	// проверка двух дефисов или пробелов подряд

	for (int i = 0; i < cleanedStr.length() - 1; i++) {
		if ((cleanedStr[i] == '-' and cleanedStr[i + 1] == '-') or
			(cleanedStr[i] == ' ' and cleanedStr[i + 1] == ' ')) {
			error_message = "Не должно быть двух дефисов или пробелов подряд";
			return false;
		}
	}

	return true;
}

bool Validator::valid_email(const string& email, string& error_message) {
	if (email == "") {
		error_message = "Email не может быть пустым";
		return false;
	}
	string cleanedStr = whitespace(email);
	
	regex pattern("^[a-zA-Z0-9]+@[a-zA-Z0-9]+\\.[a-zA-Z]{2,}$"); //разрешены: латинские буквы, цифры. @ - обязательно
	
	if (!regex_match(cleanedStr, pattern)) {
		error_message = "Некорректная запись email";
		return false;
	}
	return true;
}

bool Validator::valid_phone(const string& phone, string& error_message) {
	if (phone == "") {
		error_message = "Номер телефона не может быть пустым";
		return false;
	}
	string cleanedStr = whitespace(phone);

	regex pattern("^(\\+7|8)[\\s\\(\\)\\-]*\\d{3}[\\s\\(\\)\\-]*\\d{3}[\\s\\-]*\\d{2}[\\s\\-]*\\d{2}$"); //первая часть для форматов с дефисами, вторая - без дефисов
	if (!regex_match(cleanedStr, pattern)){
		error_message = "Некорректная запись номера телефона";
		return false;
	}
	return true;
}
bool Validator::leap_year(int year) { //високосный год или нет
	return (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0);
}

int Validator::days_in_month(int month, int year) { //сколько дней в месяце
	 //массив дней в месяцах (невисокосный год)
	int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (month < 1 or month > 12) {
		return 0;
	}
	//февраль для високосного года
	if (month == 2 and leap_year(year)) {
		return 29;
	}

	return days[month];
}
bool Validator::date_str(const string& date_str, int& day, int& month, int& year) {
	if (date_str.length() != 10 or  date_str[2] != '.' or date_str[5] != '.') //проверка формата записи
		return false;
	 //проверка что  символы - цифры (не считая точек)
	for (int i = 0; i < 10; i++) {
		if (i != 2 and i != 5 and !isdigit(date_str[i])) {
			return false;
		}
	}

	day = (date_str[0] - '0') * 10 + (date_str[1] - '0'); 
	month = (date_str[3] - '0') * 10 + (date_str[4] - '0');
	year = (date_str[6] - '0') * 1000 + (date_str[7] - '0') * 100 +
		(date_str[8] - '0') * 10 + (date_str[9] - '0');

	return true;
}
bool Validator::valid_date(int year, int month, int day) {
	if (year < 1900 or year > 2100) {
		return false;
	}
	if (month < 1 or month > 12) {
		return false;
	}
	if (day < 1 or day > days_in_month(month, year)) {
		return false;
	}

	// Дата рождения должна быть меньше текущей даты
	time_t now = time(0); //получили текущее время
	tm current_time;
	localtime_s(&current_time, &now);


	int current_year = current_time.tm_year + 1900;
	int current_month = current_time.tm_mon+ 1;
	int current_day = current_time.tm_mday;

	if (year > current_year) {
		return false;
	}
	if (year == current_year and month > current_month) {
		return false;
	}
	if (year == current_year and month == current_month and day > current_day) {
		return false;
	}
	return true;
}
bool Validator::valid_birthday(const string& birth, string& error_message) {
	if (birth == "")
		return true;


	string cleanedStr = whitespace(birth);

	regex date_pattern("^\\d{2}\\.\\d{2}\\.\\d{4}$");
	if (!regex_match(cleanedStr, date_pattern)) {
		error_message = "Неверный формат записи даты. Запишите в виде ДД.ММ.ГГГГ";
		return false;
	}

	int day, month, year;
	if (!date_str(cleanedStr, day, month, year)) {
		error_message = "Недопустимые значения";
		return false;
	}

	if (!valid_date(year, month, day)) {
		error_message = "Недопустимые значения. Возможно, дата рождения указана в будущем";
		return false;
	}
	return true;
}