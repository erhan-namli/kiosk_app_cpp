#ifndef SCANPROMPTSCREEN_H
#define SCANPROMPTSCREEN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class ScanPromptScreen : public QWidget {
    Q_OBJECT

public:
    explicit ScanPromptScreen(QWidget* parent = nullptr);
    void setTotalScans(int total);
    void setCurrentScan(int current);
    void setScanning(bool scanning);
    void reset();

signals:
    void scanRequested();

private:
    void setupUI();
    void updateUI();

    int m_totalScans;
    int m_currentScan;
    bool m_isScanning;

    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QLabel* m_progressLabel;
    QWidget* m_instructionCard;
    QLabel* m_statusLabel;
    QPushButton* m_scanButton;
};

#endif // SCANPROMPTSCREEN_H
