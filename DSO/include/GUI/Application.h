#ifndef GUI__APPLICATION__H
#define GUI__APPLICATION__H

#include <GUI/ApplicationPlatformInterface.h>
#include <Logging/LogFacility.h>
#include <GUI/DisplayInformation.h>

#include <memory>
#include <functional>
#include <mutex>
#include <list>
#include <vector>

#include <GUI/NuklearPlatform.h>
#include <GUI/PCFFont.h>

class ApplicationPlatform;
class Window;

class Application final : private ApplicationPlatformInterface {
public:
    Application(std::unique_ptr<ApplicationPlatform> &&platform);
    ~Application();
    
	void update();

    void exec();
	
    static inline Application *instance() { return m_instance; }
	
	void defer(std::function<void()> &&function);

	void addWindow(std::unique_ptr<Window> &&window);
	void removeWindow(Window *window);

	DisplayInformation displayInformation() const;

private:
    virtual void drawFrame(pixman_image_t *display) override;
    virtual void keyPressed(uint32_t key) override;
    virtual void keyReleased(uint32_t key) override;
	virtual void dispatchDeferred() override;

private:
	struct NuklearContext final : nk_context {
	public:
		NuklearContext(const nk_user_font *defaultFont);
		~NuklearContext();

		NuklearContext(const NuklearContext &other) = delete;
		NuklearContext &operator =(const NuklearContext &other) = delete;
	};

    static Application *m_instance;
    std::unique_ptr<ApplicationPlatform> m_platform;
	std::mutex m_deferQueueMutex;
	std::list<std::function<void()>> m_deferQueue;
	std::vector<std::unique_ptr<Window>> m_windows;
	PCFFont m_mainFont;
	NuklearContext m_nk;
};

extern LogFacility applicationLog;

#endif
