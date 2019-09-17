#pragma once

#include <extensionsystem/iplugin.h>

namespace Tabs::Internal {

class TabsPlugin final : public ExtensionSystem::IPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "tabs.json")

public:
    bool initialize(const QStringList &arguments,
                    QString *errorString) override;
    void extensionsInitialized() override;
};

} // namespace Tabs::Internal
