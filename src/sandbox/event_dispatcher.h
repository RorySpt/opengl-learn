#pragma once
#include "event.h"
#include "delegate.h"
#include <vector>
#include <functional>

struct EventDispatcher
{
    Delegate_MultiCast<const Event<EventType::Resize>&> resizeHandler;
    Delegate_MultiCast<const Event<EventType::Key>&> keyHandler;
    Delegate_MultiCast<const Event<EventType::MouseMove>&> mouseMoveHandler;
    Delegate_MultiCast<const Event<EventType::Scroll>&> scrollHandler;
};






