#include "databasemanager.h"
#include <QDebug>
#include <QSqlError>

DatabaseManager::DatabaseManager()
{
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::openDatabase(const QString& path)
{
    if (m_database.isOpen())
        return true;

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(path);

    if (!m_database.open()) {
        qDebug() << "Error opening database:" << m_database.lastError().text();
        return false;
    }

    qDebug() << "Database connected:" << path;
    return true;
}

QSqlDatabase& DatabaseManager::database()
{
    return m_database;
}
