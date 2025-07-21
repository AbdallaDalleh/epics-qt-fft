#ifndef FFT_VIEWER_H
#define FFT_VIEWER_H

#include <QWidget>
#include <QtCharts>
#include <QTimer>
#include <cadef.h>
#include <opencv2/core/core.hpp>
#include <iostream>

#define MAX_SOURCES 4

namespace Ui {
class QEFastFourierTransform;
}

class QEFastFourierTransform : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QString dataSourceA READ dataSourceA WRITE setDataSourceA NOTIFY dataSourceAChanged)
    Q_PROPERTY(QString dataSourceB READ dataSourceB WRITE setDataSourceB NOTIFY dataSourceBChanged)
    Q_PROPERTY(QString dataSourceC READ dataSourceC WRITE setDataSourceC NOTIFY dataSourceCChanged)
    Q_PROPERTY(QString dataSourceD READ dataSourceD WRITE setDataSourceD NOTIFY dataSourceDChanged)
    Q_PROPERTY(int numberOfPoints READ numberOfPoints WRITE setNumberOfPoints NOTIFY numberOfPointsChanged)
    Q_PROPERTY(QString title READ titleText WRITE setTitle)
    Q_PROPERTY(QString dataSourceTitleA READ dataSourceTitleA WRITE setDataSourceTitleA NOTIFY dataSourceTitleAChanged)
    Q_PROPERTY(QString dataSourceTitleB READ dataSourceTitleB WRITE setDataSourceTitleB NOTIFY dataSourceTitleBChanged)
    Q_PROPERTY(QString dataSourceTitleC READ dataSourceTitleC WRITE setDataSourceTitleC NOTIFY dataSourceTitleCChanged)
    Q_PROPERTY(QString dataSourceTitleD READ dataSourceTitleD WRITE setDataSourceTitleD NOTIFY dataSourceTitleDChanged)
    Q_PROPERTY(QString titleX READ getTitleX WRITE setTitleX NOTIFY titleXChanged)
    Q_PROPERTY(QString titleY READ getTitleY WRITE setTitleY NOTIFY titleYChanged)
    Q_PROPERTY(bool enable READ isEnabled WRITE setEnable NOTIFY enableChanged)

public:
    explicit QEFastFourierTransform(QWidget *parent = nullptr);
    ~QEFastFourierTransform();

    QString dataSourceA();
    QString dataSourceB();
    QString dataSourceC();
    QString dataSourceD();
    void setDataSourceA(QString name);
    void setDataSourceB(QString name);
    void setDataSourceC(QString name);
    void setDataSourceD(QString name);

    int numberOfPoints();
    void setNumberOfPoints(int n);

    QString titleText();
    void setTitle(QString title);

    QString dataSourceTitleA();
    QString dataSourceTitleB();
    QString dataSourceTitleC();
    QString dataSourceTitleD();
    void setDataSourceTitleA(QString name);
    void setDataSourceTitleB(QString name);
    void setDataSourceTitleC(QString name);
    void setDataSourceTitleD(QString name);

    QString getTitleX();
    void setTitleX(QString name);

    QString getTitleY();
    void setTitleY(QString name);

    bool isEnabled();
    void setEnable(bool state);

    void calculateFFT();

signals:
    void dataSourceAChanged(QString& name);
    void dataSourceBChanged(QString& name);
    void dataSourceCChanged(QString& name);
    void dataSourceDChanged(QString& name);
    void numberOfPointsChanged(int n);
    void dataSourceTitleAChanged(QString& name);
    void dataSourceTitleBChanged(QString& name);
    void dataSourceTitleCChanged(QString& name);
    void dataSourceTitleDChanged(QString& name);
    void titleXChanged(QString& name);
    void titleYChanged(QString& name);
    void enableChanged(bool& state);

private:
    Ui::QEFastFourierTransform *ui;

    void modifySeries(int index);

    QTimer* timer;
    QChart* chart;
    QValueAxis* xAxis;
    QValueAxis* yAxis;

    std::array<QLineSeries*, MAX_SOURCES> series;
    std::array<double, 200> rawData[MAX_SOURCES];
    std::array<chid, 6> ids;

    QStringList   pvs;

    int points;
    QString text;
    QStringList titles;
    QString titleX;
    QString titleY;
    bool m_enable;

    QPen colors[MAX_SOURCES] = {QPen(Qt::red), QPen(Qt::blue), QPen(Qt::green), QPen(Qt::yellow)};
};

#endif // FFT_VIEWER_H
