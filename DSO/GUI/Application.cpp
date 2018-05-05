#include <GUI/Application.h>
#include <GUI/ApplicationPlatform.h>
#include <GUI/View.h>

#include <SkSurface.h>
#include <SkCanvas.h>
#include <SkRegion.h>

Application::Application(std::unique_ptr<ApplicationPlatform> &&platform) : m_platform(std::move(platform)), m_rootView(nullptr) {
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
	applicationLog.print(LogPriority::Debug, "key pressed: %u", key);
}

void Application::keyReleased(uint32_t key) {
	applicationLog.print(LogPriority::Debug, "key released: %u", key);
}

thread_local Application *Application::m_instance;
LogFacility applicationLog(LogSyslogFacility::User, "Application");
