
#include "dbmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "manager.h"

DBManager::DBManager(QObject *parent)
    : QObject(parent)
{
    // Создаем соединение с уникальным именем
    m_database = QSqlDatabase::addDatabase("QPSQL", "phonebook_connection");
}

DBManager::~DBManager()
{
    if (m_database.isOpen()) {
        m_database.close();
    }
    QSqlDatabase::removeDatabase("phonebook_connection");
}

bool DBManager::initDatabase()
{
    // Настройки подключения
    m_database.setHostName("localhost");
    m_database.setPort(5432);
    m_database.setDatabaseName("phonebook_db");
    m_database.setUserName("postgres");
    m_database.setPassword("1932");

    if (!m_database.open()) {
        qCritical() << "Cannot open database:" << m_database.lastError().text();
        qCritical() << "Using file storage only";
        return false;
    }

    qDebug() << "Database connected successfully!";

    if (!createTables()) {
        qCritical() << "Failed to create tables";
        return false;
    }

    return true;
}

bool DBManager::isDatabaseAvailable() const
{
    return m_database.isOpen();
}

bool DBManager::createTables()
{
    QSqlQuery query(m_database);

    // Таблица контактов
    QString createContactsTable =
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id SERIAL PRIMARY KEY,"
        "first_name VARCHAR(100) NOT NULL,"
        "last_name VARCHAR(100) NOT NULL,"
        "surname VARCHAR(100),"
        "address TEXT,"
        "birthday VARCHAR(10),"
        "email VARCHAR(100) NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createContactsTable)) {
        qCritical() << "Failed to create contacts table:" << query.lastError().text();
        return false;
    }

    // Таблица телефонов
    QString createPhonesTable =
        "CREATE TABLE IF NOT EXISTS phone_numbers ("
        "id SERIAL PRIMARY KEY,"
        "contact_id INTEGER NOT NULL REFERENCES contacts(id) ON DELETE CASCADE,"
        "phone_number VARCHAR(20) NOT NULL,"
        "phone_type INTEGER NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    if (!query.exec(createPhonesTable)) {
        qCritical() << "Failed to create phone_numbers table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database tables created successfully";
    return true;
}

bool DBManager::addContact(const Contact &contact)
{
    if (!m_database.isOpen()) return false;

    QSqlQuery query(m_database);
    m_database.transaction();

    try {
        // Вставляем контакт
        query.prepare("INSERT INTO contacts (first_name, last_name, surname, address, birthday, email) "
                      "VALUES (:first_name, :last_name, :surname, :address, :birthday, :email) "
                      "RETURNING id");

        bindContactToQuery(query, contact);

        if (!query.exec() || !query.next()) {
            m_database.rollback();
            qCritical() << "Failed to insert contact:" << query.lastError().text();
            return false;
        }

        int contactId = query.value(0).toInt();

        // Вставляем телефоны
        for (size_t i = 0; i < contact.get_countPhone(); ++i) {
            query.prepare("INSERT INTO phone_numbers (contact_id, phone_number, phone_type) "
                          "VALUES (:contact_id, :phone_number, :phone_type)");
            query.bindValue(":contact_id", contactId);
            query.bindValue(":phone_number", QString::fromStdString(contact.get_phoneNumber(i)));
            query.bindValue(":phone_type", static_cast<int>(contact.get_phone_type(i)));

            if (!query.exec()) {
                m_database.rollback();
                qCritical() << "Failed to insert phone:" << query.lastError().text();
                return false;
            }
        }

        m_database.commit();
        return true;

    } catch (const std::exception &e) {
        m_database.rollback();
        qCritical() << "Exception in addContact:" << e.what();
        return false;
    }
}

bool DBManager::updateContact(int id, const Contact &contact)
{
    if (!m_database.isOpen()) return false;

    QSqlQuery query(m_database);
    m_database.transaction();

    try {
        // Обновляем контакт
        query.prepare("UPDATE contacts SET "
                      "first_name = :first_name, "
                      "last_name = :last_name, "
                      "surname = :surname, "
                      "address = :address, "
                      "birthday = :birthday, "
                      "email = :email, "
                      "updated_at = CURRENT_TIMESTAMP "
                      "WHERE id = :id");
        bindContactToQuery(query, contact);
        query.bindValue(":id", id);

        if (!query.exec()) {
            m_database.rollback();
            qCritical() << "Failed to update contact:" << query.lastError().text();
            return false;
        }

        // Удаляем старые телефоны
        query.prepare("DELETE FROM phone_numbers WHERE contact_id = :contact_id");
        query.bindValue(":contact_id", id);

        if (!query.exec()) {
            m_database.rollback();
            qCritical() << "Failed to delete old phones:" << query.lastError().text();
            return false;
        }

        // Добавляем новые телефоны
        for (size_t i = 0; i < contact.get_countPhone(); ++i) {
            query.prepare("INSERT INTO phone_numbers (contact_id, phone_number, phone_type) "
                          "VALUES (:contact_id, :phone_number, :phone_type)");
            query.bindValue(":contact_id", id);
            query.bindValue(":phone_number", QString::fromStdString(contact.get_phoneNumber(i)));
            query.bindValue(":phone_type", static_cast<int>(contact.get_phone_type(i)));

            if (!query.exec()) {
                m_database.rollback();
                qCritical() << "Failed to insert phone:" << query.lastError().text();
                return false;
            }
        }

        m_database.commit();
        return true;

    } catch (const std::exception &e) {
        m_database.rollback();
        qCritical() << "Exception in updateContact:" << e.what();
        return false;
    }
}

bool DBManager::deleteContact(int id)
{
    if (!m_database.isOpen()) return false;

    QSqlQuery query(m_database);
    query.prepare("DELETE FROM contacts WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Failed to delete contact:" << query.lastError().text();
        return false;
    }

    return query.numRowsAffected() > 0;
}

QList<Contact> DBManager::getAllContacts()
{
    QString queryStr =
        "SELECT c.*, "
        "string_agg(p.phone_number, ',') as phone_numbers, "
        "string_agg(p.phone_type::text, ',') as phone_types "
        "FROM contacts c "
        "LEFT JOIN phone_numbers p ON c.id = p.contact_id "
        "GROUP BY c.id "
        "ORDER BY c.last_name, c.first_name";

    return executeContactQuery(queryStr);
}

Contact DBManager::getContactById(int id)
{
    QString queryStr =
        "SELECT c.*, "
        "string_agg(p.phone_number, ',') as phone_numbers, "
        "string_agg(p.phone_type::text, ',') as phone_types "
        "FROM contacts c "
        "LEFT JOIN phone_numbers p ON c.id = p.contact_id "
        "WHERE c.id = :id "
        "GROUP BY c.id";

    QVariantList params;
    params << id;

    QList<Contact> contacts = executeContactQuery(queryStr, params);
    return contacts.isEmpty() ? Contact() : contacts.first();
}

QList<Contact> DBManager::searchContacts(const QString &searchText)
{
    QString queryStr =
        "SELECT DISTINCT c.*, "
        "string_agg(p.phone_number, ',') as phone_numbers, "
        "string_agg(p.phone_type::text, ',') as phone_types "
        "FROM contacts c "
        "LEFT JOIN phone_numbers p ON c.id = p.contact_id "
        "WHERE c.first_name ILIKE :pattern "
        "OR c.last_name ILIKE :pattern "
        "OR c.surname ILIKE :pattern "
        "OR c.email ILIKE :pattern "
        "OR c.address ILIKE :pattern "
        "OR p.phone_number ILIKE :pattern "
        "GROUP BY c.id "
        "ORDER BY c.last_name, c.first_name";

    QVariantList params;
    params << QString("%%1%").arg(searchText);

    return executeContactQuery(queryStr, params);
}

bool DBManager::migrateFromFile(const QString &filename)
{
    ManageContacts fileManager(filename.toStdString());
    int contactCount = fileManager.get_contact_count();

    if (contactCount == 0) {
        qDebug() << "No contacts to migrate";
        return true;
    }

    m_database.transaction();

    int migrated = 0;
    for (int i = 0; i < contactCount; ++i) {
        Contact* contact = fileManager.get_contact(i);
        if (contact) {
            if (addContact(*contact)) {
                migrated++;
            }
        }
    }

    if (migrated == contactCount) {
        m_database.commit();
        qDebug() << "Successfully migrated" << migrated << "contacts";
        return true;
    } else {
        m_database.rollback();
        qCritical() << "Failed to migrate all contacts. Rolled back.";
        return false;
    }
}

bool DBManager::backupToFile(const QString &filename)
{
    QList<Contact> contacts = getAllContacts();

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Cannot open file for backup:" << filename;
        return false;
    }

    QTextStream out(&file);
    for (const Contact &contact : contacts) {
        out << QString::fromStdString(contact.to_file_string()) << "\n";
    }

    file.close();
    qDebug() << "Backup completed. Saved" << contacts.size() << "contacts to" << filename;
    return true;
}

int DBManager::getLastContactId()
{
    QSqlQuery query(m_database);
    query.prepare("SELECT MAX(id) FROM contacts");

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return 0;
}

QList<Contact> DBManager::executeContactQuery(const QString &queryStr, const QVariantList &params)
{
    QList<Contact> contacts;

    if (!m_database.isOpen()) {
        return contacts;
    }

    QSqlQuery query(m_database);
    query.prepare(queryStr);

    for (int i = 0; i < params.size(); ++i) {
        query.bindValue(i, params[i]);
    }

    if (!query.exec()) {
        qCritical() << "Query failed:" << query.lastError().text();
        return contacts;
    }

    while (query.next()) {
        contacts.append(contactFromQuery(query));
    }

    return contacts;
}

Contact DBManager::contactFromQuery(const QSqlQuery &query)
{
    Contact contact;

    try {
        contact.set_fname(query.value("first_name").toString().toStdString());
        contact.set_lname(query.value("last_name").toString().toStdString());

        QString surname = query.value("surname").toString();
        if (!surname.isEmpty()) {
            contact.set_surname(surname.toStdString());
        }

        QString address = query.value("address").toString();
        if (!address.isEmpty()) {
            contact.set_address(address.toStdString());
        }

        QString birthday = query.value("birthday").toString();
        if (!birthday.isEmpty()) {
            contact.set_birthday(birthday.toStdString());
        }

        contact.set_email(query.value("email").toString().toStdString());

        // Обрабатываем телефоны
        QString phonesStr = query.value("phone_numbers").toString();
        QString typesStr = query.value("phone_types").toString();

        if (!phonesStr.isEmpty() && !typesStr.isEmpty()) {
            QStringList phones = phonesStr.split(',');
            QStringList types = typesStr.split(',');

            for (int i = 0; i < phones.size() && i < types.size(); ++i) {
                if (!phones[i].isEmpty()) {
                    Contact::PhoneType type = static_cast<Contact::PhoneType>(
                        types[i].toInt());
                    contact.add_phone(phones[i].toStdString(), type);
                }
            }
        }
    }
    catch (const std::exception &e) {
        qCritical() << "Error creating contact from query:" << e.what();
    }

    return contact;
}

void DBManager::bindContactToQuery(QSqlQuery &query, const Contact &contact)
{
    query.bindValue(":first_name", QString::fromStdString(contact.get_fname()));
    query.bindValue(":last_name", QString::fromStdString(contact.get_lname()));
    query.bindValue(":surname", QString::fromStdString(contact.get_surname()));
    query.bindValue(":address", QString::fromStdString(contact.get_address()));
    query.bindValue(":birthday", QString::fromStdString(contact.get_birthday()));
    query.bindValue(":email", QString::fromStdString(contact.get_email()));
}
