#include "fft_viewer_plugin.h"
#include "fft_viewer.h"

QEFastFourierTransformPlugin::QEFastFourierTransformPlugin(QObject* parent)
{
    Q_UNUSED(parent)
    m_initialized = false;
}

void QEFastFourierTransformPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QEFastFourierTransformPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QEFastFourierTransformPlugin::createWidget(QWidget *parent)
{
    return new QEFastFourierTransform(parent);
}

QString QEFastFourierTransformPlugin::name() const
{
    return QLatin1String("QEFastFourierTransform");
}

QString QEFastFourierTransformPlugin::group() const
{
    return QLatin1String("FFT");
}

QIcon QEFastFourierTransformPlugin::icon() const
{
    return QIcon();
}

QString QEFastFourierTransformPlugin::toolTip() const
{
    return QLatin1String("FFT Viewer Widget");
}

QString QEFastFourierTransformPlugin::whatsThis() const
{
    return QLatin1String("FFT Viewer Widget");
}

bool QEFastFourierTransformPlugin::isContainer() const
{
    return false;
}

QString QEFastFourierTransformPlugin::domXml() const
{
    return R"(
            <ui language="c++" displayname="FFT">
              <widget class="QEFastFourierTransform" name="fft">
                <property name="geometry">
                  <rect>
                    <x>0</x>
                    <y>0</y>
                    <width>400</width>
                    <height>300</height>
                  </rect>
                </property>
              </widget>
            </ui>
           )";
}

QString QEFastFourierTransformPlugin::includeFile() const
{
    return QLatin1String("fft_viewer.h");
}
