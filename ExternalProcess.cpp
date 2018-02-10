#include "ExternalProcess.hpp"
#include <QDir>
#include <QDebug>


ConverterThread::ConverterThread(QList < QString > list, QString progName, bool isVideo) :
    trackList(list),
    convertVideo(isVideo)
{
    programName = findFile(progName);
}
QByteArray ConverterThread::findFile(QString targetFile)
{
    QDir currentDir = QDir::current();

    for (int i = 0; i < currentDir.count(); ++i)
    {
        QString filename = currentDir[i];

        if (filename.contains(targetFile))
        {
            if (!filename.contains(".exe"))
                filename.prepend("./");
            qDebug() << "FIleName: " << filename.toLatin1().data();

            return filename.toLatin1();
        }
    }
    return "";
}

void ConverterThread::convert()
{
    for (int i = 0; i < this->trackList.size() - 1; ++i)
    {
        QString outFile = "Downloads/" +  this->trackList.at(i+1) + ".wav";
        qDebug() << outFile;
        convertProcess = new QProcess();

        args.clear();

        if (convertVideo)
            args << "-vn";

        args << "-i";
        args <<  this->trackList.at(i);
        args << outFile;

        connect(convertProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
                    this, SLOT(convertStatus(int,QProcess::ExitStatus)));

        qDebug() <<args;
        qDebug() <<programName;
        convertProcess->start(programName, args);
    }
}
void ConverterThread::convertStatus(int code, QProcess::ExitStatus status)
{
    qDebug() << "Converted with exit code: " << code;
    qDebug() << "Output: " << convertProcess->readAllStandardError();
    processCount++;
    if (this->trackList.length() - 1 == processCount)
        delete this;
}

BurnerThread::BurnerThread(QList < QString > list, QString progName) :
    ConverterThread(list, progName, false)
{
    process = new QProcess();

    args << "-scanbus";
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(extractDevId()));
    process->start(programName, args);
}
void BurnerThread::startBurn(QString trackName)
{
    QProcess *burnProcess = new QProcess();

    if (args.length() > 5)
        args.removeLast();

    args.append(trackName);

    connect(burnProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
    [=](int exitCode, QProcess::ExitStatus exitStatus){
    qDebug() << "Output: " << burnProcess->readAllStandardError();
    emit changeProgress();
    processCount++;
    if (this->trackList.length() == processCount)
        delete this;
    });

    burnProcess->start(programName, args);

}
void BurnerThread::extractDevId()
{
    qDebug() << "extract called";

    QString output(process->readAllStandardOutput());
    QString devId;

    QStringList lines = output.split("\n", QString::SkipEmptyParts);

    for (int i = 2; i < lines.size(); i++)
    {
        if (lines.at(i).contains("DVD") || lines.at(i).contains("CD"))
        {
            QRegExp expression("[0-9],[0-9],[0-9]");
            if (expression.indexIn(lines.at(i)) != -1)
            {
                qDebug() << expression.cap(0);
                devId = expression.cap(0);
            }
        }
    }

    args.clear();

    qDebug() << "The dev id is: " << devId;
    args << "dev=" + devId;
    args << "-audio";
    args << "-pad";
    args << "-tao";
    args << "-multi";

    for (int i = 0; i < this->trackList.size(); ++i)
        startBurn(this->trackList.at(i));
}

