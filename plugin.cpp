#include "plugin.h"

#include "tabbar.h"

#include <coreplugin/icore.h>

#include <QBoxLayout>
#include <QMainWindow>

namespace Tabs::Internal {

bool TabsPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                            [[maybe_unused]] QString *errorString) {
    auto *mainWindow = Core::ICore::mainWindow();
    auto wrapperLayout =
        static_cast<QVBoxLayout *>(mainWindow->centralWidget()->layout());

    // Qt parent-child relationship will cover deletion
    auto *tabBar = new TabBar(mainWindow->centralWidget());
    wrapperLayout->insertWidget(wrapperLayout->count() - 1, tabBar);

    return true;
}

void TabsPlugin::extensionsInitialized() {}

} // namespace Tabs::Internal
