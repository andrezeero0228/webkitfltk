/*
 * Copyright (C) 2011, 2014-2015 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ScrollController.h"

#include "PlatformWheelEvent.h"
#include "WebCoreSystemInterface.h"
#include <sys/sysctl.h>
#include <sys/time.h>

#if ENABLE(CSS_SCROLL_SNAP)
#include "AxisScrollSnapAnimator.h"
#include "ScrollableArea.h"
#endif

#if ENABLE(RUBBER_BANDING)

static NSTimeInterval systemUptime()
{
    if ([[NSProcessInfo processInfo] respondsToSelector:@selector(systemUptime)])
        return [[NSProcessInfo processInfo] systemUptime];

    // Get how long system has been up. Found by looking getting "boottime" from the kernel.
    static struct timeval boottime = {0, 0};
    if (!boottime.tv_sec) {
        int mib[2] = {CTL_KERN, KERN_BOOTTIME};
        size_t size = sizeof(boottime);
        if (-1 == sysctl(mib, 2, &boottime, &size, 0, 0))
            boottime.tv_sec = 0;
    }
    struct timeval now;
    if (boottime.tv_sec && -1 != gettimeofday(&now, 0)) {
        struct timeval uptime;
        timersub(&now, &boottime, &uptime);
        NSTimeInterval result = uptime.tv_sec + (uptime.tv_usec / 1E+6);
        return result;
    }
    return 0;
}


namespace WebCore {

static const float scrollVelocityZeroingTimeout = 0.10f;
static const float rubberbandDirectionLockStretchRatio = 1;
static const float rubberbandMinimumRequiredDeltaBeforeStretch = 10;

static float elasticDeltaForTimeDelta(float initialPosition, float initialVelocity, float elapsedTime)
{
    return wkNSElasticDeltaForTimeDelta(initialPosition, initialVelocity, elapsedTime);
}

static float elasticDeltaForReboundDelta(float delta)
{
    return wkNSElasticDeltaForReboundDelta(delta);
}

static float reboundDeltaForElasticDelta(float delta)
{
    return wkNSReboundDeltaForElasticDelta(delta);
}

static float scrollWheelMultiplier()
{
    static float multiplier = -1;
    if (multiplier < 0) {
        multiplier = [[NSUserDefaults standardUserDefaults] floatForKey:@"NSScrollWheelMultiplier"];
        if (multiplier <= 0)
            multiplier = 1;
    }
    return multiplier;
}

ScrollController::ScrollController(ScrollControllerClient* client)
    : m_client(client)
    , m_lastMomentumScrollTimestamp(0)
    , m_startTime(0)
    , m_snapRubberbandTimer(RunLoop::current(), this, &ScrollController::snapRubberBandTimerFired)
#if ENABLE(CSS_SCROLL_SNAP) && PLATFORM(MAC)
    , m_horizontalScrollSnapTimer(RunLoop::current(), this, &ScrollController::horizontalScrollSnapTimerFired)
    , m_verticalScrollSnapTimer(RunLoop::current(), this, &ScrollController::verticalScrollSnapTimerFired)
#endif
    , m_inScrollGesture(false)
    , m_momentumScrollInProgress(false)
    , m_ignoreMomentumScrolls(false)
    , m_snapRubberbandTimerIsActive(false)
{
}

bool ScrollController::handleWheelEvent(const PlatformWheelEvent& wheelEvent)
{
#if ENABLE(CSS_SCROLL_SNAP) && PLATFORM(MAC)
    if (!processWheelEventForScrollSnap(wheelEvent))
        return false;
#endif
    if (wheelEvent.phase() == PlatformWheelEventPhaseBegan) {
        // First, check if we should rubber-band at all.
        if (m_client->pinnedInDirection(FloatSize(-wheelEvent.deltaX(), 0))
            && !shouldRubberBandInHorizontalDirection(wheelEvent))
            return false;

        m_inScrollGesture = true;
        m_momentumScrollInProgress = false;
        m_ignoreMomentumScrolls = false;
        m_lastMomentumScrollTimestamp = 0;
        m_momentumVelocity = FloatSize();

        IntSize stretchAmount = m_client->stretchAmount();
        m_stretchScrollForce.setWidth(reboundDeltaForElasticDelta(stretchAmount.width()));
        m_stretchScrollForce.setHeight(reboundDeltaForElasticDelta(stretchAmount.height()));
        m_overflowScrollDelta = FloatSize();

        stopSnapRubberbandTimer();

        return true;
    }

    if (wheelEvent.phase() == PlatformWheelEventPhaseEnded) {
        snapRubberBand();
        return true;
    }

    bool isMomentumScrollEvent = (wheelEvent.momentumPhase() != PlatformWheelEventPhaseNone);
    if (m_ignoreMomentumScrolls && (isMomentumScrollEvent || m_snapRubberbandTimerIsActive)) {
        if (wheelEvent.momentumPhase() == PlatformWheelEventPhaseEnded) {
            m_ignoreMomentumScrolls = false;
            return true;
        }
        return false;
    }

    float deltaX = m_overflowScrollDelta.width();
    float deltaY = m_overflowScrollDelta.height();

    // Reset overflow values because we may decide to remove delta at various points and put it into overflow.
    m_overflowScrollDelta = FloatSize();

    IntSize stretchAmount = m_client->stretchAmount();
    bool isVerticallyStretched = stretchAmount.height();
    bool isHorizontallyStretched = stretchAmount.width();

    float eventCoalescedDeltaX;
    float eventCoalescedDeltaY;

    if (isVerticallyStretched || isHorizontallyStretched) {
        eventCoalescedDeltaX = -wheelEvent.unacceleratedScrollingDeltaX();
        eventCoalescedDeltaY = -wheelEvent.unacceleratedScrollingDeltaY();
    } else {
        eventCoalescedDeltaX = -wheelEvent.deltaX();
        eventCoalescedDeltaY = -wheelEvent.deltaY();
    }

    deltaX += eventCoalescedDeltaX;
    deltaY += eventCoalescedDeltaY;

    // Slightly prefer scrolling vertically by applying the = case to deltaY
    if (fabsf(deltaY) >= fabsf(deltaX))
        deltaX = 0;
    else
        deltaY = 0;

    bool shouldStretch = false;

    PlatformWheelEventPhase momentumPhase = wheelEvent.momentumPhase();

    // If we are starting momentum scrolling then do some setup.
    if (!m_momentumScrollInProgress && (momentumPhase == PlatformWheelEventPhaseBegan || momentumPhase == PlatformWheelEventPhaseChanged))
        m_momentumScrollInProgress = true;

    CFTimeInterval timeDelta = wheelEvent.timestamp() - m_lastMomentumScrollTimestamp;
    if (m_inScrollGesture || m_momentumScrollInProgress) {
        if (m_lastMomentumScrollTimestamp && timeDelta > 0 && timeDelta < scrollVelocityZeroingTimeout) {
            m_momentumVelocity.setWidth(eventCoalescedDeltaX / (float)timeDelta);
            m_momentumVelocity.setHeight(eventCoalescedDeltaY / (float)timeDelta);
            m_lastMomentumScrollTimestamp = wheelEvent.timestamp();
        } else {
            m_lastMomentumScrollTimestamp = wheelEvent.timestamp();
            m_momentumVelocity = FloatSize();
        }

        if (isVerticallyStretched) {
            if (!isHorizontallyStretched && m_client->pinnedInDirection(FloatSize(deltaX, 0))) {
                // Stretching only in the vertical.
                if (deltaY && (fabsf(deltaX / deltaY) < rubberbandDirectionLockStretchRatio))
                    deltaX = 0;
                else if (fabsf(deltaX) < rubberbandMinimumRequiredDeltaBeforeStretch) {
                    m_overflowScrollDelta.setWidth(m_overflowScrollDelta.width() + deltaX);
                    deltaX = 0;
                } else
                    m_overflowScrollDelta.setWidth(m_overflowScrollDelta.width() + deltaX);
            }
        } else if (isHorizontallyStretched) {
            // Stretching only in the horizontal.
            if (m_client->pinnedInDirection(FloatSize(0, deltaY))) {
                if (deltaX && (fabsf(deltaY / deltaX) < rubberbandDirectionLockStretchRatio))
                    deltaY = 0;
                else if (fabsf(deltaY) < rubberbandMinimumRequiredDeltaBeforeStretch) {
                    m_overflowScrollDelta.setHeight(m_overflowScrollDelta.height() + deltaY);
                    deltaY = 0;
                } else
                    m_overflowScrollDelta.setHeight(m_overflowScrollDelta.height() + deltaY);
            }
        } else {
            // Not stretching at all yet.
            if (m_client->pinnedInDirection(FloatSize(deltaX, deltaY))) {
                if (fabsf(deltaY) >= fabsf(deltaX)) {
                    if (fabsf(deltaX) < rubberbandMinimumRequiredDeltaBeforeStretch) {
                        m_overflowScrollDelta.setWidth(m_overflowScrollDelta.width() + deltaX);
                        deltaX = 0;
                    } else
                        m_overflowScrollDelta.setWidth(m_overflowScrollDelta.width() + deltaX);
                }
                shouldStretch = true;
            }
        }
    }

    if (deltaX || deltaY) {
        if (!(shouldStretch || isVerticallyStretched || isHorizontallyStretched)) {
            if (deltaY) {
                deltaY *= scrollWheelMultiplier();
                m_client->immediateScrollBy(FloatSize(0, deltaY));
            }
            if (deltaX) {
                deltaX *= scrollWheelMultiplier();
                m_client->immediateScrollBy(FloatSize(deltaX, 0));
            }
        } else {
            if (!m_client->allowsHorizontalStretching(wheelEvent)) {
                deltaX = 0;
                eventCoalescedDeltaX = 0;
            } else if (deltaX && !isHorizontallyStretched && !m_client->pinnedInDirection(FloatSize(deltaX, 0))) {
                deltaX *= scrollWheelMultiplier();

                m_client->immediateScrollByWithoutContentEdgeConstraints(FloatSize(deltaX, 0));
                deltaX = 0;
            }

            if (!m_client->allowsVerticalStretching(wheelEvent)) {
                deltaY = 0;
                eventCoalescedDeltaY = 0;
            } else if (deltaY && !isVerticallyStretched && !m_client->pinnedInDirection(FloatSize(0, deltaY))) {
                deltaY *= scrollWheelMultiplier();

                m_client->immediateScrollByWithoutContentEdgeConstraints(FloatSize(0, deltaY));
                deltaY = 0;
            }

            IntSize stretchAmount = m_client->stretchAmount();

            if (m_momentumScrollInProgress) {
                if ((m_client->pinnedInDirection(FloatSize(eventCoalescedDeltaX, eventCoalescedDeltaY)) || (fabsf(eventCoalescedDeltaX) + fabsf(eventCoalescedDeltaY) <= 0)) && m_lastMomentumScrollTimestamp) {
                    m_ignoreMomentumScrolls = true;
                    m_momentumScrollInProgress = false;
                    snapRubberBand();
                }
            }

            m_stretchScrollForce.setWidth(m_stretchScrollForce.width() + deltaX);
            m_stretchScrollForce.setHeight(m_stretchScrollForce.height() + deltaY);

            FloatSize dampedDelta(ceilf(elasticDeltaForReboundDelta(m_stretchScrollForce.width())), ceilf(elasticDeltaForReboundDelta(m_stretchScrollForce.height())));

            m_client->immediateScrollByWithoutContentEdgeConstraints(dampedDelta - stretchAmount);
        }
    }

    if (m_momentumScrollInProgress && momentumPhase == PlatformWheelEventPhaseEnded) {
        m_momentumScrollInProgress = false;
        m_ignoreMomentumScrolls = false;
        m_lastMomentumScrollTimestamp = 0;
    }

    return true;
}

static inline float roundTowardZero(float num)
{
    return num > 0 ? ceilf(num - 0.5f) : floorf(num + 0.5f);
}

static inline float roundToDevicePixelTowardZero(float num)
{
    float roundedNum = roundf(num);
    if (fabs(num - roundedNum) < 0.125)
        num = roundedNum;

    return roundTowardZero(num);
}

void ScrollController::snapRubberBandTimerFired()
{
    if (!m_momentumScrollInProgress || m_ignoreMomentumScrolls) {
        CFTimeInterval timeDelta = [NSDate timeIntervalSinceReferenceDate] - m_startTime;

        if (m_startStretch == FloatSize()) {
            m_startStretch = m_client->stretchAmount();
            if (m_startStretch == FloatSize()) {
                stopSnapRubberbandTimer();

                m_stretchScrollForce = FloatSize();
                m_startTime = 0;
                m_startStretch = FloatSize();
                m_origOrigin = FloatPoint();
                m_origVelocity = FloatSize();
                return;
            }

            m_origOrigin = m_client->absoluteScrollPosition() - m_startStretch;
            m_origVelocity = m_momentumVelocity;

            // Just like normal scrolling, prefer vertical rubberbanding
            if (fabsf(m_origVelocity.height()) >= fabsf(m_origVelocity.width()))
                m_origVelocity.setWidth(0);

            // Don't rubber-band horizontally if it's not possible to scroll horizontally
            if (!m_client->canScrollHorizontally())
                m_origVelocity.setWidth(0);

            // Don't rubber-band vertically if it's not possible to scroll vertically
            if (!m_client->canScrollVertically())
                m_origVelocity.setHeight(0);
        }

        FloatPoint delta(roundToDevicePixelTowardZero(elasticDeltaForTimeDelta(m_startStretch.width(), -m_origVelocity.width(), (float)timeDelta)),
            roundToDevicePixelTowardZero(elasticDeltaForTimeDelta(m_startStretch.height(), -m_origVelocity.height(), (float)timeDelta)));

        if (fabs(delta.x()) >= 1 || fabs(delta.y()) >= 1) {
            m_client->immediateScrollByWithoutContentEdgeConstraints(FloatSize(delta.x(), delta.y()) - m_client->stretchAmount());

            FloatSize newStretch = m_client->stretchAmount();

            m_stretchScrollForce.setWidth(reboundDeltaForElasticDelta(newStretch.width()));
            m_stretchScrollForce.setHeight(reboundDeltaForElasticDelta(newStretch.height()));
        } else {
            m_client->adjustScrollPositionToBoundsIfNecessary();

            stopSnapRubberbandTimer();
            m_stretchScrollForce = FloatSize();
            m_startTime = 0;
            m_startStretch = FloatSize();
            m_origOrigin = FloatPoint();
            m_origVelocity = FloatSize();
        }
    } else {
        m_startTime = [NSDate timeIntervalSinceReferenceDate];
        m_startStretch = FloatSize();
    }
}

bool ScrollController::isRubberBandInProgress() const
{
    if (!m_inScrollGesture && !m_momentumScrollInProgress && !m_snapRubberbandTimerIsActive)
        return false;

    return !m_client->stretchAmount().isZero();
}

void ScrollController::startSnapRubberbandTimer()
{
    m_client->startSnapRubberbandTimer();
    m_snapRubberbandTimer.startRepeating(1.0 / 60.0);
}

void ScrollController::stopSnapRubberbandTimer()
{
    m_client->stopSnapRubberbandTimer();
    m_snapRubberbandTimer.stop();
    m_snapRubberbandTimerIsActive = false;
}

void ScrollController::snapRubberBand()
{
    CFTimeInterval timeDelta = systemUptime() - m_lastMomentumScrollTimestamp;
    if (m_lastMomentumScrollTimestamp && timeDelta >= scrollVelocityZeroingTimeout)
        m_momentumVelocity = FloatSize();

    m_inScrollGesture = false;

    if (m_snapRubberbandTimerIsActive)
        return;

    m_startTime = [NSDate timeIntervalSinceReferenceDate];
    m_startStretch = FloatSize();
    m_origOrigin = FloatPoint();
    m_origVelocity = FloatSize();

    startSnapRubberbandTimer();
    m_snapRubberbandTimerIsActive = true;
}

bool ScrollController::shouldRubberBandInHorizontalDirection(const PlatformWheelEvent& wheelEvent)
{
    if (wheelEvent.deltaX() > 0)
        return m_client->shouldRubberBandInDirection(ScrollLeft);
    if (wheelEvent.deltaX() < 0)
        return m_client->shouldRubberBandInDirection(ScrollRight);

    return true;
}

#if ENABLE(CSS_SCROLL_SNAP) && PLATFORM(MAC)
bool ScrollController::processWheelEventForScrollSnap(const PlatformWheelEvent& wheelEvent)
{
    if (m_verticalScrollSnapAnimator) {
        m_verticalScrollSnapAnimator->handleWheelEvent(wheelEvent);
        if (m_verticalScrollSnapAnimator->shouldOverrideWheelEvent(wheelEvent))
            return false;
    }
    if (m_horizontalScrollSnapAnimator) {
        m_horizontalScrollSnapAnimator->handleWheelEvent(wheelEvent);
        if (m_horizontalScrollSnapAnimator->shouldOverrideWheelEvent(wheelEvent))
            return false;
    }

    return true;
}

void ScrollController::updateScrollAnimatorsAndTimers(const ScrollableArea& scrollableArea)
{
    // FIXME: Currently, scroll snap animators are recreated even though the snap offsets alone can be updated.
    if (scrollableArea.horizontalSnapOffsets())
        m_horizontalScrollSnapAnimator = std::make_unique<AxisScrollSnapAnimator>(this, *scrollableArea.horizontalSnapOffsets(), ScrollEventAxis::Horizontal);
    else if (m_horizontalScrollSnapAnimator)
        m_horizontalScrollSnapAnimator = nullptr;

    if (scrollableArea.verticalSnapOffsets())
        m_verticalScrollSnapAnimator = std::make_unique<AxisScrollSnapAnimator>(this, *scrollableArea.verticalSnapOffsets(), ScrollEventAxis::Vertical);
    else if (m_verticalScrollSnapAnimator)
        m_verticalScrollSnapAnimator = nullptr;
}

void ScrollController::updateScrollSnapPoints(ScrollEventAxis axis, const Vector<LayoutUnit>& snapPoints)
{
    // FIXME: Currently, scroll snap animators are recreated even though the snap offsets alone can be updated.
    if (axis == ScrollEventAxis::Horizontal)
        m_horizontalScrollSnapAnimator = std::make_unique<AxisScrollSnapAnimator>(this, snapPoints, ScrollEventAxis::Horizontal);

    if (axis == ScrollEventAxis::Vertical)
        m_verticalScrollSnapAnimator = std::make_unique<AxisScrollSnapAnimator>(this, snapPoints, ScrollEventAxis::Vertical);
}

void ScrollController::startScrollSnapTimer(ScrollEventAxis axis)
{
    RunLoop::Timer<ScrollController>& scrollSnapTimer = axis == ScrollEventAxis::Horizontal ? m_horizontalScrollSnapTimer : m_verticalScrollSnapTimer;
    if (!scrollSnapTimer.isActive())
        scrollSnapTimer.startRepeating(1.0 / 60.0);
}

void ScrollController::stopScrollSnapTimer(ScrollEventAxis axis)
{
    RunLoop::Timer<ScrollController>& scrollSnapTimer = axis == ScrollEventAxis::Horizontal ? m_horizontalScrollSnapTimer : m_verticalScrollSnapTimer;
    scrollSnapTimer.stop();
}

void ScrollController::horizontalScrollSnapTimerFired()
{
    if (m_horizontalScrollSnapAnimator)
        m_horizontalScrollSnapAnimator->scrollSnapAnimationUpdate();
}

void ScrollController::verticalScrollSnapTimerFired()
{
    if (m_verticalScrollSnapAnimator)
        m_verticalScrollSnapAnimator->scrollSnapAnimationUpdate();
}

LayoutUnit ScrollController::scrollOffsetOnAxis(ScrollEventAxis axis)
{
    return m_client->scrollOffsetOnAxis(axis);
}

void ScrollController::immediateScrollOnAxis(ScrollEventAxis axis, float delta)
{
    m_client->immediateScrollOnAxis(axis, delta);
}
#endif

} // namespace WebCore

#endif // ENABLE(RUBBER_BANDING)
