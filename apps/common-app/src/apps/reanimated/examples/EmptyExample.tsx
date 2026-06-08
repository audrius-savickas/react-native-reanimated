import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import { scheduleOnUI } from 'react-native-worklets';
import { Platform } from 'react-native';

export default function EmptyExample() {
  scheduleOnUI(() => {
    'worklet';
    globalThis.__fbBatchedBridgeConfig = () => () => ({});

    // require('react-native');
  });
  return (
    <View style={styles.container}>
      <Text>Hello world!</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
});
