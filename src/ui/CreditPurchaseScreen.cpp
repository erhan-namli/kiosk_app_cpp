#include "ui/CreditPurchaseScreen.h"
#include <QHBoxLayout>

CreditPurchaseScreen::CreditPurchaseScreen(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void CreditPurchaseScreen::setupUI() {
    setStyleSheet("QWidget { background-color: #F9FAFB; }");

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("Purchase Credits", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: #1F2937; font-size: 36px; font-weight: 600; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Subtitle
    m_subtitleLabel = new QLabel("Select a package to continue", this);
    m_subtitleLabel->setStyleSheet(
        "QLabel { color: #6B7280; font-size: 18px; margin-bottom: 20px; }"
    );
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    // Create credit options
    QVBoxLayout* optionsLayout = new QVBoxLayout();
    optionsLayout->setSpacing(24);
    optionsLayout->setAlignment(Qt::AlignCenter);

    QPushButton* option1 = createCreditOption(1, 3, "Single scan", false);
    QPushButton* option4 = createCreditOption(4, 10, "Best value!", true);

    optionsLayout->addWidget(option1);
    optionsLayout->addWidget(option4);

    // Info label
    QLabel* infoLabel = new QLabel("Tap a package to purchase", this);
    infoLabel->setStyleSheet(
        "QLabel { color: #6B7280; font-size: 16px; margin-top: 20px; }"
    );
    infoLabel->setAlignment(Qt::AlignCenter);

    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_subtitleLabel);
    m_mainLayout->addLayout(optionsLayout);
    m_mainLayout->addWidget(infoLabel);
}

QPushButton* CreditPurchaseScreen::createCreditOption(int credits, int price,
                                                       const QString& description,
                                                       bool popular) {
    QPushButton* button = new QPushButton(this);
    button->setMinimumHeight(180);
    button->setMaximumWidth(600);

    QString baseStyle =
        "QPushButton { "
        "  background-color: white; "
        "  border: 4px solid transparent; "
        "  border-radius: 16px; "
        "  padding: 32px; "
        "  text-align: left; "
        "} "
        "QPushButton:hover { "
        "  background-color: #F9FAFB; "
        "  border-color: #BFDBFE; "
        "} "
        "QPushButton:pressed { "
        "  background-color: #F3F4F6; "
        "}";

    button->setStyleSheet(baseStyle);

    // Create layout for button content
    QVBoxLayout* buttonLayout = new QVBoxLayout(button);
    buttonLayout->setSpacing(8);

    // Popular badge
    if (popular) {
        QLabel* badge = new QLabel("MOST POPULAR", button);
        badge->setStyleSheet(
            "QLabel { "
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "    stop:0 #F97316, stop:1 #EA580C); "
            "  color: white; "
            "  font-size: 14px; "
            "  font-weight: 600; "
            "  padding: 4px 24px; "
            "  border-radius: 12px; "
            "}"
        );
        badge->setAlignment(Qt::AlignCenter);
        badge->setMaximumWidth(200);
        buttonLayout->addWidget(badge, 0, Qt::AlignCenter);
    }

    // Price row
    QHBoxLayout* priceRow = new QHBoxLayout();
    priceRow->setSpacing(12);

    QLabel* priceLabel = new QLabel(QString("$%1").arg(price), button);
    priceLabel->setStyleSheet(
        "QLabel { color: #1F2937; font-size: 54px; font-weight: 700; }"
    );

    QLabel* currencyLabel = new QLabel("USD", button);
    currencyLabel->setStyleSheet(
        "QLabel { color: #6B7280; font-size: 22px; }"
    );
    currencyLabel->setAlignment(Qt::AlignBottom);

    priceRow->addWidget(priceLabel);
    priceRow->addWidget(currencyLabel);
    priceRow->addStretch();

    // Credits label
    QString creditsText = QString("%1 %2")
                              .arg(credits)
                              .arg(credits == 1 ? "Credit" : "Credits");
    QLabel* creditsLabel = new QLabel(creditsText, button);
    creditsLabel->setStyleSheet(
        "QLabel { color: #1F2937; font-size: 28px; font-weight: 600; }"
    );

    // Description label
    QLabel* descLabel = new QLabel(description, button);
    descLabel->setStyleSheet(
        "QLabel { color: #6B7280; font-size: 18px; }"
    );

    // Savings label (if applicable)
    if (credits > 1) {
        int savings = (credits * 3) - price;
        QLabel* savingsLabel = new QLabel(QString("Save $%1").arg(savings), button);
        savingsLabel->setStyleSheet(
            "QLabel { color: #16A34A; font-size: 16px; font-weight: 600; "
            "margin-top: 8px; }"
        );
        buttonLayout->addLayout(priceRow);
        buttonLayout->addWidget(creditsLabel);
        buttonLayout->addWidget(descLabel);
        buttonLayout->addWidget(savingsLabel);
    } else {
        buttonLayout->addLayout(priceRow);
        buttonLayout->addWidget(creditsLabel);
        buttonLayout->addWidget(descLabel);
    }

    connect(button, &QPushButton::clicked, this, [this, credits, price]() {
        onOptionClicked(credits, price);
    });

    return button;
}

void CreditPurchaseScreen::onOptionClicked(int credits, int price) {
    emit creditsPurchased(credits, price);
}
