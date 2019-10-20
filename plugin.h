#pragma once

#include <extensionsystem/iplugin.h>

class QMenuBar;

namespace Tabs::Internal {

class ChangeEventFilter;

class TabsPlugin final : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "tabs.json")

public:
    bool initialize(const QStringList &arguments,
                    QString *errorString) override;
    void extensionsInitialized() override;

private:
    QMenuBar *m_menuBar = nullptr;
    ChangeEventFilter *m_filter = nullptr;
};

} // namespace Tabs::Internal
