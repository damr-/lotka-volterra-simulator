#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QString>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupCheckBoxesSignals();

    void setupCanvas();

private slots:
    void startButtonClicked();
    void stopButtonClicked();
    void customCheckboxToggled(int index);
    void customValueChanged(int index);
    void calculate();
    void showContextMenu(const QPoint &pos);
    void saveImage();

private:
    Ui::MainWindow *ui;

    int loadSettings();
    void saveSettings();
    void setupLabelsSignals();
    void setDefaultValues();

    double preyDelta(double currentPreyCount, double currentPredatorCount);
    double predatorDelta(double currentPreyCount, double currentPredatorCount);

    void showStatusMsg(QString msg);

    void closeEvent(QCloseEvent *event);

    QString filename = "Custom Simulator Settings";

    QTimer *timer = new QTimer(this);

    double defaultPreyCount = 30;
    double defaultPreyGrowth = 1.0f;
    double defaultPreyDeath = 0.1f;

    double defaultPredatorCount = 5;
    double defaultPredatorGrowth = 0.075f;
    double defaultPredatorDeath = 1.0f;

    double customPreyCount = 30;
    double customPreyGrowth = 1.0f;
    double customPreyDeath = 0.1f;

    double customPredatorCount = 5;
    double customPredatorGrowth = 0.075f;
    double customPredatorDeath = 1.0f;

    double currentPreyCount;
    double preyGrowth;
    double preyDeath;
    double currentPredatorCount;
    double predatorGrowth;
    double predatorDeath;

    int xAxisValue = 0;
    QCPGraph *preyGraph;
    QCPGraph *predatorGraph;
};

#endif // MAINWINDOW_H
