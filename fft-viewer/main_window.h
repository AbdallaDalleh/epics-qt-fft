#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <cadef.h>
#include <opencv2/core/core.hpp>

#include <iostream>

#define MAX_SOURCES 4

QT_BEGIN_NAMESPACE
namespace Ui { class main_window; }
QT_END_NAMESPACE

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    main_window(QWidget *parent = nullptr);
    ~main_window();

    void calculateFFT();

private slots:
    void on_cbEnable_stateChanged(int arg1);

    void on_pushButton_clicked();

    void on_txtN_returnPressed();

    void on_txtFs_returnPressed();

private:
    Ui::main_window *ui;

    QTimer* timer;
    QChart* chart;
    QValueAxis* xAxis;
    QValueAxis* yAxis;

    std::array<QLineSeries*, MAX_SOURCES> series;
    std::vector<double> rawData[MAX_SOURCES];
    std::array<chid, MAX_SOURCES> ids;
    QStringList   pvs;

    bool m_enable;
    int N = -1;
    int Fs = -1;

    QPen colors[MAX_SOURCES] = {QPen(Qt::red), QPen(Qt::blue), QPen(Qt::darkGreen), QPen(Qt::yellow)};
};
#endif // MAIN_WINDOW_H
