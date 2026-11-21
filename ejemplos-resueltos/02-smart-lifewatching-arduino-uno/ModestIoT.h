/*
  ModestIoT.h
  Core of the ModestIoT nano-framework (synchronous edition for Arduino Uno):
  Event, Command, EventHandler, CommandHandler, Sensor, Actuator and Device.

  Author: Smart Lifemonitor, inc. - LifeWatching team
  Date: 2026-11-20
  Version: 1.0
*/
#ifndef MODEST_IOT_H
#define MODEST_IOT_H

#include <Arduino.h>

// Event used by sensors to notify the device.
struct Event {
  int identifier;
  int sourceId;
  explicit Event(int eventIdentifier, int source = -1) : identifier(eventIdentifier), sourceId(source) {}
  bool operator==(const Event& other) const { return identifier == other.identifier && sourceId == other.sourceId; }
};

// Command used by the device to drive actuators.
struct Command {
  int identifier;
  explicit Command(int commandIdentifier) : identifier(commandIdentifier) {}
  bool operator==(const Command& other) const { return identifier == other.identifier; }
};

// Contract for classes that react to events.
class EventHandler {
public:
  virtual void on(Event event) = 0;
  virtual ~EventHandler() {}
};

// Contract for classes that execute commands.
class CommandHandler {
public:
  virtual void handle(Command command) = 0;
  virtual ~CommandHandler() {}
};

// Base class for input devices. Subclasses read the hardware in processEvent()
// and set the identifier to SUPPRESSED_EVENT_IDENTIFIER to stop propagation.
class Sensor : public EventHandler {
protected:
  int pin;
  EventHandler* eventHandler;
  virtual void processEvent(Event& event) = 0;

public:
  static const int SUPPRESSED_EVENT_IDENTIFIER = -1;
  static const int DATA_READ_EVENT_IDENTIFIER = 20;
  static const int MEASURE_DATA_REQUESTED_EVENT_IDENTIFIER = 21;

  Sensor(int signalPin, EventHandler* parentHandler = nullptr) : pin(signalPin), eventHandler(parentHandler) {}
  void on(Event event) override;
  void setHandler(EventHandler* parentHandler) { eventHandler = parentHandler; }
  int getPin() const { return pin; }
};

// Base class for output devices.
class Actuator : public CommandHandler {
protected:
  int pin;
  CommandHandler* commandHandler;
  virtual void executeCommand(Command command) = 0;

public:
  Actuator(int signalPin, CommandHandler* parentHandler = nullptr) : pin(signalPin), commandHandler(parentHandler) {}
  void handle(Command command) override;
  void setHandler(CommandHandler* parentHandler) { commandHandler = parentHandler; }
};

// Base class for the IoT device: receives events from sensors (Mediator) and
// commands from outside. Concrete devices implement on() and update().
class Device : public EventHandler, public CommandHandler {
public:
  virtual void begin() = 0;
  virtual void update() = 0;
  void handle(Command command) override {}
};

#endif
