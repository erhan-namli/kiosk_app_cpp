#include "ui/ConfirmationScreen.h"

ConfirmationScreen::ConfirmationScreen(QWidget* parent)
    : QWidget(parent)
    , m_phoneNumber("")
{
    setupUI();
}

void ConfirmationScreen::setupUI() {
    // Green gradient background
    setStyleSheet(
        "ConfirmationScreen { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #10B981, stop:1 #059669); }"
    );

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(30);
    m_mainLayout->setAlignment(Qt::AlignCenter);

    // Success icon
    QLabel* iconLabel = new QLabel("✓", this);
    iconLabel->setStyleSheet(
        "QLabel { color: white; font-size: 120px; font-weight: bold; }"
    );
    iconLabel->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("All Done!", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: white; font-size: 48px; font-weight: 700; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Message
    m_messageLabel = new QLabel("Your scans have been sent via SMS", this);
    m_messageLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.95); font-size: 24px; }"
    );
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);

    // Phone number display
    m_phoneLabel = new QLabel("", this);
    m_phoneLabel->setStyleSheet(
        "QLabel { color: white; font-size: 32px; font-weight: 600; "
        "background-color: rgba(255, 255, 255, 0.2); "
        "border-radius: 12px; padding: 16px 32px; }"
    );
    m_phoneLabel->setAlignment(Qt::AlignCenter);

    // Additional info
    QLabel* infoLabel = new QLabel(
        "Check your phone for a text message\nwith your scanned photos",
        this
    );
    infoLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.9); font-size: 18px; "
        "margin-top: 20px; }"
    );
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);

    // Done button
    m_doneButton = new QPushButton("Done", this);
    m_doneButton->setMinimumHeight(80);
    m_doneButton->setMaximumWidth(400);
    m_doneButton->setStyleSheet(
        "QPushButton { background-color: white; color: #10B981; "
        "border-radius: 16px; font-size: 32px; font-weight: 700; "
        "border: none; margin-top: 30px; } "
        "QPushButton:hover { background-color: #F3F4F6; } "
        "QPushButton:pressed { background-color: #E5E7EB; }"
    );
    connect(m_doneButton, &QPushButton::clicked, this, &ConfirmationScreen::resetRequested);

    m_mainLayout->addWidget(iconLabel);
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_messageLabel);
    m_mainLayout->addWidget(m_phoneLabel, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(infoLabel);
    m_mainLayout->addWidget(m_doneButton, 0, Qt::AlignCenter);
}

void ConfirmationScreen::setPhoneNumber(const QString& phone) {
    m_phoneNumber = phone;
    m_phoneLabel->setText(formatPhone(phone));
}

QString ConfirmationScreen::formatPhone(const QString& phone) const {
    if (phone.length() == 10) {
        return QString("(%1) %2-%3")
                   .arg(phone.left(3))
                   .arg(phone.mid(3, 3))
                   .arg(phone.mid(6));
    }
    return phone;
}
