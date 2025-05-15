#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    QSqlDatabase& database();

    bool openDatabase(const QString& path);

private:
    DatabaseManager();
    QSqlDatabase m_database;
};

#endif // DATABASEMANAGER_H
