
#pragma once
#include <string>
#include <regex>  //
#include <ctime>

using namespace std;

class Validator {
private:

    //
    static bool valid_date(int year, int month, int day);
    static bool leap_year(int year); //
    static int days_in_month(int month, int year); //
    static bool date_str(const string& date_str, int& day, int& month, int& year); //
public:
    //
    static bool valid_name(const string& name, string& error_message); //
    static bool valid_email(const string& email, string& error_message);
    static bool valid_phone(const string& phone, string& error_message);
    static bool valid_birthday(const string& birth, string& error_message);

    static string whitespace(const string& str); //
};
