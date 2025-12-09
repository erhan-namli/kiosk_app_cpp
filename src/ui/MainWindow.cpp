#include "ui/MainWindow.h"
#include "AppController.h"
#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTouchEvent>

MainWindow::MainWindow(AppController* controller, QWidget* parent)
    : QMainWindow(parent)
    , m_controller(controller)
    , m_currentScreen(0)
{
    setupUI();
    setupInactivityTimer();
}

MainWindow::~MainWindow() {
}

void MainWindow::setupUI() {
    // Create stacked widget to hold all screens
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);

    // Create all screens
    m_logoScreen = new LogoScreen(this);
    m_phoneInputScreen = new PhoneInputScreen(this);
    m_creditPurchaseScreen = new CreditPurchaseScreen(this);
    m_tapToPayScreen = new TapToPayScreen(this);
    m_scanPromptScreen = new ScanPromptScreen(this);
    m_confirmationScreen = new ConfirmationScreen(this);

    // Add screens to stacked widget
    m_stackedWidget->addWidget(m_logoScreen);           // 0
    m_stackedWidget->addWidget(m_phoneInputScreen);     // 1
    m_stackedWidget->addWidget(m_creditPurchaseScreen); // 2
    m_stackedWidget->addWidget(m_tapToPayScreen);       // 3
    m_stackedWidget->addWidget(m_scanPromptScreen);     // 4
    m_stackedWidget->addWidget(m_confirmationScreen);   // 5

    // Connect screen signals to controller via MainWindow
    connect(m_logoScreen, &LogoScreen::continueClicked, this, [this]() {
        if (m_controller) m_controller->startNewSession();
    });
    connect(m_phoneInputScreen, &PhoneInputScreen::phoneSubmitted, this, [this](const QString& phone) {
        if (m_controller) m_controller->setPhoneNumber(phone);
    });
    connect(m_creditPurchaseScreen, &CreditPurchaseScreen::creditsPurchased, this, [this](int credits, int price) {
        if (m_controller) m_controller->setPurchaseInfo(credits, price);
    });
    connect(m_tapToPayScreen, &TapToPayScreen::paymentCompleted, this, [this]() {
        if (m_controller) m_controller->startScanning();
    });
    connect(m_scanPromptScreen, &ScanPromptScreen::scanRequested, this, [this]() {
        if (m_controller) m_controller->executeScan();
    });
    connect(m_confirmationScreen, &ConfirmationScreen::resetRequested, this, [this]() {
        if (m_controller) m_controller->resetToIdle();
    });

    // Show logo screen initially
    m_stackedWidget->setCurrentIndex(0);

    // Set window properties
    setWindowTitle("SCAN.RN - Photo Scanner Kiosk");
    resize(1024, 768);

    // Install event filter for user interaction tracking
    qApp->installEventFilter(this);
}

void MainWindow::setupInactivityTimer() {
    m_inactivityTimer = new QTimer(this);
    m_inactivityTimer->setInterval(60000); // 60 seconds
    m_inactivityTimer->setSingleShot(false);
    connect(m_inactivityTimer, &QTimer::timeout, this, &MainWindow::onInactivityTimeout);
}

void MainWindow::showScreen(int screenIndex) {
    if (screenIndex < 0 || screenIndex >= m_stackedWidget->count()) {
        qWarning() << "Invalid screen index:" << screenIndex;
        return;
    }

    m_currentScreen = screenIndex;
    m_stackedWidget->setCurrentIndex(screenIndex);

    // Reset inactivity timer (don't run on logo screen)
    if (screenIndex == 0) {
        m_inactivityTimer->stop();
    } else {
        m_inactivityTimer->start();
    }

    qInfo() << "Showing screen:" << screenIndex;
}

void MainWindow::resetToStart() {
    qInfo() << "Resetting to start";

    // Reset all screens
    m_phoneInputScreen->reset();
    m_scanPromptScreen->reset();

    // Show logo screen
    showScreen(0);
}

void MainWindow::onUserInteraction() {
    // Reset inactivity timer on any user interaction
    if (m_currentScreen != 0) {
        m_inactivityTimer->start();
    }
}

void MainWindow::onInactivityTimeout() {
    qInfo() << "Inactivity timeout - resetting to start";
    if (m_controller) {
        m_controller->resetToIdle();
    }
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    // Track user interactions
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::TouchBegin ||
        event->type() == QEvent::KeyPress) {
        onUserInteraction();
    }

    return QMainWindow::eventFilter(obj, event);
}
