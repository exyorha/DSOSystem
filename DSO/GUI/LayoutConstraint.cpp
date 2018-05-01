#include <GUI/LayoutConstraint.h>

LayoutConstraint::LayoutConstraint() : m_leftSideItem(nullptr), m_leftSideAttribute(LayoutAttribute::Top),
    m_leftSideMultiplier(1.0f), m_rightSideItem(nullptr), m_rightSideAttribute(LayoutAttribute::Top),
    m_rightSideMultiplier(1.0f), m_constant(0.0f), m_penalty(0.0f) {

}

LayoutConstraint::~LayoutConstraint() {

}
