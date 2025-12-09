#ifndef CONFIRMATIONSCREEN_H
#define CONFIRMATIONSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class ConfirmationScreen : public QWidget {
    Q_OBJECT

public:
    explicit ConfirmationScreen(QWidget* parent = nullptr);
    void setPhoneNumber(const QString& phone);

signals:
    void resetRequested();

private:
    void setupUI();
    QString formatPhone(const QString& phone) const;

    QString m_phoneNumber;

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_messageLabel;
    QLabel* m_phoneLabel;
    QPushButton* m_doneButton;
};

#endif // CONFIRMATIONSCREEN_H
