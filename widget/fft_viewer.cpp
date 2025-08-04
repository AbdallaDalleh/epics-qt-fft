#include "fft_viewer.h"
#include "ui_fft_viewer.h"

QEFastFourierTransform::QEFastFourierTransform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QEFastFourierTransform)
{
    ui->setupUi(this);

    for (int i = 0; i < MAX_SOURCES; ++i) {
        pvs.push_back("");
        titles.push_back("");
        series[i] = new QLineSeries(this);
    }
    m_enable = false;

    this->xAxis = new QValueAxis;
    this->xAxis->setTitleText("Frequencies");
    this->xAxis->setTickCount(6);
    this->xAxis->setLabelFormat("%d");

    this->yAxis = new QValueAxis;
    this->yAxis->setTickType(QValueAxis::TicksFixed);
    this->yAxis->setMinorTickCount(2);
    this->yAxis->applyNiceNumbers();
    this->yAxis->setTitleText("Amplitudes");

    chart = new QChart;
    chart->setTitle(text);
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->addAxis(this->xAxis, Qt::AlignBottom);
    chart->addAxis(this->yAxis, Qt::AlignLeft);
    ui->plot->setChart(chart);
    ui->plot->setRenderHint(QPainter::Antialiasing);

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &QEFastFourierTransform::calculateFFT);
    timer->setInterval(1000);
    timer->start();

    QObject::connect(this, &QEFastFourierTransform::dataSourceAChanged, this, [this](QString name) { Q_UNUSED(name); modifySeries(0); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceBChanged, this, [this](QString name) { Q_UNUSED(name); modifySeries(1); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceCChanged, this, [this](QString name) { Q_UNUSED(name); modifySeries(2); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceDChanged, this, [this](QString name) { Q_UNUSED(name); modifySeries(3); });

    QObject::connect(this, &QEFastFourierTransform::dataSourceTitleAChanged, this, [this](QString name) { series[0]->setName(name); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceTitleBChanged, this, [this](QString name) { series[1]->setName(name); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceTitleCChanged, this, [this](QString name) { series[2]->setName(name); });
    QObject::connect(this, &QEFastFourierTransform::dataSourceTitleDChanged, this, [this](QString name) { series[3]->setName(name); });

    QObject::connect(this, &QEFastFourierTransform::titleXChanged, this, [this](QString name) { this->xAxis->setTitleText(name); });
    QObject::connect(this, &QEFastFourierTransform::titleYChanged, this, [this](QString name) { this->yAxis->setTitleText(name); });
}

QEFastFourierTransform::~QEFastFourierTransform()
{
    delete ui;
}

QString QEFastFourierTransform::dataSourceA()
{
    return pvs[0];
}

QString QEFastFourierTransform::dataSourceB()
{
    return pvs[1];
}

QString QEFastFourierTransform::dataSourceC()
{
    return pvs[2];
}

QString QEFastFourierTransform::dataSourceD()
{
    return pvs[3];
}

void QEFastFourierTransform::setDataSourceA(QString name)
{
    pvs[0] = name;
    emit dataSourceAChanged(name);
}

void QEFastFourierTransform::setDataSourceB(QString name)
{
    pvs[1] = name;
    emit dataSourceBChanged(name);
}

void QEFastFourierTransform::setDataSourceC(QString name)
{
    pvs[2] = name;
    emit dataSourceCChanged(name);
}

void QEFastFourierTransform::setDataSourceD(QString name)
{
    pvs[3] = name;
    emit dataSourceDChanged(name);
}

int QEFastFourierTransform::numberOfPoints()
{
    return points;
}

void QEFastFourierTransform::setNumberOfPoints(int n)
{
    points = n;
    emit numberOfPointsChanged(points);
}

QString QEFastFourierTransform::titleText()
{
    return text;
}

void QEFastFourierTransform::setTitle(QString title)
{
    text = title;
    this->chart->setTitle(text);
}

void QEFastFourierTransform::modifySeries(int index)
{
    QLineSeries* s = new QLineSeries(this);
    s->setName(pvs[index]);
    s->setPen(colors[index]);
    if (chart->series().contains(series[index])) {
        chart->removeSeries(series[index]);
        series[index]->detachAxis(xAxis);
        series[index]->detachAxis(yAxis);
    }

    chart->addSeries(s);
    s->attachAxis(xAxis);
    s->attachAxis(yAxis);
    series[index] = s;
}

void QEFastFourierTransform::calculateFFT()
{
    if (!m_enable)
        return;

    int index = 0;
    for (index = 0; index < MAX_SOURCES; index++) {
        if (!pvs[index].isEmpty()) {
            ca_search(pvs[index].toStdString().c_str(), &ids[index]);
        }
    }
    if (ca_pend_io(2) != ECA_NORMAL)
        return;

    for (index = 0; index < MAX_SOURCES; index++) {
        if (!pvs[index].isEmpty())
            ca_array_get(DBR_DOUBLE, points, ids[index], rawData[index].data());
    }
    if (ca_pend_io(2) != ECA_NORMAL)
        return;

    index = 0;
    double totalMin = std::numeric_limits<double>::max();
    double totalMax = std::numeric_limits<double>::min();
    double min;
    double max;
    for (index = 0; index < MAX_SOURCES; index++) {
        if (!pvs[index].isEmpty()) {
            std::array<double, 200> data = rawData[index];

            QVector<QPointF> fft_points;

            cv::Mat input(1, data.size(), CV_64F, data.data());
            cv::Mat output;
            cv::Mat magnitude;
            cv::Mat planes[2];

            cv::dft(input, output, cv::DFT_COMPLEX_OUTPUT);
            cv::split(output, planes);
            cv::magnitude(planes[0], planes[1], magnitude);
            std::for_each(magnitude.begin<double>(),
                          magnitude.begin<double>() + data.size(),
                          [this, &fft_points](double a) {
                              fft_points.push_back(QPointF(fft_points.size(), a));
                          }
            );

            min = (*std::min_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
            max = (*std::max_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
            if (min < totalMin)
                totalMin = min;
            if (max > totalMax)
                totalMax = max;
            series[index]->replace(fft_points);
        }
    }

    xAxis->setRange(1, points / 2 - 1);
    yAxis->setRange(totalMin, totalMax);
    ui->plot->update();
}

QString QEFastFourierTransform::dataSourceTitleA()
{
    return titles[0];
}

QString QEFastFourierTransform::dataSourceTitleB()
{
    return titles[1];
}

QString QEFastFourierTransform::dataSourceTitleC()
{
    return titles[2];
}

QString QEFastFourierTransform::dataSourceTitleD()
{
    return titles[3];
}

void QEFastFourierTransform::setDataSourceTitleA(QString name)
{
    titles[0] = name;
    emit dataSourceTitleAChanged(name);
}

void QEFastFourierTransform::setDataSourceTitleB(QString name)
{
    titles[1] = name;
    emit dataSourceTitleBChanged(name);
}

void QEFastFourierTransform::setDataSourceTitleC(QString name)
{
    titles[2] = name;
    emit dataSourceTitleCChanged(name);
}

void QEFastFourierTransform::setDataSourceTitleD(QString name)
{
    titles[3] = name;
    emit dataSourceTitleDChanged(name);
}

QString QEFastFourierTransform::getTitleX()
{
    return titleX;
}

void QEFastFourierTransform::setTitleX(QString name)
{
    titleX = name;
    emit titleXChanged(name);
}

QString QEFastFourierTransform::getTitleY()
{
    return titleY;
}

void QEFastFourierTransform::setTitleY(QString name)
{
    titleY = name;
    emit titleYChanged(name);
}

bool QEFastFourierTransform::isEnabled()
{
    return m_enable;
}

void QEFastFourierTransform::setEnable(bool state)
{
    m_enable = state;
    emit enableChanged(state);
}
