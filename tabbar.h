#pragma once

#include <QTabBar>

#include <vector>

namespace Core {

class IEditor;

}

namespace Tabs::Internal {

class TabBar final : public QTabBar {
    Q_OBJECT

public:
    explicit TabBar(QWidget *parent) noexcept;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    QSize tabSizeHint(int index) const override;

private:
    std::vector<Core::IEditor *> m_editors;

private slots:
    void onEditorOpened(Core::IEditor *editor) noexcept;
    void onEditorsClosed(QList<Core::IEditor *> editors) noexcept;
    void onCurrentEditorChanged(Core::IEditor *editor) noexcept;
};

} // namespace Tabs::Internal
