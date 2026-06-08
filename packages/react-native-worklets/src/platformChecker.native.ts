'use strict';

import { isRNRuntime } from './runtimeKind';
import { WorkletsModule } from './WorkletsModule/NativeWorklets';

export type Platform = 'android' | 'ios' | 'macos';

let platform: Platform;

export function getPlatform(): Platform {
  if (platform) {
    return platform;
  } else if (isRNRuntime()) {
    platform = WorkletsModule.getPlatform();
  }
  return platform;
}

export const IS_JEST = false;
