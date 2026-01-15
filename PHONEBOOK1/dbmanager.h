
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include "contact.h"

class DBManager : public QObject
{
    Q_OBJECT

public:
    explicit DBManager(QObject *parent = nullptr);
    ~DBManager();

    bool initDatabase();
    bool isDatabaseAvailable() const;

    // Основные операции
    bool addContact(const Contact &contact);
    bool updateContact(int id, const Contact &contact);
    bool deleteContact(int id);
    QList<Contact> getAllContacts();
    Contact getContactById(int id);
    QList<Contact> searchContacts(const QString &searchText);

    // Миграция данных
    bool migrateFromFile(const QString &filename);
    bool backupToFile(const QString &filename);

    // Получение последнего ID
    int getLastContactId();

private:
    QSqlDatabase m_database;
    bool createTables();

    // Вспомогательные методы
    Contact contactFromQuery(const QSqlQuery &query);
    void bindContactToQuery(QSqlQuery &query, const Contact &contact);
    QList<Contact> executeContactQuery(const QString &queryStr,
                                       const QVariantList &params = QVariantList());
};

#endif // DBMANAGER_H
