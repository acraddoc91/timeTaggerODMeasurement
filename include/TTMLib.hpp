/**********************************************************************
 *                                                                    *
 *                             TTMLib.hpp                             *
 *                                                                    *
 **********************************************************************/
/*
 * Project:   TTM8000-Virtex5 (Time Tagging Module)
 *
 * Company:   Roithner Lasertechnik GmbH - Vienna - Austria
 * Support:   office@roithner-laser.com
 *
 * Copyright: Roithner Lasertechnik GmbH
 *            Wiedner Hauptstrasse 76
 *            1040 Vienna
 *            Austria / Europe
 *
 * Decription: */
/** @file
 *   ---- FIXME ----
 *//*
 *
 * Release 4.4.4 - 2016-01-26
 */
#ifndef TTMLib_hpp
#define TTMLib_hpp

/**********************************************************************/

#include "FlexIOLib.hpp"
#include "TTMLib.h"

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                         class TTMCntrl_c                         **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Control Connection between a FlexIO Board and the local host.
  *
  * FIXME - More Documentation required here - FIXME
  */
class LIBTTM_API TTMCntrl_c : public FlexIOCntrl_c
 {
  public:
    /** @brief Constructor for TTMCntrl
      *
      */
    TTMCntrl_c();

    /** @brief Destructor for TTMCntrl
      *
      */
    virtual ~TTMCntrl_c();

    /** @brief Configure the TTM8000 Board for a Measurement.
      *
      * ConfigMeasurement() configures the board for a new measurement
      * as defined in the fields of MeasureConfig. Note that ConfigMeasurement()
      * stops the previously running measurement (if any), but does not automatically
      * start the new measurement. It can not be used for 'on-the-fly' reconfiguration.
      * @param MeasureConfig In: Description of the requested Measurement Configuration
      * @returns Error Code
      */
    FlexIOStatus ConfigMeasurement(TTMMeasConfig_t *MeasureConfig);

    /** @brief Check if the GPX PLL is Locked
      *
      * If the 40MHz clock driving the acam GPX is unstable or not available
      * the PLL of the acam GPX does sometimes loose its lock, causing the
      * TTM module to fail. Use GetGPXPLLState() to check the stability of
      * the reference clock.
      * @param PLLLocked Out: Is the GPX PLL Locked?
      * @returns Error Code
      */
    FlexIOStatus GetGPXPLLState(bool *PLLLocked);
    /** @brief Start a Measurement.
      *
      * StartMeasurement() starts the measurement that was set
      * up using TTMCntrl::ConfigMeasurement().
      * Note that a separate call to ConfigMeasurement() is required every
      * time before calling StartMeasurement(). It is not possible to call
      * ConfigureMeasurement() once and then use that for many pairs of
      * StartMeasurement()/StopMeasurement().
      * @param PermitAutoPulseGenStart In: Allow the use of the PulseGen to
      *        create a Start Pulse in continuous I-Mode.
      * @returns Error Code
      * @see StopMeasurement()
      */
    FlexIOStatus StartMeasurement(bool PermitAutoPulseGenStart);

    /** @brief Stop the current Measurement.
      *
      * @returns Error Code
      * @see StartMeasurement()
      */
    FlexIOStatus StopMeasurement();

    /** @brief Pause the current Measurement.
      *
      * PauseMeasurement() disables all external signal input, however
      * the internal time measurement engine continues operation. This
      * is can be useful when running in continuous I-Mode when measurement
      * shall be paused, but the global time shall continue.
      * @returns Error Code
      * @see ResumeMeasurement()
      */
    FlexIOStatus PauseMeasurement();

    /** @brief Resume the current Measurement.
      *
      * @returns Error Code
      * @see PauseMeasurement()
      */
    FlexIOStatus ResumeMeasurement();

    /** @brief Send all data, that is stored on the TTM8000 Board to the Network.
      *
      * @note  FlushData() shall ONLY be called when the current Measurement
      * is stopped. It is not intended to speed up/enforce data transfer in
      * situations with low event rates. Use the DataIdleTimeout field in the
      * MeasureConfig structure to enforce periodic transfer of available data
      * even in situations with low event rates.
      * @returns Error Code
      */
    FlexIOStatus FlushData();

    /** @brief Restart the Global Time for the current Measurement.
      *
      * The 60-bit Timestamp counter used for continuous I-Mode is reset.
      * This can be used to (extremely crudely!) synchronize the timers of
      * several TTM8000-boards.
      * @note The Counter will NOT actually be reset to 0. Rather it will
      * perform a modulo (256 * 77760) operation (bringing the counter
      * value into the range of (0..19906560). Thus there is a slack of
      * up to 1.6384ms (@ 82.3045ps/Tick)
      * @returns Error Code
      */
    FlexIOStatus RestartTime();

    /** @brief Reconfigure the active Trigger Edges on a TTM8000 Board.
      *
      * SetEnabledEdges() reconfigures the active edges of the current
      * measurement. This is done on-the-fly without interruption of the running
      * measurement or loss of data.
      * @note Even so a complete Measurement Configuration is passed to
      * SetEnabledEdges() only the enabled edges are reconfigured.
      * All other settings are expected to be valid, but are not updated!
      * @param MeasureConfig In: Description of the reqeusted Measurement Configuration
      * @returns Error Code
      */
    FlexIOStatus SetEnabledEdges(TTMMeasConfig_t *MeasureConfig);

    /** @brief Set the Threshold Voltage for the Start/Stop Signal Inputs and the External Clock.
      *
      * In order to accomodate various logic families the threshold voltage that
      * is used to distinguish 'low' from 'high' states on signal and clock inputs
      * is adjustable in the range of [-4096mV...+4095mV] on the TTM8000 Board.
      * The analog voltage is created using an 8-bit DAC (Digital-to-Analog Converter).
      * Thus there is a conversion resolution of 32mV (8192mV Range / 2^8 values).
      * @param ChannelMask   In: Select Levels to Adjust (Bit 0..Start, Bits 1..8: Stop1..8, Bit 9..Clock)
      * @param MilliVolt     InOut: Input: Requested Threshold Voltage for External Signals [mV]\n
      *                             Output: Actual Threshhold Voltage for External Signals [mV]\n
      *        MilliVolt[0]  ..   Start Input [mV]\n
      *        MilliVolt[1..8] .. Stop1..8 Input [mV]\n
      *        MilliVolt[9]  ..   External Clock [mV]\n
      * @returns Error Code
      */
    FlexIOStatus SetTriggerLevel(uint32_t ChannelMask, int32_t MilliVolt[10]);

    /** @brief Get the Threshold Voltage for the Start/Stop Signal Inputs and the External Clock.
      *
      * In order to accomodate various logic families the threshold voltage that
      * is used to distinguish 'low' from 'high' states on signal and clock inputs
      * is adjustable in the range of [-4096mV...+4095mV] on the TTM8000 Board.
      * The analog voltage is created using an 8-bit DAC (Digital-to-Analog Converter).
      * Thus there is a conversion resolution of 32mV (8192mV Range / 2^8 values).
      * @param MilliVolt   Out: Threshold Voltage for External Signals [mV]\n
      *        MilliVolt[0]  ..   Start Input [mV]\n
      *        MilliVolt[1..8] .. Stop1..8 Input [mV]\n
      *        MilliVolt[9]  ..   External Clock [mV]\n
      * @returns Error Code
      */
    FlexIOStatus GetTriggerLevel(int32_t MilliVolt[10]);

    /** @brief Set the Channel Delay for Stop Signal Inputs
      *
      * In order to compensate for different cable lengths or internal delays
      * in signal sources, it is possible to add an channel specific offset
      * to every measurement. This is done before sorting is performed, meaning
      * that the events are sorted according to the time of their generation
      * (and not according to the time they are registered by the TTM8000).
      * In order to keep the effort for sorting down, we can only handle small
      * adjustments. The adjustments must be in the range 0..255 Ticks.
      * For I-Mode 256 Ticks @ 82.3045ps/Tick correspond to 21ns or
      * 4m Cable @ 200000km/s. We shall assume that we can guarantee that
      * cables lengths do not differ by more than 4m....
      *
      * @param ChannelMask    In: Select Channel Delays to Adjust (Bit 0..Start, Bits 1..8: Stop1..8)
      * @param ChannelDelay   In: Channel Delays\n
      *        ChannelDelay[0]  ..   Unused\n
      *        ChannelDelay[1..8] .. Channel Delay for Stop1..8 [Ticks]\n
      * @returns Error Code
      */
    FlexIOStatus SetChannelDelay(uint32_t ChannelMask, int ChannelDelay[9]);

    /** @brief Get the Channel Delay for Stop Signal Inputs
      *
      * Get the Channel Delays for all Stop Signals
      *
      * @param ChannelDelay   Out: Channel Delays\n
      *        ChannelDelay[0]  ..   Unused (const. 0)\n
      *        ChannelDelay[1..8] .. Channel Delay for Stop1..8 [Ticks]\n
      * @returns Error Code
      * @see SetChannelDelay()
      */
    FlexIOStatus GetChannelDelay(int ChannelDelay[9]);

    /** @brief Get the Number of Events recorded for each Channel
      *
      * This is a conveniance function that provides the number of events
      * that was recorded on each channel since the beginning of time. It
      * would be easy to compute this information from the stream of timetags,
      * however it is delivered here as preprocessed information.
      *
      * @param EventCnt Out: Number of Events for each Channel\n
      * @returns Error Code
      */
    FlexIOStatus GetEventCnt(TTMEventCnt_t *EventCnt);

    /** @brief Temporarily Configure the Reference Clock System.
      *
      * The TTM8000 Board uses a 40MHz reference clock as basis for time measurements.
      * This 40MHz clock can either be generated internally on the TTM8000 Board (using
      * a high precision/low jitter crystal oscillator) or it can be provided externally
      * (with an external source running at 10, 20, 40 or 80MHz).
      * Furthermore the TTM8000 Board can use the 40MHz reference to generate an output
      * reference clock at 1, 2, 5, 10, 20, 40 or 80MHz.
      *
      * SetExternClockConfig() will change the current configuration of the External
      * Clock Input and Output. The change is only temporary and will be reset on the
      * next power cycle. Use UpdateExternClockConfig() if you want to perform a
      * permanent change.
      *
      * @see GetExternClockConfig(), UpdateExternClockConfig()
      *
      * @param ExternClkConfig In: Configuration of the External Clock Input/Output
      * @returns Error Code
      */
    FlexIOStatus SetExternClockConfig(const TTMExternClkConfig_t *ExternClkConfig);

    /** @brief Get the Configuration of the Reference Clock System
      *
      * The TTM8000 Board uses a 40MHz reference clock as basis for time measurements.
      * This 40MHz clock can either be generated internally on the TTM8000 Board (using
      * a high precision/low jitter crystal oscillator) or it can be provided externally
      * (with an external source running at 10, 20, 40 or 80MHz).
      * Furthermore the TTM8000 Board can use the 40MHz reference to generate an output
      * reference clock at 1, 2, 5, 10, 20, 40 or 80MHz.
      *
      * @see SetExternClockConfig(), UpdateExternClockConfig()
      *
      * @param ExternClkConfig Out: Configuration of the External Clock Input/Output
      * @returns Error Code
      */
    FlexIOStatus GetExternClockConfig(TTMExternClkConfig_t *ExternClkConfig);

    /** @brief Permanently Configure the Reference Clock System.
      *
      * The TTM8000 Board uses a 40MHz reference clock as basis for time measurements.
      * This 40MHz clock can either be generated internally on the TTM8000 Board (using
      * a high precision/low jitter crystal oscillator) or it can be provided externally
      * (with an external source running at 10, 20, 40 or 80MHz).
      * Furthermore the TTM8000 Board can use the 40MHz reference to generate an output
      * reference clock at 1, 2, 5, 10, 20, 40 or 80MHz.
      *
      * UpdateExternClockConfig() will change the configuration of the External
      * Clock Input and Output. The change is permanent and will remain in effect
      * until it explicitly overwritten by another call of UpdateExternClockConfig().
      * Use SetExternClockConfig() if you want to change the configuration just
      * temporarily (so that it will automatically be reset with the next power cycle).
      *
      * @see SetExternClockConfig(), GetExternClockConfig()
      *
      * @param ExternClkConfig In: Configuration of the External Clock Input/Output
      * @returns Error Code
      */
    FlexIOStatus UpdateExternClockConfig(const TTMExternClkConfig_t *ExternClkConfig);

    /** @brief Check the Stability of the Clock Sources
      *
      * The TTM8000 Board uses two clocks as reference clocks for time measurement, an
      * internal 10MHz crystal clock or (optionally) an external clock.
      * GetClockSourceState() obtains information about the availability of these
      * clocks and checks that the frequencies match the expected values. If an external
      * clock is present, but the frequency is instable and/or far away from the expected
      * value the TTM8000 Board will periodically try to use it, then detect the problem
      * switch to the internal clock as fallback solution, detect that there is an
      * external clock and try using it again. To detect this oscillation it is useful
      * to perform this measurement repeatedly.
      *
      * @param QueryCnt In: How often shall the clocks be observed.
      * @param ClkState Out: State of the Internal and External Clock
      * @returns Error Code
      */
    FlexIOStatus GetClockSourceState(int QueryCnt, TTMClockSourceState_t *ClkState);

   /** @brief Temporarily Configure the Internal Reference Clock
      *
      * The TTM8000 Board uses a local crystal oscillator to generate an internal
      * reference clock. During the production of the TTM8000 Hardware, there are
      * various options for implementation of this oscillator. We can either
      * use a moderate-cost high-quality 'normal' crystal (CXO = clock oscillator)
      * or we can use a high-cost very-high-precision temperature-controlled
      * crystal (OCXO = oven controlled crystal oscillator), whose frequency can
      * be adjusted a trim voltage. (The trim voltage is ignored if the board
      * is populated with a 'normal' crystal only.)\n
      *
      * The trim voltage is divided in a
      *
      * The frequency of the crystal can be 10 or 40MHz (independent of the crystal
      * type).
      *
      * SetInternClockFreq() can be used to configure the AnyFreq Clock Chip to
      * handle the signal provided by the internal crystal correctly and to set
      * the trim volatage for the OCXO.\n
      * Any changes made will be lost at the next power cycle. To make permanent
      * changes, please use UpateInternClockFreq().
      *
      * @see GetInternClockFreq(), UpdateInternClockFreq()
      *
      * @param IntClockInFreq In: Frequency of the Internal Input Clock [Hz].
      *               Valid values are 10000000 and 40000000.
      * @param HasOCXO       In: Does our TTM8000 Board have an Oven Controlled Crystal Osciallator (OCXO)?
      * @param CoarseTrim    In: Coarse Crystal Trim Setting
      *               Valid range: -32000 .. +32000 - Bits 7..0 must be 0
      * @param FineTrim      In: Fine Crystal Trim Setting
      *               Valid range: -32000 .. +32000 - Bits 7..0 must be 0
      * @returns Error Code
      */
    FlexIOStatus SetInternClockFreq(uint32_t IntClockInFreq, bool HasOCXO,
                                    int32_t CoarseTrim, int32_t FineTrim);

    /** @brief  Get the Configuration of the Internal Reference Clock
      *
      * The TTM8000 Board uses a local crystal oscillator to generate an internal
      * reference clock. During the production of the TTM8000 Hardware, there are
      * various options for implementation of this oscillator. We can either
      * use a moderate-cost high-quality 'normal' crystal (CXO = clock oscillator)
      * or we can use a high-cost very-high-precision temperature-controlled
      * crystal (OCXO = oven controlled crystal oscillator), whose frequency can
      * be adjusted a trim voltage. (The trim voltage is ignored if the board
      * is populated with a 'normal' crystal only.)\n
      * The frequency of the crystal can be 10 or 40MHz (independent of the crystal
      * type).
      *
      * GetInternClockFreq() can be used to discover the speed of the crystal
      * on board the TTM8000 board (even if it hard to come up with an application
      * that would require such knowledge) and the current setting of the trim
      * voltage (which might be useful if we want to tune the crystal).
      *
      * @see SetInternClockFreq(), UpdateInternClockFreq()
      *
      * @param IntClockInFreq Out: Frequency of the Internal Input Clock [Hz].
      * @param HasOCXO        Out Does our TTM8000 Board have an Oven Controlled Crystal Osciallator (OCXO)?
      * @param CoarseTrim     Out: Coarse Crystal Trim Setting [+/- 32000]
      * @param FineTrim       Out: Fine Crystal Coarse Trim Setting [+/- 32000]
      * @returns Error Code
      */
    FlexIOStatus GetInternClockFreq(uint32_t *IntClockInFreq, bool *HasOCXO,
                                    int32_t *CoarseTrim, int32_t *FineTrim);

    /** @brief Permanently Configure the Internal Reference Clock
      *
      * The TTM8000 Board uses a local crystal oscillator to generate an internal
      * reference clock. During the production of the TTM8000 Hardware, there are
      * various options for implementation of this oscillator. We can either
      * use a moderate-cost high-quality 'normal' crystal (CXO = clock oscillator)
      * or we can use a high-cost very-high-precision temperature-controlled
      * crystal (OCXO = oven controlled crystal oscillator), whose frequency can
      * be adjusted a trim voltage. (The trim voltage is ignored if the board
      * is populated with a 'normal' crystal only.)\n
      * The frequency of the crystal can be 10 or 40MHz (independent of the crystal
      * type).
      *
      * UpdateInternClockFreq() can be used to configure the AnyFreq Clock Chip to
      * handle the signal provided by the internal crystal correctly and to set
      * the trim volatage for the OCXO.\n
      * Any changes made are permanent, and will remain in effect until explicitly
      * overwritten by another call to UpdateInternClockFreq(). If you want to
      * make just temporary changes, you should use SetUpdateInternClockFreq().
      *
      * @note Since the frequency of the internal crystal is determined when
      * the TTM8000 board is populated during production, it should be sufficient
      * to set the IntClockInFreq() once after production. User code should
      * never need to call UpdateInternClockFreq().
      *
      * @see SetInternClockFreq(), GetInternClockFreq()
      *
      * @param IntClockInFreq In: Frequency of the Internal Input Clock [Hz].
      *               Valid values are 10000000 and 40000000.
      * @param HasOCXO In: Does our TTM8000 Board have an Oven Controlled Crystal Osciallator (OCXO)?
      * @param CoarseTrim    In: Coarse Crystal Trim Setting
      *               Valid range: -32000 .. +32000 - Bits 7..0 must be 0
      * @param FineTrim    In: Fine Crystal Trim Setting
      *               Valid range: -32000 .. +32000 - Bits 7..0 must be 0
      * @returns Error Code
      */
    FlexIOStatus UpdateInternClockFreq(uint32_t IntClockInFreq, bool HasOCXO,
                                       int32_t CoarseTrim, int32_t FineTrim);

    /** @brief Configure the Pulse Generator.
      *
      * The TTM8000 Board contains a built-in Pulse Generator that can be used to
      * generate pulse patterns on two channels - Start and Stop.\n
      * The Start channel can be used to drive the TTM8000 Start Input internally
      * (instead of using a signal from the external Start In connector). If (and
      * only if!) it is used to drive the Start input, the signal will become
      * externally visible on the Start Out connector.\n
      * The Calibration Signal drives the Cal. Clk Out connector. Using an external
      * cable this signal can be connected to any Stop input to provide a
      * reference stop signal. (Beware of the delay introduced by the length of
      * the cable: 40cm @ 200 000km/s = 2ns.)\n
      * Pulse generation is based on a 125MHz reference clock. Thus all delays
      * are multiples of 8ns and all pulses are 8ns long. The pattern definition
      * is based on frames. Each Frame starts with an (optional) Start pulse. After
      * a delay of StopPulsePos * 8ns a burst of Stop Pulses is sent. Thus burst
      * consists of BurstPulseCnt pulses spaced PulseBurstDelay * 8ns (rising-edge
      * to rising-edge).
      * @note If you choose a BurstPulseDelay of 1, there will be no time for the
      * Stop signal to go low between two high pulses, causing the pulses to fuse
      * into one long pulse. If you need 'real' edges, BurstPulseDelay must be at
      * least 2 (for a max. pulse rate of 62.5MHz).
      * @note ConfigPulseGen() will ONLY configure/start/stop the pulse
      * generator. It will NOT route the generated start signal to the internal
      * start port of the timetagging logic. Be sure to set the field UsePulseGenStart
      * of MeasureConfig appropriatly before calling ConfigMeasurement() or use
      * SetSignalSource() to adjust the start source for a running measurement.
      * @param Enable   In: Enable/Disable Pulse Generator
      * @param FrameCnt In: Number of Frames to Generate
      * @param FrameLen In: Duration of a Frame [8ns]
      * @param StopPulsePos    In: Delay between the Start-of-Frame and the first Stop-Pulse [8ns]
      * @param BurstPulseCnt   In: Number of Stop Pulses within a Frame
      * @param BurstPulseDelay In: Delay between the rising-edge of one Stop-Pulse and the
      *        rising-edge of the next Stop-Pulse within a burst of Stop-Pulses. [8ns]
      * @param SendFirstStartPulse In: Generate a Start-Pulse for the first Frame?
      * @param SendNextStartPulse  In: Generate a Start-Pulse for the next (i.e. non-first) Frame?
      * @returns Error Code
      * @see GetPulseGenProgress(), SetStartSource(), GetStartSource()
      */
    FlexIOStatus ConfigPulseGen(bool Enable,
                                uint32_t FrameCnt, uint32_t FrameLen, uint32_t StopPulsePos,
                                uint16_t BurstPulseCnt, uint16_t BurstPulseDelay,
                                bool SendFirstStartPulse, bool SendNextStartPulse);

    /** @brief Check the Progress of the Pulse Generator.
      *
      * If we want to wait until the Pulse Generator has finished its sequence,
      * it is useful to check its progress (e.g. to display a progress bar to the
      * user).
      * @param Enabled    Out: Is the Pulse Generator Enabled?
      *        Pass NULL if you do not require this information.
      * @param Done       Out: Is the Pulse Generator Done? This flag will
      *        remain set until the Pulse Generator is disabled or a new
      *        pulse sequence is started.
      *        Pass NULL if you do not require this information.
      * @param FramesToGo Out: How many more frames must the pulse generator produce,
      *        before it is done?
      *        Pass NULL if you do not require this information.
      * @returns Error Code
      * @see ConfigPulseGen(), SetStartSource(), GetStartSource()
      */
    FlexIOStatus GetPulseGenProgress(bool *Enabled, bool *Done, uint32_t *FramesToGo);

    /** @brief Set the Signal Source for Start
      *
      * The internal Start signal of the TTM8000 Board can be driven by
      * either a signal from the external Start connector or by the
      * Pulse Generator built into the TTM8000 Board. SetStartSource()
      * selects the signal source for Start.
      * @param UsePulseGenStart In: Use the Pulse Generator (TRUE) or External Connector
      *        (FALSE) to drive the Start signal
      * @returns Error Code
      * @see ConfigPulseGen(), GetPulseGenProgress(), GetStartSource()
      */
    FlexIOStatus SetStartSource(bool UsePulseGenStart);

    /** @brief Get the Signal Source for Start
      *
      * The internal Start signal of the TTM8000 Board can be driven by
      * either a signal from the external Start connector or by the
      * Pulse Generator built into the TTM8000 Board. SetStartSource()
      * selects the signal source for Start.
      * @param UsePulseGenStart Out: Use the Pulse Generator (TRUE) or External Connector
      *        (FALSE) to drive the Start signal
      *        Pass NULL if you do not require this information.
      * @returns Error Code
      * @see ConfigPulseGen(), GetPulseGenProgress(), SetStartSource()
      */
    FlexIOStatus GetStartSource(bool *UsePulseGenStart);

    /** @brief Reconfigure the User Data.
      *
      * SetUserData() set the User Data field. This is done on-the-fly without
      * interruption of the running measurement or loss of data.\n
      * The UserData field will be copied unchanged to each Timetag Data packet
      * and can be used to pass signaling information from the control application
      * to the data processing application.
      * @param UserData In: The User Data
      * @returns Error Code
      */
    FlexIOStatus SetUserData(uint16_t UserData);

    /** @brief Set the State of the DigitalIO Pins
      *
      * The TTM8000 Board has an 8-bit wide DigitalIO port using 3.3V TTL logic.
      * Each bit is individually configurable as input or output.
      * @param DataOut  In: Bitmask of Output Data
      * @param DataDir  In: Bitmask of Data Direction (0..Out, 1..In)
      * @returns Error Code
      * @see TTMCntrlGetUserIO
      */
    FlexIOStatus SetUserIO(uint8_t DataOut, uint8_t DataDir);

    /** @brief Get the State of the DigitalIO Pins
      *
      * The TTM8000 Board has an 8-bit wide DigitalIO port using 3.3V TTL logic.
      * Each bit is individually configurable as input or output.
      * @param DataOut Out: Bitmask of Output Data
      *        Pass NULL if you do not require this information.
      * @param DataDir Out: Bitmask of Data Direction (0..Out, 1..In)
      *        Pass NULL if you do not require this information.
      * @param DataIn  Out: Bitmask of Current State of IOPort
      *        Pass NULL if you do not require this information.
      * @returns Error Code
      * @see TTMCntrlSetUserIO
      */
    FlexIOStatus GetUserIO(uint8_t *DataIn, uint8_t *DataOut, uint8_t *DataDir);


  protected:
 }; /* class TTMCntrl_c */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                         class TTMData_c                          **
 **                                                                  **
 **********************************************************************
 **********************************************************************/
/** @brief Core TTMData_c Class (Internal)
  */
class CoreTTMData_c;

/** @brief Data Connection between a TTM8000 Board and the local host.
  *
  * FIXME - More Documentation required here - FIXME
  */
 /* Note: It seems that DoxyGen has some trouble with the TTMLIB_API
  * makro here. It might be necessary to comment it out before creating
  * the documentation!
  */
class LIBTTM_API TTMData_c
 {
  public:
    /** @brief Constructor for TTMData
      *
      */
    TTMData_c();

    /** @brief Destructor for TTMData
      *
      */
    ~TTMData_c();

    /** @brief Create a new TTMData Connection.
      *
      * @note Every TTMData Connection that was opened with Connect() must be
      *       closed with a corresponding call to Disconnect()!
      * @param BoardIPAddr    In: IP-Address of the requested TTM8000-Board
      * @param BoardDataPort  In: Usually you should pass FlexIODataPort
      * @param LocalIPAddr    In: IP-Address at the local host to be used for the
      *        connection. Unless you have a clear reason to use a specific
      *        network interface, you should pass INADDR_ANY and let TTMLib select
      *        a suitable local interface for you.
      *        A common reason for using a specific IP-Address port would be that the
      *        TTM8000 control task and TTM8000 data task are run by two separate applications
      *        that have no (easy) way to communicate at runtime and that therefore
      *        use pre-agreed-apon network parameters.
      * @param LocalDataPort  In: UDP Port at the local host to be used for the
      *        connection. Unless you have a clear reason to use a specific UDP
      *        Port, you should pass 0 and let TTMLib select a suitable port for you.
      *        A common reason for using a specific UDP port would be that the TTM8000
      *        control task and TTM8000 data task are run by two separate applications
      *        that have no (easy) way to communicate at runtime and that therefore
      *        use pre-agreed-apon network ports.
      * @param SocketBufferSize In: Network Buffer Size at the Local Host. Running
      *        at full speed, the TTM8000 Board can generate up to 80MByte/s of Timetags.
      *        Since the lost host will (probably) run a interactive multitasking
      *        operating system, we can make no guarantees about sceduling and it is
      *        quite likely that the data receiving application is not sceduled for
      *        several 100ms at a time. In order to avoid packet loss during such
      *        times, we must make sure that the network input buffer is sufficiently
      *        big. It is recommended to make the buffer at least 8MByte or better
      *        32MByte large. [Byte]
      * @param DataSocketIn In: Usually DataSocket will be INVALID_SOCKET and
      *        TTMData_c will provide its own socket. However if you wish to
      *        force TTMData_c to use a given socket (e.g. because it it is part
      *        of a bigger structure or class), you can provide a socket descriptor
      *        here. The provided socket must remain valid until Disconnect() is called.
      * @returns Error Code
      * @see Disconnect()
      */
    FlexIOStatus Connect(in_addr_t BoardIPAddr, in_port_t BoardDataPort = FlexIODataPort,
                            in_addr_t LocalIPAddr = INADDR_ANY, in_port_t LocalDataPort = 0,
                            uint32_t SocketBufferSize = 8 * 1024 * 1024,
                            SOCKET DataSocketIn = INVALID_SOCKET);

    /** @brief Check if a Data Connection has been established
      *
      * IsConnected() checks is a Data Connection has been established.
      * Note that IsConnected() only checks if Connect() has been called.
      * We can not know if our data source is still alive and willing to
      * send data to us.
      *
      * @returns Connection Status
      */
    bool IsConnected();

     /** @brief Dispose of a Data Connection to a TTM8000 Board.
      *
      * Disconnect closes a TTMData Connection.
      */
    FlexIOStatus Disconnect();

    /** @brief Send a dummy Network Packet from the PC to the TTM8000 Data Port
      *
      * The communication on the data link is basically unidirectional.
      * The TTM8000 sends timetag data and the PC receives it and thats
      * all that should theoretically be required. In real life however
      * there are firewalls on the PC that try to eliminate unwanted
      * or dangerous network traffic. During Connect() the PC sends a
      * packet to the TTM8000 (that is ignored by the TTM8000) so that
      * the firewall can learn that there is a legitimate application
      * on the PC that is interested in packets from the TTM8000. However
      * depending on the firewall the priviledges that were aquired
      * by this dummy packet will be revoked after some time without
      * additional outbound traffic has passed. Thus it is important
      * to send some dummy packet from the PC to the TTM8000 to keep
      * the hole that was punched into the firewall from closing again.
      * SendDummyPacket() does exactly that. It should be called from
      * time to time (maybe every 10 seconds) - As often as needed to
      * keep the firewall from closing the port and as rarely as possible
      * to avoid unnecessary network traffic.
      *
      * @returns Error Code
      */
    FlexIOStatus SendDummyPacket();

    /** @brief Send a dummy Network Packet from the PC to the TTM8000 Data Port
      *
      * Some users can not use TTMData_c::Connect() to build the network
      * connection. To make it possible to punch a hole into the firewall
      * even for these users, we shall offer SendDummyPacket as a static
      * function too. Applications that use TTMData_c objects and member
      * functions don't need this static function.
      *
      * @param DataSocket    In: The Socket to be used for sending the Network Packet
      * @param BoardIPAddr   In: Board IP-Address (Host Byte Order)
      * @param BoardDataPort In: UDP-Port for the Data Port of the Board (Host Byte Order)
      * @returns Error Code
      *
      * @see SendDummyPacket()
      */
    static FlexIOStatus SendDummyPacket(SOCKET DataSocket, in_addr_t BoardIPAddr, in_port_t BoardDataPort);

    /** @brief Get the BSD socket used to receive data.
      *
      * GetSocket() provides access to the handle of the BSD-style socket
      * used for the TTMData connection. You can use it if your application has
      * special requirements (e.g. configuration settings) that can not be addressed
      * otherwise. However you should remember that the socket belongs to TTMLib
      * and must not be closed etc.
      * @note Most applications will NOT need to use GetSocket().
      * @param DataSocket Out: Socket Handle
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus GetSocket(SOCKET *DataSocket);

    /** @brief Get the IP-Address/UDP Port used to receive data.
      *
      * GetSocketAddr() provides the IP-Address/UDP Port of the socket
      * used for the TTMData connection.
      * This might be useful for status displays/debugging however most applications
      * will probably never need to call GetSocketAddr().
      * @param LocalIPAddr Out: IP-Address of the Data-Socket on the local host
      *        Pass NULL if you are not interested in this information.
      * @param LocalDataPort Out: UDP Port of the Data-Socket on the local host
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus GetSocketAddr(in_addr_t *LocalIPAddr, in_port_t *LocalDataPort);

    /** @brief Set Network Buffer Size of the Recv. Socket
      *
      * SetSocketBufferSize() sets the buffer size of the Timetag Receive
      * Socket at the Local Host.\n
      * @note Running at full speed, the TTM8000 Board can generate up to 80MByte/s of
      *  Timetags. Since the lost host will (probably) run a interactive multitasking
      *  operating system, we can make no guarantees about sceduling and it is quite
      *  likely that the data receiving application is not sceduled for several 100ms
      *  at a time. In order to avoid packet loss during such times, we must make sure
      *  that the network input buffer is sufficiently big. It is recommended to make
      *  the buffer at least 8MByte or better 32MByte large.
      * @param BufferSize In: Requested Network Buffer Size [Byte] - Note that this is a
      *  request, not an order. The operating system will try to honor it, generally
      *  overfulfilling the request to be on the safe side when lots of small packets
      *  with much management overhead have to be handled, however it might also choose
      *  to limit the size to fit local memory availability.
      *  On Linux you should check /proc/sys/net/core/rmem_max for the max. buffer size
      *  permitted on your system. If the limit stated there is too low, you can fix
      *  it using "echo 33554432 >/proc/sys/net/core/rmem_max" or by adding the line
      *  "net.core.rmem_max = 33554432" to /etc/sysctl.conf. Both operations will
      *  require root privileges.
      * @returns Error Code
      * @see GetSocketBufferSize()
      */
    FlexIOStatus SetSocketBufferSize(uint32_t BufferSize);

    /** @brief Get Network Buffer Size of the Recv. Socket
      *
      * GetSocketBufferSize() gets the buffer size of the Timetag Receive
      * Socket at the Local Host.\n
      * @note Running at full speed, the TTM8000 Board can generate up to 80MByte/s of
      *  Timetags. Since the lost host will (probably) run a interactive multitasking
      *  operating system, we can make no guarantees about sceduling and it is quite
      *  likely that the data receiving application is not sceduled for several 100ms
      *  at a time. In order to avoid packet loss during such times, we must make sure
      *  that the network input buffer is sufficiently big. It is recommended to make
      *  the buffer at least 8MByte or better 32MByte large.
      * @param BufferSize Out: Actual Network Buffer Size [Byte] - Note that the
      *  operating system will generally round up the BufferSize when setting it.
      *  Thus it is likely that GetSocketBufferSize() does NOT exactly report the
      *  value requested when calling by SetSocketBufferSize() but a significantly
      *  higher value (a factor of 2 is quite common!).
      * @returns Error Code
      * @see SetSocketBufferSize()
      */
    FlexIOStatus GetSocketBufferSize(uint32_t *BufferSize);

    /** @brief Check if Timetag Data Packets are available
      *
      * @param DataAvailable Out: Data is available (TRUE) or not available (FALSE)
      * @param MilliTimeout   In: If no data is immediatly available, how long do we want
      *        to wait for data to arrive before we concede failure. [ms]
      * @returns Error Code
      */
    FlexIOStatus DataAvailable(bool *DataAvailable, uint32_t MilliTimeout = 0);

    /** @brief Fetch a Packet of Timetag Data Packets
      *
      * @note  FetchData() already takes care of all endian issues for both the
      *        packet header and the packet data. You MUST NOT call TimeTagPacketNetToHost()
      *        for packets received by TTMDataFetchData().
      * @param TimetagBuffer Out: Received Timetag Data Packet
      * @param MilliTimeout   In: If no data is immediatly available, how long do we want
      *        to wait for data to arrive before we concede failure. [ms]
      * @returns Error Code
      */
    FlexIOStatus FetchData(TTMDataPacket_t *TimetagBuffer, uint32_t MilliTimeout);

    /** @brief Adjust Byte Order of Timetag Data Packets
      *
      * TimeTagPacketNetToHost() convert a TTMDataPacket to Local Byte Order.
      * The packet header is converted from network byte order to local byte order.
      * The packet payload is converted from the order it comes in (which is configurable
      * in MeasureConfig) to local byte order (if it isn't already in correct byte order).
      * @note This function is provided for people that want/need to receive the network
      * packets using custom code (rather than FetchData()) (e.g. the Qt-Socket library)
      * and now need to convert the raw received packet to local byte order. Do NOT use
      * TimeTagPacketNetToHost() after receiving a packet via FetchData().
      * @param TimetagBuffer In/Out: Timetag Data Packet
      * @returns Error Code
      */
    static FlexIOStatus TimeTagPacketNetToHost(TTMDataPacket_t *TimetagBuffer);

    /** @brief Adjust Byte Order of Timetag Data Packets
      *
      * TimeTagPacketHostToNet() converts the Header of a TTMDataPacket from
      * Local Byte order to Network Byte Order. Note that this only affects the
      * Header. It does NOT change the byte order of the payload, since this
      * would incur much work and we do not know the byte order of the receiver.
      * We shall assume that the receiver will call TimeTagPacketNetToHost() that
      * will know the correct byte order required for itself and will fix any
      * byte order mismatch. Thus if we are lucky and both sender and receiver
      * use the same byte order, we never need to touch anything.
      *
      * Note: TimeTagPacketHostToNet() is NOT the same as TimeTagPacketNetToHost()
      * since TimeTagPacketNetToHost() does look at the byte order of the measurement
      * data, while TimeTagPacketHostToNet() does not.
      *
      * @param TimetagBuffer In/Out: Timetag Data Packet
      * @returns Error Code
      */
    static FlexIOStatus TimeTagPacketHostToNet(TTMDataPacket_t *TimetagBuffer);

    /** @brief Magic Value for Undefined TimeHigh Value
      *
      * ExpandData() expands Timetags in compressed I-Mode Continous format to
      * uncompressed format. When it starts its operation it does not know the
      * current HighTime value and has to discard all timetags until the first
      * HighTime marker comes along. At high data rates, this might cause several
      * complete data packets to become discarded. Thus we need a magic marker
      * that is never used in real data (Note that 0 is a valid (and common)
      * value for HighTime and thus not suitable for that purpose). Thus we
      * have chosen 1 << 60.
      */
#if defined (_WIN32) && defined (_MSC_VER) && (_MSC_VER == 1200)
// Microsoft Visual Studio 6 has problems with modern constant definitions ;(
    static const uint64_t TimeHighUndefined;
#else
    static const uint64_t TimeHighUndefined = __UINT64_C(1LL << 60);
#endif

    /** @brief Count/Limit the Number of Events in a Timetag Packet
      *
      * Sometimes we want to process a given number of events (rather than processing an
      * 'infinite' stream. Thus we need a function that will count the number of events
      * in a Timetag Packet and will discard any extranous events.
      *
      * Using a sufficiently high limit, it is also possible to use TimeTagClipPacket()
      * to count the number of valid events in a packet.
      *
      * @param TimetagBuffer In: Timetag Data Packet
      * @param TimeHigh      In: Most recently transmitted field of High Bits when using
      *        IMode_Ext64_Packet Format. Ignored for all other formats.
      *        Use TimeHighUndefined if there has not been a High Bit field, and any other
      *        value if a High bit field has come by.
      * @param MaxEventCnt   In: How many events do we want at most. If there are more than
      *        MaxEventCnt events in the packet, the additional events will be discarded. If
      *        there are less events, the packet will be unchanged.
      * @param EventCnt     Out: How many events are in the packet after clipping.
      * @returns Error Code
      */
    static FlexIOStatus TimeTagClipPacket(TTMDataPacket_t *TimetagBuffer, uint64_t *TimeHigh,
                                       int MaxEventCnt, int *EventCnt);

    /** @brief Expand Timetag Data from IMode_Ext64_Packed Format to IMode_Ext64_Flat Format
      *
      * Handling 60 Bits of Time information for each Timetag provides extended protection
      * against overflow, however it also requires 8 Byte/Timetag to be handled. Since the
      * high bits of the 60 Bit Time information change only rarely, there is a packed data
      * format that transmits these high bits only when they change and manages to encode
      * most Timetags in just 4 Byte/Timetag (reducing network bandwidth and storage requirement
      * by 50%). ExpandData() converts Timetag Data in this packed format to flat,
      * unpacked data that is easier to process.
      * @note  The Source Data MUST be in IMode_Ext64_Packed Format. All other formats
      *  generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param TimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to ExpandData(),
      *        then reuse the same variable for each further call to ExpandData().
      * @returns Error Code
      *
      * @see CompressData()
      */
    static FlexIOStatus ExpandData(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src, uint64_t *TimeHigh);

    /** @brief Compress Timetag Data from IMode_Ext64_Flat Format to IMode_Ext64_Packed Format
      *
      * Handling 60 Bits of Time information for each Timetag provides extended protection
      * against overflow, however it also requires 8 Byte/Timetag to be handled. Since the
      * high bits of the 60 Bit Time information change only rarely, there is a packed data
      * format that transmits these high bits only when they change and manages to encode
      * most Timetags in just 4 Byte/Timetag (reducing network bandwidth and storage requirement
      * by 50%). CompressData() converts Timetag Data in the flat format to this packed format,
      * @note  The Source Data MUST be in IMode_Ext64_Flat Format. All other formats
      *  generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param TimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to CompressData(),
      *        then reuse the same variable for each further call to CompressData().
      * @returns Error Code
      *
      * @see ExpandData()
      */
    static FlexIOStatus CompressData(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src, uint64_t *TimeHigh);

    /** @brief Expand Timetag Data from TTFormat_MultiIMode_EXT64_PACK Format to
      *                                    TTFormat_MultiIMode_EXT64_FLAT Format
      *
      * Handling 56 Bits of Time information for each Timetag provides extended protection
      * against overflow, however it also requires 8 Byte/Timetag to be handled. Since the
      * high bits of the 56 Bit Time information change only rarely, there is a packed data
      * format that transmits these high bits only when they change and manages to encode
      * most Timetags in just 4 Byte/Timetag (reducing network bandwidth and storage requirement
      * by 50%). ExpandMultiData() converts Timetag Data in this packed format to flat,
      * @note  The Source Data MUST be in TTFormat_MultiIMode_EXT64_PACK Format. All other formats
      *  generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param TimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to ExpandMultiData(),
      *        then reuse the same variable for each further call to ExpandMultiData().
      * @returns Error Code
      *
      * @see CompressData()
      */
    static FlexIOStatus ExpandMultiData(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src, uint64_t *TimeHigh);

    /** @brief Compress Timetag Data from TTFormat_MultiIMode_EXT64_FLAT Format to
      *                                      TTFormat_MultiIMode_EXT64_PACK Format
      *
      * Handling 56 Bits of Time information for each Timetag provides extended protection
      * against overflow, however it also requires 8 Byte/Timetag to be handled. Since the
      * high bits of the 60 Bit Time information change only rarely, there is a packed data
      * format that transmits these high bits only when they change and manages to encode
      * most Timetags in just 4 Byte/Timetag (reducing network bandwidth and storage requirement
      * by 50%). CompressMultiData() converts Timetag Data in the flat format to this packed format,
      * @note  The Source Data MUST be in TTFormat_MultiIMode_EXT64_FLAT Format. All other formats
      *  generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param TimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to CompressMultiData(),
      *        then reuse the same variable for each further call to CompressMultiData().
      * @returns Error Code
      *
      * @see ExpandData()
      */
    static FlexIOStatus CompressMultiData(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src, uint64_t *TimeHigh);

    /** @brief Convert Timetag Data from any I-Mode EXT64 (=Continuous) format to any other
      *
      * There are 4 different data types, that all deal with timetags recorded in I-Mode
      * continous format (= EXT64). Data can either come from just a single TTM board
      * or several TTM boards and it can be either compressed or uncompressed.
      * ConvertMultiBoardData() offers conversion from any of these 4 formats to any
      * other format.
      * @note  The Source Data MUST be in *IMode_EXT64_* Format. All other formats
      *  generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param DestFormat In: Requested data format for Dest
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param BoardID In: If the source format does not contain BoardID information,
      *        but the destination format does, then BoardID is inserted in this field.
      *        If the source format does contain BoardID information, but the destination
      *        format does not, than only data elements that refer to the given board
      *        are reported. Otherwise BoardID is ignored.
      * @param SrcTimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to ConvertMultiBoardData(),
      *        then reuse the same variable for each further call to ConvertMultiBoardData().
      *        SrcTimeHigh is not used if no decompression of data takes place.
      * @param DestTimeHigh In/Out: Most recently transmitted field of High Bits
      *        Initialize with TimeHighUndefined before the first call to ConvertMultiBoardData(),
      *        then reuse the same variable for each further call to ConvertMultiBoardData().
      *        DestTimeHigh is not used if no compression of data takes place.
      * @returns Error Code
      *
      * @see ConvertMultiBoardData()
      */
    static FlexIOStatus ConvertMultiBoardData(TTMDataPacket_t *Dest, TTMDataFormat_t DestFormat,
                                    const TTMDataPacket_t *Src, int BoardID,
                                    uint64_t *SrcTimeHigh, uint64_t *DestTimeHigh);

    /** @brief Eliminate Duplicate Events from Timetag Data in IMode_Ext64_Flat
      *
      * Some signal sources produce tend to 'bounce'. I.e. When a 'real-world'
      * event trigger occurs (e.g. arrival of a optical signal), the detector
      * that converts this extenal event into a electronic event does not create
      * a single, nice, steep transition from one voltage level to another, but
      * creates a transition that has serious over-/undershoot and/or swings
      * back and forth between various voltage levels multiple times. If
      * such a signal is sampled with a slow device, this can be acceptable.
      * However the TTM8000 is designed for optimal timing resolution and will
      * create separate events for every time the signal bounces back and forth.
      * While these extra events are a faithful measurement of the events that
      * actually took place at the inputs of the TTM8000 they are sometimes not
      * welcome. In such situations we wish to group all events that occured
      * within a short time on a single channel into a single event that occurred
      * at the time of the first event of this group. All other events of that
      * group are discarded. Thus we introduce a 'dead-time' after each event
      * were the specific channel is no longer sensitive to further events.
      *
      * @note  The Source Data MUST be in IMode_Ext64_Flat Format. All other formats
      *        generate an error of TTM_InvalidPacketFormat.
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param PrevEvent In/Out: When did the last event occur for each channel
      *        Initialize with TimeHighUndefined before the first call to UniqueEventData(),
      *        then reuse the same variable for each further call to UniqueEventData().
      *        Initialize with
      * @param DeadTime In: How far apart must two events be to be considered
      *        two independent events (and not the rebounce of the same event?)
      *        The DeadTime is measured in Ticks of 82.3045ps. DeadTime[0..7]
      *        are used for the rising edge of Stop1..Stop8, DeadTime[8..15] are
      *        used for the falling edges.
      * @param ExtendDeadTimeOnRetrigger In: Does a group of events end when the
      *        _first_ event of the group occured more than DeadTime Ticks ago, or
      *        does the current group of events remain active until the _most recent_
      *        event of the group occured more than DeadTime Ticks ago. i.e. do
      *        we restart the DeadTime every time a 'dead' event arrives?
      * @returns Error Code
      */
    static FlexIOStatus RemoveDuplicateEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src,
                                        uint64_t PrevEvent[16], const uint64_t DeadTime[16],
                                        bool ExtendDeadTimeOnRetrigger);

    /** @brief Add Channel Offsets to Timetag Event Data
      *
      * Timetag Data that is delivered by the TTM8000 module reflects the time when signals
      * _arrive_ at the TTM8000. Usually this is not the quantity that the user is most
      * interested in. The user will generally be more interested at the time when the
      * events occurred that caused these events. However there is a always a delay between
      * the events that cause a pulse to be sent to the TTM8000 and the arrival/detection
      * of this pulse by the TTM8000. This delay is caused by signal transformers (e.g.
      * detectors), amplifiers and signal shapers that create nice, detectable signals from
      * raw sensor data and cables. Each of these components has its own delay, so if we
      * wish to compare the timestamps that are generated by the TTM8000 we need to subtract
      * these delays to get to the raw data.
      *
      * TimeShiftEvents() will add a channel- and edge-dependent Offset to every timetag
      * to align all Timetags according to the delays of their sources.
      *
      * Note: TimeShiftEvents() will only work for Data in TTFormat_IMode_EXT64_FLAT format.
      *
      * Note: Timetag Data that comes from the TTM8000 module is chronologically sorted
      * according to the time the events were detected by the TTM8000. After adding an
      * offset the data will generally no longer be chronologically sorted. If you need
      * sorted data, you should use SortEvents() (and friends).
      *
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src   In: Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param OffsetTicks In: Number of ticks to add to each measurement result (by channel/edge)
      * @returns Error Code
      */
    static FlexIOStatus TimeShiftEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src,
                                              int32_t OffsetTicks[16]);

    /** @brief Exchange BoardIDs / ChannelIDs / Slopes of Timetag Data
      *
      * Sometimes it can be useful to relabel the event sources of Timetag
      * data. ChannelExchange() can perform arbitrary permutations on channel
      * numbers and can also join events from two or more sources into a
      * a single event source by relabeling events from all joining sources
      * to the same output channel. Furthermore events from certain sources
      * can be removed.
      *
      * A Timetag Source is identified by a 4-bit BoardID, a 3-bit ChannelID
      * and a 1-bit Slope resulting in a 8-bit Identifier. Some measurement
      * modes use only one Board (for which BoardID 0 is used) and/or only
      * two instead of 8 Channels, thus not all Identifiers can be present
      * in the input data stream.
      *
      * @param Dest Out: Destination Timetag Data Packet
      * @param Src  In:  Source Timetag Data Packet - It is safe to use
      *        the same area of memory for Src and Dest!
      * @param ExchangeRule In: New value of BoardID/ChannelID/Slope for
      *        every possible old value of BoardID/ChannelID/Slope. Use
      *        -1 to eliminate events from a given source from the output
      *        stream.
      * @returns Error code
      */
    static FlexIOStatus ChannelExchange(TTMDataPacket_t *Dest,
                        const TTMDataPacket_t *Src, int16_t ExchangeRule[256]);

  private:
    CoreTTMData_c *TTMDataHnd; /**< Handle to the TTMData Connection */
 }; // class TTMData_c

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                        class TTMEvtSort_c                        **
 **                                                                  **
 **********************************************************************
 **********************************************************************/
/** @brief Core TTMEvtSort_c Class (Internal)
  */
class CoreTTMEvtSort_c;

/** @brief Sort a Stream of Timetag Event Data
  *
  * Timetag Data that comes directly from the TTM8000 module is
  * chronologically sorted. However if you add an offset to each timetag
  * to compensate for channel-dependent delays in the event creation
  * system (e.g. cable delays) using e.g. TTMData_t::TimeShiftEvents()
  * the events are no longer sorted. TTMEvtSort_c() can be used to
  * obtain a stream of sorted timetags again.
  *
  * Note that TTMEvtSort_c sorts a stream of timetags, not just a finite
  * array of timetags. Thus it needs to buffer where it can store timetags
  * that might be preceeded by timetags that arrive in laters packets.
  * Also note that due to this buffering the number of timetag returned
  * by a single call of SortEvents() will generally not be the same as
  * the number of timetags provided for this call. (Looking at the entire
  * stream of timetags the number of provided and returned timetags will
  * of course match). Be sure that the destination buffer can hold at least
  * 2048 timetags more than provided by the source buffer.
  *
  * Note: SortEvents() will only work for Data in TTFormat_IMode_EXT64_FLAT format.
  */
class LIBTTM_API TTMEvtSort_c
 {
  public:
    /** @brief Constructor for TTMEvtSort
      *
      */
    TTMEvtSort_c();

    /** @brief Destructor for TTMEvtSort
      *
      */
    ~TTMEvtSort_c();

    /** @brief Define the maximal amount of missorting
      *
      * TTMEvtSort sorts a stream of timetags packet by packet, not just
      * a single array. Thus there is always just a partial view of the
      * complete stream and without knowledge of the maximal amount of
      * missorting we could never be sure that any timetag can not be preceeded
      * by a timetag arriving in the future. Thus we shall define a limit
      * for the maximal amount of missorting we expect and commit all timetags
      * that are at least MaxShuffleTicks older than the current timetag to
      * the output stream.
      *
      * @param MaxShuffleTicks In: Number of Ticks that a Timetag might be missorted.
      * @returns Error Code
      */
    FlexIOStatus SetMaxShuffleTicks(uint32_t MaxShuffleTicks);

    /** @brief Sort Events by Timestamp
      *
      * SortEvent() will read a packet of unsorted events and provide a
      * packet of sorted events. Note that TTMEvtSort performs internal
      * buffering to make sure that sorting works even over beyond the
      * boundaries of timetag packets. Thus the SortEvent() will keep
      * some of the source events in its internal buffer and provide
      * some other events from its internal buffer that were kept from
      * previous calls to SortEvent(). The number of events in Src and Dest
      * will usually not be the same...
      *
      * @param Dest Out: Packet of Sorted Data
      * @param Src   In: Packet of Unsorted Data - Using the same memory for
      *                  Src and Dest is acceptable, however performance will
      *                  be better if different locations are used.
      * @returns Error Code
      */
    FlexIOStatus SortEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src);

    /** @brief Fetch all remaining Timetags from the internal Timetag Buffer
      *
      * @param Dest Out: Destination Timetag Data Packet
      * @returns Error Code
      */
    FlexIOStatus Flush(TTMDataPacket_t *Dest);

  private:
    CoreTTMEvtSort_c *TTMEvtSortHnd; /**< Handle to the TTMEventSort Core */
 }; /* class TTMEvtSort_c */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                    class TTMSoloEvtFilter_c                      **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Core CoreTTMSoloEvtFilter_c Class (Internal)
  */
class CoreTTMSoloEvtFilter_c;

/** @brief Delete all solitary Timetags from a Stream of Timetag Events
  *
  * Sometimes we are only interested in groups of events that occurr
  * at (approximately) the same time. Solitary events, that do not
  * have a neighbor within a given amount of time are of no interest.
  * TTMSoloEvtFilter_c() will scan a stream of Timetags and will
  * remove all timetags that have no neighbor within a specified
  * interval.
  *
  * TTMSoloEvtFilter_c() will only work correctly if the timetags
  * are given in TTFormat_IMode_EXT64_FLAT format and the stream is
  * chronologically sorted.
  */
class LIBTTM_API TTMSoloEvtFilter_c
 {
  public:
    /** @brief Constructor for TTMSoloEvtFilter
      *
      */
    TTMSoloEvtFilter_c();

    /** @brief Destructor for TTMSoloEvtFilter
      *
      */
    ~TTMSoloEvtFilter_c();

    /** @brief Define the Neighborhood
      *
      * TTMSoloEvtFilter removes solitary timetags from a stream of
      * timetags. A timetag is considered solitary if its nearest
      * neighbor on any channel is more than MaxDeltaTicks ticks away.
      * Since we are looking for the nearst neighbor on any channel
      * an efficient algorithm can be used to check if there is a neighbor.
      *
      * @param MaxDeltaTicks In: Number of Ticks (at 12.15GHz) that any
      *   two event may be separated and still be considered neighbors
      * @returns Error Code
      */
    FlexIOStatus SetMaxDeltaTicks(uint32_t MaxDeltaTicks);

    /** @brief Define the Neighborhood
      *
      * TTMSoloEvtFilter removes solitary timetags from a stream of
      * timetags. A timetag is considered solitary if for all channels
      * the nearest preceeding neighbor timetag is further away than
      * MaxChnDeltaTicks[OtherChn][CurrChn] and the nearest succeeding
      * timetag is further away than MaxChnDeltaTicks[CurrChn][OtherChn].
      * This algorithm requires more computational power than simply
      * looking for the nearest neighbor on any channel, but also offers
      * much more flexibility.
      *
      * Often the application domain provides knowledge that any timetag
      * can have at most one preceeding neighbor on each channel. If we
      * have this additional information, we can abort the search for
      * neighbors as soon as a match has been found and save some time.
      * If we do not have this quarantee, we need to do a DeepSearch.
      *
      * @param MaxChnDeltaTicks In: Number of Ticks (at 12.15GHz) that any
      *   two event may be separated and still be considered neighbors.
      *   Order of Index: MaxDeltaTicks[EarlyEvt][LateEvt]!
      * @param PerformDeepSearch In: Do we need to look for multiple
      *   neighbors preceeding an event, or is sufficient to look for
      *   just one?
      * @returns Error Code
      */
    FlexIOStatus SetMaxDeltaTicks(uint32_t MaxChnDeltaTicks[16][16], bool PerformDeepSearch);

    /** @brief Remove Solitary Events from a Stream of Timetags
      *
      * FilterEvents() will read a packet of timetags and remove all
      * timetags that have no neighbor as defined by SetMaxDeltaTicks().
      * Note that TTMSoloEvtFilter performs internal buffering to make
      * sure that filtering works even over beyond the boundaries of
      * timetag packets. Thus the FilterEvents() will keep some of the
      * source events in its internal buffer and provide some other events
      * from its internal buffer that were kept from previous calls to
      * FilterEvents(). The number of events in Src and Dest
      * will usually not be the same...
      *
      * @param Dest Out: Packet of Filtered Data
      * @param Src   In: Packet of Unfiltered Data - Using the same memory for
      *                  Src and Dest is acceptable, however performance will
      *                  be better if different locations are used.
      * @returns Error Code
      */
    FlexIOStatus FilterEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src);

    /** @brief Fetch all remaining Timetags from the internal Timetag Buffer
      *
      * @param Dest Out: Destination Timetag Data Packet
      * @returns Error Code
      */
    FlexIOStatus Flush(TTMDataPacket_t *Dest);

  private:
    CoreTTMSoloEvtFilter_c *TTMSoloEvtFilterHnd; /**< Handle to the TTMSoloEvtFilter Core */
 }; /* class TTMSoloEvtFilter_c */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                    class MultiTTMEvtSorter_c                     **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Core CoreMultiTTMEvtSorter_c Class (Internal)
  */
class CoreMultiTTMEvtSorter_c;

/** @brief Merge TTMs from multiple TTM8000 Modules into a single sorted data stream
  *
  * A single TTM8000 module offers just 8 channels. When more channels
  * are needed, up to 16 TTM8000 modules can be used to work with up
  * to 128 parallel channel. The reference clocks of all the TTM8000s
  * can be perfectly synchronized thus providing all measurements with
  * a clock that progresses at equal speed. Thus the timestamps from all
  * channel refer to a single common time reference and can freely be
  * mixed.
  *
  * Notes: While the input clocks of all modules are derived from a
  * common reference clock, the actual high speed clock for the TDC
  * (time-to-digital converter) is generated by an independent PLL on
  * each module. This introduces a timing jitter with of about 200ps.
  * Furthermore the Start signal is daisy-chained with cables from
  * one one TTM8000-board to the next. Each of these cables introduces
  * a delay, that is however fixed for a given setup. Since every input
  * has its own specific compensation delay to accomodate for different
  * sensor/pulse source speeds and/or different cable length from the
  * sensor to the input of the TTM8000 the additional delay for the
  * start offset can easily be accounted for.
  *
  * MultiTTMEventSorter is a class that takes events from several
  * TTM8000 boards (all measured in I-Mode continous, using a roughly
  * synchronized start), adds a channel specific offset to each event
  * and sorts all events into a single stream of events. This takes
  * into account that some TTM8000 boards might report their events
  * earlier than other and that extensive buffering might be needed.
  * Furthermore MultiTTMEventSorter can debounce inputs. If several
  * events occur on a single channel within a given (channel dependent)
  * interval, all but the first event are discarded.
  * The sorted events can than be obtained in packets for further
  * processing.
  */
class LIBTTM_API MultiTTMEvtSorter_c
 {
  public:
    /** @brief Constructor for MultiTTMEvtSorter
      *
      * @param ChannelCnt In: Number of active Channels (Indexed [0..ChannelCnt-1])
      * @param NetMilliTimeout In: Maximal Interval between to TTM8000 frames (used to detect idle TTM8000s)
      * @param MaxChnEventFreq In: Maximal number of events per second per channel (used to size buffers)
      */
    MultiTTMEvtSorter_c(int ChannelCnt = 128, int NetMilliTimeout = 20, int MaxChnEventFreq = 5000000);

    /** @brief Constructor for MultiTTMEvtSorter
      *
      * @param ActiveChannelMaskChannelCnt In: Mask active Channels - One Byte for each Board with Bit7<->Stop8 .. Bit0<->Stop1
      * @param NetMilliTimeout In: Maximal Interval between to TTM8000 frames (used to detect idle TTM8000s)
      * @param MaxChnEventFreq In: Maximal number of events per second per channel (used to size buffers)
      */
    MultiTTMEvtSorter_c(uint8_t ActiveChannelMaskChannelCnt[16], int NetMilliTimeout = 20, int MaxChnEventFreq = 5000000);

    /** @brief Destructor for MultiTTMEvtSorter
      */
    ~MultiTTMEvtSorter_c();

    /** @brief Set the List of active Channels
      *
      * MultiTTMEvtSorter allocates buffer memory only for channels
      * that are actually used. Thus it needs to know which channels
      * are active. SetChannelMask takes one byte for each TTM8000 board.
      * Each bit in this byte represents one Stop channel (Bit7 <-> Stop8,
      * Bit6 <-> Stop7, ... Bit0<->Stop1)
      *
      * @param ChannelMask In: List of active Channels
      * @returns Error Code
      *
      * @see GetChannelMask()
      */
    FlexIOStatus SetChannelMask(uint8_t ChannelMask[16]);

    /** @brief Get the List of active Channels
      *
      * MultiTTMEvtSorter allocates buffer memory only for channels
      * that are actually used. Thus it needs to know which channels
      * are active. GetChannelMask takes one byte for each TTM8000 board.
      * Each bit in this byte represents one Stop channel (Bit7 <-> Stop8,
      * Bit6 <-> Stop7, ... Bit0<->Stop1)
      *
      * @param ChannelMask Out: List of active Channels
      * @returns Error Code
      *
      * @see SetChannelMask()
      */
    FlexIOStatus GetChannelMask(uint8_t ChannelMask[16]);

    /** @brief Set the Channel Offsets
      *
      * Different channels obtain their pulses from different sources
      * that are connected with cables of different length. Some sources
      * are faster, others require more time to generate a output pulse
      * after their real-world event occured. Some cables are longer and
      * thus require more time to propagate the signal from the pulse source
      * to the TTM8000 than others. Thus events that logically occur at
      * the same instant, will reach the TTM8000 at different times and
      * will create different timestamps.
      *
      * However the differences in timing between the different channels
      * are constant for a given setup, and thus can easily be compensated
      * by adding a channel dependent offset to each measurement to bring
      * all events to the same time base.
      *
      * SetChannelOffsetTicks() can be used to define the required offsets.
      *
      * @param Offset In: Offset for each channel (in Ticks of 82.3045ps)
      * @returns Error Code:
      *
      * @see GetChannelOffsetTicks()
      */
    FlexIOStatus SetChannelOffsetTicks(int32_t Offset[]);

    /** @brief Get the Channel Offsets
      *
      * Different channels obtain their pulses from different sources
      * that are connected with cables of different length. Some sources
      * are faster, others require more time to generate a output pulse
      * after their real-world event occured. Some cables are longer and
      * thus require more time to propagate the signal from the pulse source
      * to the TTM8000 than others. Thus events that logically occur at
      * the same instant, will reach the TTM8000 at different times and
      * will create different timestamps.
      *
      * However the differences in timing between the different channels
      * are constant for a given setup, and thus can easily be compensated
      * by adding a channel dependent offset to each measurement to bring
      * all events to the same time base.
      *
      * GetChannelOffsetTicks() can be used to get the currently used offsets.
      *
      * @param Offset Out: Offset for each channel (in Ticks of 82.3045ps)
      * @returns Error Code:
      *
      * @see SetChannelOffsetTicks()
      */
    FlexIOStatus GetChannelOffsetTicks(int32_t Offset[]);

    /** @brief Set Channel Deadtimes
      *
      * Some Sensors tend to generate a stream of quick pulses after a
      * single external event occured. Only the first of these pulses
      * actually corresponds to the external event, all others are just
      * afterpulses/bounces. These artefacts, that do not correspond to
      * any external event should be discarded before further processing.
      *
      * How much afterpulsing/bouncing occurs depends on the type of
      * sensor that is used. MultiTTMEvtSorter can use a common limit for
      * all sensors (assuming that they are of equal type) or set a
      * individual limit for each sensor. Furthermore we need to decide
      * what shall happen if a bounce is detected. Do we want to allow
      * retriggering and extend the debouncing time window every time
      * a bound is detected (i.e. wait until the event channel has
      * been quiet for the duration of one DeadTime before accepting
      * new events) or do we simply want to wait for one DeadTime after
      * the first event (of a bouncing chain of events) was detected and
      * then accept events again?
      *
      * @param DeadtimeTicks In: Common duration of DeadTime used for all channels [Ticks *82.3045ps]
      * @param RetriggerEnable In: Extend the DeadTime Window every time a pulse is detected?
      * @returns Error Code
      *
      * @see SetChannelDeadtimeTicks(), GetChannelDeadtimeTicks()
      */
    FlexIOStatus SetChannelDeadtimeTicks(int32_t DeadtimeTicks, bool RetriggerEnable);

    /** @brief Set Channel Deadtimes
      *
      * Some Sensors tend to generate a stream of quick pulses after a
      * single external event occured. Only the first of these pulses
      * actually corresponds to the external event, all others are just
      * afterpulses/bounces. These artefacts, that do not correspond to
      * any external event should be discarded before further processing.
      *
      * How much afterpulsing/bouncing occurs depends on the type of
      * sensor that is used. MultiTTMEvtSorter can use a common limit for
      * all sensors (assuming that they are of equal type) or set a
      * individual limit for each sensor. Furthermore we need to decide
      * what shall happen if a bounce is detected. Do we want to allow
      * retriggering and extend the debouncing time window every time
      * a bound is detected (i.e. wait until the event channel has
      * been quiet for the duration of one DeadTime before accepting
      * new events) or do we simply want to wait for one DeadTime after
      * the first event (of a bouncing chain of events) was detected and
      * then accept events again?
      *
      * @param DeadtimeTicks In: Duration of DeadTime for each Channel [Ticks *82.3045ps]
      * @param RetriggerEnable In: Extend the DeadTime Window every time a pulse is detected?
      * @returns Error Code
      *
      * @see SetChannelDeadtimeTicks(), GetChannelDeadtimeTicks()
      */
    FlexIOStatus SetChannelDeadtimeTicks(int32_t DeadtimeTicks[128], bool RetriggerEnable);

    /** @brief Get Channel Deadtimes
      *
      * @param DeadtimeTicks Out: Duration of DeadTime for each Channel [Ticks *82.3045ps]
      * @param RetriggerEnable Out: Extend the DeadTime Window every time a pulse is detected?
      * @returns Error Code
      *
      * @see SetChannelDeadtimeTicks(), GetChannelDeadtimeTicks()
      */
    FlexIOStatus GetChannelDeadtimeTicks(int32_t DeadtimeTicks[128], bool *RetriggerEnable);

    /** @brief Add Timetags to the Event Sorter
      *
      * AddSortEvents feeds a Packet of TimeStamps to the EventSorter.
      * The Timestamps must be measured in I-Mode continous. They can be
      * encoded with or without compression and can originate from a single
      * board (in which case they are assigned the provided BoardID) or
      * already from a combined data stream (with an encoded BoardID in each
      * event).
      *
      * @param BoardID In: If the events in the TimeTag Packet do not contain
      *    their own BoardID for each event, all events are assigned this BoardID.
      *    Otherwise BoardID is ignored.
      * @param Src In: Packet of Timetags
      * @param TimeHigh In: If the Timetags are stored in a compressed format,
      *    their high bits are extended using TimeHigh. Set this parameter to
      *    TimeHighUndefined before adding the first events from a given board
      *    and use the updated value for successive calls.
      * @returns Error Code
      */
    FlexIOStatus AddSortEvents(int BoardID, const TTMDataPacket_t *Src, uint64_t *TimeHigh);

    /** @brief Add Timetags to the Event Sorter
      *
      * AddSortEvents feeds an array of Timetags in MultiBoardTimetagI64
      * format to the Event Sorter.
      *
      * @param TTag In: Array of Timetags
      * @param Cnt In: Number of entries in TTag
      * @returns Error Code
      */
    FlexIOStatus AddSortEvents(struct MultiBoardTimetagI64 TTag[], int Cnt);

    /** @brief Add a single Timetag to the Event Sorter
      *
      * AddSortEvents feeds a single Timetag in MultiBoardTimetagI64
      * format to the Event Sorter.
      *
      * @param TTag In: Timetag
      * @returns Error Code
      */
    FlexIOStatus AddSortEvent(struct MultiBoardTimetagI64 TTag);

    /** @brief Flush the Timetag Buffer
      *
      * After events have been fed to the EventSorter, they are immediately
      * visible since there is a chance that events that will be fed later
      * will need to be sorted before those events that are already in the
      * event queue. Only events that are old enough will be released from
      * the queue.
      *
      * If we know that no additional events will arrive within reasonable
      * time (or ever), we need to flush the sort buffer so that all events
      * become available for processing.
      *
      * @returns Error Code
      */
    FlexIOStatus Flush();

    /** @brief Obtain Sorted Events
      *
      * Fetch one Packet full of sorted Timetag Data
      *
      * @param Dest Out: Packet of Timetag Data
      * @returns Error Code
      *
      * @see SortEventsAvailable()
      */
    FlexIOStatus GetSortEvents(TTMDataPacket_t *Dest);

    /** @brief Obtain Sorted Events
      *
      * Fetch one Packet full of sorted Timetag Data
      *
      * @param Dest Out: Packet of Timetag Data
      * @param MaxEventCnt In: Max. Number of Events to be placed in Dest
      * @returns Error Code
      *
      * @see SortEventsAvailable()
      */
    FlexIOStatus GetSortEvents(TTMDataPacket_t *Dest, int MaxEventCnt);

    /** @brief Check if Sorted Events are available for processing
      *
      * @returns Are Events available?
      *
      * @see GetSortEvents()
      */
    bool SortEventsAvailable();

  protected:
    CoreMultiTTMEvtSorter_c *MultiTTMEvtSorterHnd; /**< Handle to the MultiTTMEvtSorter Core */
 }; /* class MultiTTMEvtSorter_c */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                  class MultiTTMCoincCntFilter_c                  **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Core CoreMultiTTMCoincCntFilter_c Class (Internal)
  */
class CoreMultiTTMCoincCntFilter_c;

/** @brief Filter a Stream of Events to find Coincidences of Events
  *
  * When events occur on many channels, some events will be uncorrelated
  * and some will be generated by a common source and arrive at their
  * sensors/detectors at the same time creating correlated events.
  * Sometimes these correlated events are much more interesting than
  * single events (that can either be noise or a part of a group of
  * correlated events, where some events actually triggered their
  * sensors/detectors while others where lost in the measurement
  * setup).
  *
  * MultiTTMCoincCntFilter will take a stream of sorted events and
  * will attempt to find groups of events that occured within a
  * user defined time window. These events are then considered to be
  * correlated and have originated from the same source (even so there
  * can of course also be multiple uncorrelated events that just happen
  * to occur at the same time by chance). If the number of events
  * that participate in such a correlation is within given bounds the
  * events participating in the correlation are preserved for further
  * inspection. All other events are discarded.
  */
class LIBTTM_API MultiTTMCoincCntFilter_c
 {
  public:
    /** @brief Constructor for MultiTTMCoincCntFilter
      *
      * @param MinCoincCnt In: Minimal number of events for a valid correlation
      * @param MaxCoincCnt In: Maximal number of events for a valid correltation
      * @param CoincWndSize In: Duration of the time window considered for correltaion [Ticks *82.3045ps]
      * @param ChannelMask In: Channels to consider for correlation -
      *           All events not in the given list of channels are discarded BEFORE looking for correlations.
      */
    MultiTTMCoincCntFilter_c(int MinCoincCnt, int MaxCoincCnt = 128, int CoincWndSize = 61, uint8_t *ChannelMask = NULL);

    /** @brief Destructor for MultiTTMCoincCntFilter
      *
      */
    ~MultiTTMCoincCntFilter_c();

    /** @brief Insert Events into the Coincidence Count Filter
      *
      * AddCoincEvents feeds a Packet of TimeStamps to the CoincCntFilter.
      * The Timestamps must be measured in I-Mode continous. They can be
      * encoded with or without compression and can originate from a single
      * board (in which case they are assigned the provided BoardID) or
      * already from a combined data stream (with an encoded BoardID in each
      * event).
      *
      * @param BoardID In: If the events in the TimeTag Packet do not contain
      *    their own BoardID for each event, all events are assigned this BoardID.
      *    Otherwise BoardID is ignored.
      * @param Src In: Packet of Timetags
      * @param SrcTimeHigh In: If the Timetags are stored in a compressed format,
      *    their high bits are extended using TimeHigh. Set this parameter to
      *    TimeHighUndefined before adding the first events from a given board
      *    and use the updated value for successive calls.
      * @returns Error Code
      */
    FlexIOStatus AddCoincEvents(int BoardID, const TTMDataPacket_t *Src, uint64_t *SrcTimeHigh);

    /** @brief Insert Events into the Coincidence Count Filter
      *
      * AddCoincEvents feeds an array of Timetags in MultiBoardTimetagI64
      * format to the Coincidence Count Filter..
      *
      * @param TTag In: Array of Timetags
      * @param Cnt In: Number of entries in TTag
      * @returns Error Code
      */
    FlexIOStatus AddCoincEvents(const struct MultiBoardTimetagI64 TTag[], int Cnt);

    /** @brief Add a single Timetag to the Coincidence Count Filter
      *
      * AddCoincEvent feeds a single Timetag in MultiBoardTimetagI64
      * format to the Coincidence Count Filter.
      *
      * @param TTag In: Timetag
      * @returns Error Code
      */
    FlexIOStatus AddCoincEvent(struct MultiBoardTimetagI64 TTag);

    /** @brief Process all pending Events in the Coincidence Count Filter
      *
      * @returns Error Code
      */
    FlexIOStatus Flush();

    /** @brief Fetch a Packet of Timetag Events that passed the CoincCnt Filter
      *
      * Fetch one packet full of filtered Events. The Timetags are
      * copied to provided TTMDataPacket structure. If time is of
      * essence there is also a version where you can access the
      * internal structure of MultiTTMCoincCntFilter directly and
      * avoid the copy operation
      *
      * @param CoincEvts Out: Packet Timetag Events that passed the CoincCnt Filter
      * @returns Error Code
      */
    FlexIOStatus GetCoincEvents(TTMDataPacket_t *CoincEvts);

    /** @brief Fetch a Packet of Timetag Events that passed the CoincCnt Filter
      *
      * MultiTTMCoincCntFilter_c() has an internal TTMDataPacket_t, that it
      * fills with the data that matches the required coincidence count. The
      * application can get direct access to this packet, thus avoiding a copy
      * operation.
      *
      * @param CoincEvts Out: Pointer to a Packet Timetag Events that passed the CoincCnt Filter
      * @returns Error Code
      */
    FlexIOStatus GetCoincEvents(const TTMDataPacket_t **CoincEvts);

    /** @brief Are there new Events in the CoincCntFilter
      *
      * @returns Are there new Events in the CoincCntFilter?
      */
    bool         CoincEventsAvailable();

  private:
    CoreMultiTTMCoincCntFilter_c *TTMCoincCntFilterHnd; /**< Handle to the MultiTTMCoincCntFilter Core */
 }; /* class MultiTTMCoincCntFilter_c */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                       class TTMEventCnt_c                        **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Core TTMEventCnt_c Class (Internal)
  */
class CoreTTMEventCnt_c;

/** @brief An Event Counter for Timetagging Events from an TTM8000 Board
  *
  * An EventCounter is an object that processes Timetags and counts how
  * often each type of event occurs. The results are available in two
  * ways. On the one hand, there are (64-bit) counters, that simply count
  * events since the beginning of time and that are never reset. If you
  * want to compute the number of events within a certain time, you will
  * need to look at the counters at the beginning of that time and again
  * at the end of the timeslot and perform a subtraction yourself.
  * On the other hand there is a concept of timeslots. Timeslots are
  * defined in milliseconds when using continous I-Mode and user-defined
  * for all other modes. Whenever a new timeslot starts, the state of
  * the EventCounter is automatically recorded. It is then possible
  * to compute the number of events that took place in the most recent
  * (completed) timeslot. It is also possible to compute the average
  * frequency of events that occurred during the N (N < 24) most recent
  * completed timeslots.
  *
  * For example if we want the event-frequency (in Hz), averaged over
  * the last 2 seconds and updated 4x per second, we could start a
  * new timeslot every 250ms (= 4x per second) and compute the moving
  * average freqency over the last 8 timeslots (8x 250ms = 2s).
  *
  * FIXME - More Documentation required here - FIXME
  */
 /* Note: It seems that DoxyGen has some trouble with the TTMLIB_API
  * makro here. It might be necessary to comment it out before creating
  * the documentation!
  */
class LIBTTM_API TTMEventCnt_c
 {
  public:
    /** @brief Constructor for TTMEventCnt
      *
      */
    TTMEventCnt_c();

    /** @brief Destructor for TTMEventCnt
      *
      */
    ~TTMEventCnt_c();

    /** @brief Reset all counters of an Event Counter
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param SlotMilliDuration In: Duration of a Timeslot in Milliseconds
      * @returns Error Code
      */
    FlexIOStatus Reset(uint32_t SlotMilliDuration);

    /** @brief Process Timetag Events / Count Events
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param DataPacket In: Timetags to Process
      * @returns Error Code
      */
    FlexIOStatus ProcessEvents(const TTMDataPacket_t *DataPacket);

    /** @brief Force the Beginning of a new Timeslot
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new
      * Timeslots every SlotMilliDuration Milliseconds and ForceNewSlot()
      * should not be used.
      * When using other (non-I-Mode-continous) modes a new Timeslot must
      * manually be started using TTMEventCnt_ForceNewSlot() every so often...
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @returns Error Code
      */
    FlexIOStatus ForceNewSlot();

    /** @brief Get the Timestamp of the most recent Event
      *
      * When using Continous I-Mode TTMEventCnt automatically records the
      * most recent processed timestamp.
      * When using other (non-I-Mode-continous) modes, there is no concept
      * of global time and GetCurrTime() always returns 0.
      *
      * @param CurrTime  Out: Current Time (in I-Mode Ticks @ 82.3045ps)
      * @returns Error Code
      */
    FlexIOStatus GetCurrTime(uint64_t *CurrTime);

    /** @brief Get the number of Timeslots processed
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new
      * timeslot after the timetag clock has ticked for SlotMilliDuration
      * milliseconds. In all other modes a new timeslot is started by calling
      * ForceNewSlot() manually.
      *
      * GetSlotCnt() fetches the number of Timeslots that have been
      * completed. It can be used to determine if it is worth looking a
      * new frequency measurements.
      *
      * @param SlotCnt   Out: Number of Slots
      * @returns Error Code
      */
    FlexIOStatus GetSlotCnt(uint32_t *SlotCnt);

    /** @brief Fetch Number of Timetagging Events for each Channel/Edge
      *
      * TTMEventCnt counts the number of events for each of the TTM8000 Stop
      * inputs separated for rising/falling edges. These counters are adjusted
      * every time more Timetags are processed (after a call to ProcessEvents())
      * and are never (automatically) reset. (They can be manually reset using
      * Reset() however it is not recommended to use this function!).
      *
      * If you need to know how many events occurred within a given interval of
      * time, you should query the counter values at the beginning of the interval
      * then process the events, query the counter values again and perform the
      * subtraction yourself. This approach makes it possible to use _one_ set of
      * counters for multiple overlapping intervals without interference caused by
      * one measurments resetting the counters behind the backs of all other
      * parallel measurements.
      *
      * Note: EventCnt[] is an array of 16 elements. EventCnt[0..7] correspond to
      * the rising edges of Stop1..Stop8, while EventCnt[8..15] correspond to the
      * falling edges of Stop1..Stop8.
      *
      * @param EventCnt  Out: Number of Timetagging Events/Channel
      * @returns Error Code
      */
    FlexIOStatus GetCounter(uint64_t EventCnt[16]);

    /** @brief Fetch Frequency of Timetagging Events for each Channel/Edge
      *
      * TTMEventCnt counts the number of events for each of the TTM8000 Stop
      * inputs separated for rising/falling edges. These counters are adjusted
      * every time more Timetags are processed (after a call to ProcessEvents())
      * and are never (automatically) reset.
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new Timeslot
      * every SlotMilliDuration Milliseconds; In all other modes new slots can be
      * manually started by calling by calling ForceNewSlot().
      *
      * GetFrequency() fetches the _average_ (NOT _total_(!)) number of events
      * that occurred within the last AvgSlotCnt (< 30) completed timeslots.
      *
      * Note: Freq[] is an array of 16 elements. Freq[0..7] correspond to the
      * rising edges of Stop1..Stop8, while Freq[8..15] correspond to the
      * falling edges of Stop1..Stop8.
      *
      * @param TimeSlotCnt In: How many Timeslots form one second?
      * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
      * @param Freq       Out: Frequency of Timetagging Events/Channel
      * @returns Error Code
      */
    FlexIOStatus GetFrequency(int TimeSlotCnt, int AvgSlotCnt, uint32_t Freq[16]);

  private:
    CoreTTMEventCnt_c *TTMEventCntHnd; /**< Handle to the TTMEventCnt Core */
 }; // class TTMEventCnt_c

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                    class TTMCorrelationCnt_c                     **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Core TTMCorrelationCnt_c Class (Internal)
  */
class CoreTTMCorrelationCnt_c;

/** @brief An Event and Correlation Counter for Timetagging Events from an TTM8000 Board
  *
  * An TTMCorrelationCntis an object that processes Timetags and counts how
  * often each type of event occurs. Furthermore, if the Timetags are
  * measured in continuous I-Mode, TTMCorrEvtCnt will also count
  * correlations between two or more events. A correlation is detected
  * if all selected events occur within a small time window. The width
  * of this time window can be selected. Furthermore a fixed, channel
  * dependant time offset can be added to the events from each channel
  * to compensate for different cable length, delays in logic elements etc.
  *
  * Both the event counter and the correlation counter results are available
  * in two formats ways. On the one hand, there are (64-bit) counters,
  * that simply count events since the beginning of time and that are
  * usually never reset. If you want to compute the number of events within
  * a certain time, you will need to look at the counters at the beginning
  * of that time and again at the end of the timeslot and perform a
  * subtraction yourself.
  * On the other hand there is a concept of timeslots. Timeslots are
  * defined in milliseconds when using continous I-Mode and user-defined
  * for all other modes. Whenever a new timeslot starts, the state of
  * the EventCounter is automatically recorded. It is then possible
  * to compute the number of events that took place in the most recent
  * (completed) timeslot. It is also possible to compute the average
  * frequency of events that occurred during the N (N < 24) most recent
  * completed timeslots.
  *
  * For example if we want the event-frequency (in Hz), averaged over
  * the last 2 seconds and updated 4x per second, we could start a
  * new timeslot every 250ms (= 4x per second) and compute the moving
  * average freqency over the last 8 timeslots (8x 250ms = 2s).
  *
  * FIXME - More Documentation required here - FIXME
  */
 /* Note: It seems that DoxyGen has some trouble with the TTMLIB_API
  * makro here. It might be necessary to comment it out before creating
  * the documentation!
  */
class LIBTTM_API TTMCorrelationCnt_c
 {
  public:
    /** @brief Constructor for TTMCorrelationCnt
      *
      */
    TTMCorrelationCnt_c();

    /** @brief Destructor for TTMCorrelationCnt
      *
      */
    ~TTMCorrelationCnt_c();
#if defined (_WIN32) && defined (_MSC_VER) && (_MSC_VER == 1200)
    // Microsoft Visual Studio 6 has problems with modern constant definitions ;(
    enum {
      ChannelCnt = 16,
      MultiCorrMaxDefCnt = 256 };
#else
    static const int ChannelCnt = 16;  //!< Number of Event Channels (Stop1..8 rising + Stop1..8 falling = 8 + 8 Channels = 16 Channels)
    static const int MultiCorrMaxDefCnt = 256; //!< Max. Number of MultiChannel Definitions (256 = 2^8!)
#endif

    /** @brief Reset all counters of an Event and Correlation Counter
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param SlotMilliDuration In: Duration of a Timeslot in Milliseconds
      * @returns Error Code
      */
    FlexIOStatus Reset(uint32_t SlotMilliDuration);

    /** @brief Configure the Timing of the Correlation Detectors
      *
      * A correlation is detected if all selected events occur within a
      * small time window. Before the logic checks for coincidence a fixed,
      * channel dependent time offset can be added to the events from each
      * channel to compensate for different cable length, delays in logic elements etc.
      *
      * ConfigTiming() can be used to define the delays that are added to
      * the events from each channel and the width of the time window. In
      * order to trigger an event the time difference between the earliest
      * event and the latest event of the event group must not exceed
      * the time window.
      *
      * @param ChannelPicoDelay In: Delay to be added to each event in Picoseconds.
      *         ChannelPicoDelay[0..7] correspond to the rising edges of Stop1..8.
      *         ChannelPicoDelay[8..15] correspond to the falling edges of Stop1..8.
      * @param WindowPicoWidth In: Width of the Correlation Time Window in Picoseconds.
      * @returns Error Code
      */
    FlexIOStatus ConfigTiming(int32_t ChannelPicoDelay[ChannelCnt], uint32_t WindowPicoWidth);

    /** @brief Configure the Timing of the Correlation Detectors
      *
      * In a perfect world, a correlation is detected if all selected
      * events occur at the exactly same instant. In reality correlated
      * events carry some channel dependent, fixed timing offset due to
      * different cable length and delays from logic elements. In addition
      * to this offset, each event is also affected by timing jitter.
      * Thus if we want to detect if two events occurred at the same time,
      * we need to shift all events by their respective offsets and create
      * a timing window around each event that allows for the jitter. If
      * these time windows overlap, then we shall say that a correlation
      * occurred.
      *
      * ConfigTiming() can be used to define the delays that are added to
      * the events from each channel and the width of the time windows. In
      * order to trigger an event the time difference between the earliest
      * event and the latest event of the event group must not exceed
      * the time window.
      *
      * @param ChannelPicoDelay In: Delay to be added to each event in Picoseconds.
      *         ChannelPicoDelay[0..7] correspond to the rising edges of Stop1..8.
      *         ChannelPicoDelay[8..15] correspond to the falling edges of Stop1..8.
      * @param WindowPicoWidth In: Width of the Correlation Time Windows in Picoseconds
      *         Note: This is the _total_ width of the Time Window. Half of the window
      *         width will be added before and after the measured timestamp.
      *         WindowPicoWidth[0..7] correspond to the rising edges of Stop1..8.
      *         WindowPicoWidth[8..15] correspond to the falling edges of Stop1..8.
      * @returns Error Code
      */
    FlexIOStatus ConfigTiming(int32_t ChannelPicoDelay[ChannelCnt],
                                                   uint32_t WindowPicoWidth[ChannelCnt]);

    /** @brief Configure the Interesting Correlations
      *
      * Detecting Correlations is a fairly CPU-expensive operation, thus it
      * should only be enabled, if the correlations are actually required
      * and counting single events alone is not sufficient.
      * Once counting of correlations is enabled, we shall always count all
      * possible pairs of two events. In additions to that we shall count
      * up to 32 arbitrary combinations of any of the 16 possible events.
      *
      * @param EnableCorr In: Do we want to count correlations at all?
      * @param MultiCorrDef[][] In: Define up to 32 correlation patterns
      *    using arbitrary combinations of the events.
      * @param MultiCorrDefCnt In: How many of the 32 entries in MultiCorrDef
      *    are actually valid?
      * @returns Error Code
      */
    FlexIOStatus ConfigCorr(bool EnableCorr,
                            bool MultiCorrDef[MultiCorrMaxDefCnt][ChannelCnt], int MultiCorrDefCnt);

    /** @brief Configure the Interesting Correlations
      *
      * Detecting Correlations is a fairly CPU-expensive operation, thus it
      * should only be enabled, if the correlations are actually required
      * and counting single events alone is not sufficient.
      * Once counting of correlations is enabled, we shall always count all
      * possible pairs of two events. In additions to that we shall count
      * up to 32 arbitrary combinations of any of the 16 possible events.
      *
      * @param EnableCorr In: Do we want to count correlations at all?
      * @param MultiCorrDef[] In: Define up to 32 correlation patterns
      *    using arbitrary combinations of the events.
      * @param MultiCorrDefCnt In: How many of the 32 entries in MultiCorrDef
      *    are actually valid?
      * @returns Error Code
      */
    FlexIOStatus ConfigCorr(bool EnableCorr,
                            struct MultiCorrInfo MultiCorrDef[MultiCorrMaxDefCnt],
                            int MultiCorrDefCnt);

    /** @brief Process Timetag Events / Count Events and Correlations
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param DataPacket In: Timetags to Process
      * @returns Error Code
      */
    FlexIOStatus ProcessEvents(const TTMDataPacket_t *DataPacket);

    /** @brief Process queued Timetag Events / Count Events and Correlations
      *
      * ProcessEvents will always keep a queue of events that have been received
      * but not yet processed, since events that arrive later might need to be
      * processed earlier due to delay compensation. However if we know that no
      * earlier events can arrive any more (e.g. because we have stopped processing)
      * this queue should be emptied.
      *
      * @returns Error Code
      */
    FlexIOStatus FlushEventQueue();

    /** @brief Force the Beginning of a new Timeslot
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new
      * Timeslots every SlotMilliDuration Milliseconds and ForceNewSlot()
      * should not be used.
      * When using other (non-I-Mode-continous) modes a new Timeslot must
      * manually be started using TTMEventCnt_ForceNewSlot() every so often...
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @returns Error Code
      */
    FlexIOStatus ForceNewSlot();

    /** @brief Define a Callback function for new Timeslots
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new
      * Timeslots every SlotMilliDuration Milliseconds.
      * If a (non-NULL) Callback function has been configured using
      * SetNewSlotCallback(), this callback function is called every
      * time a new timeslot has been started.
      *
      * @param CallbackFunct In: Callback function
      * @param CallbackParam In: Parameter for the Callback function
      * @returns Error Code
      */
    FlexIOStatus SetNewSlotCallback(void (*CallbackFunct)(void*), void *CallbackParam);

    /** @brief Get the Timestamp of the most recent Event
      *
      * When using Continous I-Mode TTMEventCnt automatically records the
      * most recent processed timestamp.
      * When using other (non-I-Mode-continous) modes, there is no concept
      * of global time and GetCurrTime() always returns 0.
      *
      * @param CurrTime  Out: Current Time (in I-Mode Ticks @ 82.3045ps)
      * @returns Error Code
      */
    FlexIOStatus GetCurrTime(uint64_t *CurrTime);

    /** @brief Get the number of Timeslots processed
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new
      * timeslot after the timetag clock has ticked for SlotMilliDuration
      * milliseconds. In all other modes a new timeslot is started by calling
      * ForceNewSlot() manually.
      *
      * GetSlotCnt() fetches the number of Timeslots that have been
      * completed. It can be used to determine if it is worth looking
      * for a new frequency measurements.
      *
      * @param SlotCnt   Out: Number of Slots
      * @returns Error Code
      */
    FlexIOStatus GetSlotCnt(uint32_t *SlotCnt);

    /** @brief Fetch Number of Timetagging Events for each Channel/Edge
      *
      * TTMCorrEventCnt counts the number of events for each of the TTM8000 Stop
      * inputs separated for rising/falling edges. These counters are adjusted
      * every time more Timetags are processed (after a call to ProcessEvents())
      * and are never (automatically) reset. (They can be manually reset using
      * Reset() however it is not recommended to use this function!).
      *
      * If you need to know how many events occurred within a given interval of
      * time, you should query the counter values at the beginning of the interval
      * then process the events, query the counter values again and perform the
      * subtraction yourself. This approach makes it possible to use _one_ set of
      * counters for multiple overlapping intervals without interference caused by
      * one measurments resetting the counters behind the backs of all other
      * parallel measurements.
      *
      * Note: EventCnt[] is an array of 16 elements. EventCnt[0..7] correspond to
      * the rising edges of Stop1..Stop8, while EventCnt[8..15] correspond to the
      * falling edges of Stop1..Stop8.
      *
      * @param EventCnt  Out: Number of Timetagging Events/Channel
      * @returns Error Code
      */
    FlexIOStatus GetEvtCounter(uint64_t EventCnt[ChannelCnt]);

    /** @brief Fetch Number of Timetagging Events for each Channel/Edge for a dated slot
      *
      * While GetEvtCounter() always reports the current number of events,
      * GetDatedEvtCounter() reports the number of events that have happened
      * in the past. This makes it easy to compute differences between the
      * current counter value and a past value to obtain the number of events
      * that happend in a given interval of time.
      * The length of a slot is defined by the SlotMilliDuration parameter
      * of Reset(). A history of 250 values is recorded in the history table.
      * Thsu the SlotIdx must be in the range SlotCnt-250 ... SlotCnt-1.
      *
      * @param SlotIdx   In: Index of the requested, dated Slot
      * @param EventCnt  Out: Number of Timetagging Events/Channel
      * @returns Error Code
      *
      * @see GetEvtCounter(), GetSlotCnt()
      */
    FlexIOStatus GetDatedEvtCounter(int SlotIdx, uint64_t EventCnt[ChannelCnt]);

    /** @brief Fetch Frequency of Timetagging Events for each Channel/Edge
      *
      * TTMEventCnt counts the number of events for each of the TTM8000 Stop
      * inputs separated for rising/falling edges. These counters are adjusted
      * every time more Timetags are processed (after a call to ProcessEvents())
      * and are never (automatically) reset.
      *
      * When using Continous I-Mode TTMEventCnt automatically starts a new Timeslot
      * every SlotMilliDuration Milliseconds; In all other modes new slots can be
      * manually started by calling by calling ForceNewSlot().
      *
      * GetFrequency() fetches the average frequency in Hertz over the last
      * AvgSlotCnt (< 30) completed timeslots.
      *
      * Note: Freq[] is an array of 16 elements. Freq[0..7] correspond to the
      * rising edges of Stop1..Stop8, while Freq[8..15] correspond to the
      * falling edges of Stop1..Stop8.
      *
      * @param TimeSlotCnt In: How many Timeslots form one second?
      * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
      * @param EventFreq  Out: Frequency of Timetagging Events/Channel
      * @returns Error Code
      */
    FlexIOStatus GetEvtFreq(int TimeSlotCnt, int AvgSlotCnt, uint32_t EventFreq[ChannelCnt]);

    /** @brief Fetch the Number of Pairwise Correlation Events
      *
      * FIXME - More Documentation required here - FIXME
      *
      * Note: CorrCnt[] is an array of 16x16 elements. Indeces 0..7 in each matrix
      * direction correspond to the rising edges of Stop1..Stop8, indices 8..15
      * correspond to the falling edges of Stop1..Stop8.
      *
      * Note: Correlation are always counted for both contributing events. Thus
      * CorrCnt[x][y] is always equal to CorrCnt[y][x].
      *
      * @param CorrCnt  Out: Number of correlation between two channels
      * @returns Error Code
      */
    FlexIOStatus GetCorrCounter(uint64_t CorrCnt[ChannelCnt][ChannelCnt]);

    /** @brief Fetch the Number of Pairwise Correlation Events  for a dated slot
      *
      * FIXME - More Documentation required here - FIXME
      *
      * Note: CorrCnt[] is an array of 16x16 elements. Indeces 0..7 in each matrix
      * direction correspond to the rising edges of Stop1..Stop8, indices 8..15
      * correspond to the falling edges of Stop1..Stop8.
      *
      * Note: Correlation are always counted for both contributing events. Thus
      * CorrCnt[x][y] is always equal to CorrCnt[y][x].
      *
      * @param SlotIdx   In: Index of the requested, dated Slot
      * @param CorrCnt  Out: Number of correlation between two channels
      * @returns Error Code
      *
      * @see GetCorrCounter(), GetSlotCnt()
      */
    FlexIOStatus GetDatedCorrCounter(int SlotIdx, uint64_t CorrCnt[ChannelCnt][ChannelCnt]);

    /** @brief Fetch the Frequency of Pairwise Correlation Events
      *
      * FIXME - More Documentation required here - FIXME
      *
      * Note: CorrFreq[] is an array of 16x16 elements. Indices 0..7 in each matrix
      * direction correspond to the rising edges of Stop1..Stop8, indices 8..15
      * correspond to the falling edges of Stop1..Stop8.
      *
      * Note: Correlation are always counted for both contributing events. Thus
      * CorrFreq[x][y] is always equal to CorrFreq[y][x].
      *
      * @param TimeSlotCnt In: How many Timeslots form one second?
      * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
      * @param CorrFreq  Out: Frequency of correlation between two channels
      * @returns Error Code
      */
    FlexIOStatus GetCorrFreq(int TimeSlotCnt, int AvgSlotCnt, uint32_t CorrFreq[ChannelCnt][ChannelCnt]);

    /** @brief Fetch the Number of MultiChannel Correlation Events
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param MultiCorrCnt  Out: Number of MultiEvent Correlations
      * @returns Error Code
      */
    FlexIOStatus GetMultiCorrCounter(uint64_t MultiCorrCnt[MultiCorrMaxDefCnt]);

    /** @brief Fetch the Number of MultiChannel Correlation Events for a dated slot
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param SlotIdx       In: Index of the requested, dated Slot
      * @param MultiCorrCnt  Out: Number of MultiEvent Correlations
      * @returns Error Code
      *
      * @see GetMultiCorrCounter(), GetSlotCnt()
      */
    FlexIOStatus GetDatedMultiCorrCounter(int SlotIdx, uint64_t MultiCorrCnt[MultiCorrMaxDefCnt]);

    /** @brief Fetch the Frequency of MultiChannel Correlation Events
      *
      * FIXME - More Documentation required here - FIXME
      *
      * @param TimeSlotCnt In: How many Timeslots form one second?
      * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
      * @param MultiCorrFreq  Out: Frequency of MultiEvent Correlations
      * @returns Error Code
      */
    FlexIOStatus GetMultiCorrFreq(int TimeSlotCnt, int AvgSlotCnt,
                                 uint32_t MultiCorrFreq[MultiCorrMaxDefCnt]);

  private:
    CoreTTMCorrelationCnt_c *TTMCorrelationCntHnd; /**< Handle to the TTMCorrelationCnt Core */
 }; // class TTMCorrelationCnt_c

/**********************************************************************
 *                             TTMLib.hpp                             *
 **********************************************************************/
#endif // TTMLib_hpp
