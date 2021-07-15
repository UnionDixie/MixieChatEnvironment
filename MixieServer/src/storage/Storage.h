#pragma once

#include <QMap>
#include <QString>
#include <QStringList>
#include <QtDebug>

#include "src/sql/SqlWrapper.h"

class Storage
{
public:
	Storage();
	~Storage() = default;
	void write(QString sender, QString receiver, QString message);
	QStringList get(QString sender, QString receiver);
	void erase(QString user);
	void saveDB();
private:
	SqlWrapper dbWrapper;
	QMap<QString, QMap<QString, QStringList>> hashDB;
};

