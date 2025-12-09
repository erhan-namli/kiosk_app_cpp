#ifndef EMAILMANAGER_H
#define EMAILMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include "AppSettings.h"

class EmailManager : public QObject {
    Q_OBJECT

public:
    explicit EmailManager(AppSettings* settings, QObject* parent = nullptr);
    ~EmailManager();

    bool validateEmail(const QString& email) const;
    bool sendScans(const QString& recipientEmail, const QStringList& scanPaths);
    bool saveScansLocally(const QStringList& scanPaths, const QString& backupDir);

signals:
    void emailSent();
    void emailFailed(const QString& errorMessage);

private:
    AppSettings* m_settings;

    QString buildEmailBody(int scanCount) const;
    bool sendViaSmtp(const QString& to, const QString& subject,
                     const QString& body, const QStringList& attachments);
};

#endif // EMAILMANAGER_H
