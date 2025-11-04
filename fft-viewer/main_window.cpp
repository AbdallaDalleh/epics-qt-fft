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
    int status;
    int index = 0;
    for(index = 0; index < MAX_SOURCES; index++) {
        if (pvs[index].isEmpty())
            continue;
        else
            ca_search(pvs[index].toStdString().c_str(), &ids[index]);
    }

    status = ca_pend_io(1);
    if (status != ECA_NORMAL) {
        QMessageBox::information(this, "CA Search Error", QString::asprintf("Error code: %d | %d", status, ECA_TIMEOUT));
        ui->cbEnable->setChecked(0);
        return;
    }

    index = 0;
    for(index = 0; index < MAX_SOURCES; index++) {
        rawData[index].clear();
        rawData[index].resize(this->N);
        if (pvs[index].isEmpty())
            continue;
        else
            ca_array_get(DBR_DOUBLE, N, ids[index], rawData[index].data());
    }

    status = ca_pend_io(1);
    if (status != ECA_NORMAL) {
        QMessageBox::information(this, "CA Error", QString::asprintf("CA Read Error: %s",  ca_message(status)));
        ui->cbEnable->setChecked(0);
        return;
    }

    index = 0;
    double _min = std::numeric_limits<double>::max();
    double _max = std::numeric_limits<double>::min();
    double min;
    double max;

    QString info = "";
    info += QString::asprintf("\nFrequency resolution (Fs/N): %.3f Hz\n", static_cast<double>(Fs) / N);
    info += QString::asprintf("Nyquist Frequency: %d Hz\n\n", Fs / 2);
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
            double value = a;
            if (ui->cbPower->isChecked())
                value = 20 * log10(a);
            fft_points.push_back(QPointF(fft_points.size() * static_cast<double>(Fs) / N, value));
        });

        if (ui->cbSuppressDC->isChecked())
            fft_points[0] = QPointF(0, 0);

        min = (*std::min_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
        max = (*std::max_element(fft_points.begin(), fft_points.end(), [](QPointF& a, QPointF& b) { return a.y() < b.y(); })).y();
        if (min < _min)
            _min = min;
        if (max > _max)
            _max = max;

        int x_max = (*std::find_if(fft_points.begin(), fft_points.end(), [max](QPointF point) {
            return point.y() == max;
        })).x();

        info += QString::asprintf("%s\nPeak = %.3f at %d Hz\n\n", pvs[index].toStdString().c_str(), max, x_max);

        series[index]->replace(fft_points);
    }

    xAxis->setRange(0, Fs / 2 - 1);
    yAxis->setRange(_min, _max);
    ui->plot->update();
    ui->lblStats->setText(info);
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

    on_txtFs_returnPressed();
    on_txtN_returnPressed();
    if (N <= 0)
        QMessageBox::warning(this, "Error", "Set the number of samples first.");
    else if(Fs <= 0)
        QMessageBox::warning(this, "Error", "Set proper sampling frequency first.");
    else
        ui->cbEnable->setChecked(1);
}

void main_window::on_txtN_returnPressed()
{
    QString arg;
    chid id;
    bool ok;
    bool eca = true;

    arg = ui->txtN->text();
    if (arg.isEmpty())
        return;

    double samples = arg.toUInt(&ok);
    if (!ok) {
        ca_search(arg.toStdString().c_str(), &id);
        if (ca_pend_io(0.5) != ECA_NORMAL)
            eca = false;

        ca_get(DBR_LONG, id, &samples);
        if (ca_pend_io(0.5) != ECA_NORMAL)
            eca = false;

        if (!eca) {
            QMessageBox::warning(this, "Error", "Invalid number of samples from CA.");
            return;
        }
    }

    N = samples;
}

void main_window::on_txtFs_returnPressed()
{
    bool ok;
    QString arg = ui->txtFs->text();

    double frequency = arg.toDouble(&ok);
    if (!ok)
        return;

    Fs = frequency;
}
