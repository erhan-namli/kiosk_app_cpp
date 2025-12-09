#ifndef PHONEINPUTSCREEN_H
#define PHONEINPUTSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>

class PhoneInputScreen : public QWidget {
    Q_OBJECT

public:
    explicit PhoneInputScreen(QWidget* parent = nullptr);
    QString getPhone() const { return m_phone; }
    void reset();

signals:
    void phoneSubmitted(const QString& phone);

private slots:
    void onNumberClicked(const QString& number);
    void onDeleteClicked();
    void onContinueClicked();

private:
    void setupUI();
    QString formatPhone(const QString& phone) const;

    QString m_phone;

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_phoneDisplayLabel;
    QGridLayout* m_numberPad;
    QPushButton* m_deleteButton;
    QPushButton* m_continueButton;
};

#endif // PHONEINPUTSCREEN_H
