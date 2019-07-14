#include "tabbar.h"

#include "constants.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/session.h>

#include <QAction>
#include <QMenu>
#include <QShortcut>
#include <QSizePolicy>

namespace Tabs::Internal {

TabBar::TabBar(QWidget *parent) : QTabBar(parent) {
    this->setDocumentMode(true);
    this->setExpanding(false);
    this->setMovable(true);
    this->setTabsClosable(true);
    this->setUsesScrollButtons(true);
    this->setDrawBase(false);

    auto sp = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sp.setHorizontalStretch(1);
    sp.setVerticalStretch(0);
    sp.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sp);

    this->setFixedHeight(32);

    connect(this, &QTabBar::tabMoved, [this](int from, int to) {
        this->m_editors.move(from, to);
    });

    Core::EditorManager *editorManager = Core::EditorManager::instance();

    connect(editorManager,
            &Core::EditorManager::editorOpened,
            this,
            &TabBar::addEditorTab);
    connect(editorManager,
            &Core::EditorManager::editorsClosed,
            this,
            &TabBar::removeEditorTabs);
    connect(editorManager,
            &Core::EditorManager::currentEditorChanged,
            this,
            &TabBar::selectEditorTab);

    connect(this, &QTabBar::currentChanged, this, &TabBar::activateEditor);
    connect(this, &QTabBar::tabCloseRequested, this, &TabBar::closeTab);

    ProjectExplorer::SessionManager *sessionManager =
        ProjectExplorer::SessionManager::instance();

    connect(sessionManager,
            &ProjectExplorer::SessionManager::sessionLoaded,
            [editorManager]() {
                for (Core::DocumentModel::Entry *entry :
                     Core::DocumentModel::entries()) {
                    editorManager->activateEditorForEntry(
                        entry, Core::EditorManager::DoNotChangeCurrentEditor);
                }
            });

    QAction *prevTabAction = new QAction(tr("Switch to previous tab"), this);
    Core::Command *prevTabCommand = Core::ActionManager::registerAction(
        prevTabAction,
        Tabs::Constants::PREV_TAB_ID,
        Core::Context(Core::Constants::C_GLOBAL));
    prevTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+J")));
    connect(prevTabAction, &QAction::triggered, this, &TabBar::prevTabAction);

    QAction *nextTabAction = new QAction(tr("Switch to next tab"), this);
    Core::Command *nextTabCommand = Core::ActionManager::registerAction(
        nextTabAction,
        Tabs::Constants::NEXT_TAB_ID,
        Core::Context(Core::Constants::C_GLOBAL));
    nextTabCommand->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+K")));
    connect(nextTabAction, &QAction::triggered, this, &TabBar::nextTabAction);
}

void TabBar::activateEditor(int index) {
    if (index < 0 || index >= this->m_editors.size()) {
        return;
    }

    Core::EditorManager::instance()->activateEditor(this->m_editors[index]);
}

void TabBar::addEditorTab(Core::IEditor *editor) {
    Core::IDocument *document = editor->document();

    const int index = this->addTab(document->displayName());
    this->setTabIcon(
        index,
        Core::FileIconProvider::icon(document->filePath().toFileInfo()));
    this->setTabToolTip(index, document->filePath().toString());

    this->m_editors.append(editor);

    connect(document, &Core::IDocument::changed, [this, editor, document]() {
        const int editorIndex = this->m_editors.indexOf(editor);
        if (editorIndex == -1) {
            return;
        }
        QString tabText = document->displayName();
        if (document->isModified()) {
            tabText += QLatin1Char('*');
        }
        this->setTabText(editorIndex, tabText);
    });
}

void TabBar::removeEditorTabs(QList<Core::IEditor *> editors) {
    this->blockSignals(true); // Avoid calling activateEditor()
    for (Core::IEditor *editor : editors) {
        const int index = this->m_editors.indexOf(editor);
        if (index == -1) {
            continue;
        }
        this->m_editors.removeAt(index);
        this->removeTab(index);
    }
    this->blockSignals(false);
}

void TabBar::selectEditorTab(Core::IEditor *editor) {
    const int index = this->m_editors.indexOf(editor);
    if (index == -1) {
        return;
    }
    this->setCurrentIndex(index);
}

void TabBar::closeTab(int index) {
    if (index < 0 || index >= m_editors.size()) {
        return;
    }

    Core::EditorManager::instance()->closeEditor(
        this->m_editors.takeAt(index));
    this->removeTab(index);
}

void TabBar::prevTabAction() {
    const int index = this->currentIndex();
    if (index >= 1) {
        this->setCurrentIndex(index - 1);
    } else {
        this->setCurrentIndex(this->count() - 1);
    }
}

void TabBar::nextTabAction() {
    const int index = this->currentIndex();
    if (index < count() - 1) {
        this->setCurrentIndex(index + 1);
    } else {
        this->setCurrentIndex(0);
    }
}

void TabBar::contextMenuEvent(QContextMenuEvent *event) {
    const int index = this->tabAt(event->pos());
    if (index == -1) {
        return;
    }

    QScopedPointer<QMenu> menu(new QMenu());

    Core::IEditor *editor = this->m_editors[index];
    Core::DocumentModel::Entry *entry =
        Core::DocumentModel::entryForDocument(editor->document());
    Core::EditorManager::addSaveAndCloseEditorActions(
        menu.data(), entry, editor);
    menu->addSeparator();
    Core::EditorManager::addNativeDirAndOpenWithActions(menu.data(), entry);

    menu->exec(this->mapToGlobal(event->pos()));
}

void TabBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        this->closeTab(this->tabAt(event->pos()));
    }
    QTabBar::mouseReleaseEvent(event);
}

QSize TabBar::tabSizeHint(int index) const {
    auto retVal = QTabBar::tabSizeHint(index);
    if (retVal.width() < 200) {
        retVal.setWidth(200);
    }
    if (retVal.height() < 32) {
        retVal.setHeight(32);
    }
    return retVal;
}

} // namespace Tabs::Internal
