#include <reanimated/CSS/core/transition/CSSLoopTransition.h>
#include <reanimated/CSS/core/transition/CSSTransition.h>
#include <reanimated/Fabric/updates/OperationsLoop.h>

#include <memory>
#include <utility>
#include <vector>

namespace reanimated::css {

CSSTransition::CSSTransition(
    std::shared_ptr<const ShadowNode> shadowNode,
    const std::shared_ptr<ViewStylesRepository> &viewStylesRepository,
    const std::shared_ptr<CSSPlatformTransitionProxy> &platformTransitionProxy,
    const std::shared_ptr<OperationsLoop> &loop,
    Observer &observer)
    : shadowNode_(std::move(shadowNode)),
      viewStylesRepository_(viewStylesRepository),
      platformTransitionProxy_(platformTransitionProxy),
      loop_(loop),
      observer_(observer) {}

CSSTransition::~CSSTransition() {
  if (platformTransition_) {
    platformTransition_->cancelAll();
  }
}

TransitionProperties CSSTransition::getProperties() const {
  TransitionProperties result = routing_.loop;
  result.reserve(routing_.loop.size() + routing_.platform.size());
  result.insert(routing_.platform.begin(), routing_.platform.end());
  return result;
}

CSSTransitionConfig CSSTransition::runPlatformProps(jsi::Runtime &rt, CSSTransitionConfig &&config) {
  const auto timestamp = loop_->resolveTimestamp();

  // Split into platform vs loop sides; platform-routed props run immediately.
  auto processed = platformTransitionProxy_->processConfig(std::move(config), routing_);
  routing_ = std::move(processed.routing);

  if (!processed.platform.empty()) {
    ensurePlatformTransition().run(rt, processed.platform, timestamp);
  }

  // Hand the loop-side config back to the caller, which applies settings / runs as needed.
  return std::move(processed.loop);
}

void CSSTransition::updateSettings(
    const PropertiesSettingsMap &changedPropertiesSettings,
    const std::vector<std::string> &removedProperties) {
  ensureLoopTransition().updateSettings(changedPropertiesSettings, removedProperties);
}

folly::dynamic
CSSTransition::run(jsi::Runtime &rt, const PropertyValueDiffsMap &propertyDiffs, const folly::dynamic &lastUpdates) {
  const auto timestamp = loop_->resolveTimestamp();
  auto initialUpdate = ensureLoopTransition().run(rt, shadowNode_, propertyDiffs, lastUpdates, timestamp);
  scheduleLoop(timestamp);
  return initialUpdate;
}

folly::dynamic CSSTransition::run(
    const PropertyValueDynamicDiffsMap &propertyDiffs,
    const folly::dynamic &lastUpdates) {
  const auto timestamp = loop_->resolveTimestamp();
  auto initialUpdate = ensureLoopTransition().run(shadowNode_, propertyDiffs, lastUpdates, timestamp);
  scheduleLoop(timestamp);
  return initialUpdate;
}

void CSSTransition::cancel() {
  if (loopTransition_) {
    loop_->remove(loopTransition_);
  }
  if (platformTransition_) {
    platformTransition_->cancelAll();
  }
}

CSSPlatformTransition &CSSTransition::ensurePlatformTransition() {
  if (!platformTransition_) {
    platformTransition_ = std::make_unique<CSSPlatformTransition>(shadowNode_->getTag(), platformTransitionProxy_);
  }
  return *platformTransition_;
}

CSSLoopTransition &CSSTransition::ensureLoopTransition() {
  if (!loopTransition_) {
    loopTransition_ = std::make_shared<CSSLoopTransition>(
        shadowNode_->getTag(),
        shadowNode_->getComponentName(),
        viewStylesRepository_,
        [&observer = observer_](Tag viewTag) { observer.onTransitionUpdate(viewTag); });
  }
  return *loopTransition_;
}

void CSSTransition::scheduleLoop(const double timestamp) {
  loop_->schedule(loopTransition_, timestamp + loopTransition_->getMinDelay(timestamp));
}

} // namespace reanimated::css
