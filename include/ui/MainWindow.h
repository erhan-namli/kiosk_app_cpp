#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTimer>
#include "ui/LogoScreen.h"
#include "ui/PhoneInputScreen.h"
#include "ui/CreditPurchaseScreen.h"
#include "ui/TapToPayScreen.h"
#include "ui/ScanPromptScreen.h"
#include "ui/ConfirmationScreen.h"

class AppController;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(AppController* controller, QWidget* parent = nullptr);
    ~MainWindow();

    void showScreen(int screenIndex);
    void resetToStart();

    // Screen accessors
    PhoneInputScreen* phoneInputScreen() const { return m_phoneInputScreen; }
    CreditPurchaseScreen* creditPurchaseScreen() const { return m_creditPurchaseScreen; }
    TapToPayScreen* tapToPayScreen() const { return m_tapToPayScreen; }
    ScanPromptScreen* scanPromptScreen() const { return m_scanPromptScreen; }
    ConfirmationScreen* confirmationScreen() const { return m_confirmationScreen; }

private slots:
    void onUserInteraction();
    void onInactivityTimeout();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void setupUI();
    void setupInactivityTimer();

    AppController* m_controller;
    QStackedWidget* m_stackedWidget;
    QTimer* m_inactivityTimer;

    // Screens (matching TypeScript design flow)
    LogoScreen* m_logoScreen;           // 0
    PhoneInputScreen* m_phoneInputScreen;     // 1
    CreditPurchaseScreen* m_creditPurchaseScreen; // 2
    TapToPayScreen* m_tapToPayScreen;      // 3
    ScanPromptScreen* m_scanPromptScreen;   // 4
    ConfirmationScreen* m_confirmationScreen; // 5

    int m_currentScreen;
};

#endif // MAINWINDOW_H
