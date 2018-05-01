#ifndef GUI__OBJECT__H
#define GUI__OBJECT__H

#include <memory>
#include <vector>

class Event;
class ChildEvent;

class Object {
public:
    Object();
    virtual ~Object();

    Object(const Object &other) = delete;
    Object &operator =(const Object &other) = delete;

    void addChild(std::unique_ptr<Object> &&child);
    std::unique_ptr<Object> removeChild(Object *child);

    template<typename T, typename... Args>
    T *createChild(Args&&... args) {
        auto ptr = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        auto rawPtr = ptr.get();

        addChild(std::move(ptr));

        return rawPtr;
    }

    inline const std::vector<std::unique_ptr<Object>> &children() const { return m_children; }
    inline Object *parent() const { return m_parent; }

    virtual bool event(Event *event);
    
    template<typename T, typename Lambda>
    void enumerateChildObjects(Lambda &&lambda) {
        for (const auto &child : m_children) {
            auto typedChild = dynamic_cast<T *>(child.get());
            if (typedChild) {
                lambda(typedChild);
            }
        }
    }

protected:
    virtual void childEvent(ChildEvent *event);

private:
    std::vector<std::unique_ptr<Object>> m_children;
    Object *m_parent;
};

#endif
