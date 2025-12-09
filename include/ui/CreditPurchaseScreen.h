#ifndef CREDITPURCHASESCREEN_H
#define CREDITPURCHASESCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class CreditPurchaseScreen : public QWidget {
    Q_OBJECT

public:
    explicit CreditPurchaseScreen(QWidget* parent = nullptr);

signals:
    void creditsPurchased(int credits, int price);

private slots:
    void onOptionClicked(int credits, int price);

private:
    void setupUI();
    QPushButton* createCreditOption(int credits, int price, const QString& description,
                                     bool popular = false);

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_subtitleLabel;
};

#endif // CREDITPURCHASESCREEN_H
