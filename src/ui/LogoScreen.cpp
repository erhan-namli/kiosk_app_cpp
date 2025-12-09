#include "ui/LogoScreen.h"
#include <QMouseEvent>
#include <QPropertyAnimation>

LogoScreen::LogoScreen(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void LogoScreen::setupUI() {
    // Set background gradient (blue)
    setStyleSheet(
        "LogoScreen { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "stop:0 #2563EB, stop:1 #1E40AF); }"
    );

    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignCenter);

    // Title
    m_titleLabel = new QLabel("SCAN.RN", this);
    m_titleLabel->setStyleSheet(
        "QLabel { color: white; font-size: 72px; font-weight: bold; "
        "letter-spacing: 8px; }"
    );
    m_titleLabel->setAlignment(Qt::AlignCenter);

    // Tap to begin label
    m_tapLabel = new QLabel("Tap to begin scanning", this);
    m_tapLabel->setStyleSheet(
        "QLabel { color: rgba(255, 255, 255, 230); font-size: 28px; "
        "font-weight: 500; margin-top: 80px; }"
    );
    m_tapLabel->setAlignment(Qt::AlignCenter);

    // Animate the tap label
    QPropertyAnimation* animation = new QPropertyAnimation(m_tapLabel, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1.0);
    animation->setEndValue(0.3);
    animation->setLoopCount(-1); // Infinite loop
    animation->start();

    m_layout->addWidget(m_titleLabel);
    m_layout->addWidget(m_tapLabel);

    setCursor(Qt::PointingHandCursor);
}

void LogoScreen::mousePressEvent(QMouseEvent* event) {
    Q_UNUSED(event);
    emit continueClicked();
}
