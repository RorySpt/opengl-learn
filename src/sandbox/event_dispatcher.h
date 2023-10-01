#pragma once
#include "event.h"
#include "delegate.h"

struct EventDispatcher
{
    using ResizeHandler = Delegate_MultiCast<const Event<EventType::Resize>>;
    using KeyHandler = Delegate_MultiCast<const Event<EventType::Key>>;
    using MouseMoveHandler = Delegate_MultiCast<const Event<EventType::MouseMove>>;
    using ScrollHandler = Delegate_MultiCast<const Event<EventType::Scroll>>;

    ResizeHandler resizeHandler;
    KeyHandler keyHandler;
    MouseMoveHandler mouseMoveHandler;
    ScrollHandler scrollHandler;
};






