#include <GUI/View.h>
#include <GUI/Application.h>
#include <GUI/MoveEvent.h>
#include <GUI/ResizeEvent.h>
#include <GUI/PaintEvent.h>
#include <GUI/KeyEvent.h>
#include <GUI/FocusEvent.h>

#include <SkSurface.h>
#include <SkCanvas.h>

#include <unordered_map>

#pragma warning(push)
#pragma warning(disable:5033)
#include <ClpSimplex.hpp>
#pragma warning(pop)

#include <assert.h>

View::View() : m_lastVisibleGeometry(SkIRect::MakeEmpty()), m_visibleOnScreen(false), m_layoutEnabled(false), m_focusPolicy(false), m_focusPending(false) {

}

View::~View() {
	clearFocus();
}

void View::pushGeometryUpdates(const SkIRect &current, const SkIRect &geometry) {

	if (current.x() != geometry.x() || current.y() != geometry.y()) {
		MoveEvent moveEvent(SkIPoint::Make(current.x(), current.y()), SkIPoint::Make(geometry.x(), geometry.y()));
		Application::sendEvent(this, &moveEvent);
	}

	if (current.size() != geometry.size()) {
		ResizeEvent resizeEvent(current.size(), geometry.size());
		Application::sendEvent(this, &resizeEvent);

		if (m_layoutEnabled) {
			updateLayout();
		}
	}
}

void View::setGeometryImpl(const SkIRect &geometry) {
	auto current = View::geometry();
	
	LayoutItem::setGeometryImpl(geometry);

	if (isVisible()) {
		m_lastVisibleGeometry = geometry;

		pushGeometryUpdates(current, geometry);

		if (current != geometry) {
			update();
		}
	}
}

void View::setVisibilityImpl(ViewVisibility visibility) {
	auto current = View::visibility();

	bool wasVisible = m_visibleOnScreen;

	if (wasVisible && visibility != ViewVisibility::Visible) {
		update();
	}

	LayoutItem::setVisibilityImpl(visibility);

	bool nowVisible = isVisibleOnScreen();
	m_visibleOnScreen = nowVisible;

	if (!wasVisible && nowVisible) {
		update();
	}

	if (wasVisible != nowVisible) {
		if (nowVisible) {
			pushGeometryUpdates(m_lastVisibleGeometry, geometry());
			m_lastVisibleGeometry = geometry();

			Event showEvent(Event::Type::Show);
			Application::sendEvent(this, &showEvent);

			enumerateChildObjects<View>([&](View *view) {
				view->setVisibility(view->visibility());
			});

			if (m_focusPending) {
				m_focusPending = false;

				setFocus();
			}
		}
		else {
			enumerateChildObjects<View>([&](View *view) {
				view->setVisibility(view->visibility());
			});

			Event hideEvent(Event::Type::Hide);
			Application::sendEvent(this, &hideEvent);
		}
	}
}

bool View::event(Event *event) {
	switch (event->type()) {
	case Event::Type::Move:
		moveEvent(static_cast<MoveEvent *>(event));
		return true;

	case Event::Type::Resize:
		resizeEvent(static_cast<ResizeEvent *>(event));
		return true;

	case Event::Type::Show:
		showEvent(event);
		return true;

	case Event::Type::Hide:
		hideEvent(event);
		return true;

	case Event::Type::Paint:
		paintEvent(static_cast<PaintEvent *>(event));
		return true;

	case Event::Type::KeyPressed:
		keyPressEvent(static_cast<KeyEvent *>(event));
		return true;

	case Event::Type::KeyReleased:
		keyReleaseEvent(static_cast<KeyEvent *>(event));
		return true;
		
	case Event::Type::FocusIn:
		focusInEvent(static_cast<FocusEvent *>(event));
		return true;

	case Event::Type::FocusOut:
		focusOutEvent(static_cast<FocusEvent *>(event));
		return true;

	default:
		return Object::event(event);
	}
}

void View::moveEvent(MoveEvent *event) {

}

void View::resizeEvent(ResizeEvent *event) {

}

void View::showEvent(Event *event) {

}

void View::hideEvent(Event *event) {

}

void View::paintEvent(PaintEvent *event) {
	auto canvas = event->canvas();

	canvas->clear(SK_ColorGRAY);
	
	SkPaint paint;
	paint.setColor(SK_ColorRED);
	paint.setStyle(SkPaint::kStroke_Style);
	canvas->drawRect(SkRect::MakeXYWH(0.5f, 0.5f, width() - 1.5f, height() - 1.5f), paint);
}

void View::update(SkRegion &&region) {
	View *discoveredRootView = nullptr;
	auto application = Application::instance();

	for (View *view = this; view; view = static_cast<View *>(view->parent())) {
		discoveredRootView = view;

		if(!view->isVisible())
			return;

		region.translate(view->x(), view->y());
	}

	assert(discoveredRootView == application->rootView());

	application->update(std::move(region));
}

bool View::isVisibleOnScreen() const {
	const View *discoveredRootView = nullptr;
	auto application = Application::instance();

	for (const View *view = this; view; view = static_cast<const View *>(view->parent())) {
		discoveredRootView = view;

		if (!view->isVisible())
			return false;
	}

	assert(discoveredRootView == application->rootView());

	return true;
}

void View::drawView(const sk_sp<SkSurface> &surface, const SkIPoint &translation, const SkRegion &visibleRegion) {
	SkRegion thisWindowRegion;
	
	auto translatedPosition = translation + position();

	thisWindowRegion.op(visibleRegion, SkIRect::MakeXYWH(translatedPosition.x(), translatedPosition.y(), width(), height()), SkRegion::kIntersect_Op);

	enumerateChildObjects<View>([&](View *view) {
		if (view->isVisible()) {
			thisWindowRegion.op(view->geometry().makeOffset(translation.x(), translation.y()), SkRegion::kDifference_Op);
			view->drawView(surface, translatedPosition, visibleRegion);
		}
	});

	if (!thisWindowRegion.isEmpty()) {
		auto canvas = surface->getCanvas();
		int saveCount = canvas->save();

		canvas->clipRegion(thisWindowRegion);
		canvas->translate(static_cast<SkScalar>(translatedPosition.x()), static_cast<SkScalar>(translatedPosition.y()));

		PaintEvent event(canvas);
		Application::sendEvent(this, &event);

		canvas->restoreToCount(saveCount);
	}
}

LayoutConstraint *View::createConstraint(
	float leftMultiplier, LayoutItem *leftItem, LayoutAttribute leftAttribute,
	LayoutConstraint::Relation relation,
	float rightMultiplier, LayoutItem *rightItem, LayoutAttribute rightAttribute, float constant, float penalty) {

	auto constraint = createChild<LayoutConstraint>();
	constraint->setLeftSideMultiplier(leftMultiplier);
	constraint->setLeftSideItem(leftItem);
	constraint->setLeftSideAttribute(leftAttribute);
	constraint->setRelation(relation);
	constraint->setRightSideMultiplier(rightMultiplier);
	constraint->setRightSideItem(rightItem);
	constraint->setRightSideAttribute(rightAttribute);
	constraint->setConstant(constant);
	constraint->setPenalty(penalty);

	return constraint;
}

void View::setLayoutEnabled(bool enabled) {
	bool wasEnabled = m_layoutEnabled;
	m_layoutEnabled = enabled;

	if (wasEnabled != enabled && enabled) {
		updateLayout();
	}
}


void View::updateLayout() {
	solveLayout(LayoutSolveMode::Update);
}

SkISize View::solveLayout(LayoutSolveMode mode) {
	std::unordered_map<LayoutConstraint *, int> constraints;
	std::unordered_map<LayoutItem *, int> views;

	views.emplace(this, 0);

	enumerateChildObjects<LayoutConstraint>([&](LayoutConstraint *constraint) {
		constraints.emplace(constraint, static_cast<int>(constraints.size()));
	});

	enumerateChildObjects<LayoutItem>([&](LayoutItem *view) {
		views.emplace(view, static_cast<int>(views.size()));
	});

	viewLog.print(LogPriority::Debug, "updating layout: %zu constraints, %zu views, width: %d, height: %d", constraints.size(), views.size(), width(), height());

	enum {
		ModelColumnTop = 0,
		ModelColumnBottom = 1,
		ModelColumnLeft = 2,
		ModelColumnRight = 3,

		ModelColumns
	};
	
	CoinPackedMatrix matrix2;
	ClpPackedMatrix matrix(matrix2);
	matrix.setDimensions(static_cast<int>(constraints.size() + views.size() * 2), static_cast<int>(views.size() * ModelColumns + constraints.size() * 2));

	std::vector<double> columnLowerBound(views.size() * ModelColumns + constraints.size() * 2, 0.0);
	std::vector<double> columnUpperBound(views.size() * ModelColumns + constraints.size() * 2, std::numeric_limits<double>::infinity());

	std::vector<double> rowLowerBound(constraints.size() + views.size() * 2, -std::numeric_limits<double>::infinity());
	std::vector<double> rowUpperBound(constraints.size() + views.size() * 2, std::numeric_limits<double>::infinity());

	std::vector<double> objective(views.size() * ModelColumns + constraints.size() * 2, 0.0);

	columnLowerBound[ModelColumnTop] = 0.0f;
	columnLowerBound[ModelColumnLeft] = 0.0f;
	columnUpperBound[ModelColumnTop] = 0.0f;
	columnUpperBound[ModelColumnLeft] = 0.0f;

	if (mode == LayoutSolveMode::Update) {
		columnLowerBound[ModelColumnBottom] = height();
		columnLowerBound[ModelColumnRight] = width();
		columnUpperBound[ModelColumnBottom] = height();
		columnUpperBound[ModelColumnRight] = width();
	}
	else {
		columnLowerBound[ModelColumnBottom] = 0.0f;
		columnLowerBound[ModelColumnRight] = 0.0f;
		columnUpperBound[ModelColumnBottom] = std::numeric_limits<int32_t>::max();
		columnUpperBound[ModelColumnRight] = std::numeric_limits<int32_t>::max();
	}

	if(mode == LayoutSolveMode::Update) {
		for (const auto &pair : views) {
			objective[views.size() * ModelColumns + pair.second] = 1.0;
		}
	}
	else if (mode == LayoutSolveMode::MaximumSize) {
		objective[ModelColumnBottom] = -1.0f;
		objective[ModelColumnRight] = -1.0f;
	}
	else if (mode == LayoutSolveMode::MinimumSize) {
		objective[ModelColumnBottom] = 1.0f;
		objective[ModelColumnRight] = 1.0f;
	}
	
	for (const auto &pair : constraints) {
		auto row = pair.second;
		auto constraint = pair.first;

		auto leftItemIt = views.find(constraint->leftSideItem());
		assert(leftItemIt != views.end());
		auto leftItemIndex = leftItemIt->second;

		auto rightItemIt = views.find(constraint->rightSideItem());
		assert(rightItemIt != views.end());
		auto rightItemIndex = rightItemIt->second;

		float leftMul = constraint->leftSideMultiplier();
		float rightMul = constraint->rightSideMultiplier();

		switch (constraint->leftSideAttribute()) {
		case LayoutAttribute::Top:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnTop, constraint->leftSideMultiplier());
			break;

		case LayoutAttribute::CenterY:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnTop, constraint->leftSideMultiplier() * 0.5f);
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnBottom, constraint->leftSideMultiplier() * 0.5f);
			break;

		case LayoutAttribute::Bottom:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnBottom, constraint->leftSideMultiplier());
			break;

		case LayoutAttribute::Height:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnTop, -constraint->leftSideMultiplier());
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnBottom, constraint->leftSideMultiplier());
			break;

		case LayoutAttribute::Left:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnLeft, constraint->leftSideMultiplier());
			break;

		case LayoutAttribute::CenterX:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnLeft, constraint->leftSideMultiplier() * 0.5f);
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnRight, constraint->leftSideMultiplier() * 0.5f);
			break;

		case LayoutAttribute::Right:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnRight, constraint->leftSideMultiplier());
			break;

		case LayoutAttribute::Width:
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnLeft, -constraint->leftSideMultiplier());
			matrix.modifyCoefficient(pair.second, leftItemIndex * ModelColumns + ModelColumnRight, constraint->leftSideMultiplier());
			break;
		}

		switch (constraint->rightSideAttribute()) {
		case LayoutAttribute::Top:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnTop, -constraint->rightSideMultiplier());
			break;

		case LayoutAttribute::CenterY:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnTop, -constraint->rightSideMultiplier() * 0.5f);
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnBottom, -constraint->rightSideMultiplier() * 0.5f);
			break;

		case LayoutAttribute::Bottom:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnBottom, -constraint->rightSideMultiplier());
			break;

		case LayoutAttribute::Height:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnTop, constraint->rightSideMultiplier());
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnBottom, -constraint->rightSideMultiplier());
			break;

		case LayoutAttribute::Left:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnLeft, -constraint->rightSideMultiplier());
			break;

		case LayoutAttribute::CenterX:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnLeft, -constraint->rightSideMultiplier() * 0.5f);
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnRight, -constraint->rightSideMultiplier() * 0.5f);
			break;

		case LayoutAttribute::Right:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnRight, -constraint->rightSideMultiplier());
			break;

		case LayoutAttribute::Width:
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnLeft, constraint->rightSideMultiplier());
			matrix.modifyCoefficient(pair.second, rightItemIndex * ModelColumns + ModelColumnRight,- constraint->rightSideMultiplier());
			break;
		}

		switch (constraint->relation()) {
		case LayoutConstraint::Relation::Equal:
			rowUpperBound[pair.second] = constraint->constant();
			rowLowerBound[pair.second] = constraint->constant();
			break;

		case LayoutConstraint::Relation::LessOrEqual:
			rowUpperBound[pair.second] = constraint->constant();
			break;

		case LayoutConstraint::Relation::GreaterOrEqual:
			rowLowerBound[pair.second] = constraint->constant();
			break;
		}

		matrix.modifyCoefficient(pair.second, views.size() * ModelColumns, constraint->penalty());
	}

	for (const auto &pair : views) {
		if (pair.first == this)
			continue;

		matrix.modifyCoefficient(constraints.size() + pair.second, pair.second * ModelColumns + ModelColumnLeft, -1.0);
		matrix.modifyCoefficient(constraints.size() + pair.second, pair.second * ModelColumns + ModelColumnRight, 1.0);
		matrix.modifyCoefficient(constraints.size() + views.size() + pair.second, pair.second * ModelColumns + ModelColumnTop, -1.0);
		matrix.modifyCoefficient(constraints.size() + views.size() + pair.second, pair.second * ModelColumns + ModelColumnBottom, 1.0);
		
		auto minimumSize = pair.first->effectiveMinimumSize();
		auto maximumSize = pair.first->effectiveMaximumSize();

		rowLowerBound[constraints.size() + pair.second] = minimumSize.width();
		rowLowerBound[constraints.size() + views.size() + pair.second] = minimumSize.height();
		rowUpperBound[constraints.size() + pair.second] = maximumSize.width();
		rowUpperBound[constraints.size() + views.size() + pair.second] = maximumSize.height();
	}

	ClpSimplex model;
	model.loadProblem(matrix, columnLowerBound.data(), columnUpperBound.data(), objective.data(), rowLowerBound.data(), rowUpperBound.data(), nullptr);
	model.setOptimizationDirection(1.0);
	model.setLogLevel(0);

	for (size_t variable = 0; variable < views.size() * ModelColumns; variable++) {
		model.setInteger(static_cast<int>(variable));
	}
	
	model.primal();

	auto solution = model.primalColumnSolution();

	if (mode == LayoutSolveMode::Update) {
		for (const auto pair : views) {
			if (pair.first == this)
				continue;

			pair.first->setGeometry(SkIRect::MakeLTRB(
				static_cast<int32_t>(solution[pair.second * ModelColumns + ModelColumnLeft]),
				static_cast<int32_t>(solution[pair.second * ModelColumns + ModelColumnTop]),
				static_cast<int32_t>(solution[pair.second * ModelColumns + ModelColumnRight]),
				static_cast<int32_t>(solution[pair.second * ModelColumns + ModelColumnBottom])
			));
		}
	}

	return SkISize::Make(static_cast<int32_t>(solution[ModelColumnRight]), static_cast<int32_t>(solution[ModelColumnBottom]));
}

SkISize View::minimumSizeHint() const {
	if (m_layoutEnabled) {
		return const_cast<View *>(this)->solveLayout(LayoutSolveMode::MinimumSize);
	}
	else {
		return LayoutItem::minimumSizeHint();
	}
}

SkISize View::maximumSizeHint() const {
	if (m_layoutEnabled) {
		return const_cast<View *>(this)->solveLayout(LayoutSolveMode::MaximumSize);
	}
	else {
		return LayoutItem::maximumSizeHint();
	}
}

void View::keyPressEvent(KeyEvent *event) {
	event->ignore();
}

void View::keyReleaseEvent(KeyEvent *event) {
	event->ignore();
}

void View::focusInEvent(FocusEvent *event) {

}

void View::focusOutEvent(FocusEvent *event) {

}

bool View::focusNextPrevChild(bool next) {
	return false;
}

void View::setFocusPolicy(bool focusPolicy) {
	m_focusPolicy = focusPolicy;

	if (!focusPolicy) {
		clearFocus();
	}
}

void View::clearFocus() {
	if (hasFocus()) {
		Application::instance()->setFocusView(nullptr);
	}
	m_focusPending = false;
}

void View::setFocus() {
	if (m_focusPolicy) {
		if (isVisible()) {
			Application::instance()->setFocusView(this);
		}
		else {
			m_focusPending = true;
		}
	}
}

bool View::hasFocus() const {
	return Application::instance()->focusView() == this;
}

LogFacility viewLog(LogSyslogFacility::User, "View");
