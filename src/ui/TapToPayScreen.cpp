#include "ui/TapToPayScreen.h"
#include <QTimer>

TapToPayScreen::TapToPayScreen(QWidget* parent)
    : QWidget(parent)
    , m_credits(1)
    , m_price(3)
{
    setupUI();
}

void TapToPayScreen::setupUI() {
    // Purple gradient background
    setStyleSheet(
        "TapToPayScreen { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #9333EA, stop:1 #7E22CE); }"
    );

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(30);
    m_mainLayout->setAlignment(Qt::AlignCenter);

    // Icon placeholder (could use QIcon with credit card icon)
    QLabel* iconLabel = new QLabel("💳", this);
    iconLabel->setStyleSheet(
        "QLabel { color: white; font-size: 96px; }"
    );
    iconLabel->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("Tap to Pay", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: white; font-size: 48px; font-weight: 700; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Payment info card
    QWidget* infoCard = new QWidget(this);
    infoCard->setStyleSheet(
        "QWidget { background-color: rgba(255, 255, 255, 0.1); "
        "border-radius: 16px; padding: 24px; max-width: 500px; }"
    );
    infoCard->setMaximumWidth(500);

    QVBoxLayout* cardLayout = new QVBoxLayout(infoCard);
    cardLayout->setSpacing(16);

    QLabel* amountHeaderLabel = new QLabel("Payment Amount", this);
    amountHeaderLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.8); font-size: 18px; }"
    );
    amountHeaderLabel->setAlignment(Qt::AlignCenter);

    m_amountLabel = new QLabel(QString("$%1.00").arg(m_price), this);
    m_amountLabel->setStyleSheet(
        "QLabel { color: white; font-size: 64px; font-weight: 700; }"
    );
    m_amountLabel->setAlignment(Qt::AlignCenter);

    m_creditsLabel = new QLabel(
        QString("%1 %2").arg(m_credits).arg(m_credits == 1 ? "Credit" : "Credits"),
        this
    );
    m_creditsLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.8); font-size: 18px; }"
    );
    m_creditsLabel->setAlignment(Qt::AlignCenter);

    cardLayout->addWidget(amountHeaderLabel);
    cardLayout->addWidget(m_amountLabel);
    cardLayout->addWidget(m_creditsLabel);

    // Instruction label
    m_instructionLabel = new QLabel("Hold your card or phone near the reader", this);
    m_instructionLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.9); font-size: 22px; }"
    );
    m_instructionLabel->setAlignment(Qt::AlignCenter);

    // Tap button (for demo/simulation)
    m_tapButton = new QPushButton("Tap Here to Pay", this);
    m_tapButton->setMinimumHeight(100);
    m_tapButton->setMaximumWidth(600);
    m_tapButton->setStyleSheet(
        "QPushButton { background-color: white; color: #9333EA; "
        "border-radius: 16px; font-size: 36px; font-weight: 700; "
        "border: none; } "
        "QPushButton:hover { background-color: #F3F4F6; } "
        "QPushButton:pressed { background-color: #E5E7EB; }"
    );
    connect(m_tapButton, &QPushButton::clicked, this, &TapToPayScreen::onTapClicked);

    // Security label
    QLabel* securityLabel = new QLabel("Payment is secure and encrypted", this);
    securityLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.8); font-size: 14px; }"
    );
    securityLabel->setAlignment(Qt::AlignCenter);

    m_mainLayout->addWidget(iconLabel);
    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(infoCard, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(m_instructionLabel);
    m_mainLayout->addWidget(m_tapButton);
    m_mainLayout->addWidget(securityLabel);
}

void TapToPayScreen::setPaymentInfo(int credits, int price) {
    m_credits = credits;
    m_price = price;

    m_amountLabel->setText(QString("$%1.00").arg(price));
    m_creditsLabel->setText(
        QString("%1 %2").arg(credits).arg(credits == 1 ? "Credit" : "Credits")
    );
}

void TapToPayScreen::onTapClicked() {
    // Simulate payment processing
    m_tapButton->setEnabled(false);
    m_tapButton->setText("Processing...");

    QTimer::singleShot(1500, this, [this]() {
        m_tapButton->setEnabled(true);
        m_tapButton->setText("Tap Here to Pay");
        emit paymentCompleted();
    });
}
