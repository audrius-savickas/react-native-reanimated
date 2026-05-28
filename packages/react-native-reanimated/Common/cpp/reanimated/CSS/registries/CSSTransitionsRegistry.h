#pragma once

#include <reanimated/CSS/configs/CSSTransitionConfig.h>
#include <reanimated/CSS/core/transition/CSSTransition.h>
#include <reanimated/CSS/misc/ViewStylesRepository.h>
#include <reanimated/Fabric/updates/OperationsLoop.h>
#include <reanimated/Fabric/updates/UpdatesRegistry.h>
#include <reanimated/Fabric/updates/UpdatesRegistryManager.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace reanimated::css {

class CSSTransitionsRegistry : public UpdatesRegistry, public CSSTransition::Observer {
 public:
  CSSTransitionsRegistry(
      const std::shared_ptr<ViewStylesRepository> &viewStylesRepository,
      const std::shared_ptr<OperationsLoop> &loop,
      const std::shared_ptr<CSSPlatformTransitionProxy> &platformTransitionProxy);

  bool needsFlush() const;

  void updateConfigOrRun(
      jsi::Runtime &rt,
      const std::shared_ptr<const ShadowNode> &shadowNode,
      CSSTransitionConfig &&config);
  void run(const std::shared_ptr<const ShadowNode> &shadowNode, const PropertyValueDynamicDiffsMap &propertyDiffs);

  // CSSTransition::Observer
  void onTransitionUpdate(Tag viewTag) override;

  void flushUpdates(UpdatesBatch &updatesBatch);
#if REACT_NATIVE_VERSION_MINOR >= 85
  void flushUpdates(UpdatesBatchAnimatedProps &updatesBatch);
#endif

 private:
  using Registry = std::unordered_map<Tag, std::shared_ptr<CSSTransition>>;

  const std::shared_ptr<ViewStylesRepository> viewStylesRepository_;
  const std::shared_ptr<OperationsLoop> loop_;
  const std::shared_ptr<CSSPlatformTransitionProxy> platformTransitionProxy_;

  Registry registry_;
  // Tags reported by owned transitions between flushes.
  std::unordered_set<Tag> updatedTags_;

  void removeTag(Tag viewTag) override;
  const std::shared_ptr<CSSTransition> &getOrCreateTransition(const std::shared_ptr<const ShadowNode> &shadowNode);
  void updateInUpdatesRegistry(const std::shared_ptr<CSSTransition> &transition, const folly::dynamic &updates);
  void recordInitialUpdate(const std::shared_ptr<CSSTransition> &transition, const folly::dynamic &initialUpdate);
};

} // namespace reanimated::css
