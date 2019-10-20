#include "plugin.h"

#include "mainwindowfilter.h"
#include "tabbar.h"

#include <coreplugin/icore.h>

#include <QBoxLayout>
#include <QMenuBar>

namespace Tabs::Internal {

bool TabsPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                            [[maybe_unused]] QString *errorString) {
    auto *mainWindow = Core::ICore::mainWindow();
    auto wrapperLayout =
        static_cast<QVBoxLayout *>(mainWindow->centralWidget()->layout());

    // Qt parent-child relationship will cover deletion
    auto *widget = new QWidget(mainWindow->centralWidget());
    auto *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
    this->m_menuBar = Core::ICore::mainWindow()->menuBar();
    layout->addWidget(this->m_menuBar);
    auto *tabBar = new TabBar(mainWindow->centralWidget());
    layout->addWidget(tabBar);
    wrapperLayout->insertWidget(wrapperLayout->count() - 1, widget);

    this->m_filter = new ChangeEventFilter(
        [this] { this->m_menuBar->update(); },
        [this] { Core::ICore::mainWindow()->setMenuBar(this->m_menuBar); },
        this);
    Core::ICore::mainWindow()->installEventFilter(this->m_filter);

    return true;
}

void TabsPlugin::extensionsInitialized() {}

} // namespace Tabs::Internal
