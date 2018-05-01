#include <GUI/Object.h>
#include <GUI/ChildEvent.h>
#include <GUI/Application.h>

#include <algorithm>
#include <assert.h>

Object::Object() : m_parent(nullptr) {

}

Object::~Object() {

}

void Object::addChild(std::unique_ptr<Object> &&child) {
    assert(child->m_parent == nullptr);

    auto rawChild = child.get();
    
    m_children.emplace_back(std::move(child));

    rawChild->m_parent = this;
    
    ChildEvent event(Event::Type::ChildAdded, rawChild);
    Application::sendEvent(this, &event);
}

std::unique_ptr<Object> Object::removeChild(Object *child) {
    auto childIt = std::remove_if(m_children.begin(), m_children.end(),
        [child](const std::unique_ptr<Object> &item) {
        return item.get() == child;
    });

    assert(childIt != m_children.end() && childIt + 1 == m_children.end());

    auto childPtr = std::move(*childIt);

    assert(childPtr->m_parent == this);

    auto rawChild = childPtr.get();

    m_children.erase(childIt, m_children.end());
    
    rawChild->m_parent = nullptr;

    ChildEvent event(Event::Type::ChildRemoved, rawChild);
    Application::sendEvent(this, &event);

    return std::move(childPtr);
}

bool Object::event(Event *event) {
    switch (event->type()) {
    case Event::Type::ChildAdded:
        [[fallthrough]];
    case Event::Type::ChildRemoved:
        childEvent(static_cast<ChildEvent *>(event));
        return true;

    default:
        return false;
    }
}

void Object::childEvent(ChildEvent *event) {

}
