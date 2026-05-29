#pragma once

#include <reanimated/CSS/configs/CSSTransitionConfig.h>
#include <reanimated/CSS/core/transition/CSSLoopTransition.h>
#include <reanimated/CSS/core/transition/CSSPlatformTransition.h>
#include <reanimated/CSS/core/transition/CSSPlatformTransitionProxy.h>
#include <reanimated/CSS/easing/EasingFunctions.h>
#include <reanimated/CSS/misc/ViewStylesRepository.h>
#include <reanimated/CSS/progress/TransitionProgressProvider.h>
#include <reanimated/Fabric/updates/OperationsLoop.h>

#include <react/renderer/core/ShadowNode.h>

#include <folly/dynamic.h>
#include <jsi/jsi.h>
#include <memory>
#include <string>
#include <vector>

namespace reanimated::css {

class CSSTransition {
 public:
  class Observer {
   public:
    virtual ~Observer() = default;
    virtual void onTransitionUpdate(Tag viewTag) = 0;
  };

  CSSTransition(
      std::shared_ptr<const ShadowNode> shadowNode,
      const std::shared_ptr<ViewStylesRepository> &viewStylesRepository,
      const std::shared_ptr<CSSPlatformTransitionProxy> &platformTransitionProxy,
      const std::shared_ptr<OperationsLoop> &loop,
      Observer &observer);
  ~CSSTransition();

  Tag getViewTag() const {
    return shadowNode_->getTag();
  }

  std::shared_ptr<const ShadowNode> getShadowNode() const {
    return shadowNode_;
  }

  ShadowNodeFamily::Shared getShadowNodeFamily() const {
    return shadowNode_->getFamilyShared();
  }

  TransitionProperties getProperties() const;

  /// Returns the loop transition if one has been created (lazy), nullptr otherwise.
  CSSLoopTransition *getLoopTransition() const {
    return loopTransition_.get();
  }

  /// Splits the config via the platform proxy, running the platform-routed props
  /// immediately, and returns the loop-side config (settings + value diffs + removals)
  /// for the caller to feed into updateSettings / run.
  CSSTransitionConfig splitForPlatformRouting(jsi::Runtime &rt, CSSTransitionConfig &&config);
  /// Reconfigures the loop transition's per-property settings.
  void updateSettings(
      const PropertiesSettingsMap &changedPropertiesSettings,
      const std::vector<std::string> &removedProperties);
  /// Runs + schedules the loop transition for the given value diffs. Returns the initial update.
  folly::dynamic run(jsi::Runtime &rt, const PropertyValueDiffsMap &propertyDiffs, const folly::dynamic &lastUpdates);
  /// Loop-only run for already-computed (dynamic) diffs; used by pseudo-style state changes.
  folly::dynamic run(const PropertyValueDynamicDiffsMap &propertyDiffs, const folly::dynamic &lastUpdates);
  /// Tears down both sides: removes the loop transition from the loop and cancels any
  /// active platform animations.
  void unschedule();

 private:
  const std::shared_ptr<const ShadowNode> shadowNode_;
  const std::shared_ptr<ViewStylesRepository> viewStylesRepository_;
  const std::shared_ptr<CSSPlatformTransitionProxy> platformTransitionProxy_;
  const std::shared_ptr<OperationsLoop> loop_;
  Observer &observer_;

  CSSTransitionRouting routing_;
  std::unique_ptr<CSSPlatformTransition> platformTransition_;
  std::shared_ptr<CSSLoopTransition> loopTransition_;

  CSSPlatformTransition &ensurePlatformTransition();
  CSSLoopTransition &ensureLoopTransition();
  void scheduleLoop(double timestamp);
};

} // namespace reanimated::css
