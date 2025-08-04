#ifndef WIDGETSCOLLECTIONPLUGIN_H
#define WIDGETSCOLLECTIONPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class WidgetsCollectionPlugin : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.WidgetsCollectionPlugin")

public:
    explicit WidgetsCollectionPlugin(QObject* parent = 0);
    QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

private:
    QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif // WIDGETSCOLLECTIONPLUGIN_H
