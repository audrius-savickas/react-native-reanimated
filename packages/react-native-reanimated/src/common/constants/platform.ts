'use strict';
import { type Platform as RNPlatform } from 'react-native';
import { getPlatform, isWorkletRuntime } from 'react-native-worklets';

function isWindowAvailable() {
  // the window object is unavailable when building the server portion of a site that uses SSG
  // this function shouldn't be used to conditionally render components
  // https://www.joshwcomeau.com/react/the-perils-of-rehydration/
  // @ts-ignore Fallback if `window` is undefined.
  return typeof window !== 'undefined';
}

let PlatformOS: typeof RNPlatform.OS;

if (isWorkletRuntime()) {
  PlatformOS = getPlatform();
} else {
  // eslint-disable-next-line @typescript-eslint/no-require-imports, @typescript-eslint/no-var-requires
  PlatformOS = require('react-native').Platform.OS;
}
export const IS_ANDROID: boolean = /* @__PURE__ */ (() =>
  PlatformOS === 'android')();
export const IS_IOS: boolean = /* @__PURE__ */ (() => PlatformOS === 'ios')();
export const IS_WEB: boolean = PlatformOS === 'web';
export const IS_JEST: boolean = !!process.env.JEST_WORKER_ID;
/** @knipIgnore */
export const IS_WINDOWS: boolean = PlatformOS === 'windows';

export const IS_WINDOW_AVAILABLE: boolean = isWindowAvailable();

export const SHOULD_BE_USE_WEB = IS_JEST || IS_WEB || IS_WINDOWS;
