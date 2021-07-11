#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QByteArray>

void logErr(const QByteArray& data){
    QFile file;
    file.setFileName("logJson.json");
    if (file.open(QIODevice::Append | QFile::Text)) {
           file.write(data);
           file.write("\n");
       }
    file.close();
}


#endif // LOGGER_H
