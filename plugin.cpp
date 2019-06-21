#include "plugin.h"

#include "tabbar.h"

#include <coreplugin/icore.h>

#include <QBoxLayout>
#include <QMainWindow>

namespace Tabs::Internal {

bool TabsPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                            [[maybe_unused]] QString *errorString) {
    QMainWindow *mainWindow = Core::ICore::mainWindow();
    auto wrapperLayout =
        static_cast<QVBoxLayout *>(mainWindow->centralWidget()->layout());

    this->m_tabBar = new TabBar(mainWindow->centralWidget());

    wrapperLayout->insertWidget(wrapperLayout->count() - 1, this->m_tabBar);

    return true;
}

} // namespace Tabs::Internal
