#ifndef GUI__APPLICATION__H
#define GUI__APPLICATION__H

#include <GUI/Object.h>
#include <GUI/ApplicationPlatformInterface.h>
#include <Logging/LogFacility.h>

#include <memory>
#include <functional>
#include <mutex>
#include <list>

#include <SkRegion.h>

class ApplicationPlatform;
class View;
class KeyEvent;

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

	inline View *focusView() const {
		return m_focusView;
	}

	void setFocusView(View *view);

	void defer(std::function<void()> &&function);

private:
    virtual void drawFrame(const sk_sp<SkSurface> &surface) override;
    virtual void keyPressed(uint32_t key) override;
    virtual void keyReleased(uint32_t key) override;
	virtual void dispatchDeferred() override;

private:
	void deliverKeyEvent(KeyEvent *event);

    static Application *m_instance;
    std::unique_ptr<ApplicationPlatform> m_platform;
    View *m_rootView, *m_focusView;
    SkRegion m_dirtyRegion;
	std::mutex m_deferQueueMutex;
	std::list<std::function<void()>> m_deferQueue;
};

extern LogFacility applicationLog;

#endif
