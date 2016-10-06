/**********************************************************************
 *                                                                    *
 *                            TTMLabView.h                            *
 *                                                                    *
 **********************************************************************/
/*
 * Project:   TTM8-Virtex5 (Time Tagging Module)
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
 * LabVIEW (http://www.ni.com/labview/) is a popular PC-application from
 * National Instruments. It offers an easy graphical programming language
 * that allows engineers and scientists to process data from a wide range
 * of (hardware) sensors and set the state of (hardware) actuators using
 * a simple GUI interface on their PC.
 *
 * To access external hardware LabVIEW can call external code from DLLs.
 * While LabVIEW can officially handle C structs, the code required to do
 * so is extremely messy for structures with lots of distinct fields, as
 * they are used to control the TTM8000 boards. We have therefore written
 * some wrappers around the normal TTM8000 functions that are optimized
 * for simple interfacing with LabVIEW.
 *
 * The LabVIEW interface is currently designed to count single events per
 * channel, to count correlations between all possible combinations of two
 * different channels and to count correlations of up to 32 arbitrary
 * combinations of channels (MultiCorr).
 *
 * The LabView interface for TTM8000 provides two modes of operation:
 * Timetag Mode and UserIOCntrl Mode.\n
 * Timetag Mode is focused on converting external events into timetags
 * and then using these timetags to compute how often certain events
 * or groups of correlated events occurred.\n
 * UserIOCntrl Mode controls the is used to control the digital and
 * analog input/output pins of the TTM8000 that are free for application
 * specific use (i.e. not used for timetag measurement) to control
 * external devices. UserIOCntrl Mode is independent of any measurement.\n
 *
 * If you want to use the LabView Interface in Timetag mode, you must
 * create measurement connection using TTMLabView_StartMeasurement() to
 * obtain a DeviceID. You can now use this DeviceID to control the
 * measurement and obtain results. When you are done with your measurement,
 * you MUST call TTMLabView_StopMeasurement() to release the measurement
 * connection.
 *
 * If you want to use the LabView Interface in UserIOCntrl Mode, you must
 * create a UserIOCntrl connection using TTMLabView_UserIOConnect() to
 * obtain an DeviceID. You can then use that DeviceID to set/get the state
 * of the digital and analog input/output pins. When you are done you must
 * call TTMLabView_UserIODisconnect() to release the UserIOCntrl connection.
 *
 * Note that Timetag mode and UserIOCntrl mode are strictly separate modes.
 * You can NOT use the DeviceID obtained for UserIOCntrl Mode to perform
 * a timetag measurement!
 *//*
 *
 * Release 4.4.4 - 2016-01-26
 */
#ifndef TTMLabView_h
#define TTMLabView_h

/**********************************************************************/

#ifndef SysTypes_h
  #include "SysTypes.h"
#endif

#ifndef FlexIOLib_h
  #include "FlexIOLib.h"
#endif

#ifndef TTMLib_h
  #include "TTMLib.h"
#endif

/**********************************************************************
 *                                                                    *
 **********************************************************************/

#if defined __cplusplus
extern "C" {
#endif

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                   TTM8000 - LabView Interface                    **
 **                           Timetag Mode                           **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Initialize all global Variables for the TTMLabView Interface
  *
  * Note: Calling TTMLabView_Init() is optional. If it is not called
  * manually beforehand, it will be automatically called when the first
  * measurement in started.
  *
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_Init();

/** @brief Shutdown all open connections of the TTMLabView Interface
  *
  * Note: This function should be called before LabView is closed. It
  * makes sure that all connections are terminated.
  *
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_Close();

/** @brief Get a list of all TTM8000-Boards available on the net.
  *
  * TTMLabView_GetAvailableBoards() queries all boards available on all
  * network interfaces of the local host and reports their IP-addresses.
  *
  * @param BoardIPAddr   Out: Array of IP-addresses of TTM8000-Boards
  * @param BoardCnt      Out: Number of TTM8000-Boards actually discovered
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetAvailableBoards(in_addr_t BoardIPAddr[16], int *BoardCnt);

/** @brief Configure and Start a Measurement
  *
  * @param DeviceID    Out: Identifier of the TTM8000-Board (required for all other TTMLabView_* functions)
  * @param BoardIPAddr  In: IPAddress of the TTM8000-Boards that shall be used for the measurement.
  *         Use a valid IPv4 address to connect to a specific board, or use
  *         INADDR_ANY (= 0) if you want to TTMLabView_StartMeasurement() to
  *         automatically use the first board it finds on the net.
  * @param EnableEdge   In: Select which signal edges on the Stop* inputs are interesting.\n
  *         EnableEdge[0..7]:  Rising Edges of Stop1..Stop8\n
  *         EnableEdge[8..15]: Falling Edges of Stop1..Stop8
  * @param TriggerLevel In: The TTM8000-Board can be used to measure signals
  *         from many different logic families that use different signal levels
  *         for HIGH- and LOW-state. The TriggerLevel defines the threshold
  *         voltage that the TTM8000 board uses to distinguish between these
  *         states. It is NOT the HIGH voltage level! Ideally you should use
  *         the voltage, where the slope of the transition from LOW to HIGH
  *         (or from HIGH to LOW, whichever is more important to your application)
  *         is steepest (as measured e.g. with an oscilloscope). However for
  *         more applications the mean between HIGH and LOW will do fine.
  *         The Triggerlevel is given in Millivolt in the range of -4096mV to +4095mV.
  * @param EnableCorrelationCnt In: Enable the CoincidenceCounter Logic. If
  *         the CoincidenceCounter Logic is disabled, only Single Events will
  *         be counted.
  * @param ChannelPicoDelay In: Each Stop* signal arrives at the TTM-8000
  *         from a diffent signal source using a different cable. Some of
  *         these sources are faster than others and some of the cables are
  *         shorter (i.e. have less propagation delay) than others. Thus
  *         signals that actually occur at the same time in the real world,
  *         will arrive at the TTM-8000 with a certain offset.
  *         ChannelPicoDelay defines the offset (in Picoseconds) that will
  *         be added(!) to each event to compensate for the delays.\n
  *         ChannelPicoDelay[0..7]:  Delay for the rising Edges of Stop1..Stop8\n
  *         ChannelPicoDelay[8..15]: Delay for the falling Edges of Stop1..Stop8
  * @param WindowPicoWidth In: How many Picoseconds timing difference are
  *         permitted between two signal edges (after the adjustment from
  *         ChannelPicoDelay[]) that are considered to have occured
  *         at the same time. Usually this will be 1000-5000ps (1-5ns).
  * @param MultiCorrDef In: If the CorrelationCounter Logic is enabled, it
  *         will always count the coincidences for all possible pairs of
  *         events. It is however also possible to count the occurrance of
  *         multi-event correlations, however since there are too many
  *         possible combinations to count them all (with reasonable effort)
  *         the user has to specify the combinations that are interesting.
  *         Up to 32 different combinations each covering any combination
  *         of the 8 rising and 8 falling edges can be counted.
  * @param MultiCorrDefCnt In: How many sets of MultiCorrDef are valid.
  *
  * @return Error Code
  *
  * @see TTMLabView_StartMeasurementExt(), TTMLabView_StartMeasurementExt2()
  */
FlexIOStatus LIBTTM_API TTMLabView_StartMeasurement(int *DeviceID, in_addr_t BoardIPAddr,
                          bool_t EnableEdge[16], int TriggerLevel[8],
                          bool_t EnableCorrelationCnt,
                          int ChannelPicoDelay[16], int WindowPicoWidth,
                          bool_t MultiCorrDef[32][16], int MultiCorrDefCnt);

/** @brief Configure and Start a Measurement with Channel Deadtime
  *
  * TTMLabView_StartMeasurementExt() is an extension to TTMLabView_StartMeasurement()
  * that adds the option of specifing a Dead-Time for each channel.
  *
  * @param DeviceID    Out: Identifier of the TTM8000-Board (required for all other TTMLabView_* functions)
  * @param BoardIPAddr  In: IPAddress of the TTM8000-Boards that shall be used for the measurement.
  *         Use a valid IPv4 address to connect to a specific board, or use
  *         INADDR_ANY (= 0) if you want to TTMLabView_StartMeasurement() to
  *         automatically use the first board it finds on the net.
  * @param EnableEdge   In: Select which signal edges on the Stop* inputs are interesting.\n
  *         EnableEdge[0..7]:  Rising Edges of Stop1..Stop8\n
  *         EnableEdge[8..15]: Falling Edges of Stop1..Stop8
  * @param TriggerLevel In: The TTM8000-Board can be used to measure signals
  *         from many different logic families that use different signal levels
  *         for HIGH- and LOW-state. The TriggerLevel defines the threshold
  *         voltage that the TTM8000 board uses to distinguish between these
  *         states. It is NOT the HIGH voltage level! Ideally you should use
  *         the voltage, where the slope of the transition from LOW to HIGH
  *         (or from HIGH to LOW, whichever is more important to your application)
  *         is steepest (as measured e.g. with an oscilloscope). However for
  *         more applications the mean between HIGH and LOW will do fine.
  *         The Triggerlevel is given in Millivolt in the range of -4096mV to +4095mV.
  * @param NanoDeadTime In: How far apart must two events on the same channel
  *        be to be considered two independent events (and not and event and
  *        the rebounces of the same event?). Event that occurr within
  *        NanoDeadtime Nanoseconds after another event on the same channel
  *        are considered rebounces of the original event and are automatically
  *        removed from forther processing. Thus they will not contribute to
  *        the results of EventCnt or CorrelationCnt.
  *        NanoDeadTime[0..7] are used for the rising edge of Stop1..Stop8
  *        NanoDeadTime[8..15] are used for the falling edges.
  * @param ExtendDeadTimeOnRetrigger  In: Does a group of events end when the
  *        _first_ event of the group occured more than DeadTime Nanoseconds ago, or
  *        does the current group of events remain active until the _most recent_
  *        event of the group occured more than DeadTime Nanoseconds ago. i.e. do
  *        we restart the DeadTime every time a 'dead' event arrives?
  * @param EnableCorrelationCnt In: Enable the CoincidenceCounter Logic. If
  *         the CoincidenceCounter Logic is disabled, only Single Events will
  *         be counted.
  * @param ChannelPicoDelay In: Each Stop* signal arrives at the TTM-8000
  *         from a diffent signal source using a different cable. Some of
  *         these sources are faster than others and some of the cables are
  *         shorter (i.e. have less propagation delay) than others. Thus
  *         signals that actually occur at the same time in the real world,
  *         will arrive at the TTM-8000 with a certain offset.
  *         ChannelPicoDelay defines the offset (in Picoseconds) that will
  *         be added(!) to each event to compensate for the delays.\n
  *         ChannelPicoDelay[0..7]:  Delay for the rising Edges of Stop1..Stop8\n
  *         ChannelPicoDelay[8..15]: Delay for the falling Edges of Stop1..Stop8
  * @param WindowPicoWidth In: How many Picoseconds timing difference are
  *         permitted between two signal edges (after the adjustment from
  *         ChannelPicoDelay[]) that are considered to have occured
  *         at the same time. Usually this will be 1000-5000ps (1-5ns).
  * @param MultiCorrDef In: If the CorrelationCounter Logic is enabled, it
  *         will always count the coincidences for all possible pairs of
  *         events. It is however also possible to count the occurrance of
  *         multi-event correlations, however since there are too many
  *         possible combinations to count them all (with reasonable effort)
  *         the user has to specify the combinations that are interesting.
  *         Up to 32 different combinations each covering any combination
  *         of the 8 rising and 8 falling edges can be counted.
  * @param MultiCorrDefCnt In: How many sets of MultiCorrDef are valid.
  *
  * @return Error Code
  * @see TTMLabView_StartMeasurementExt2(), TTMLabView_StopMeasurement(), TTMLabView_AdjustDeadTime()
  */
FlexIOStatus LIBTTM_API TTMLabView_StartMeasurementExt(int *DeviceID, in_addr_t BoardIPAddr,
                          bool_t EnableEdge[16], int TriggerLevel[8],
                          int NanoDeadTime[16], bool_t ExtendDeadTimeOnRetrigger,
                          bool_t EnableCorrelationCnt,
                          int ChannelPicoDelay[16], int WindowPicoWidth,
                          bool_t MultiCorrDef[32][16], int MultiCorrDefCnt);

/** @brief Configure and Start a Measurement with Channel Deadtime and Channle Window Width
  *
  * TTMLabView_StartMeasurementExt2() is an extension to TTMLabView_StartMeasurementExt()
  * and TTMLabView_StartMeasurement() that adds the option of specifing a Dead-Time for
  * each channel and an individual Window Width per Channel
  *
  * @param DeviceID    Out: Identifier of the TTM8000-Board (required for all other TTMLabView_* functions)
  * @param BoardIPAddr  In: IPAddress of the TTM8000-Boards that shall be used for the measurement.
  *         Use a valid IPv4 address to connect to a specific board, or use
  *         INADDR_ANY (= 0) if you want to TTMLabView_StartMeasurement() to
  *         automatically use the first board it finds on the net.
  * @param EnableEdge   In: Select which signal edges on the Stop* inputs are interesting.\n
  *         EnableEdge[0..7]:  Rising Edges of Stop1..Stop8\n
  *         EnableEdge[8..15]: Falling Edges of Stop1..Stop8
  * @param TriggerLevel In: The TTM8000-Board can be used to measure signals
  *         from many different logic families that use different signal levels
  *         for HIGH- and LOW-state. The TriggerLevel defines the threshold
  *         voltage that the TTM8000 board uses to distinguish between these
  *         states. It is NOT the HIGH voltage level! Ideally you should use
  *         the voltage, where the slope of the transition from LOW to HIGH
  *         (or from HIGH to LOW, whichever is more important to your application)
  *         is steepest (as measured e.g. with an oscilloscope). However for
  *         more applications the mean between HIGH and LOW will do fine.
  *         The Triggerlevel is given in Millivolt in the range of -4096mV to +4095mV.
  * @param NanoDeadTime In: How far apart must two events on the same channel
  *        be to be considered two independent events (and not and event and
  *        the rebounces of the same event?). Event that occurr within
  *        NanoDeadtime Nanoseconds after another event on the same channel
  *        are considered rebounces of the original event and are automatically
  *        removed from forther processing. Thus they will not contribute to
  *        the results of EventCnt or CorrelationCnt.
  *        NanoDeadTime[0..7] are used for the rising edge of Stop1..Stop8
  *        NanoDeadTime[8..15] are used for the falling edges.
  * @param ExtendDeadTimeOnRetrigger  In: Does a group of events end when the
  *        _first_ event of the group occured more than DeadTime Nanoseconds ago, or
  *        does the current group of events remain active until the _most recent_
  *        event of the group occured more than DeadTime Nanoseconds ago. i.e. do
  *        we restart the DeadTime every time a 'dead' event arrives?
  * @param EnableCorrelationCnt In: Enable the CoincidenceCounter Logic. If
  *         the CoincidenceCounter Logic is disabled, only Single Events will
  *         be counted.
  * @param ChannelPicoDelay In: Each Stop* signal arrives at the TTM-8000
  *         from a diffent signal source using a different cable. Some of
  *         these sources are faster than others and some of the cables are
  *         shorter (i.e. have less propagation delay) than others. Thus
  *         signals that actually occur at the same time in the real world,
  *         will arrive at the TTM-8000 with a certain offset.
  *         ChannelPicoDelay defines the offset (in Picoseconds) that will
  *         be added(!) to each event to compensate for the delays.\n
  *         ChannelPicoDelay[0..7]:  Delay for the rising Edges of Stop1..Stop8\n
  *         ChannelPicoDelay[8..15]: Delay for the falling Edges of Stop1..Stop8
  * @param ChnWindowPicoWidth In: How much jitter (in Picoseconds) has to be
  *         expected for each individual channel. When searching for
  *         coincidences a coincidence window of total width ChnWindowPicoWidth
  *         will be created around each event (half of the window before and
  *         half of the window after the event). Two events will be considered
  *         to haved happened at the same time, if their windows overlap.
  *         Usually this will be 1000-5000ps (1-5ns).
  * @param MultiCorrDef In: If the CorrelationCounter Logic is enabled, it
  *         will always count the coincidences for all possible pairs of
  *         events. It is however also possible to count the occurrance of
  *         multi-event correlations, however since there are too many
  *         possible combinations to count them all (with reasonable effort)
  *         the user has to specify the combinations that are interesting.
  *         Up to 32 different combinations each covering any combination
  *         of the 8 rising and 8 falling edges can be counted.
  * @param MultiCorrDefCnt In: How many sets of MultiCorrDef are valid.
  * @param AutoStopMilliTime In: Do we want to measure until StopMeasurement()
  *         is called or shall the measurement automatically end after some
  *         time? Use 0 for infinite measurement or state the number of
  *         milliseconds you want the measurement to run.
  *
  * @return Error Code
  * @see TTMLabView_StartMeasurementExt(), TTMLabView_StopMeasurement(), TTMLabView_AdjustDeadTime()
  */
FlexIOStatus LIBTTM_API TTMLabView_StartMeasurementExt2(int *DeviceID, in_addr_t BoardIPAddr,
                          bool_t EnableEdge[16], int TriggerLevel[8],
                          int NanoDeadTime[16], bool_t ExtendDeadTimeOnRetrigger,
                          bool_t EnableCorrelationCnt,
                          int ChannelPicoDelay[16], int ChnWindowPicoWidth[16],
                          bool_t MultiCorrDef[32][16], int MultiCorrDefCnt,
                          int AutoStopMilliTime);

/** @brief Stop a Measurement
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_StopMeasurement(int DeviceID);

/** @brief Check if a Measurement is active
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param MeasActive Out: Is a measurement in progress (true) or not (false)
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_MeasurementActive(int DeviceID, bool_t *MeasActive);

/** @brief Process Pending Events
  *
  * After the Measurement has been started, Timetags arrive at the ethernet
  * interface and are automatically queued by the operating system. However
  * we need to make sure that they are processed (i.e. events and coincidences
  * are counted). Thus we shall periodically call TTMLabView_Tickle() that
  * processes all events waiting in the input queues.
  *
  * Note: The network queue can hold up to ~8MByte, which is sufficient for
  * almost 2 million events, however you should not try to exploit these
  * queues to their limit, since there is always a chance that the OS is
  * busy with some other task when the TTM8000 queue is almost full. So
  * if you have a low data rate (e.g. < 2MEvents/s) you should call
  * TTMLabView_Tickle() about every 250ms, otherwise increase the rate as
  * needed.
  *
  * Note: To ensure liveness of the LabView environment even in high load
  * scenarios, TTMLabView_Tickle() will process at most 2500 network packets
  * per call and then return even if more packets are waiting to be processed.
  *
  * Note: It might be tempting to place this code in a (hidden) thread that
  * runs independent from LabView. However this could lead to trouble if
  * LabView does not shut down these threads during debugging, thus this
  * function is currently not enabled.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_Tickle(int DeviceID);

/** @brief Adjust the list of Enabled Edges
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param EnableEdge In: Select which signal edges on the Stop* inputs are interesting.\n
  *         EnableEdge[0..7]:  Rising Edges of Stop1..Stop8\n
  *         EnableEdge[8..15]: Falling Edges of Stop1..Stop8
  * @return Error Code
  *
  * @see TTMLabView_StartMeasurement()
  */
FlexIOStatus LIBTTM_API TTMLabView_AdjustEnabledEdges(int DeviceID, bool_t EnableEdge[16]);

/** @brief Adjust the Trigger Levels of the Stop Inputs
  *
  * @param DeviceID   In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param TriggerLevel In: The TTM8000-Board can be used to measure signals
  *         from many different logic families that use different signal levels
  *         for HIGH- and LOW-state. The TriggerLevel defines the threshold
  *         voltage that the TTM8000 board uses to distinguish between these
  *         states. It is NOT the HIGH voltage level! Ideally you should use
  *         the voltage, where the slope of the transition from LOW to HIGH
  *         (or from HIGH to LOW, whichever is more important to your application)
  *         is steepest (as measured e.g. with an oscilloscope). However for
  *         more applications the mean between HIGH and LOW will do fine.
  *         The Triggerlevel is given in Millivolt in the range of -4096mV to +4095mV.
  * @return Error Code
  *
  * @see TTMLabView_StartMeasurement()
  */
FlexIOStatus LIBTTM_API TTMLabView_AdjustTriggerLevels(int DeviceID, int TriggerLevel[8]);

/** @brief Adjust the DeadTimes of the Stop Inputs
  *
  * Some signal sources produce tend to 'bounce'. I.e. When a 'real-world'
  * event trigger occurs (e.g. arrival of a optical signal), the detector
  * that converts this extenal event into a electronic event does not create
  * a single, nice, steep transition from one voltage level to another, but
  * creates a transition that has serious over-/undershoot and/or creates
  * swings back and forth between various voltage levels multiple times. If
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
  * @param DeviceID   In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param NanoDeadTime In: How far apart must two events on the same channel
  *        be to be considered two independent events (and not and event and
  *        the rebounces of the same event?). Event that occurr within
  *        NanoDeadtime Nanoseconds after another event on the same channel
  *        are considered rebounces of the original event and are automatically
  *        removed from forther processing. Thus they will not contribute to
  *        the results of EventCnt or CorrelationCnt.
  *        NanoDeadTime[0..7] are used for the rising edge of Stop1..Stop8
  *        NanoDeadTime[8..15] are used for the falling edges.
  * @param ExtendDeadTimeOnRetrigger  In: Does a group of events end when the
  *        _first_ event of the group occured more than DeadTime Nanoseconds ago, or
  *        does the current group of events remain active until the _most recent_
  *        event of the group occured more than DeadTime Nanoseconds ago. i.e. do
  *        we restart the DeadTime every time a 'dead' event arrives?
  * @return Error Code
  *
  * @see TTMLabView_StartMeasurement(), TTMLabView_StartMeasurementExt()
  */
FlexIOStatus LIBTTM_API TTMLabView_AdjustDeadTime(int DeviceID, int NanoDeadTime[16],
                                                  bool_t ExtendDeadTimeOnRetrigger);

/** @brief Adjust the Timing Offsets and Window Width for Correlation Detection
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param ChannelPicoDelay In: Each Stop* signal arrives at the TTM-8000
  *         from a diffent signal source using a different cable. Some of
  *         these sources are faster than others and some of the cables are
  *         shorter (i.e. have less propagation delay) than others. Thus
  *         signals that actually occur at the same time in the real world,
  *         will arrive at the TTM-8000 with a certain offset.
  *         ChannelPicoDelay defines the offset (in Picoseconds) that will
  *         be added(!) to each event to compensate for the delays.\n
  *         ChannelPicoDelay[0..7]:  Delay for the rising Edges of Stop1..Stop8\n
  *         ChannelPicoDelay[8..15]: Delay for the falling Edges of Stop1..Stop8
  * @param WindowPicoWidth In: How many Picoseconds timing difference are
  *         permitted between two signal edges (after the adjustment from
  *         ChannelPicoDelay[]) that are considered to have occured
  *         at the same time. Usually this will be a 1000-3000ps (1-3ns).
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_AdjustCorrelationTimes(int DeviceID, int ChannelPicoDelay[16], int WindowPicoWidth);

/** @brief Adjust the Correlations that are measured
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param EnableCorrelationCnt In: Enable the CoincidenceCounter Logic. If
  *         the CoincidenceCounter Logic is disabled, only Single Events will
  *         be counted. If it is enabled, all 2-fold (pairwise) correlations
  *         and the defined set of multi-fold correlations will be computed.
  * @param MultiCorrDef In: If the CorrelationCounter Logic is enabled, it
  *         will always count the coincidences for all possible pairs of
  *         events. It is however also possible to count the occurrance of
  *         multi-event correlations, however since there are too many
  *         possible combinations to count them all (with reasonable effort)
  *         the user has to specify the combinations that are interesting.
  *         Up to 32 different combinations each covering any combination
  *         of the 8 rising and 8 falling edges can be counted.
  * @param MultiCorrDefCnt In: How many sets of MultiCorrDef are valid.
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_AdjustMultiCorrDefs(int DeviceID, bool_t EnableCorrelationCnt,
                                                       bool_t MultiCorrDef[32][16], int MultiCorrDefCnt);

/** @brief Reset all Single Event and Correlation Counters
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_ResetCounter(int DeviceID);

/** @brief Fetch the number of Single Events
  *
  * @param DeviceID  In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param EventCnt Out: Number of Single-Events since StartMeasurement() (or Reset())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetEventCnt(int DeviceID, uint64_t EventCnt[16]);

/** @brief Fetch the number of Single Events (32-bit)
  *
  * TTMLabView_GetEventCnt32() is a substitute for TTMLabView_GetEventCnt()
  * that can be used with versions of LabView that do not support 64-bit
  * integers.
  *
  * @param DeviceID  In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param EventCnt Out: Number of Single-Events since StartMeasurement() (or Reset())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetEventCnt32(int DeviceID, uint32_t EventCnt[16]);

/** @brief Fetch the current frequency of Single Events
  *
  * @param DeviceID   In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param EventFreq Out: Number of Single Events that occurred in the last second.
  *                       This value is updated every 250ms.
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetEventFreq(int DeviceID, uint32_t EventFreq[16]);

/** @brief Fetch the number of Correlation Events
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param CorrCnt Out: Number of Correlation-Events between two(!) channels
  *                     since StartMeasurement() (or Reset())
  * @param MultiCorrCnt Out: Number of Correlation-Events for arbitrary combinations of
  *                     channels (as defined by TTMLabView_AdjustMultiCorrDefs())
  *                     since StartMeasurement() (or Reset())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetCorrCnt(int DeviceID, uint64_t CorrCnt[16][16], uint64_t MultiCorrCnt[32]);

/** @brief Fetch the number of Correlation Events (32-bit)
  *
  * TTMLabView_GetCorrCnt32() is a substitute for TTMLabView_GetCorrCnt()
  * that can be used with versions of LabView that do not support 64-bit
  * integers.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param CorrCnt Out: Number of Correlation-Events between two(!) channels
  *                     since StartMeasurement() (or Reset())
  * @param MultiCorrCnt Out: Number of Correlation-Events for arbitrary combinations of
  *                     channels (as defined by TTMLabView_AdjustMultiCorrDefs())
  *                     since StartMeasurement() (or Reset())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetCorrCnt32(int DeviceID, uint32_t CorrCnt[16][16], uint32_t MultiCorrCnt[32]);

/** @brief Fetch the current frequency of Correlation Events
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param CorrFreq Out: Frequency of Correlation-Events between two(!) channels in the last second.
  * @param MultiCorrFreq Out: Frequency of Correlation-Events for arbitrary combinations of
  *                     channels (as defined by TTMLabView_AdjustMultiCorrDefs()) in the last second.
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetCorrFreq(int DeviceID, uint32_t CorrFreq[16][16], uint32_t MultiCorrFreq[32]);

/** @brief Get the current Slot Number
  *
  * The TTM8000-LabView Library counts events and coincidences using 64-bit
  * counters that are reset when the measurement is started and then count
  * forward towards infinity, without ever being reset. Every time a 250ms
  * have passed (as measured on the TTM8000 internal clock) a new timeslot
  * is started and the current state of the counters are stored in a history
  * table (and the counters continue counting undisturbed).
  * Use GetSlotCnt() to discover how many slots have completed(!) since
  * the measurement began. You can use this to information to call
  * GetDatedEventCnt() or GetDatedCorrCnt() for the most recent completed
  * timeslot and another timeslot that is further back in the past and
  * compute the difference to see how many events occurred in that time
  * interval.
  *
  * Note: While SlotCnt counts towards infinity, the history tables for the
  * counters are of course finite, and currently contains 250 entries. Since
  * each timeslot is 250ms long, this is sufficient for about one minute (incl.
  * sufficient security margin for unexpected delays).
  *
  * @see TTMLabView_GetDatedEventCnt(), TTMLabView_GetDatedCorrCnt()
  */
FlexIOStatus LIBTTM_API TTMLabView_GetSlotCnt(int DeviceID, uint32_t *SlotCnt);

/** @brief Fetch the number of Single Events at a past time
  *
  * @param DeviceID  In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param SlotIdx   In: Slot Number of the slot to query (Valid Range: [SlotCnt-250 .. SlotCnt-1])
  * @param EventCnt Out: Number of Single-Events since StartMeasurement() (or Reset())
  * @return Error Code
  *
  * @see TTMLabView_GetSlotCnt()
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDatedEventCnt(int DeviceID, int SlotIdx, uint64_t EventCnt[16]);

/** @brief Fetch the number of Single Events at a past time (32-bit)
  *
  * TTMLabView_GetDatedEventCnt32() is a substitute for TTMLabView_GetDatedEventCnt()
  * that can be used with versions of LabView that do not support 64-bit
  * integers.
  *
  * @param DeviceID  In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param SlotIdx   In: Slot Number of the slot to query (Valid Range: [SlotCnt-250 .. SlotCnt-1])
  * @param EventCnt Out: Number of Single-Events since StartMeasurement() (or Reset())
  * @return Error Code
  *
  * @see TTMLabView_GetSlotCnt()
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDatedEventCnt32(int DeviceID, int SlotIdx, uint32_t EventCnt[16]);

/** @brief Fetch the number of Correlation Events at a past time
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param SlotIdx   In: Slot Number of the slot to query (Valid Range: [SlotCnt-250 .. SlotCnt-1])
  * @param CorrCnt Out: Number of Correlation-Events between two(!) channels
  *                     since StartMeasurement() (or Reset())
  * @param MultiCorrCnt Out: Number of Correlation-Events for arbitrary combinations of
  *                     channels (as defined by TTMLabView_AdjustMultiCorrDefs())
  *                     since StartMeasurement() (or Reset())
  * @return Error Code
  *
  * @see TTMLabView_GetSlotCnt()
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDatedCorrCnt(int DeviceID, int SlotIdx,
                          uint64_t CorrCnt[16][16], uint64_t MultiCorrCnt[32]);
/** @brief Fetch the number of Correlation Events at a past time
  *
  * TTMLabView_GetCorrCnt32() is a substitute for TTMLabView_GetCorrCnt()
  * that can be used with versions of LabView that do not support 64-bit
  * integers.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_StartMeasurement())
  * @param SlotIdx   In: Slot Number of the slot to query (Valid Range: [SlotCnt-250 .. SlotCnt-1])
  * @param CorrCnt Out: Number of Correlation-Events between two(!) channels
  *                     since StartMeasurement() (or Reset())
  * @param MultiCorrCnt Out: Number of Correlation-Events for arbitrary combinations of
  *                     channels (as defined by TTMLabView_AdjustMultiCorrDefs())
  *                     since StartMeasurement() (or Reset())
  * @return Error Code
  *
  * @see TTMLabView_GetSlotCnt()
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDatedCorrCnt32(int DeviceID, int SlotIdx,
                          uint32_t CorrCnt[16][16], uint32_t MultiCorrCnt[32]);

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                   TTM8000 - LabView Interface                    **
 **                      User I/O Control Mode                       **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Create a Digital/Analog Input/Output Control Connection
  *
  * Besides the ability to measure timetags, the TTM8000 also has some
  * input/output ports that can be used to control and monitor external
  * hardware.
  * There are 8 digital I/O pins, where the direction of each pin is
  * individually controlable, 4 analog input pins and 4 analog output
  * pins.
  *
  * To use any of these pins, you need to create a ControlIO Connection.
  * Remember to close this connection with TTMLabView_UserIODisconnect()
  * when you are done!
  *
  * @param DeviceID    Out: Identifier of the TTM8000-Board
  * @param BoardIPAddr  In: IPAddress of the TTM8000-Board whose input/output
  *         ports shall be used.
  *         Use a valid IPv4 address to connect to a specific board, or use
  *         INADDR_ANY (= 0) if you want to TTMLabView_StartMeasurement() to
  *         automatically use the first board it finds on the net.
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_UserIOConnect(int *DeviceID, in_addr_t BoardIPAddr);

/** @brief Disconnect an Digital/Analog Input/Output Control Connection
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_UserIODisconnect(int DeviceID);

/** @brief Check if a Digital/Analog Input/Output Control Connection is still active
  *
  * After sucessfully calling TTMLabView_UserIOConnect() there will be a valid connection
  * to a TTM8000 module that ususally remains valid until TTMLabView_UserIODisconnect()
  * is called. However this connection can inadvertedly break e.g. by powering down the
  * TTM8000 module or disconnecting the network cable.
  * Since the connection between the TTMLabView Library and the TTM8000 module is based
  * on UDP/IP a broken network connection will not become visible until the next network
  * packet is sent. TTMLabView_UserIOIsConnected() can be used to force a network packet
  * to be sent to check for liveness for the network connection.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param IsConnected Out: Is the Connection still active?
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_UserIOIsConnected(int DeviceID, bool *IsConnected);

/** @brief Set the State of the User LEDs
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param LEDID    In: LED Identified (0..LED A, 1..LED B, 2..LED C)
  * @param Color    In: Requested LED Color (0..Off, 1..Red, 2..Green, 3..Yellow)
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_SetUserLED(int DeviceID, int LEDID, int Color);

/** @brief Query the State of the User Buttons
  *
  * @param DeviceID   In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param ButtonID   In: Button Identifier (0..'#'-Button , 1..'*'-Button)
  * @param IsPressed Out: Is the Button Pressed?
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetUserButton(int DeviceID, int ButtonID, bool_t *IsPressed);

/** @brief Set the Direction of a DigitalIO Pin
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param Pin In: Number of the DigitalIO Pin (0..7)
  * @param Dir In: Direction of the DigitalIO Pin (0..Output, 1..Input)
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_SetDigitalIODir(int DeviceID, int Pin, int Dir);

/** @brief Set the State of a DigitalIO Output Pin
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param Pin In: Number of the DigitalIO Pin (0..7)
  * @param Value In: Requested State of the DigitalIO Output Pin (0/1)
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_SetDigitalIOOut(int DeviceID, int Pin, int Value);

/** @brief Get the State of a DigitalIO Input Pin
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param Pin In: Number of the DigitalIO Pin (0..7)
  * @param Value Out: Current State of the DigitalIO Input Pin
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDigitalIOIn(int DeviceID, int Pin, int *Value);

/** @brief Get the complete state of the Digital I/O Pins
  *
  * Usually a LabView application will use keep track of the digital I/O pins
  * internally and use TTMLabView_SetDigitalIODir(), TTMLabView_SetDigitalIOOut()
  * and TTMLabView_GetDigitalIOIn() to set/get the state of the digital I/O pins.
  * However after startup a LabView might need to query the state left behind by
  * the previous application. TTMLabView_GetDigitalIOState() reports the complete
  * state of all digital I/O pins and thus allows the LabView application to
  * synchronize its internal representation to the actual TTM8000 settings.
  *
  * @param DeviceID  In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param ValueIn  Out: State of the Digital Input Pins
  * @param ValueOut Out: State of the Digital Output Pins
  * @param ValueDir Out: Direction of the Digital Input/Output (1..Input, 0..Output)
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetDigitalIOState(int DeviceID, int ValueIn[8], int ValueOut[8], int ValueDir[8]);

/** @brief Set the Voltage on a Analog Output Channel
  *
  * The TTM8000 Board contains 4 Analog Output Channels available to the
  * user that can produce voltages between +/- 4095mV. A 8-bit ADC is
  * used to generated the voltages, thus the resolution is 32mV.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param Channel In: Number of the Analog Output Channel (0..3)
  * @param MilliVolt In: Requested Output Voltage in MilliVolt [+/-4095mV]
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_SetAnalogOut(int DeviceID, int Channel, int MilliVolt);

/** @brief Get the Voltage on a Analog Input Channel
  *
  * The TTM8000 Board contains 4 Analog Input Channels available to the
  * user. They can operate either in unipolar mode to measure volatages
  * between 0..4095mV or in bipolar mode for voltages between +/-2047mV.
  * The measurement is performed using a 12-bit ADC, providing a resolution of 1mV.
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param Channel In: Number of the Analog Input Channel (0..3)
  * @param BipolarMode In: Do we want to measure negative voltages too?
  * @param MilliVolt Out: Current Input Voltage in MilliVolt
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetAnalogIn(int DeviceID, int Channel, bool_t BipolarMode, int *MilliVolt);

/** @brief Get the complete state of the Analog Out Pins
  *
  * Usually a LabView application will keep track of the voltages at the
  * analog output pins internally and use TTMLabView_SetAnalogOut() to set a
  * new voltage to some pin. However after startup a LabView might need to
  * query the state of the analog output pins as left behind by the previous
  * application. TTMLabView_GetAnalogOutState() reports the complete
  * state of all analog ouput pins and thus allows the LabView application to
  * synchronize its internal representation to the actual TTM8000 settings.
  * Note: To obtain the state of the analog input pins the LabView application
  * shall use TTMLabView_GetAnalogIn().
  *
  * @param DeviceID       In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param MilliVoltOut  Out: Current Voltage at the analog output pins
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetAnalogOutState(int DeviceID, int MilliVoltOut[4]);

/** @brief Get the Number of Events for each Channel
  *
  * Usually we will receive data from the TTM8000-Board and compute the number of events
  * that occurred on the PC. However there is also a hardware counter in the TTM8000
  * board that works correctely even if there are lots of events and the PC's CPU is
  * overloaded.\n
  * Note that EventCnt uses just a single counter per Channel to count both rising and
  * falling events (as enabled). Thus if both rising and falling events are enabled, the
  * counter will tick twice as fast, as if just rising (or just falling) events were
  * enabled.\n
  * Note that the counters are raw counters that are never reset (not even when a new
  * measurement is started). If you want to measure the number of events within a given
  * time, be sure to record the counter values at the beginning of your time window,
  * and do the proper subtraction yourself!\n
  * The counters are 32 bit wide. This can lead to overflow if you look at long time
  * spans of more than several minutes. (If you use 10MEvents/Channel/sec, the counter
  * will overflow every ~7 minutes; At 1MEvent/Cannel/sec it will take more than 1 hour
  * between two overflow). Since overflows are a rare occurrance, it is easy to implement
  * bit extension in software (if it is actually needed).
  *
  * @param DeviceID In: Identifier of the TTM8000-Board (see: TTMLabView_UserIOConnect())
  * @param EventCnt Out: Number of Events for each Channel
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMLabView_GetHWEvtCounter(int DeviceID, uint32_t EventCnt[8]);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined __cplusplus
}  /* extern "C" */
#endif

/**********************************************************************
 *                            TTMLabView.h                            *
 **********************************************************************/
#endif // TTMLabView_h
