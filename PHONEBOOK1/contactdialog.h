#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QComboBox>
#include <QDateEdit>

#include "contact.h"

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    // Constructor for adding a new contact
    explicit ContactDialog(QWidget* parent = nullptr);
    // Constructor for editing an existing contact
    explicit ContactDialog(Contact* contact, QWidget* parent = nullptr);
    ~ContactDialog();

    Contact getContact() const;

private slots:
    void onAddPhoneClicked();
    void onRemovePhoneClicked();
    void onDialogAccepted();

private:
    // Widgets
    QLineEdit* firstNameEdit;
    QLineEdit* lastNameEdit;
    QLineEdit* surnameEdit;
    QLineEdit* emailEdit;
    QDateEdit* birthdayEdit;
    QTextEdit* addressEdit;
    QLineEdit* phoneEdit;
    QComboBox* phoneTypeCombo;
    QListWidget* phonesList;
    QPushButton* addPhoneButton;
    QPushButton* removePhoneButton;

    Contact currentContact;
    bool editingMode = false;

    void setupUI();
    void loadContactData(const Contact& contact);
    bool validateForm();
};

#endif // CONTACTDIALOG_H
