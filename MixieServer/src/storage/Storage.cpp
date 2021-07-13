#include "Storage.h"

void Storage::write(QString sender, QString receiver, QString message)
{
    hashDB[sender][receiver].push_back("@" + message);
    if(sender != receiver)
        hashDB[receiver][sender].push_back(message);
}

QStringList Storage::get(QString sender, QString receiver)
{
    return hashDB[sender][receiver];
}
