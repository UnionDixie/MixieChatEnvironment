#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QSqlRecord>

class SqlWrapper
{
public:
	SqlWrapper();
	~SqlWrapper() = default;
	void getAll(QMap<QString, QMap<QString, QStringList>>& hashDB);
	void saveAll(QMap<QString, QMap<QString, QStringList>>& hashDB);
private:
	QSqlDatabase db;
};

