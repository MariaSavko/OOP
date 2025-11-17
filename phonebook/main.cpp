#include "manager.h"
#include <iostream>
#include <clocale>

using namespace std;

void menu() {
    cout << "------------МЕНЮ------------" << endl;
    cout << "1. Добавить контакт " << endl;
    cout << "2. Удалить контакт " << endl;
    cout << "3. Редактировать контакт " << endl;
    cout << "4. Список контактов " << endl;
    cout << "5. Поиск контактов " << endl;
    cout << "6. Сортировать контакты " << endl;
    cout << "7. Закрыть " << endl;
    cout << "Выберите опцию " << endl;
}

void print_contact(const Contact& contact) {
    cout << "ФИО: " << contact.get_fname() << " " << contact.get_lname();

    if (!contact.get_surname().empty()) {
        cout << " " << contact.get_surname();
    }
    cout << endl;
    cout << "Адрес: " << contact.get_address() << endl;
    cout << "Дата рождения: " << contact.get_birthday() << endl;
    cout << "Email: " << contact.get_email() << endl;
    cout << "Телефон(ы): ";
    for (int j = 0; j < contact.get_countPhone(); j++) {
        cout << contact.get_phoneNumber(j) << " ";
    }
    cout << endl;
}
Contact create_contact() {
    string fname;
    string lname;
    string sname;
    string address;
    string birthday;
    string email;
    string phone;
    do {
        cout << "ИМЯ (обязательно): ";
        getline(cin, fname);
    } while (fname == "");
    do {
        cout << "ФАМИЛИЯ (обязательно): ";
        getline(cin, lname);
    } while (lname == "");
    do {
        cout << "EMAIL: ";
        getline(cin, email);
    } while (email == "");
    do {
        cout << "ТЕЛЕФОН (обязательно): ";
        getline(cin, phone);
    } while (phone == "");
    
    cout << "ОТЧЕСТВО: ";
    getline(cin, sname);
    cout << "АДРЕС: ";
    getline(cin, address);
    cout << "ДАТА РОЖДЕНИЯ (ДД.ММ.ГГГГ): ";
    getline(cin, birthday);
    
    
   //создаем контакт с обязательными полями
    Contact contact(fname, lname, email, phone);
   //добавляем дополнительные поля
    if (!(sname == "")) {
        contact.set_surname(sname);
    }
    if (!(address == "")) {
        contact.set_address(address);
    }
    if (!(birthday == "")) {
        contact.set_birthday(birthday);
    }
    return contact;

}

int main()
{
    setlocale(LC_ALL, "Russian");
    ManageContacts phonebook;
    int option;
    do {
        menu();
        cin >> option;
        cin.ignore();
        switch (option) {
        case 1: {
            try {
                Contact contact = create_contact();
                if (phonebook.add_contact(contact)) {
                    cout << "Контакт добавлен" << endl;
                }
                else {
                    cout << "Ошибка при создании контакта " << endl;
                }
            }
            catch (const exception& e) {
                cout << "Ошибка: " << e.what() << endl;
            }
            break;
        }
        case 2: {
            int index;
            cout << "Введите индекс контакта, который нужно удалить: ";
            cin >> index;
            if (phonebook.remove_contact(index - 1)) {
                cout << "Контакт удалён " << endl;
            }
            else {
                cout << "Некорректный индекс!" << endl;
            }
            break;
        }
        case 3: {
            int index;
            cout << "Введите номер контакта, который нужно отредактировать: ";
            cin >> index;
            cin.ignore();
            Contact* contact = phonebook.get_contact(index - 1);
            if (contact) {
                cout << "Данные контакта: " << endl;
                print_contact(*contact);
                cout << "Введите новые данные: " << endl;
                try {
                    Contact new_contact = create_contact();
                    if (phonebook.update_contact(index - 1, new_contact)) {
                        cout << "Данные контакта изменены" << endl;
                    }
                    else {
                        cout << "Ошибка при изменении данных. Убедитесь, что обязательные поля заполнены" << endl;

                    }
                }
                catch (const exception& e) {
                    cout << "Ошибка: " << e.what() << endl;
                }
            }
            else {
                cout << "Некорректный индекс" << endl;

            }
            break;
        }
        case 4:
            phonebook.print_all();
            break;
        case 5: {
            string search;
            cout << "Поиск: ";
            getline(cin, search);
            vector<Contact*>results = phonebook.find_contacts(search);
            cout << "Результаты поиска: " << endl;
            for (int i = 0; i < results.size(); i++) {
                cout << i + 1 << ") "
                    << results[i]->get_fname() << " "
                    << results[i]->get_lname() << " "
                    << results[i]->get_email() << " "
                    << results[i]->get_phoneNumber(0) << endl;
            }
            break;
        }
        case 6: {
            string field;
            cout << "Сортировать по (имя, фамилия, email): ";
            getline(cin, field);
            phonebook.sort_by_field(field);
            cout << "Контакты отсортированы" << endl;
            break;
        }
        case 7:
            break;
        default:
            cout << "Некорректный номер опции" << endl;
        }
    } while (option != 7);
    return 0;
}




