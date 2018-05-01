#ifndef GUI__LAYOUT_CONSTRAINT__H
#define GUI__LAYOUT_CONSTRAINT__H

#include <GUI/Object.h>

class LayoutItem;

enum class LayoutAttribute : uint32_t {
    Top,
    CenterY,
    Bottom,
    Height,
    Left,
    CenterX,
    Right,
    Width
};

class LayoutConstraint final : public Object {
public:
    enum class Relation : uint32_t {
        Equal,
        LessOrEqual,
        GreaterOrEqual
    };

    LayoutConstraint();
    virtual ~LayoutConstraint();

    inline LayoutItem *leftSideItem() const { return m_leftSideItem; }
    inline void setLeftSideItem(LayoutItem *object) { m_leftSideItem = object; }

    inline LayoutAttribute leftSideAttribute() const { return m_leftSideAttribute; }
    inline void setLeftSideAttribute(LayoutAttribute attribute) { m_leftSideAttribute = attribute; }

    inline float leftSideMultiplier() const { return m_leftSideMultiplier; }
    inline void setLeftSideMultiplier(float multiplier) { m_leftSideMultiplier = multiplier; }

    inline LayoutItem *rightSideItem() const { return m_rightSideItem; }
    inline void setRightSideItem(LayoutItem *object) { m_rightSideItem = object; }

    inline LayoutAttribute rightSideAttribute() const { return m_rightSideAttribute; }
    inline void setRightSideAttribute(LayoutAttribute attribute) { m_rightSideAttribute = attribute; }

    inline float rightSideMultiplier() const { return m_rightSideMultiplier; }
    inline void setRightSideMultiplier(float multiplier) { m_rightSideMultiplier = multiplier; }

    inline Relation relation() const { return m_relation; }
    inline void setRelation(Relation relation) { m_relation = relation; }

    inline float constant() const { return m_constant; }
    inline void setConstant(float constant) { m_constant = constant; }

    inline float penalty() const { return m_penalty; }
    inline void setPenalty(float penalty) { m_penalty = penalty; }

private:
    LayoutItem *m_leftSideItem;
    LayoutAttribute m_leftSideAttribute;
    float m_leftSideMultiplier;
    LayoutItem *m_rightSideItem;
    LayoutAttribute m_rightSideAttribute;
    float m_rightSideMultiplier;
    Relation m_relation;
    float m_constant;
    float m_penalty;
};

#endif
