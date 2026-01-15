
#include "contactdialog.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

ContactDialog::ContactDialog(QWidget* parent) :
    QDialog(parent),
    editingMode(false)
{
    setupUI();
    setWindowTitle("Add Contact");
}

ContactDialog::ContactDialog(Contact* contact, QWidget* parent) :
    QDialog(parent),
    editingMode(true)
{
    if (contact) {
        currentContact = *contact;
    }
    setupUI();
    loadContactData(currentContact);
    setWindowTitle("Edit Contact");
}

ContactDialog::~ContactDialog()
{
}

void ContactDialog::setupUI()
{
    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Contact fields form
    QFormLayout* formLayout = new QFormLayout();

    // Create widgets
    firstNameEdit = new QLineEdit(this);
    lastNameEdit = new QLineEdit(this);
    surnameEdit = new QLineEdit(this);
    emailEdit = new QLineEdit(this);
    birthdayEdit = new QDateEdit(this);
    birthdayEdit->setDisplayFormat("dd.MM.yyyy");
    birthdayEdit->setDate(QDate(2000, 1, 1));
    birthdayEdit->setMaximumDate(QDate::currentDate());
    birthdayEdit->setCalendarPopup(true);
    birthdayEdit->setMinimumDate(QDate(1900, 1, 1));

    addressEdit = new QTextEdit(this);
    addressEdit->setMaximumHeight(80);

    // Add fields to form
    formLayout->addRow("First Name*:", firstNameEdit);
    formLayout->addRow("Last Name*:", lastNameEdit);
    formLayout->addRow("Surname:", surnameEdit);
    formLayout->addRow("Email*:", emailEdit);
    formLayout->addRow("Birth Date:", birthdayEdit);
    formLayout->addRow("Address:", addressEdit);

    // Phone numbers group
    QGroupBox* phoneGroup = new QGroupBox("Phone Numbers", this);
    QVBoxLayout* phoneLayout = new QVBoxLayout(phoneGroup);

    // New phone input field
    QHBoxLayout* newPhoneLayout = new QHBoxLayout();
    phoneEdit = new QLineEdit(this);
    phoneEdit->setPlaceholderText("Enter phone number");

    // ComboBox for phone type selection
    phoneTypeCombo = new QComboBox(this);
    phoneTypeCombo->addItem("Mobile", static_cast<int>(Contact::MOBILE));
    phoneTypeCombo->addItem("Home", static_cast<int>(Contact::HOME));
    phoneTypeCombo->addItem("Work", static_cast<int>(Contact::WORK));
    phoneTypeCombo->addItem("Office", static_cast<int>(Contact::OFFICE));
    phoneTypeCombo->addItem("Other", static_cast<int>(Contact::OTHER));
    phoneTypeCombo->setCurrentIndex(0);

    addPhoneButton = new QPushButton("+", this);
    addPhoneButton->setFixedWidth(30);
   // addPhoneButton->setStyleSheet("font-size: 16px; font-weight: bold;");
    connect(addPhoneButton, &QPushButton::clicked, this, &ContactDialog::onAddPhoneClicked);

    newPhoneLayout->addWidget(phoneEdit);
    newPhoneLayout->addWidget(phoneTypeCombo);
    newPhoneLayout->addWidget(addPhoneButton);

    // Phone list
    phonesList = new QListWidget(this);
    phonesList->setMaximumHeight(100);

    // Remove phone button
    removePhoneButton = new QPushButton("Remove Phone", this);
    connect(removePhoneButton, &QPushButton::clicked, this, &ContactDialog::onRemovePhoneClicked);

    phoneLayout->addLayout(newPhoneLayout);
    phoneLayout->addWidget(phonesList);
    phoneLayout->addWidget(removePhoneButton);

    // Dialog buttons
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this
    );
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ContactDialog::onDialogAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ContactDialog::reject);

    // Assemble everything
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(phoneGroup);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    resize(500, 550);
}

void ContactDialog::loadContactData(const Contact& contact)
{
    firstNameEdit->setText(QString::fromStdString(contact.get_fname()));
    lastNameEdit->setText(QString::fromStdString(contact.get_lname()));
    surnameEdit->setText(QString::fromStdString(contact.get_surname()));
    emailEdit->setText(QString::fromStdString(contact.get_email()));
    QString birthdayStr = QString::fromStdString(contact.get_birthday());
        if (!birthdayStr.isEmpty()) {
            QDate date = QDate::fromString(birthdayStr, "dd.MM.yyyy");
            if (date.isValid()) {
                birthdayEdit->setDate(date);
            } else {
                birthdayEdit->setDate(QDate(2000, 1, 1)); // Дата по умолчанию при ошибке
            }
        } else {
            birthdayEdit->setDate(QDate(2000, 1, 1)); // Пустая дата - устанавливаем по умолчанию
        }
    addressEdit->setText(QString::fromStdString(contact.get_address()));

    // Load phones
    phonesList->clear();
    for (size_t i = 0; i < contact.get_countPhone(); ++i) {
        QString phone = QString::fromStdString(contact.get_phoneNumber(i));
        QString type = QString::fromStdString(
            Contact::phoneTypeToString(contact.get_phone_type(i)));
        QString itemText = QString("%1 (%2)").arg(phone).arg(type);

        QListWidgetItem* item = new QListWidgetItem(itemText, phonesList);
        // Сохраняем тип в user data
        item->setData(Qt::UserRole, static_cast<int>(contact.get_phone_type(i)));
    }
}

void ContactDialog::onAddPhoneClicked()
{
    QString phone = phoneEdit->text().trimmed();
    if (!phone.isEmpty()) {
        int typeIndex = phoneTypeCombo->currentIndex();
        Contact::PhoneType type = static_cast<Contact::PhoneType>(
            phoneTypeCombo->itemData(typeIndex).toInt());

        //
        QString typeStr = QString::fromStdString(Contact::phoneTypeToString(type));
        QString itemText = QString("%1 (%2)").arg(phone).arg(typeStr);

        //
        QListWidgetItem* item = new QListWidgetItem(itemText, phonesList);
        item->setData(Qt::UserRole, static_cast<int>(type));

        phoneEdit->clear();
        phoneEdit->setFocus();
    }
}

void ContactDialog::onRemovePhoneClicked()
{
    QListWidgetItem* item = phonesList->currentItem();
    if (item) {
        delete phonesList->takeItem(phonesList->row(item));
    }
}

bool ContactDialog::validateForm()
{
    //
    if (firstNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "First Name field is required");
        firstNameEdit->setFocus();
        return false;
    }
    if (lastNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Last Name field is required");
        lastNameEdit->setFocus();
        return false;
    }
    if (emailEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Error", "Email field is required");
        emailEdit->setFocus();
        return false;
    }
    if (phonesList->count() == 0) {
        QMessageBox::warning(this, "Error", "Add at least one phone number");
        phoneEdit->setFocus();
        return false;
    }

    return true;
}

void ContactDialog::onDialogAccepted()
{
    if (!validateForm()) {
        return;
    }

    try {
        // Create contact from form data
        Contact contact;

        contact.set_fname(firstNameEdit->text().trimmed().toStdString());
        contact.set_lname(lastNameEdit->text().trimmed().toStdString());
        contact.set_surname(surnameEdit->text().trimmed().toStdString());
        contact.set_email(emailEdit->text().trimmed().toStdString());
        QDate date = birthdayEdit->date();
                if (date.isValid()) {
                    contact.set_birthday(date.toString("dd.MM.yyyy").toStdString());
                } else {
                    contact.set_birthday(""); // Пустая строка если дата невалидна
                }
        contact.set_address(addressEdit->toPlainText().trimmed().toStdString());

        // Add phones from list
        for (int i = 0; i < phonesList->count(); ++i) {
            QListWidgetItem* item = phonesList->item(i);
            QString itemText = item->text();

            //
            QString phone = itemText.split('(').first().trimmed();

            //
            Contact::PhoneType type = static_cast<Contact::PhoneType>(
                item->data(Qt::UserRole).toInt());
            if (!phone.isEmpty()) {
                            contact.add_phone(phone.toStdString(), type);
                        }
                    }

                    // If contact is valid, save it
                    currentContact = contact;
                    accept();
                }
                catch (const std::exception& e) {
                    QMessageBox::warning(this, "Error", QString("Invalid data: %1").arg(e.what()));
                }
            }

            Contact ContactDialog::getContact() const
            {
                return currentContact;
            }
