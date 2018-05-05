#include <GUI/Application.h>
#include <GUI/ApplicationPlatform.h>
#include <GUI/View.h>
#include <GUI/KeyEvent.h>
#include <GUI/FocusEvent.h>

#include <SkSurface.h>
#include <SkCanvas.h>
#include <SkRegion.h>

Application::Application(std::unique_ptr<ApplicationPlatform> &&platform) : m_platform(std::move(platform)), m_rootView(nullptr),
	m_focusView(nullptr) {

    m_platform->setApplicationPlatformInterface(this);
    m_instance = this;
}

Application::~Application() {
    m_instance = nullptr;
}

void Application::setRootView(View *view) {
    m_rootView = view;

    if (view != nullptr) {
        auto info = m_platform->displayInformation();

        view->setGeometry(SkIPoint::Make(0, 0), info.displaySize);
        view->setVisibility(ViewVisibility::Visible);
    }
}

void Application::update(SkRegion &&region) {
    if (m_dirtyRegion.op(region, SkRegion::kUnion_Op)) {
        m_platform->requestFrame();
    }
}

void Application::exec() {
    m_platform->exec();
}

void Application::drawFrame(const sk_sp<SkSurface> &surface) {
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
}

void Application::keyPressed(uint32_t key) {
	KeyEvent event(Event::Type::KeyPressed, key);
	deliverKeyEvent(&event);
}

void Application::keyReleased(uint32_t key) {
	KeyEvent event(Event::Type::KeyReleased, key);
	deliverKeyEvent(&event);
}

void Application::deliverKeyEvent(KeyEvent *event) {
	for (auto view = m_focusView; view; view = static_cast<View *>(view->parent())) {
		sendEvent(view, event);

		if (event->isAccepted())
			break;
	}
}

void Application::setFocusView(View *view) {
	if (view != m_focusView) {
		applicationLog.print(LogPriority::Debug, "changing focus view, new view is %p", view);

		if (m_focusView) {
			FocusEvent event(Event::Type::FocusOut);
			sendEvent(m_focusView, &event);
		}

		m_focusView = view;

		if (m_focusView) {
			FocusEvent event(Event::Type::FocusIn);
			sendEvent(m_focusView, &event);
		}
	}
}

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

Application *Application::m_instance;
LogFacility applicationLog(LogSyslogFacility::User, "Application");
