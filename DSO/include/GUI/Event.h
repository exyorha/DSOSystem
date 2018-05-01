#ifndef GUI__EVENT__H
#define GUI__EVENT__H

#include <stdint.h>

class Event {
public:
    enum class Type : uint32_t {
        ChildAdded,
        ChildRemoved,
        Move,
        Resize,
        Show,
        Hide,
        Paint
    };

    explicit Event(Type type);
    virtual ~Event();

    Event(const Event &event) = delete;
    Event &operator =(const Event &event) = delete;

    inline Type type() const { return m_type; }

private:
    Type m_type;
};

#endif
