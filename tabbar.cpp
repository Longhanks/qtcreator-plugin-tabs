#include "tabbar.h"

#include "constants.h"

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/fileiconprovider.h>
#include <projectexplorer/session.h>

#include <QAction>
#include <QMenu>
#include <QScopeGuard>
#include <QShortcut>
#include <QSizePolicy>

#include <algorithm>
#include <type_traits>

namespace Tabs::Internal {

template <typename T,
          std::enable_if_t<std::is_signed_v<T>, std::nullptr_t> * = nullptr>
static inline auto to_unsigned(T t) -> std::make_unsigned_t<T> {
    return static_cast<std::make_unsigned_t<T>>(t);
}

TabBar::TabBar(QWidget *parent) noexcept : QTabBar(parent) {
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

    QObject::connect(this, &QTabBar::tabMoved, [this](int from, int to) {
        auto posFrom = std::begin(this->m_editors);
        std::advance(posFrom, from);

        auto *popped = *posFrom;

        this->m_editors.erase(posFrom);

        auto posTo = std::begin(this->m_editors);
        std::advance(posTo, to);

        this->m_editors.insert(posTo, popped);
    });

    auto *editorManager = Core::EditorManager::instance();

    QObject::connect(editorManager,
                     &Core::EditorManager::editorOpened,
                     this,
                     &TabBar::onEditorOpened);
    QObject::connect(editorManager,
                     &Core::EditorManager::editorsClosed,
                     this,
                     &TabBar::onEditorsClosed);
    QObject::connect(editorManager,
                     &Core::EditorManager::currentEditorChanged,
                     this,
                     &TabBar::onCurrentEditorChanged);

    QObject::connect(this, &QTabBar::currentChanged, this, [this](int index) {
        if (index < 0 || to_unsigned(index) >= this->m_editors.size()) {
            return;
        }
        Core::EditorManager::instance()->activateEditor(
            this->m_editors[to_unsigned(index)]);
    });

    QObject::connect(
        this, &QTabBar::tabCloseRequested, this, [this](int index) {
            if (index < 0 || to_unsigned(index) >= m_editors.size()) {
                return;
            }

            auto *editor = this->m_editors[to_unsigned(index)];
            auto it = std::begin(this->m_editors);
            std::advance(it, index);
            this->m_editors.erase(it);
            Core::EditorManager::instance()->closeEditor(editor);
            this->removeTab(index);
        });

    auto *sessionManager = ProjectExplorer::SessionManager::instance();

    QObject::connect(
        sessionManager,
        &ProjectExplorer::SessionManager::sessionLoaded,
        [editorManager]() {
            for (auto *entry : Core::DocumentModel::entries()) {
                editorManager->activateEditorForEntry(
                    entry, Core::EditorManager::DoNotChangeCurrentEditor);
            }
        });

    auto *prevTabAction =
        new QAction(this->tr("Switch to previous tab"), this);
    auto *prevTabCommand = Core::ActionManager::registerAction(
        prevTabAction,
        Constants::PREV_TAB_ID,
        Core::Context(Core::Constants::C_GLOBAL));
    prevTabCommand->setDefaultKeySequence(
        QKeySequence(this->tr("Ctrl+Shift+J")));
    QObject::connect(prevTabAction, &QAction::triggered, this, [this] {
        const int index = this->currentIndex();
        if (index >= 1) {
            this->setCurrentIndex(index - 1);
        } else {
            this->setCurrentIndex(this->count() - 1);
        }
    });

    auto *nextTabAction = new QAction(this->tr("Switch to next tab"), this);
    Core::Command *nextTabCommand = Core::ActionManager::registerAction(
        nextTabAction,
        Constants::NEXT_TAB_ID,
        Core::Context(Core::Constants::C_GLOBAL));
    nextTabCommand->setDefaultKeySequence(
        QKeySequence(this->tr("Ctrl+Shift+K")));
    QObject::connect(nextTabAction, &QAction::triggered, this, [this] {
        const int index = this->currentIndex();
        if (index < this->count() - 1) {
            this->setCurrentIndex(index + 1);
        } else {
            this->setCurrentIndex(0);
        }
    });
}

void TabBar::contextMenuEvent(QContextMenuEvent *event) {
    const int index = this->tabAt(event->pos());
    if (index == -1) {
        return;
    }

    QScopedPointer<QMenu> menu(new QMenu());

    auto *editor = this->m_editors[to_unsigned(index)];
    auto *entry = Core::DocumentModel::entryForDocument(editor->document());
    Core::EditorManager::addSaveAndCloseEditorActions(
        menu.data(), entry, editor);
    menu->addSeparator();
    Core::EditorManager::addNativeDirAndOpenWithActions(menu.data(), entry);

    menu->exec(this->mapToGlobal(event->pos()));
}

void TabBar::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        const int index = this->tabAt(event->pos());
        if (index < 0 || to_unsigned(index) >= m_editors.size()) {
            return;
        }

        auto *editor = this->m_editors[to_unsigned(index)];
        auto it = std::begin(this->m_editors);
        std::advance(it, index);
        this->m_editors.erase(it);
        Core::EditorManager::instance()->closeEditor(editor);
        this->removeTab(index);
    }

    QTabBar::mouseReleaseEvent(event);

    this->reloadTabTexts();
}

QSize TabBar::tabSizeHint(int index) const {
    auto size = QTabBar::tabSizeHint(index);
    if (size.width() < Constants::TAB_MIN_WIDTH) {
        size.setWidth(Constants::TAB_MIN_WIDTH);
    }
    if (size.height() < Constants::TAB_MIN_HEIGHT) {
        size.setHeight(Constants::TAB_MIN_HEIGHT);
    }
    return size;
}

void TabBar::reloadTabTexts() noexcept {
    for (std::size_t i = 0; i < this->m_editors.size(); ++i) {
        auto *editor = this->m_editors[i];
        auto tabText = editor->document()->displayName();
        if (editor->document()->isModified()) {
            tabText += QLatin1Char('*');
        }
        this->setTabText(static_cast<int>(i), tabText);
    }
}

void TabBar::onEditorOpened(Core::IEditor *editor) noexcept {
    auto *document = editor->document();

    const int index = this->addTab(document->displayName());
    this->setTabIcon(
        index,
        Core::FileIconProvider::icon(document->filePath().toFileInfo()));
    this->setTabToolTip(index, document->filePath().toString());

    this->m_editors.push_back(editor);

    QObject::connect(document, &Core::IDocument::changed, [this, editor]() {
        auto editorIt = std::find(
            std::begin(this->m_editors), std::end(this->m_editors), editor);
        if (editorIt == std::end(this->m_editors)) {
            return;
        }

        auto tabText = editor->document()->displayName();
        if (editor->document()->isModified()) {
            tabText += QLatin1Char('*');
        }
        this->setTabText(static_cast<int>(std::distance(
                             std::begin(this->m_editors), editorIt)),
                         tabText);
    });

    this->reloadTabTexts();
}

void TabBar::onEditorsClosed(QList<Core::IEditor *> editors) noexcept {
    // Avoid calling activateEditor()
    this->blockSignals(true);
    auto cleanup = qScopeGuard([this] { this->blockSignals(false); });

    for (auto *editor : editors) {
        auto editorIt = std::find(
            std::begin(this->m_editors), std::end(this->m_editors), editor);
        if (editorIt == std::end(this->m_editors)) {
            continue;
        }

        this->removeTab(static_cast<int>(
            std::distance(std::begin(this->m_editors), editorIt)));
        this->m_editors.erase(editorIt);
    }

    this->reloadTabTexts();
}

void TabBar::onCurrentEditorChanged(Core::IEditor *editor) noexcept {
    auto editorIt = std::find(
        std::begin(this->m_editors), std::end(this->m_editors), editor);
    if (editorIt == std::end(this->m_editors)) {
        return;
    }
    this->setCurrentIndex(static_cast<int>(
        std::distance(std::begin(this->m_editors), editorIt)));
}

} // namespace Tabs::Internal
