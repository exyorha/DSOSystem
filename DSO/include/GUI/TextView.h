#ifndef GUI__TEXT_VIEW__H
#define GUI__TEXT_VIEW__H

#include <GUI/View.h>

#include <string>

class PaintEvent;

class TextView final : public View {
public:
    TextView();
    virtual ~TextView();

    inline const std::string &text() const { return m_text; }
    void setText(const std::string &text);

    virtual SkISize minimumSizeHint() const override;

protected:
    virtual void paintEvent(PaintEvent *event) override;

private:
    SkISize sizeForWidth(int32_t width) const;

    std::string m_text;
};

#endif
