
#include "mainwindow.h"
#include "contactdialog.h"
#include "contact.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QHeaderView>
#include <QStatusBar>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , phoneBook("phonebook.txt")
    , dbManager(nullptr)
    , useDatabase(false)
    , databaseAvailable(false)

{



    dbManager = new DBManager(this);

    setWindowTitle("Phonebook (File + PostgreSQL)");
    resize(1650, 600);
    // Создание UI
    setupUI();
    // Попытка подключения к БД
    databaseAvailable = dbManager->initDatabase();

        // Обновляем текст кнопки
    if (toggleStorageButton) {
        QString buttonText = databaseAvailable ? "Switch to DB" : "DB not available";
        toggleStorageButton->setText(buttonText);
        toggleStorageButton->setEnabled(databaseAvailable);
    }

    // Обновление таблицы
    updateTable();
}
MainWindow::~MainWindow()
{
    delete dbManager;
}

void MainWindow::setupUI()
{
    // Создаем центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    //this->setStyleSheet(
      //      "QMainWindow {"
        //    "    background-color: #f8e1f4;"  // очень светло-розовый
          //  "}"
         //   ""
          //  "QPushButton {"
           // "    background-color: #f8c8dc;"  // пастельно-розовый
       //     "    color: #6d2c4d;"
       //     "    border-radius: 8px;"
       //    "    padding: 6px 14px;"
       //     "    font-weight: bold;"
       //     "    border: 1px solid #e893c3;"
       //     "}"
       //     ""
       //     "QPushButton:hover {"
         //   "    background-color: #f0b0d0;"
        //    "    border: 1px solid #d8a0c0;"
          //  "}"
            //""
         //   "QPushButton:pressed {"
         //   "    background-color: #e8a0c8;"
         //   "}"
         //   ""
         //   "QLineEdit, QTextEdit {"
         //   "    background-color: #fef5fd;"  // почти белый с розовым оттенком
         //   "    border: 1px solid #e893c3;"
         //   "    border-radius: 6px;"
         //   "    padding: 4px;"
         //   "}"
         //   ""
         //   "QTableWidget {"
         //   "    background-color: white;"
         //   "    alternate-background-color: #fef5fd;"
         //   "    gridline-color: #f4afd3;"
         //   "    border: 1px solid #e893c3;"
         //   "    border-radius: 6px;"
         //   "}"
         //   ""
         //   "QTableWidget::item:selected {"
         //   "    background-color: #f4afd3;"
         //   "    color: #5d0032;"
         //  "}"
         //   ""
         //   "QHeaderView::section {"
         //   "    background-color: #f4afd3;"
         //   "    color: #5d0032;"
         //   "    padding: 6px;"
         //   "    border: 1px solid #e893c3;"
          //  "}"
  //  );
    // Основной вертикальный layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // === 1. СОЗДАЕМ ПАНЕЛЬ С КНОПКАМИ И ПОИСКОМ ===
    QHBoxLayout *topLayout = new QHBoxLayout();
    QString buttonText = databaseAvailable ? "Switch to DB" : "DB not available";
    toggleStorageButton = new QPushButton(buttonText, this);
    toggleStorageButton->setEnabled(databaseAvailable);

    // Создаем кнопки
    addButton = new QPushButton("Add", this);
    editButton = new QPushButton("Edit", this);
    deleteButton = new QPushButton("Delete", this);

    // Кнопки сортировки
    sortFirstNameButton = new QPushButton("Sort by First Name", this);
    sortLastNameButton = new QPushButton("Sort by Last Name", this);
    sortEmailButton = new QPushButton("Sort by Email", this);
    sortAddressButton = new QPushButton("Sort by Address", this);



    // Создаем поле поиска
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search by name, surname, address or email...");
    searchEdit->setClearButtonEnabled(true);

    // Добавляем элементы в горизонтальный layout
    topLayout->addWidget(toggleStorageButton);
    topLayout->addWidget(addButton);
    topLayout->addWidget(editButton);
    topLayout->addWidget(deleteButton);
    topLayout->addWidget(sortFirstNameButton);
    topLayout->addWidget(sortLastNameButton);
    topLayout->addWidget(sortEmailButton);
    topLayout->addWidget(sortAddressButton);
    topLayout->addWidget(searchEdit);

    // === 2. СОЗДАЕМ ТАБЛИЦУ ===
    contactsTable = new QTableWidget(this);

    // Настраиваем таблицу
    contactsTable->setColumnCount(8);  // 6 столбцов
    QStringList headers = {"First Name", "Last Name", "Surname", "Email", "Phone", "Type","Birth date", "Address"};
    contactsTable->setHorizontalHeaderLabels(headers);

    // Настройка поведения
    contactsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    contactsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    contactsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Не редактируем напрямую
    contactsTable->setAlternatingRowColors(true);  // Чередование цветов строк

    // Настраиваем ширину столбцов
    contactsTable->horizontalHeader()->setStretchLastSection(true);
    contactsTable->verticalHeader()->setVisible(false);  // Скрываем нумерацию строк

    // === 3. СОБИРАЕМ ВСЁ ВМЕСТЕ ===
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(contactsTable);

    // === 4. ПОДКЛЮЧАЕМ СИГНАЛЫ И СЛОТЫ ===
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddContact);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::onEditContact);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteContact);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    connect(sortFirstNameButton, &QPushButton::clicked, this, &MainWindow::onSortByFirstName);
    connect(sortLastNameButton, &QPushButton::clicked, this, &MainWindow::onSortByLastName);
    connect(sortEmailButton, &QPushButton::clicked, this, &MainWindow::onSortByEmail);
    connect(sortAddressButton, &QPushButton::clicked,
                this, &MainWindow::onSortByAddress);
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(toggleStorageButton, &QPushButton::clicked, this, &MainWindow::onToggleStorage);
    // Двойной клик по строке таблицы
    connect(contactsTable, &QTableWidget::cellDoubleClicked, [this](int, int)  {
        this->onEditContact();  // Редактируем при двойном клике
    });
    // Статус бар для информации о хранилище
    statusBar()->showMessage(useDatabase ? "Using PostgreSQL Database" : "Using File Storage");
}

void MainWindow::updateTable()
{
    // Очищаем таблицу
    contactsTable->setRowCount(0);
    ManageContacts* manager = &phoneBook;

    // Получаем все контакты
    for (int i = 0; i < manager->get_contact_count(); i++) {
        Contact* contact = manager->get_contact(i);
        if (!contact) continue;

        // Добавляем новую строку
        int row = contactsTable->rowCount();
        contactsTable->insertRow(row);

        // Заполняем ячейки
        contactsTable->setItem(row, 0,
            new QTableWidgetItem(QString::fromStdString(contact->get_fname())));
        contactsTable->setItem(row, 1,
            new QTableWidgetItem(QString::fromStdString(contact->get_lname())));
        contactsTable->setItem(row, 2,
            new QTableWidgetItem(QString::fromStdString(contact->get_surname())));
        contactsTable->setItem(row, 3,
            new QTableWidgetItem(QString::fromStdString(contact->get_email())));

        // Первый телефон (если есть)
        QString phone = "no phones";
        QString typeStr = "";
        if (contact->get_countPhone() > 0) {
                    phone = QString::fromStdString(contact->get_phoneNumber(0));
                    typeStr = QString::fromStdString(
                        Contact::phoneTypeToString(contact->get_phone_type(0)));
                }
        contactsTable->setItem(row, 4, new QTableWidgetItem(phone));
        contactsTable->setItem(row, 5, new QTableWidgetItem(typeStr));

        // Дата рождения
        contactsTable->setItem(row, 6,
                               new QTableWidgetItem(QString::fromStdString(contact->get_birthday())));
                           //
                           QString address = QString::fromStdString(contact->get_address());
                           if (address.isEmpty()) {
                               address = "-"; // Если адрес пустой, показываем прочерк
                           }
                           contactsTable->setItem(row, 7, new QTableWidgetItem(address));
                       }
                       // Обновляем статус
                       QString storageInfo = useDatabase ? "Database" : "File";
                       QString contactCount = QString::number(manager->get_contact_count());
                       statusBar()->showMessage(QString("%1 Storage | Contacts: %2").arg(storageInfo).arg(contactCount));
                   }

                   void MainWindow::onAddContact()
                   {
                       ContactDialog dialog(this);

                       if (dialog.exec() == QDialog::Accepted) {
                           Contact newContact = dialog.getContact();

                           bool fileSuccess = phoneBook.add_contact(newContact);
                           bool dbSuccess = true;

                           if (databaseAvailable) {
                               dbSuccess = dbManager->addContact(newContact);
                           }

                           if (fileSuccess) {
                               QString message = "Contact added to file storage";
                               if (databaseAvailable) {
                                   message += dbSuccess ? " and database" : " (database failed)";
                               }
                               QMessageBox::information(this, "Success", message);
                               updateTable();
                           } else {
                               QMessageBox::warning(this, "Error", "Failed to add contact");
                           }
                       }
                   }

                   void MainWindow::onEditContact()
                   {
                       int row = contactsTable->currentRow();
                       if (row < 0) {
                           QMessageBox::warning(this, "Error", "Select contact to edit");
                           return;
                       }

                       Contact* contact = phoneBook.get_contact(row);
                       if (!contact) return;

                       ContactDialog dialog(contact, this);

                       if (dialog.exec() == QDialog::Accepted) {
                           Contact updatedContact = dialog.getContact();
                           bool fileSuccess = phoneBook.update_contact(row, updatedContact);
                           bool dbSuccess = true;
                           if (databaseAvailable) {
                               // Для простоты: обновляем по индексу (в реальном приложении нужен ID)
                               // Здесь упрощенный подход
                               QList<Contact> dbContacts = dbManager->getAllContacts();
                               if (row < dbContacts.size()) {
                                   // Получаем ID из БД
                                   dbSuccess = dbManager->updateContact(row + 1, updatedContact);
                               }
                           }

                           if (fileSuccess) {
                               QString message = "Contact updated in file storage";
                               if (databaseAvailable) {
                                   message += dbSuccess ? " and database" : " (database update failed)";
                               }
                               QMessageBox::information(this, "Success", message);
                               updateTable();
                           } else {
                               QMessageBox::warning(this, "Error", "Failed to update contact");
                           }
                       }
                   }
                   void MainWindow::onSearchTextChanged(const QString &text)
                   {
                       // Простой поиск - скрываем строки, которые не соответствуют
                       QString searchText = text.toLower().trimmed();;
                       const QVector<int> searchColumns = {0, 1, 2, 3, 7};
                       for (int i = 0; i < contactsTable->rowCount(); i++) {
                           bool match = searchText.isEmpty();;

                           // Проверяем первые 4 столбца (имя, фамилия, отчество, email)
                           if (!searchText.isEmpty()) {
                               for (int col : searchColumns) {
                                   QTableWidgetItem *item = contactsTable->item(i, col);
                                   if (item && item->text().toLower().contains(searchText)) {
                                       match = true;
                                       break;
                                   }
                               }
                           }

                           // Показываем/скрываем строку
                           contactsTable->setRowHidden(i, !match);
                       }
                   }
                   void MainWindow::onDeleteContact()
                   {
                       int row = contactsTable->currentRow();
                       if (row < 0) {
                           QMessageBox::warning(this, "Error", "Select contact to delete");
                           return;
                       }

                       // Подтверждение удаления
                       QMessageBox::StandardButton reply;
                       reply = QMessageBox::question(this, "Delete Contact",
                           "Are you sure you want to delete selected contact?",
                           QMessageBox::Yes | QMessageBox::No);

                       if (reply == QMessageBox::Yes) {
                           bool fileSuccess = phoneBook.remove_contact(row);
                           bool dbSuccess = true;

                       if (databaseAvailable) {
                               // Удаляем из БД по индексу (упрощенно)
                           dbSuccess = dbManager->deleteContact(row + 1);
                       }
                       if (fileSuccess) {
                           QString message = "Contact deleted from file storage";
                           if (databaseAvailable) {
                               message += dbSuccess ? " and database" : " (database delete failed)";
                           }
                           QMessageBox::information(this, "Success", message);
                           updateTable();
                       } else {
                           QMessageBox::warning(this, "Error", "Failed to delete contact");
                           }
                       }

                   }
                   void MainWindow::onSortByFirstName() {
                       phoneBook.sort_by_field("first_name");
                       updateTable();
                   }

                   void MainWindow::onSortByLastName() {
                       phoneBook.sort_by_field("last_name");
                       updateTable();
                   }

                   void MainWindow::onSortByEmail() {
                       phoneBook.sort_by_field("email");
                       updateTable();
                   }

                   void MainWindow::onSortByAddress(){
                       phoneBook.sort_by_field("address");
                       updateTable();
                   }
                   void MainWindow::onToggleStorage()
                   {
                       if (!databaseAvailable) {
                           QMessageBox::warning(this, "Error", "Database is not available!");
                           return;
                       }

                       useDatabase = !useDatabase;
                       if (useDatabase) {
                           toggleStorageButton->setText("Switch to File");
                           // Можно предложить миграцию данных
                           QMessageBox::StandardButton reply;
                           reply = QMessageBox::question(this, "Migration",
                                   "Migrate data from file to database?\n\n"
                                   "Yes: Copy file data to database\n"
                                   "No: Use empty database",
                           QMessageBox::Yes | QMessageBox::No);

                           if (reply == QMessageBox::Yes) {
                               dbManager->migrateFromFile("phonebook.txt");
                           }
                           QMessageBox::information(this, "Switched", "Now using PostgreSQL Database");
                       } else {
                           toggleStorageButton->setText("Switch to DB");
                           QMessageBox::information(this, "Switched", "Now using File Storage");
                       }

                       updateTable();
                   }
