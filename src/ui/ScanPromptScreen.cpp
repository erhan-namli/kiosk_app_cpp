#include "ui/ScanPromptScreen.h"
#include <QPainter>

ScanPromptScreen::ScanPromptScreen(QWidget* parent)
    : QWidget(parent)
    , m_totalScans(1)
    , m_currentScan(0)
    , m_isScanning(false)
{
    setupUI();
}

void ScanPromptScreen::setupUI() {
    // Blue gradient background
    setStyleSheet(
        "ScanPromptScreen { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #2563EB, stop:1 #1E40AF); }"
    );

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(40, 40, 40, 40);
    m_mainLayout->setSpacing(24);
    m_mainLayout->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("Ready to Scan", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: white; font-size: 36px; font-weight: 700; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Progress label
    m_progressLabel = new QLabel("Scanning 1 of 1", this);
    m_progressLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.9); font-size: 28px; "
        "font-weight: 600; }"
    );
    m_progressLabel->setAlignment(Qt::AlignCenter);

    // Instruction card (scanner alignment guide)
    m_instructionCard = new QWidget(this);
    m_instructionCard->setStyleSheet(
        "QWidget { background-color: rgba(255, 255, 255, 0.95); "
        "border-radius: 16px; padding: 24px; max-width: 500px; }"
    );
    m_instructionCard->setMaximumWidth(500);

    QVBoxLayout* cardLayout = new QVBoxLayout(m_instructionCard);

    QLabel* alignLabel = new QLabel("Align document to LEFT edge", this);
    alignLabel->setStyleSheet(
        "QLabel { color: #1F2937; font-size: 18px; font-weight: 600; }"
    );
    alignLabel->setAlignment(Qt::AlignCenter);

    // Scanner bed visualization (simplified)
    QLabel* scannerVisual = new QLabel(this);
    scannerVisual->setMinimumHeight(120);
    scannerVisual->setStyleSheet(
        "QLabel { background-color: #E5E7EB; border: 4px solid #9CA3AF; "
        "border-radius: 8px; }"
    );

    QLabel* alignInstruction = new QLabel("← Insert document flush with left edge", this);
    alignInstruction->setStyleSheet(
        "QLabel { color: #DC2626; font-size: 14px; font-weight: 600; "
        "margin-top: 12px; }"
    );
    alignInstruction->setAlignment(Qt::AlignCenter);

    cardLayout->addWidget(alignLabel);
    cardLayout->addWidget(scannerVisual);
    cardLayout->addWidget(alignInstruction);

    // Status label
    m_statusLabel = new QLabel("Make sure document is flat and fully inserted", this);
    m_statusLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.8); font-size: 14px; }"
    );
    m_statusLabel->setAlignment(Qt::AlignCenter);

    // Scan button
    m_scanButton = new QPushButton("Start Scanning", this);
    m_scanButton->setMinimumHeight(80);
    m_scanButton->setMaximumWidth(600);
    m_scanButton->setStyleSheet(
        "QPushButton { background-color: white; color: #2563EB; "
        "border-radius: 16px; font-size: 32px; font-weight: 700; "
        "border: none; } "
        "QPushButton:hover { background-color: #F3F4F6; } "
        "QPushButton:pressed { background-color: #E5E7EB; }"
    );
    connect(m_scanButton, &QPushButton::clicked, this, &ScanPromptScreen::scanRequested);

    // Remaining label
    QLabel* remainingLabel = new QLabel("1 scan remaining", this);
    remainingLabel->setObjectName("remainingLabel");
    remainingLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 0.8); font-size: 14px; "
        "margin-top: 16px; }"
    );
    remainingLabel->setAlignment(Qt::AlignCenter);

    m_mainLayout->addWidget(m_titleLabel);
    m_mainLayout->addWidget(m_progressLabel);
    m_mainLayout->addWidget(m_instructionCard, 0, Qt::AlignCenter);
    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_scanButton);
    m_mainLayout->addWidget(remainingLabel);
}

void ScanPromptScreen::setTotalScans(int total) {
    m_totalScans = total;
    updateUI();
}

void ScanPromptScreen::setCurrentScan(int current) {
    m_currentScan = current;
    updateUI();
}

void ScanPromptScreen::setScanning(bool scanning) {
    m_isScanning = scanning;
    updateUI();
}

void ScanPromptScreen::updateUI() {
    if (m_isScanning) {
        m_titleLabel->setText("Scanning...");
        m_scanButton->setEnabled(false);
        m_scanButton->setText("Scanning...");
    } else {
        if (m_currentScan == 0) {
            m_titleLabel->setText("Ready to Scan");
            m_scanButton->setText("Start Scanning");
        } else {
            m_titleLabel->setText("Next Scan Ready");
            m_scanButton->setText("Scan Next");
        }
        m_scanButton->setEnabled(true);
    }

    m_progressLabel->setText(
        QString("Scanning %1 of %2").arg(m_currentScan + 1).arg(m_totalScans)
    );

    int remaining = m_totalScans - m_currentScan;
    QLabel* remainingLabel = findChild<QLabel*>("remainingLabel");
    if (remainingLabel) {
        remainingLabel->setText(
            QString("%1 %2 remaining").arg(remaining).arg(remaining == 1 ? "scan" : "scans")
        );
    }
}

void ScanPromptScreen::reset() {
    m_currentScan = 0;
    m_isScanning = false;
    updateUI();
}
