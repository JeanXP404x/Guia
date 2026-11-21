#ifndef STATUS_RGB_LED_H
#define STATUS_RGB_LED_H

/**
 * @file StatusRgbLed.h
 * @brief Declares the StatusRgbLed actuator (common cathode RGB LED status indicator).
 *
 * Green = free, red = busy, blue = reserved, off = blinking phase while overtime.
 *
 * @author Libelium Smart Cities Team - Smart Parking Spot S1
 * @date 2026-11-20
 * @version 1.0
 */

#include <ModestIoT.h>

/**
 * @brief RGB LED actuator driven through ModestIoT commands.
 */
class StatusRgbLed : public Actuator {
private:
    int greenGpioPin; ///< GPIO pin for the green channel.
    int blueGpioPin;  ///< GPIO pin for the blue channel.
    int activeCommandIdentifier; ///< Last color command applied.

    /**
     * @brief Writes the three channel levels.
     * @param redOn Red channel state.
     * @param greenOn Green channel state.
     * @param blueOn Blue channel state.
     */
    void writeChannels(bool redOn, bool greenOn, bool blueOn) const;

protected:
    /**
     * @brief Template-method hook: applies the color matching the command.
     * @param command Command to execute.
     */
    void executeCommand(Command command) override;

public:
    static constexpr int SHOW_GREEN_COMMAND_IDENTIFIER = 300; ///< Free spot color.
    static constexpr int SHOW_RED_COMMAND_IDENTIFIER   = 301; ///< Busy spot color.
    static constexpr int SHOW_BLUE_COMMAND_IDENTIFIER  = 302; ///< Reserved spot color.
    static constexpr int TURN_OFF_COMMAND_IDENTIFIER   = 303; ///< All channels off.

    static constexpr Command SHOW_GREEN_COMMAND = Command(SHOW_GREEN_COMMAND_IDENTIFIER);
    static constexpr Command SHOW_RED_COMMAND   = Command(SHOW_RED_COMMAND_IDENTIFIER);
    static constexpr Command SHOW_BLUE_COMMAND  = Command(SHOW_BLUE_COMMAND_IDENTIFIER);
    static constexpr Command TURN_OFF_COMMAND   = Command(TURN_OFF_COMMAND_IDENTIFIER);

    /**
     * @brief Creates the RGB LED actuator (starts turned off).
     * @param redGpioPin GPIO pin for the red channel.
     * @param greenGpioPin GPIO pin for the green channel.
     * @param blueGpioPin GPIO pin for the blue channel.
     * @param parentHandler Optional handler that receives propagated commands.
     */
    StatusRgbLed(int redGpioPin, int greenGpioPin, int blueGpioPin, CommandHandler* parentHandler = nullptr);

    /**
     * @brief Returns the identifier of the last applied command.
     * @return Command identifier.
     */
    int getActiveCommandIdentifier() const;
};

#endif // STATUS_RGB_LED_H
