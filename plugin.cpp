#include "plugin.h"

#include "tabbar.h"

#include <coreplugin/icore.h>

#include <QBoxLayout>
#include <QMainWindow>

namespace Tabs::Internal {

bool TabsPlugin::initialize(const QStringList &arguments,
                            QString *errorString) {
    QMainWindow *mainWindow = Core::ICore::mainWindow();
    mainWindow->layout()->setSpacing(0);

    auto *wrapper = new QWidget(mainWindow);
    wrapper->setMinimumHeight(0);

    auto *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    this->m_tabBar = new TabBar();
    layout->addWidget(m_tabBar);
    layout->addWidget(mainWindow->centralWidget());

    wrapper->setLayout(layout);

    mainWindow->setCentralWidget(wrapper);

    return true;
}

} // namespace Tabs::Internal
