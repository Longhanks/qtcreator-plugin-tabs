#pragma once

#include <QTabBar>

namespace Core {

class IEditor;

}

namespace Tabs::Internal {

class TabBar : public QTabBar {
    Q_OBJECT

public:
    explicit TabBar(QWidget *parent = nullptr);

private slots:
    void activateEditor(int index);

    void addEditorTab(Core::IEditor *editor);
    void removeEditorTabs(QList<Core::IEditor *> editors);
    void selectEditorTab(Core::IEditor *editor);

    void closeTab(int index);

    void prevTabAction();
    void nextTabAction();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    QSize tabSizeHint(int index) const override;

private:
    QList<Core::IEditor *> m_editors;
};

} // namespace Tabs::Internal
