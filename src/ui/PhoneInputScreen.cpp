#include "ui/PhoneInputScreen.h"
#include <QHBoxLayout>

PhoneInputScreen::PhoneInputScreen(QWidget* parent)
    : QWidget(parent)
    , m_phone("")
{
    setupUI();
}

void PhoneInputScreen::setupUI() {
    setStyleSheet("QWidget { background-color: #F9FAFB; }");

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("Enter Your Phone Number", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: #1F2937; font-size: 36px; font-weight: 600; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Phone display
    QWidget* displayWidget = new QWidget(this);
    displayWidget->setStyleSheet(
        "QWidget { background-color: white; border-radius: 16px; "
        "padding: 24px; max-width: 500px; }"
    );
    displayWidget->setMaximumWidth(500);

    QVBoxLayout* displayLayout = new QVBoxLayout(displayWidget);

    QLabel* labelText = new QLabel("Phone Number", displayWidget);
    labelText->setStyleSheet("QLabel { color: #6B7280; font-size: 16px; }");
    labelText->setAlignment(Qt::AlignCenter);

    m_phoneDisplayLabel = new QLabel("(___) ___-____", this);
    m_phoneDisplayLabel->setStyleSheet(
        "QLabel { color: #D1D5DB; font-size: 36px; font-weight: 500; "
        "min-height: 64px; }"
    );
    m_phoneDisplayLabel->setAlignment(Qt::AlignCenter);

    displayLayout->addWidget(labelText);
    displayLayout->addWidget(m_phoneDisplayLabel);

    // Number pad
    QWidget* padWidget = new QWidget(this);
    padWidget->setMaximumWidth(500);
    m_numberPad = new QGridLayout(padWidget);
    m_numberPad->setSpacing(16);

    // Create number buttons (1-9)
    for (int i = 1; i <= 9; i++) {
        QString num = QString::number(i);
        QPushButton* btn = new QPushButton(num, this);
        btn->setStyleSheet(
            "QPushButton { background-color: white; border-radius: 16px; "
            "font-size: 36px; font-weight: 500; min-height: 80px; "
            "border: none; } "
            "QPushButton:hover { background-color: #F3F4F6; } "
            "QPushButton:pressed { background-color: #E5E7EB; }"
        );
        connect(btn, &QPushButton::clicked, this, [this, num]() { onNumberClicked(num); });

        int row = (i - 1) / 3;
        int col = (i - 1) % 3;
        m_numberPad->addWidget(btn, row, col);
    }

    // Zero button (centered in row 3)
    QPushButton* zeroBtn = new QPushButton("0", this);
    zeroBtn->setStyleSheet(
        "QPushButton { background-color: white; border-radius: 16px; "
        "font-size: 36px; font-weight: 500; min-height: 80px; "
        "border: none; } "
        "QPushButton:hover { background-color: #F3F4F6; } "
        "QPushButton:pressed { background-color: #E5E7EB; }"
    );
    connect(zeroBtn, &QPushButton::clicked, this, [this]() { onNumberClicked("0"); });
    m_numberPad->addWidget(zeroBtn, 3, 0, 1, 3); // Span all 3 columns

    // Action buttons
    QHBoxLayout* actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(16);

    m_deleteButton = new QPushButton("Delete", this);
    m_deleteButton->setStyleSheet(
        "QPushButton { background-color: #EF4444; color: white; "
        "border-radius: 16px; font-size: 24px; min-height: 80px; "
        "border: none; } "
        "QPushButton:hover { background-color: #DC2626; } "
        "QPushButton:pressed { background-color: #B91C1C; }"
    );
    connect(m_deleteButton, &QPushButton::clicked, this, &PhoneInputScreen::onDeleteClicked);

    m_continueButton = new QPushButton("Continue", this);
    m_continueButton->setStyleSheet(
        "QPushButton { background-color: #2563EB; color: white; "
        "border-radius: 16px; font-size: 28px; font-weight: 600; "
        "min-height: 80px; border: none; } "
        "QPushButton:hover:enabled { background-color: #1D4ED8; } "
        "QPushButton:pressed:enabled { background-color: #1E40AF; } "
        "QPushButton:disabled { background-color: #D1D5DB; }"
    );
    m_continueButton->setEnabled(false);
    connect(m_continueButton, &QPushButton::clicked, this, &PhoneInputScreen::onContinueClicked);

    actionLayout->addWidget(m_deleteButton);
    actionLayout->addWidget(m_continueButton);

    // Add to main layout
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(displayWidget, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(padWidget);
    m_mainLayout->addLayout(actionLayout);
}

void PhoneInputScreen::onNumberClicked(const QString& number) {
    if (m_phone.length() < 10) {
        m_phone += number;
        m_phoneDisplayLabel->setText(formatPhone(m_phone));
        m_phoneDisplayLabel->setStyleSheet(
            "QLabel { color: #1F2937; font-size: 36px; font-weight: 500; "
            "min-height: 64px; }"
        );

        // Enable continue button when phone is complete
        m_continueButton->setEnabled(m_phone.length() == 10);
    }
}

void PhoneInputScreen::onDeleteClicked() {
    if (!m_phone.isEmpty()) {
        m_phone.chop(1);

        if (m_phone.isEmpty()) {
            m_phoneDisplayLabel->setText("(___) ___-____");
            m_phoneDisplayLabel->setStyleSheet(
                "QLabel { color: #D1D5DB; font-size: 36px; font-weight: 500; "
                "min-height: 64px; }"
            );
        } else {
            m_phoneDisplayLabel->setText(formatPhone(m_phone));
        }

        m_continueButton->setEnabled(m_phone.length() == 10);
    }
}

void PhoneInputScreen::onContinueClicked() {
    if (m_phone.length() == 10) {
        emit phoneSubmitted(m_phone);
    }
}

QString PhoneInputScreen::formatPhone(const QString& phone) const {
    if (phone.length() <= 3) {
        return phone;
    } else if (phone.length() <= 6) {
        return QString("(%1) %2").arg(phone.left(3)).arg(phone.mid(3));
    } else {
        return QString("(%1) %2-%3")
                   .arg(phone.left(3))
                   .arg(phone.mid(3, 3))
                   .arg(phone.mid(6));
    }
}

void PhoneInputScreen::reset() {
    m_phone.clear();
    m_phoneDisplayLabel->setText("(___) ___-____");
    m_phoneDisplayLabel->setStyleSheet(
        "QLabel { color: #D1D5DB; font-size: 36px; font-weight: 500; "
        "min-height: 64px; }"
    );
    m_continueButton->setEnabled(false);
}
