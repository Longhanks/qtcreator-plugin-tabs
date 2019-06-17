#include "plugin.h"

#include "tabbar.h"

#include <coreplugin/icore.h>

#include <QBoxLayout>
#include <QMainWindow>

namespace Tabs::Internal {

bool TabsPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                            [[maybe_unused]] QString *errorString) {
    this->m_tabBar = new TabBar();

    QMainWindow *mainWindow = Core::ICore::mainWindow();
    if (mainWindow->centralWidget()->objectName() == "CSDWrapper") {
        auto wrapperLayout =
            static_cast<QVBoxLayout *>(mainWindow->centralWidget()->layout());
        wrapperLayout->insertWidget(1, this->m_tabBar);
        return true;
    }

    mainWindow->layout()->setSpacing(0);

    auto *wrapper = new QWidget(mainWindow);
    wrapper->setObjectName("CSDWrapper");
    wrapper->setMinimumHeight(0);

    auto *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_tabBar);
    layout->addWidget(mainWindow->centralWidget());

    wrapper->setLayout(layout);

    mainWindow->setCentralWidget(wrapper);

    return true;
}

} // namespace Tabs::Internal
