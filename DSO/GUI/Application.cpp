#include <GUI/Application.h>
#include <GUI/ApplicationPlatform.h>
#include <GUI/Window.h>
#include <GUI/NuklearRenderer.h>

#include <assert.h>

#include <algorithm>

#include <DSOResources.h>

Application::Application(std::unique_ptr<ApplicationPlatform> &&platform) : m_platform(std::move(platform)), 
	m_mainFont(helvR12, sizeof(helvR12)), m_nk(m_mainFont.font()) {

    m_platform->setApplicationPlatformInterface(this);
    m_instance = this;
}

Application::~Application() {
    m_instance = nullptr;
}

void Application::update() {
    m_platform->requestFrame();
}

void Application::exec() {
    m_platform->exec();
}

DisplayInformation Application::displayInformation() const {
	return m_platform->displayInformation();
}

void Application::drawFrame(pixman_image_t *display) {
	for (auto it = m_windows.begin(); it != m_windows.end(); ) {
		auto &win = **it;
		auto hasBody = nk_begin_titled(
			&m_nk, win.name(), win.title().c_str(), win.bounds(), win.flags()
		);

		if (hasBody) {
			win.execute(&m_nk);
		}

		nk_end(&m_nk);

		it++;
	}

	NuklearRenderer renderer(display);
	renderer.renderContext(&m_nk);

	nk_clear(&m_nk);

#if 0
    if (m_dirtyRegion.isEmpty()) {
        m_platform->keepPreviousFrame();

        return;
    }

    while (!m_dirtyRegion.isEmpty()) {
        auto canvas = surface->getCanvas();

        auto info = m_platform->displayInformation();
        SkRegion validScreen;
        if (validScreen.op(SkIRect::MakeXYWH(0, 0, info.displaySize.width(), info.displaySize.height()), m_dirtyRegion, SkRegion::kDifference_Op)) {
            m_platform->keepPreviousFrame();
        }

        if (m_rootView && m_rootView->visibility() == ViewVisibility::Visible) {
            SkRegion rootGeometry(m_rootView->geometry());

            rootGeometry.op(m_dirtyRegion, SkRegion::kIntersect_Op);

            m_dirtyRegion.setEmpty();

            m_rootView->drawView(surface, SkIPoint::Make(0, 0), rootGeometry);
        }
        else {
            m_dirtyRegion.setEmpty();
        }
    }
#endif
}

void Application::keyPressed(uint32_t key) {
/*	KeyEvent event(Event::Type::KeyPressed, key);
	deliverKeyEvent(&event);*/
}

void Application::keyReleased(uint32_t key) {
/*	KeyEvent event(Event::Type::KeyReleased, key);
	deliverKeyEvent(&event);*/
}

#if 0
void Application::deliverKeyEvent(KeyEvent *event) {
	for (auto view = m_focusView; view; view = static_cast<View *>(view->parent())) {
		sendEvent(view, event);

		if (event->isAccepted())
			break;
	}
}
#endif

void Application::defer(std::function<void()> &&function) {
	{
		std::unique_lock<std::mutex> locker(m_deferQueueMutex);
		m_deferQueue.emplace_back(std::move(function));
	}
	m_platform->postDefer();
}

void Application::dispatchDeferred() {
	std::unique_lock<std::mutex> locker(m_deferQueueMutex);

	while (!m_deferQueue.empty()) {
		auto func = std::move(*m_deferQueue.begin());

		m_deferQueue.pop_front();

		locker.unlock();

		func();

		locker.lock();
	}
}

void Application::addWindow(std::unique_ptr<Window> &&window) {
	m_windows.emplace_back(std::move(window));
	update();
}

void Application::removeWindow(Window *window) {
	m_windows.erase(std::remove_if(m_windows.begin(), m_windows.end(), [=](const std::unique_ptr<Window> &win) { return win.get() == window; }), m_windows.end());
	update();
}

Application::NuklearContext::NuklearContext(const nk_user_font *defaultFont) {
	bool result = nk_init_default(this, defaultFont);
	assert(result);
}

Application::NuklearContext::~NuklearContext() {

}

Application *Application::m_instance;
LogFacility applicationLog(LogSyslogFacility::User, "Application");
