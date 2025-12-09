#ifndef TAPTOPAYSCREEN_H
#define TAPTOPAYSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class TapToPayScreen : public QWidget {
    Q_OBJECT

public:
    explicit TapToPayScreen(QWidget* parent = nullptr);
    void setPaymentInfo(int credits, int price);

signals:
    void paymentCompleted();

private slots:
    void onTapClicked();

private:
    void setupUI();

    int m_credits;
    int m_price;

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_amountLabel;
    QLabel* m_creditsLabel;
    QLabel* m_instructionLabel;
    QPushButton* m_tapButton;
};

#endif // TAPTOPAYSCREEN_H
