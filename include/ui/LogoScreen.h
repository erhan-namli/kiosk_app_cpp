#ifndef LOGOSCREEN_H
#define LOGOSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class LogoScreen : public QWidget {
    Q_OBJECT

public:
    explicit LogoScreen(QWidget* parent = nullptr);

signals:
    void continueClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();

    QVBoxLayout* m_layout;
    QLabel* m_titleLabel;
    QLabel* m_tapLabel;
};

#endif // LOGOSCREEN_H
