'use strict';

import { Platform } from 'react-native';

export const IS_JEST: boolean = !!process.env.JEST_WORKER_ID;

export function getPlatform(): typeof Platform.OS {
  return Platform.OS;
}
