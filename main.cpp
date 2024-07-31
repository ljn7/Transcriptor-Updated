#include "tool.h"
#include <QApplication>
#include<QSettings>

const int MaxLogLines = 10000;

bool isTrimming = true;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (isTrimming)
        return;

    Q_UNUSED(context);

    QString dateTime = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
    QString dateTimeText = QString("[%1] ").arg(dateTime);

    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";

    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
        break;
    }

    QFile outFile("LogFile.log");
    outFile.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text);

    QTextStream textStream(&outFile);
    //Qt6
    // textStream.setCodec("UTF-8");
    textStream.setEncoding(QStringConverter::Utf8);

    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug:"},
                                            {QtInfoMsg, "Info:"},
                                            {QtWarningMsg, "Warning:"},
                                            {QtCriticalMsg, "Critical:"},
                                            {QtFatalMsg, "Fatal:"}}
                                           );
    QString logLevelName = msgLevelHash[type];
    QString logText = QString("%1 %2 %3 (%4)").arg(dateTimeText, logLevelName, msg,  context.file);

    textStream << logText << "\n";
}

void trimLogFile()
{
    QFile logFile("LogFile.log");
    if (!logFile.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qWarning("Failed to open log file for trimming.");
        isTrimming = false;
        return;
    }

    QStringList lines;
    QTextStream textStream(&logFile);
    while (!textStream.atEnd()) {
        lines.append(textStream.readLine());
    }

    int excessLines = lines.size() - MaxLogLines;
    if (excessLines > 0) {
        lines = lines.mid(excessLines);
        logFile.resize(0); // Clear the file content
        QTextStream out(&logFile);
        for (const QString &line : lines) {
            out << line << "\n"; // Write trimmed lines back to the file
        }
    }

    logFile.close();
    isTrimming = false;
}


int main(int argc, char *argv[])
{

    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);

    Tool w;
    w.show();

    trimLogFile();

    return a.exec();
}

