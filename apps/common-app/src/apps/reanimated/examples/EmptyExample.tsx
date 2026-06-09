import React from 'react';
import { StyleSheet, Text, View } from 'react-native';
import { scheduleOnUI } from 'react-native-worklets';
import { Button } from 'react-native';

export default function EmptyExample() {
  return (
    <View style={styles.container}>
      <Text>Hello world!</Text>
      <Button
        title="Log platform"
        onPress={() => {
          scheduleOnUI(() => {
            'worklet';
            require('react-native').Platform;
          });
        }}
      />
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
