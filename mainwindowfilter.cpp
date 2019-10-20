#include "mainwindowfilter.h"

namespace Tabs::Internal {

ChangeEventFilter::ChangeEventFilter(std::function<void()> onActivationChange,
                                     std::function<void()> onClose,
                                     QObject *parent) noexcept
    : QObject(parent), m_onActivationChange(std::move(onActivationChange)),
      m_onClose(std::move(onClose)) {}

ChangeEventFilter::~ChangeEventFilter() noexcept = default;

bool ChangeEventFilter::eventFilter([[maybe_unused]] QObject *watched,
                                    QEvent *event) {
    if (event->type() == QEvent::ActivationChange) {
        this->m_onActivationChange();
    } else if (event->type() == QEvent::Close) {
        this->m_onClose();
    }
    return false;
}

} // namespace Tabs::Internal
