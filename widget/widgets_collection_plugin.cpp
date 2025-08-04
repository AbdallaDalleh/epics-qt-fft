#include "fft_viewer_plugin.h"
#include "widgets_collection_plugin.h"

WidgetsCollectionPlugin::WidgetsCollectionPlugin(QObject *parent)
    : QObject(parent)
{
    m_widgets.append(new QEFastFourierTransformPlugin(this));
}

QList<QDesignerCustomWidgetInterface*> WidgetsCollectionPlugin::customWidgets() const
{
    return m_widgets;
}
