#include <DSO/DSORootView.h>
#include <DSO/DSOScreenView.h>
#include <DSO/DSOAcquisition.h>

#include <GUI/TextView.h>
#include <GUI/LayoutGuide.h>
#include <GUI/LayoutConstraint.h>

DSORootView::DSORootView() : m_acquisition(nullptr) {
	auto topStatusBar = createChild<LayoutGuide>();
	auto topStatusFiller = createChild<LayoutGuide>();

	m_runState = createChild<TextView>();
	m_runState->setText("Run state");
	m_runState->setVisibility(ViewVisibility::Visible);

	m_screenView = createChild<DSOScreenView>();
	m_screenView->setVisibility(ViewVisibility::Visible);

	m_bottomStatusBar = createChild<TextView>();
	m_bottomStatusBar->setText("Bottom Status Bar");
	m_bottomStatusBar->setVisibility(ViewVisibility::Visible);
	
	auto sidePanel = createChild<LayoutGuide>();

	float padding = 4.0f;
	
	createConstraint(1.0f, topStatusBar, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Top, padding);
	createConstraint(1.0f, topStatusBar, LayoutAttribute::Left, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Left, padding);
	createConstraint(1.0f, topStatusBar, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, sidePanel, LayoutAttribute::Left, padding);
	createConstraint(1.0f, m_screenView, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Bottom, padding);
	createConstraint(1.0f, m_screenView, LayoutAttribute::Left, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Left, padding);
	createConstraint(1.0f, m_screenView, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, sidePanel, LayoutAttribute::Left, padding);
	createConstraint(1.0f, m_bottomStatusBar, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, m_screenView, LayoutAttribute::Bottom, padding);
	createConstraint(1.0f, m_bottomStatusBar, LayoutAttribute::Left, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Left, padding);
	createConstraint(1.0f, m_bottomStatusBar, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, sidePanel, LayoutAttribute::Left, padding);
	createConstraint(1.0f, m_bottomStatusBar, LayoutAttribute::Bottom, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Bottom, -padding);
	createConstraint(1.0f, sidePanel, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Top, padding);
	createConstraint(1.0f, sidePanel, LayoutAttribute::Bottom, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Bottom, -padding);
	createConstraint(1.0f, sidePanel, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Right, -padding);

	createConstraint(1.0f, topStatusFiller, LayoutAttribute::Left, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Left, 0.0f);
	createConstraint(1.0f, topStatusFiller, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Top, 0.0f);
	createConstraint(1.0f, topStatusFiller, LayoutAttribute::Bottom, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Bottom, 0.0f);
	createConstraint(1.0f, topStatusFiller, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, m_runState, LayoutAttribute::Left, 0.0f);
	createConstraint(1.0f, m_runState, LayoutAttribute::Top, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Top, 0.0f);
	createConstraint(1.0f, m_runState, LayoutAttribute::Bottom, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Bottom, 0.0f);
	createConstraint(1.0f, m_runState, LayoutAttribute::Right, LayoutConstraint::Relation::Equal, 1.0f, topStatusBar, LayoutAttribute::Right, 0.0f);

	createConstraint(1.0f, m_screenView, LayoutAttribute::Width, LayoutConstraint::Relation::GreaterOrEqual, 1.0f, this, LayoutAttribute::Width, 0.0f, 1.0f);
	createConstraint(1.0f, m_screenView, LayoutAttribute::Height, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Height, 0.0f, 0.5f);
	createConstraint(1.0f, topStatusFiller, LayoutAttribute::Width, LayoutConstraint::Relation::Equal, 1.0f, this, LayoutAttribute::Width, 0.0f, 1.0f);

	setLayoutEnabled(true);
}

DSORootView::~DSORootView() {
	if (m_acquisition) {
		m_acquisition->unregisterEventReceiver(this);
	}
}

void DSORootView::initializeAcquisition(DSOAcquisition &acquisition) {
	if (m_acquisition) {
		m_acquisition->unregisterEventReceiver(this);
	}

	m_acquisition = &acquisition;
	m_acquisition->registerEventReceiver(this);

	m_screenView->initializeAcquisition(acquisition);

	acquisitionStateChanged();
}

void DSORootView::acquisitionStateChanged() {
	switch (m_acquisition->state()) {
	case DSOAcquisition::State::Stopped:
		m_runState->setText("Stopped");
		break;

	case DSOAcquisition::State::Starting:
		m_runState->setText("Starting");
		break;

	case DSOAcquisition::State::Running:
		m_runState->setText("Running");
		break;

	case DSOAcquisition::State::Stopping:
		m_runState->setText("Stopping");
		break;
	}

	//m_runState->update();
	updateLayout();
}
