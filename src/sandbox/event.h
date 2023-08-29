#pragma once
#include "input_defines.h"
// 事件类型枚举
enum class EventType {
    Resize,
    Key,
    MouseButton,
    MouseMove,
    Scroll
};
class EventBase {  // NOLINT(cppcoreguidelines-special-member-functions)
public:
	virtual ~EventBase() = default;

    virtual EventType getType() = delete;

    bool isConsumed() const {
        return consumed;
    }

    void consume() const
    {
        consumed = true;
    }

private:
    mutable bool consumed = false;
};

// 通用 Event 模板
template <EventType Type>
class Event : public EventBase{
public:
    Event() = default;

    static EventType getType()
    {
        return Type;
    }
};


// 特化 Event 模板，为不同事件类型提供特定实现
template <>
class Event<EventType::Resize> : public EventBase {
public:
    Event(int w, int h) : width(w), height(h) {}

    static EventType getType()
    {
        return EventType::MouseMove;
    }

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

private:
    int width;
    int height;
};

template <>
class Event<EventType::Key> : public EventBase {
public:
	Event(const int key_code, const int key_action, const int key_mod)
		: keyCode(key_code),
		  keyAction(key_action),
		  keyMods(key_mod)
	{
	}

	static EventType getType()
	{
        return EventType::Key;
    }

    int getKeyCode() const {
        return keyCode;
    }
    int getKeyAction() const {
        return keyAction;
    }
    int getKeyMod() const {
        return keyMods;
    }

private:
    int keyCode;
    int keyAction;
    int keyMods;
};


template <>
class Event<EventType::MouseMove> : public EventBase {
public:
	Event(const float mouse_x, const float mouse_y, const float delta_x, const float delta_y)
		: mouseX(mouse_x),
		  mouseY(mouse_y),
		  deltaX(delta_x),
		  deltaY(delta_y)
	{

	}

	static EventType getType()
	{
        return EventType::MouseMove;
    }

    float getX() const {
        return mouseX;
    }

    float getY() const {
        return mouseY;
    }

    float getDeltaX() const {
        return deltaX;
    }

    float getDeltaY() const {
        return deltaY;
    }
private:
    float mouseX;
    float mouseY;
    float deltaX;
    float deltaY;
};

template <>
class Event<EventType::Scroll> : public EventBase {
public:
    Event(float x, float y) : deltaX(x), deltaY(y) {}

    static EventType getType()
    {
        return EventType::Scroll;
    }

    float getScrollX() const {
        return deltaX;
    }

    float getScrollY() const {
        return deltaY;
    }

private:
    float deltaX;
    float deltaY;
};



