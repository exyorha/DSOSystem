#ifndef GUI__APPLICATION__H
#define GUI__APPLICATION__H

#include <GUI/Object.h>
#include <GUI/ApplicationPlatformInterface.h>

#include <memory>

#include <SkRegion.h>

class ApplicationPlatform;
class View;

class Application final : public Object, private ApplicationPlatformInterface {
public:
    Application(std::unique_ptr<ApplicationPlatform> &&platform);
    ~Application();
    
    inline View *rootView() const { return m_rootView; }
    void setRootView(View *view);

    void update(SkRegion &&region);

    void exec();

    static inline bool sendEvent(Object *object, Event *event) {
        return object->event(event);
    }

    static inline Application *instance() { return m_instance; }

private:
    virtual void drawFrame(const sk_sp<SkSurface> &surface) override;
    virtual void keyPressed(uint32_t key) override;
    virtual void keyReleased(uint32_t key) override;

private:
    static thread_local Application *m_instance;
    std::unique_ptr<ApplicationPlatform> m_platform;
    View *m_rootView;
    SkRegion m_dirtyRegion;
};

#endif