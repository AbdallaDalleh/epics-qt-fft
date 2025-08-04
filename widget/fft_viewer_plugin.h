#ifndef QEFASTFOURIERTRANSFORMPLUGIN_H
#define QEFASTFOURIERTRANSFORMPLUGIN_H

#include <QtUiPlugin/QDesignerCustomWidgetInterface>
#include <QObject>

class QEFastFourierTransformPlugin : public QObject, public QDesignerCustomWidgetInterface
{
public:
    QEFastFourierTransformPlugin();

    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    QEFastFourierTransformPlugin(QObject* parent = 0);

    bool isContainer() const;
    bool isInitialized() const;
    QIcon icon() const;
    QString domXml() const;
    QString group() const;
    QString includeFile() const;
    QString name() const;
    QString toolTip() const;
    QString whatsThis() const;
    QWidget *createWidget(QWidget *parent);
    void initialize(QDesignerFormEditorInterface *core);

private:
    bool m_initialized;
};

#endif // QEFASTFOURIERTRANSFORMPLUGIN_H
