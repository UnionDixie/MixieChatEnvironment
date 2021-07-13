#pragma once

#include <QMap>
#include <QString>
#include <QStringList>

class Storage
{
public:
	Storage() = default;
	~Storage() = default;
	void write(QString sender, QString receiver, QString message);
	QStringList get(QString sender, QString receiver);
private:
	QMap<QString, QMap<QString, QStringList>> hashDB;
};

