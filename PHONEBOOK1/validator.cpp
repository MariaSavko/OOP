
#include "validator.h"
#include <cctype>
#include <locale>
using namespace std;

string Validator::whitespace(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r"); //
    if (start == string::npos) //
        return "";
    size_t end = str.find_last_not_of(" \t\n\r"); //
    return str.substr(start, end - start + 1); //
}

bool Validator::valid_name(const string& name, string& error_message) {
    if (name == "") {
        error_message = "Name cannot be empty";
        return false;
    }
    string cleanedStr = whitespace(name);
    if (cleanedStr.empty()) {
        error_message = "Name cannot be empty after trimming";
        return false;
    }
    const string restrictions = "!@#$%^&*()+=[]{}|\?/<>,:;'~`";
    for (char c : cleanedStr) {
        if (restrictions.find(c) != string::npos) {
            error_message = "Invalid characters in name";
            return false;
        }
    }
    //
    if (cleanedStr[0] == '-' || cleanedStr[0] == ' ') {
        error_message = "Name must start with a letter";
        return false;
    }

    if (cleanedStr.back() == '-' || cleanedStr.back() == ' ') {
        error_message = "Name cannot end with hyphen or space";
        return false;
    }

    //

    for (size_t i = 0; i < cleanedStr.length() - 1; i++) {
        if ((cleanedStr[i] == '-' && cleanedStr[i + 1] == '-') ||
            (cleanedStr[i] == ' ' && cleanedStr[i + 1] == ' ')) {
            error_message = "Cannot have consecutive hyphens or spaces";
            return false;
        }
    }

    return true;
}

bool Validator::valid_email(const string& email, string& error_message) {
    if (email.empty()) {
        error_message = "Email cannot be empty";
        return false;
    }
    string cleanedStr = whitespace(email);

    regex pattern("^[a-zA-Z0-9]+@[a-zA-Z0-9]+\\.[a-zA-Z]{2,}$");

    if (!regex_match(cleanedStr, pattern)) {
        error_message = "Invalid email format.";
        return false;
    }
    return true;
}

bool Validator::valid_phone(const string& phone, string& error_message) {
    if (phone.empty()) {
        error_message = "Phone number cannot be empty";
        return false;
    }
    string cleanedStr = whitespace(phone);

    regex pattern("^(\\+7|8)[\\s\\(\\)\\-]*\\d{3}[\\s\\(\\)\\-]*\\d{3}[\\s\\-]*\\d{2}[\\s\\-]*\\d{2}$"); //
    if (!regex_match(cleanedStr, pattern)){
        error_message = "Invalid phone number format";
        return false;
    }
    return true;
}
bool Validator::leap_year(int year) { //
    return (year % 4 == 0 and year % 100 != 0) or (year % 400 == 0);
}

int Validator::days_in_month(int month, int year) { //
    int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if (month < 1 || month > 12) {
        return 0;
    }
    if (month == 2 && leap_year(year)) {
        return 29;
    }

    return days[month];
}
bool Validator::date_str(const string& date_str, int& day, int& month, int& year) {
    if (date_str.length() != 10 || date_str[2] != '.' || date_str[5] != '.')
        return false;

    for (int i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit(date_str[i])) {
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
    if (year < 1900 || year > 2100) {
        return false;
    }
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > days_in_month(month, year)) {
        return false;
    }

    //
    time_t now = time(0); //
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
    if (birth.empty())
        return true;


    string cleanedStr = whitespace(birth);

    regex date_pattern("^\\d{2}\\.\\d{2}\\.\\d{4}$");
    if (!regex_match(cleanedStr, date_pattern)) {
        error_message = "Date must be in format DD.MM.YYYY";
        return false;
    }

    return true;
}
