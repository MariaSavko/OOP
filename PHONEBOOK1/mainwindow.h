
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include "manager.h"
#include "dbmanager.h"



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddContact();
    void onEditContact();
    void onDeleteContact();
    void onSearchTextChanged(const QString &text);

    void onSortByFirstName();
    void onSortByLastName();
    void onSortByEmail();
    void onSortByAddress();

    void onToggleStorage();

private:

    QTableWidget *contactsTable;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QLineEdit *searchEdit;
    QPushButton *sortFirstNameButton;
    QPushButton *sortLastNameButton;
    QPushButton *sortEmailButton;
    QPushButton *sortAddressButton;

    QPushButton *toggleStorageButton;

    ManageContacts phoneBook;
    DBManager *dbManager;

    bool useDatabase;            // true = используем БД, false = используем файл
    bool databaseAvailable;      // доступна ли БД

    void updateTable();
    void setupUI();  // Новая функция для создания интерфейса

    // Метод для получения текущего менеджера
    ManageContacts* getCurrentManager();

    // Метод для работы с обоими хранилищами
    //void addToBothStorages(const Contact &contact);
   // void updateInBothStorages(int index, const Contact &contact);
    //void deleteFromBothStorages(int index);
};

#endif // MAINWINDOW_H
