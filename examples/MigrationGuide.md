# Migration Guide: From Old IO to New Dynamic IO System

## Overview

The IO system has been completely refactored to support multiple input devices using OOP principles. Here's how to migrate your existing code.

## Old System vs New System

### Old System (Before)

```cpp
// Old way - single encoder/button
IO& io = IO::getInstance();
io.setEncoderPins(32, 33, 25);
io.enablePullups(true);
io.initialize();

// In loop
io.update();
if (io.hasNewInput()) {
    int delta = io.getEncoderDelta();
    bool pressed = io.wasEncoderButtonPressed();
    // Handle input...
}
```

### New System (After)

```cpp
// New way - multiple devices with names
IO& io = IO::getInstance();

// Add devices with configurations
RotaryEncoder::Config config;
config.pinA = 32;
config.pinB = 33;
config.buttonPin = 25;
config.enablePullups = true;

RotaryEncoder* encoder = io.addRotaryEncoder("main_encoder", config);
io.initialize();

// In loop
io.update();
if (encoder->hasNewInput()) {
    int delta = encoder->getDelta();
    bool pressed = encoder->wasButtonPressed();
    // Handle input...
}
```

## Migration Steps

### Step 1: Replace Direct IO Usage

**Old:**

```cpp
IO& io = IO::getInstance();
io.setEncoderPins(32, 33, 25);
io.initialize();
```

**New:**

```cpp
IO& io = IO::getInstance();
RotaryEncoder::Config config = {
    .pinA = 32,
    .pinB = 33,
    .buttonPin = 25,
    .enablePullups = true,
    .hasButton = true
};
RotaryEncoder* encoder = io.addRotaryEncoder("main_encoder", config);
io.initialize();
```

### Step 2: Update Input Reading

**Old:**

```cpp
io.update();
if (io.hasNewInput()) {
    long position = io.getEncoderPosition();
    int delta = io.getEncoderDelta();
    bool pressed = io.wasEncoderButtonPressed();
}
```

**New:**

```cpp
io.update();
RotaryEncoder* encoder = io.getRotaryEncoder("main_encoder");
if (encoder && encoder->hasNewInput()) {
    long position = encoder->getPosition();
    int delta = encoder->getDelta();
    bool pressed = encoder->wasButtonPressed();
}
```

### Step 3: Convert Callbacks

**Old:**

```cpp
io.setEncoderCallback([](int delta) {
    Serial.printf("Encoder: %d\n", delta);
});
```

**New:**

```cpp
encoder->setEncoderCallback([](int delta) {
    Serial.printf("Encoder: %d\n", delta);
});
```

## Advanced Features

### Multiple Devices

```cpp
// Add multiple encoders
auto mainEncoder = io.addRotaryEncoder("main", {.pinA = 32, .pinB = 33});
auto volumeEncoder = io.addRotaryEncoder("volume", {.pinA = 26, .pinB = 27, .hasButton = false});

// Add standalone buttons
auto button1 = io.addButton("btn1", {.pin = 14});
auto button2 = io.addButton("btn2", {.pin = 12});
```

### Global Input Monitoring

```cpp
// Monitor all devices at once
io.setGlobalInputCallback([](const String& deviceId, InputDevice::DeviceType type) {
    Serial.printf("Input from %s (type: %d)\n", deviceId.c_str(), static_cast<int>(type));
});
```

### Dynamic Device Management

```cpp
// Add devices at runtime
auto newEncoder = RotaryEncoder::create("runtime_encoder", config);
io.addDevice(std::move(newEncoder));

// Remove devices
io.removeDevice("runtime_encoder");

// Query devices
auto allEncoders = io.getDevicesOfType<RotaryEncoder>();
auto deviceIds = io.getDeviceIds();
```

## Benefits of New System

1. **Multiple Devices**: Support for multiple encoders, buttons, and other input devices
2. **Type Safety**: Template-based type-safe access to devices
3. **Flexibility**: Easy to add new device types by inheriting from InputDevice
4. **Better Organization**: Each device manages its own state and configuration
5. **Extensibility**: Easy to add new input device types (joysticks, potentiometers, etc.)
6. **Memory Management**: Automatic cleanup with smart pointers

## Backward Compatibility

If you need to maintain backward compatibility during migration, you can create wrapper functions:

```cpp
// Wrapper for old-style access (temporary compatibility)
class IOLegacyWrapper {
public:
    static long getEncoderPosition() {
        auto* encoder = IO::getInstance().getRotaryEncoder("main_encoder");
        return encoder ? encoder->getPosition() : 0;
    }

    static int getEncoderDelta() {
        auto* encoder = IO::getInstance().getRotaryEncoder("main_encoder");
        return encoder ? encoder->getDelta() : 0;
    }

    static bool wasEncoderButtonPressed() {
        auto* encoder = IO::getInstance().getRotaryEncoder("main_encoder");
        return encoder ? encoder->wasButtonPressed() : false;
    }
};
```

## Next Steps

1. Create your device configurations
2. Add devices to the IO manager
3. Update your input reading code
4. Test thoroughly
5. Remove any legacy wrapper code once migration is complete

The new system provides much more flexibility while maintaining the same ease of use for simple applications.
