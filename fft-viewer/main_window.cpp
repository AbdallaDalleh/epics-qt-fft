#include "main_window.h"
#include "ui_main_window.h"

main_window::main_window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::main_window)
{
    ui->setupUi(this);

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
    chart->setMargins(QMargins(0, 0, 0, 0));
    chart->addAxis(this->xAxis, Qt::AlignBottom);
    chart->addAxis(this->yAxis, Qt::AlignLeft);
    ui->plot->setChart(chart);
    ui->plot->setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < MAX_SOURCES; ++i) {
        pvs.push_back("");
        series[i] = new QLineSeries(this);
        series[i]->setPen(colors[i]);
        chart->addSeries(series[i]);
        series[i]->attachAxis(xAxis);
        series[i]->attachAxis(yAxis);
    }

    timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, &main_window::calculateFFT);
    timer->setInterval(1000);
}

main_window::~main_window()
{
    delete ui;
}

void main_window::calculateFFT()
{
    int index = 0;
    for(index = 0; index < MAX_SOURCES; index++) {
        if (pvs[index].isEmpty())
            continue;
        else
            ca_search(pvs[index].toStdString().c_str(), &ids[index]);
    }

    if (ca_pend_io(2) != ECA_NORMAL)
        return;

    index = 0;
    for(index = 0; index < MAX_SOURCES; index++) {
        rawData[index].clear();
        rawData[index].resize(this->N);
        if (pvs[index].isEmpty())
            continue;
        else
            ca_array_get(DBR_DOUBLE, N, ids[index], rawData[index].data());
    }

    if (ca_pend_io(2) != ECA_NORMAL)
        return;

    index = 0;
    double _min = std::numeric_limits<double>::max();
    double _max = std::numeric_limits<double>::min();
    double min;
    double max;

    for(index = 0; index < MAX_SOURCES; index++) {
        std::vector<double> data = rawData[index];
        if (pvs[index].isEmpty())
            continue;

        QVector<QPointF> fft_points;
        cv::Mat input(1, data.size(), CV_64F, data.data());
        cv::Mat output;
        cv::Mat magnitude;
        cv::Mat planes[2];

        cv::dft(input, output, cv::DFT_COMPLEX_OUTPUT | cv::DFT_SCALE);
        cv::split(output, planes);
        cv::magnitude(planes[0], planes[1], magnitude);
        std::for_each(magnitude.begin<double>(), magnitude.begin<double>() + N / 2, [this, &fft_points](double a)
        {
            double value;
            if (ui->cbScale->currentIndex() == 1)
                value = 20 * log10(a);
            else
                value = a / N;
            fft_points.push_back(QPointF(fft_points.size(), value));
        });

        if (ui->cbSuppressDC->isChecked())
            fft_points[0] = QPointF(0, 0);

        min = (*std::min_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
        max = (*std::max_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
        if (min < _min)
            _min = min;
        if (max > _max)
            _max = max;

        series[index]->replace(fft_points);
    }

    this->xAxis->setRange(0, N / 2 - 1);
    yAxis->setRange(_min, _max);
    ui->plot->update();
}

void main_window::on_cbEnable_stateChanged(int arg1)
{
    if (arg1)
        timer->start();
    else
        timer->stop();
}


void main_window::on_pushButton_clicked()
{
    ui->cbEnable->setChecked(0);

    QLineEdit* sources[4] = {ui->txtPV_1, ui->txtPV_2, ui->txtPV_3, ui->txtPV_4};
    for (int i = 0; i < MAX_SOURCES; ++i) {
        pvs[i] = sources[i]->text();
        if (chart->series().contains(series[i])) {
            chart->removeSeries(series[i]);
            series[i]->detachAxis(xAxis);
            series[i]->detachAxis(yAxis);
            series[i] = nullptr;
        }
        if (!sources[i]->text().isEmpty()) {
            series[i] = new QLineSeries(this);
            series[i]->setPen(colors[i]);
            series[i]->setName(pvs[i]);
            chart->addSeries(series[i]);
            series[i]->attachAxis(xAxis);
            series[i]->attachAxis(yAxis);
        }
    }

    if (N <= 0)
        QMessageBox::warning(this, "Error", "Set the number of samples first.");
    else
        ui->cbEnable->setChecked(1);
}

void main_window::on_txtFs_textChanged(const QString &arg1)
{
    bool ok;

    ui->cbEnable->setChecked(0);
    double frequency = arg1.toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Error", "Invalid sampling frequency.");
        return;
    }

    Fs = frequency;
    ui->cbEnable->setChecked(1);
}

void main_window::on_txtN_textChanged(const QString &arg1)
{
    chid id;
    bool ok;
    bool eca = true;

    if (arg1.isEmpty())
        return;

    double samples = arg1.toUInt(&ok);
    if (!ok) {
        ca_search(arg1.toStdString().c_str(), &id);
        if (ca_pend_io(2) != ECA_NORMAL)
            eca = false;

        ca_get(DBR_LONG, id, &samples);
        if (ca_pend_io(2) != ECA_NORMAL)
            eca = false;

        if (!eca) {
            QMessageBox::warning(this, "Error", "Invalid number of samples.");
            return;
        }
    }

    N = samples;
}

