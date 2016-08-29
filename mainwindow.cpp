#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSignalMapper>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setDefaultValues();

    setupCheckBoxesSignals();
    setupLabelsSignals();

    QString msg = "Loaded settings file.";
    int res = loadSettings();
    if(res == 0)
        msg = "Settings file not found. Applied default settings.";
    else if(res == -1)
        msg = "Saved data is corrupted. Applied default settings.";

    showStatusMsg(msg);

    connect(ui->startButton, SIGNAL(clicked(bool)), this, SLOT(startButtonClicked()));
    connect(ui->stopButton, SIGNAL(clicked(bool)), this, SLOT(stopButtonClicked()));

    setupCanvas();

    connect(timer, SIGNAL(timeout()), this, SLOT(calculate()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::loadSettings()
{
    QFile file(filename);

    if(!file.exists())
        return 0;

    if(!file.open(QFile::ReadOnly))
        return 0;

    QTextStream input(&file);
    QStringList options = input.readLine().split("|");

    if(options.size() == 7)
    {
        customPreyCount = options[0].toInt();
        customPreyGrowth = options[1].toDouble();
        customPreyDeath = options[2].toDouble();
        customPredatorCount = options[3].toInt();
        customPredatorGrowth = options[4].toDouble();
        customPredatorDeath = options[5].toDouble();
        ui->delaySpinBox->setValue(options[6].toDouble());

        options = input.readLine().split("|");

        if(options.size() == 6)
        {
            ui->preyCountCustom->setChecked(options[0] == "1");
            ui->preyGrowthCustom->setChecked(options[1] == "1");
            ui->preyDeathCustom->setChecked(options[2] == "1");
            ui->predatorCountCustom->setChecked(options[3] == "1");
            ui->predatorGrowthCustom->setChecked(options[4] == "1");
            ui->predatorDeathCustom->setChecked(options[5] == "1");
            return 1;
        }
    }

    file.close();
    file.remove();
    return -1;
}

void MainWindow::saveSettings()
{
    QFile file(filename);
    if(!file.open(QFile::WriteOnly | QFile::Text))
    {
        showStatusMsg("Unable to open the file!");
        return;
    }

    QTextStream output(&file);
    QString out;

    out.append(QString::number(customPreyCount) + "|");
    out.append(QString::number(customPreyGrowth) + "|");
    out.append(QString::number(customPreyDeath) + "|");
    out.append(QString::number(customPredatorCount) + "|");
    out.append(QString::number(customPredatorGrowth) + "|");
    out.append(QString::number(customPredatorDeath) + "|");

    out.append(QString::number(ui->delaySpinBox->value()));

    out.append("\n");

    out.append(QString::number(ui->preyCountCustom->isChecked()) + "|");
    out.append(QString::number(ui->preyGrowthCustom->isChecked()) + "|");
    out.append(QString::number(ui->preyDeathCustom->isChecked()) + "|");
    out.append(QString::number(ui->predatorCountCustom->isChecked()) + "|");
    out.append(QString::number(ui->predatorGrowthCustom->isChecked()) + "|");
    out.append(QString::number(ui->predatorDeathCustom->isChecked()));

    output << out;
    file.close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    timer->stop();
    saveSettings();
    event->accept();
}

void MainWindow::startButtonClicked()
{
    currentPreyCount = ui->preyCount->value();
    preyGrowth = ui->preyGrowth->value();
    preyDeath = ui->preyDeath->value();
    currentPredatorCount = ui->predatorCount->value();
    predatorGrowth = ui->predatorGrowth->value();
    predatorDeath = ui->predatorDeath->value();

    ui->settingsBox->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    ui->canvas->legend->setVisible(true);

    xAxisValue = 0;
    preyGraph->clearData();
    predatorGraph->clearData();

    timer->start(1);
}

void MainWindow::stopButtonClicked()
{
    timer->stop();

    ui->settingsBox->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void MainWindow::customCheckboxToggled(int index)
{
    QDoubleSpinBox *spinBox;
    QCheckBox *checkBox;
    double customValue;
    double defaultValue;

    switch(index)
    {
    case 0:
        spinBox = ui->preyCount;
        checkBox = ui->preyCountCustom;
        customValue = customPreyCount;
        defaultValue = defaultPreyCount;
        break;
    case 1:
        spinBox = ui->preyGrowth;
        checkBox = ui->preyGrowthCustom;
        customValue = customPreyGrowth;
        defaultValue = defaultPreyGrowth;
        break;
    case 2:
        spinBox = ui->preyDeath;
        checkBox = ui->preyDeathCustom;
        customValue = customPreyDeath;
        defaultValue = defaultPreyDeath;
        break;
    case 3:
        spinBox = ui->predatorCount;
        checkBox = ui->predatorCountCustom;
        customValue = customPredatorCount;
        defaultValue = defaultPredatorCount;
        break;
    case 4:
        spinBox = ui->predatorGrowth;
        checkBox = ui->predatorGrowthCustom;
        customValue = customPredatorGrowth;
        defaultValue = defaultPredatorGrowth;
        break;
    default:
        spinBox = ui->predatorDeath;
        checkBox = ui->predatorDeathCustom;
        customValue = customPredatorDeath;
        defaultValue = defaultPredatorDeath;
        break;
    }

    bool custom = checkBox->isChecked();
    spinBox->setEnabled(custom);
    spinBox->setValue(custom ? customValue : defaultValue);
}

void MainWindow::customValueChanged(int index)
{
    switch(index)
    {
    case 0:
        customPreyCount = ui->preyCount->value();
        break;
    case 1:
        customPreyGrowth = ui->preyGrowth->value();
        break;
    case 2:
        customPreyDeath = ui->preyDeath->value();
        break;
    case 3:
        customPredatorCount = ui->predatorCount->value();
        break;
    case 4:
        customPredatorGrowth = ui->predatorGrowth->value();
        break;
    case 5:
        customPredatorDeath = ui->predatorDeath->value();
        break;
    }
}

void MainWindow::setupLabelsSignals()
{
    QSignalMapper *customValueMapper = new QSignalMapper(this);

    connect(ui->preyCount, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));
    connect(ui->preyGrowth, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));
    connect(ui->preyDeath, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));
    connect(ui->predatorCount, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));
    connect(ui->predatorGrowth, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));
    connect(ui->predatorDeath, SIGNAL(editingFinished()), customValueMapper, SLOT(map()));

    customValueMapper->setMapping(ui->preyCount, 0);
    customValueMapper->setMapping(ui->preyGrowth, 1);
    customValueMapper->setMapping(ui->preyDeath, 2);
    customValueMapper->setMapping(ui->predatorCount, 3);
    customValueMapper->setMapping(ui->predatorGrowth, 4);
    customValueMapper->setMapping(ui->predatorDeath, 5);

    connect(customValueMapper, SIGNAL(mapped(int)), this, SLOT(customValueChanged(int)));
}

void MainWindow::setupCheckBoxesSignals()
{
    QSignalMapper *customValueCBMapper = new QSignalMapper(this);

    connect(ui->preyCountCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));
    connect(ui->preyGrowthCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));
    connect(ui->preyDeathCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));
    connect(ui->predatorCountCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));
    connect(ui->predatorGrowthCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));
    connect(ui->predatorDeathCustom, SIGNAL(toggled(bool)), customValueCBMapper, SLOT(map()));

    customValueCBMapper->setMapping(ui->preyCountCustom, 0);
    customValueCBMapper->setMapping(ui->preyGrowthCustom, 1);
    customValueCBMapper->setMapping(ui->preyDeathCustom, 2);
    customValueCBMapper->setMapping(ui->predatorCountCustom, 3);
    customValueCBMapper->setMapping(ui->predatorGrowthCustom, 4);
    customValueCBMapper->setMapping(ui->predatorDeathCustom, 5);

    connect(customValueCBMapper, SIGNAL(mapped(int)), this, SLOT(customCheckboxToggled(int)));
}

void MainWindow::setDefaultValues()
{
    ui->preyCount->setValue(defaultPreyCount);
    ui->preyGrowth->setValue(defaultPreyGrowth);
    ui->preyDeath->setValue(defaultPreyDeath);
    ui->predatorCount->setValue(defaultPredatorCount);
    ui->predatorGrowth->setValue(defaultPredatorGrowth);
    ui->predatorDeath->setValue(defaultPredatorDeath);
    ui->delaySpinBox->setValue(2);
}

void MainWindow::setupCanvas()
{
    ui->canvas->xAxis->setLabel("Time");
    ui->canvas->yAxis->setLabel("Count");

    preyGraph = ui->canvas->addGraph();
    preyGraph->setPen(QPen(Qt::blue));
    preyGraph->setName("Prey");

    predatorGraph = ui->canvas->addGraph();
    predatorGraph->setPen(QPen(Qt::red));
    predatorGraph->setName("Predators");
}

double MainWindow::preyDelta(double currentPreyCount, double currentPredatorCount)
{
    double val = currentPreyCount * (preyGrowth - preyDeath * currentPredatorCount);
    return val * 0.01;
}

double MainWindow::predatorDelta(double currentPreyCount, double currentPredatorCount)
{
    double val = currentPredatorCount * (predatorGrowth * currentPreyCount - predatorDeath);
    return val * 0.01;
}

void MainWindow::calculate()
{
    double xk_1 = preyDelta(currentPreyCount, currentPredatorCount);
    double yk_1 = predatorDelta(currentPreyCount, currentPredatorCount);
    double xk_2 = preyDelta(currentPreyCount + xk_1, currentPredatorCount + yk_1);
    double yk_2 = predatorDelta(currentPreyCount + xk_1, currentPredatorCount + yk_1);

    currentPreyCount += (xk_1 + xk_2) * 0.5;
    currentPredatorCount += (yk_1 + yk_2) * 0.5;

    xAxisValue++;
    preyGraph->addData(xAxisValue, currentPreyCount);
    predatorGraph->addData(xAxisValue, currentPredatorCount);

    ui->canvas->rescaleAxes();
    ui->canvas->replot();

    timer->setInterval(ui->delaySpinBox->value());
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);

    QAction action1("Save As Image", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(saveImage()));
    contextMenu.addAction(&action1);

    contextMenu.exec(ui->canvas->mapToGlobal(pos));
}

void MainWindow::saveImage()
{
    QString fileName = "graph_" + QDate::currentDate().toString("dd-MM-yyyy") + "_" + QTime::currentTime().toString("hh-mm-ss") + ".jpg";
    bool result = ui->canvas->saveJpg(fileName, ui->canvas->width(), ui->canvas->height());

    if(!result)
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("The graph could not be saved.");
        msgBox.setWindowIcon(windowIcon());
        msgBox.exec();
    }
    else {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Information");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("The graph has been saved.");
        msgBox.setWindowIcon(windowIcon());

        QPushButton *showButton = msgBox.addButton(tr("Show in Folder"), QMessageBox::ActionRole);
        QPushButton *closeButton = msgBox.addButton(tr("Close"), QMessageBox::ActionRole);

        msgBox.setDefaultButton(closeButton);
        msgBox.exec();

        if (msgBox.clickedButton() == showButton)
        {
            QString path = QDir::toNativeSeparators(QDir::currentPath());
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }

        msgBox.accept();
    }
}

void MainWindow::showStatusMsg(QString msg)
{
    ui->statusBar->showMessage(msg, 3000);
}
