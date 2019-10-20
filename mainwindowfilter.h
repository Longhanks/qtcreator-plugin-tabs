#pragma once

#include <QEvent>
#include <QObject>

#include <functional>

namespace Tabs::Internal {

class ChangeEventFilter final : public QObject {

public:
    ChangeEventFilter(std::function<void()> onActivationChange,
                      std::function<void()> onClose,
                      QObject *parent = nullptr) noexcept;
    ~ChangeEventFilter() noexcept override;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    std::function<void()> m_onActivationChange;
    std::function<void()> m_onClose;
};

} // namespace Tabs::Internal
