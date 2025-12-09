#include "EmailManager.h"
#include <QRegularExpression>
#include <QFile>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <curl/curl.h>

EmailManager::EmailManager(AppSettings* settings, QObject* parent)
    : QObject(parent)
    , m_settings(settings)
{
    // Initialize CURL globally
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

EmailManager::~EmailManager() {
    curl_global_cleanup();
}

bool EmailManager::validateEmail(const QString& email) const {
    QRegularExpression emailRegex(
        "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"
    );
    return emailRegex.match(email).hasMatch();
}

bool EmailManager::sendScans(const QString& recipientEmail, const QStringList& scanPaths) {
    if (!validateEmail(recipientEmail)) {
        emit emailFailed("Invalid email address");
        return false;
    }

    qInfo() << "Sending" << scanPaths.count() << "scans to" << recipientEmail;

    QString subject = "Your Scanned Photos from SCAN.RN Kiosk";
    QString body = buildEmailBody(scanPaths.count());

    bool success = sendViaSmtp(recipientEmail, subject, body, scanPaths);

    if (success) {
        emit emailSent();
    } else {
        emit emailFailed("Failed to send email");
    }

    return success;
}

QString EmailManager::buildEmailBody(int scanCount) const {
    QString body = "Hello,\n\n";
    body += QString("Thank you for using our photo scanning kiosk!\n\n");
    body += QString("Attached %1 %2 your scanned photo%3.\n\n")
               .arg(scanCount == 1 ? "is" : "are")
               .arg(scanCount)
               .arg(scanCount == 1 ? "" : "s");
    body += "Best regards,\n";
    body += "SCAN.RN Team\n";

    return body;
}

bool EmailManager::sendViaSmtp(const QString& to, const QString& subject,
                                const QString& body, const QStringList& attachments) {
    // For simplicity, we'll use Python's email library via QProcess
    // Alternatively, you could use a C++ SMTP library like VMime or SmtpClient-for-Qt

    // Create a Python script to send email
    QString pythonScript = QString(
        "import smtplib\n"
        "from email.mime.multipart import MIMEMultipart\n"
        "from email.mime.text import MIMEText\n"
        "from email.mime.base import MIMEBase\n"
        "from email import encoders\n"
        "import sys\n"
        "\n"
        "smtp_server = '%1'\n"
        "smtp_port = %2\n"
        "sender_email = '%3'\n"
        "password = '%4'\n"
        "recipient = '%5'\n"
        "subject = '%6'\n"
        "body = '''%7'''\n"
        "\n"
        "msg = MIMEMultipart()\n"
        "msg['From'] = sender_email\n"
        "msg['To'] = recipient\n"
        "msg['Subject'] = subject\n"
        "msg.attach(MIMEText(body, 'plain'))\n"
        "\n"
    ).arg(m_settings->smtpServer)
     .arg(m_settings->smtpPort)
     .arg(m_settings->senderEmail)
     .arg(m_settings->emailPassword)
     .arg(to)
     .arg(subject)
     .arg(body);

    // Add attachments
    for (const QString& path : attachments) {
        pythonScript += QString(
            "with open('%1', 'rb') as f:\n"
            "    part = MIMEBase('application', 'octet-stream')\n"
            "    part.set_payload(f.read())\n"
            "    encoders.encode_base64(part)\n"
            "    part.add_header('Content-Disposition', 'attachment; filename=%2')\n"
            "    msg.attach(part)\n"
            "\n"
        ).arg(path).arg(QFileInfo(path).fileName());
    }

    pythonScript +=
        "try:\n"
        "    server = smtplib.SMTP(smtp_server, smtp_port)\n"
        "    server.starttls()\n"
        "    server.login(sender_email, password)\n"
        "    server.send_message(msg)\n"
        "    server.quit()\n"
        "    print('Email sent successfully')\n"
        "except Exception as e:\n"
        "    print(f'Error: {e}', file=sys.stderr)\n"
        "    sys.exit(1)\n";

    // Write script to temp file
    QString scriptPath = m_settings->logsDir.filePath("send_email.py");
    QFile scriptFile(scriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical() << "Failed to create email script";
        return false;
    }

    QTextStream out(&scriptFile);
    out << pythonScript;
    scriptFile.close();

    // Execute Python script
    QProcess process;
    process.start("python3", QStringList() << scriptPath);
    process.waitForFinished(30000); // 30 second timeout

    bool success = (process.exitCode() == 0);

    if (!success) {
        qCritical() << "Email send failed:" << process.readAllStandardError();
    } else {
        qInfo() << "Email sent successfully";
    }

    // Clean up script
    scriptFile.remove();

    return success;
}

bool EmailManager::saveScansLocally(const QStringList& scanPaths, const QString& backupDir) {
    QDir dir;
    if (!dir.mkpath(backupDir)) {
        qCritical() << "Failed to create backup directory:" << backupDir;
        return false;
    }

    qInfo() << "Saving" << scanPaths.count() << "scans to" << backupDir;

    for (const QString& sourcePath : scanPaths) {
        QFileInfo fileInfo(sourcePath);
        QString destPath = backupDir + "/" + fileInfo.fileName();

        if (!QFile::copy(sourcePath, destPath)) {
            qWarning() << "Failed to copy" << sourcePath << "to" << destPath;
            return false;
        }
    }

    qInfo() << "Scans saved locally to" << backupDir;
    return true;
}
