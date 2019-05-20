#pragma once

#include <extensionsystem/iplugin.h>

#include <QStringList>

namespace Tabs::Internal {

class TabBar;

class TabsPlugin : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "tabs.json")

public:
    bool initialize(const QStringList &arguments,
                    QString *errorString) override;
    void extensionsInitialized() override {}

private:
    TabBar *m_tabBar;
};

} // namespace Tabs::Internal
