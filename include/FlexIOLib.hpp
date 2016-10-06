/**********************************************************************
 *                                                                    *
 *                            FlexIOLib.hpp                           *
 *                                                                    *
 **********************************************************************/
/*
 * Project:   FlexIO-Virtex5 (Flexible Input/Output Module)
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

#ifndef FlexIOLib_hpp
#define FlexIOLib_hpp

/**********************************************************************/

#include "FlexIOLib.h"

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                        class FlexIOCntrl_c                       **
 **                                                                  **
 **********************************************************************
 **********************************************************************/
/** @brief Core FlexIOCntrl_c Class (Internal)
  */
class CoreFlexIOCntrl_c;

/** @brief Control Connection between a FlexIO Board and the local host.
  *
  * FIXME - More Documentation required here - FIXME
  */
class LIBFLEXIO_API FlexIOCntrl_c
 {
  public:
    /** @brief Constructor for FlexIOCntrl
     */
    FlexIOCntrl_c();

    /** @brief Destructor for FlexIOCntrl
      *
      * @note The Destructor automatically calls Disconnect() if needed.
      */
    virtual ~FlexIOCntrl_c();

    /** @brief Get the Version number of FlexIOLib.
      *
      * @return Returns the version number of FlexIOLib at run-time 32-bit integer in format
      * 0xMMNNPPSS (MM = major, NN = minor, PP = patch, SS = sub(0=none, 1='a', ...).
      * For example FlexIOLib 1.4.12b will return 0x01040C02.
      */
    static uint32_t GetVersion();

    /** @brief Get a list of all FlexIO Boards available on the net.
      *
      * GetAvailableBoards() queries a single board, all boards in a specific subnet
      * or all boards available on all subnets connected to the local host and reports
      * their configuration and current status in an array of FlexIOBoardInfos.
      *
      * @param ApplMagic      In: Look only for FlexIO Boards that run a specific
      *        application. Use AnyApplCookie to search for Boards running any application.
      * @param BoardSubNet    In: The broadcast address of the subnet that shall be
      *        searched for boards. Use INADDR_ANY if you want to look at the subnets
      *        of all network interfaces on the local host. Use the broadcast address
      *        of a specific subnet to search just this subnet and use the IP-address
      *        of a specific FlexIO board if you just want to check the availability
      *        and obtain configuration/status data for a single board.
      * @param BoardCntrlPort In: The Control Port used by the FlexIO boards.
      *        Usually you will use FlexIOCntrlPort.
      * @param BoardInfo     Out: Array of FlexIOBoardInfos
      * @param MaxBoardCnt    In: Capacity of BoardInfo (Number of Entries available)
      * @param BoardCnt      Out: Number of FlexIO-Boards actually discovered
      * @return Error Code
      */
    static FlexIOStatus GetAvailableBoards(uint32_t ApplMagic,
                                    in_addr_t BoardSubNet, in_port_t BoardCntrlPort,
                                    FlexIOBoardInfo_t *BoardInfo, int MaxBoardCnt, int *BoardCnt);
    /** @brief Get the local network interface, whose subnet contains a given FlexIO Board.
      *
      * The FlexIO Boards only operate in the local network. They do not support ethernet routing
      * with gateways. Thus when we need a destination address of the application data, it must
      * be in the same subnet as the FlexIO Board. GetNearestNetInterface() will determine the
      * IP-Address of the network interface on the local host, that is closest to a given board.
      * @param BoardIPAddr   In: IP-address of the FlexIO Board
      * @param LocalIPAddr  Out: IP-address of the network interface closest to the FlexIO Board
      *                          i.e. containing the IP-address of the FlexIO Board in its subnet.
      *                          The IP-address of any network interface, if none matches the
      *                          above criterion.
      * @returns Error Code
      */
    static FlexIOStatus GetNearestNetInterface(in_addr_t BoardIPAddr, in_addr_t *LocalIPAddr);

    /** @brief Set the Size of the Send- or Receive-Buffer of a Socket
      *
      * SetSockOptBufferSize() is a wrapper for the standard setsockopt() function, that will
      * set the size of the send- or receive buffer of a socket. If the requested size can not
      * be set, SetSockOptBufferSize() will try to determine a valid size that is as close as
      * possible to the requested size.
      * Note: setsockopt() will automatically use the best valid size on Linux, however on
      * Apple OS X setsockopt() will simply fail if the requested size can not be set.
      *
      * @param SocketDescr   In: Socket Descriptor
      * @param OptName In: Option Name - Use SO_SNDBUF to set the Send Buffer Size or
      *                               SO_RCVBUF to set the Receive Buffer Size
      * @param RequestSize In: Requested BufferSize in Byte
      * @param ActualSize Out: Actual BufferSize in Byte - Use NULL if you are not interested
      *              in this result.
      * @returns Operating System Error Code (< 0..Error, 0..Ok)
      */
    static int SetSockOptBufferSize(SOCKET SocketDescr, int OptName,
                                             uint32_t RequestSize, uint32_t *ActualSize);

    /** @brief Get a human-readable String for an Error Code.
      *
      * @param Err In: The Error Code
      * @returns Pointer to a null-terminated string. Note that this string belongs to
      *          FlexIOLib and must not be modified. It is only valid until the next call
      *          to GetErrorName().
      */
    static const char *GetErrorName(FlexIOStatus Err);

    /** @brief Connect the FlexIOCntrl Object to a FlexIO Board.
      *
      * @note Every FlexIOLib Connection that was opened with Connect() must be
      *       closed with a corresponding call to Disconnect()!
      * @param BoardInfo      Out: Configuration/Status Information of the FlexIO Board.
      *        Pass NULL if you are not interested in this information.
      * @param ApplMagic       In:  Only connect to the FlexIO Board if it runs a
      *        given application. Use AnyApplCookie to connect to the FlexIO Board
      *        regardless of the application it is running.
      * @param BoardIPAddr     In: IP-Address of the requested FlexIO Board
      * @param BoardCntrlPort  In: Usually you should pass FlexIOCntrlPort
      * @param LocalIPAddr     In: IP-Address at the local host to be used for the
      *        connection. Unless you have a good reason to use a specific
      *        network interface, you should pass INADDR_ANY and let FlexIOLib select
      *        a suitable local interface for you.
      * @param LocalCntrlPort  In: UDP Port at the local host to be used for the
      *        connection. Unless you have a good reason to use a specific UDP
      *        Port, you should pass 0 and let FlexIOLib select a suitable port for you.
      * @param MilliNetTimeout In: Timeout [ms] for waiting for responses for network
      *        commands. Usually the FlexIOServer will answer all requests within much
      *        less than 500ms.
      * @returns Error Code
      * @see Disconnect()
      */
    FlexIOStatus Connect(FlexIOBoardInfo_t *BoardInfo, uint32_t ApplMagic,
                      in_addr_t BoardIPAddr, in_port_t BoardCntrlPort,
                      in_addr_t LocalIPAddr, in_port_t LocalCntrlPort,
                      uint32_t  MilliNetTimeout = 2000);
    /** @brief Check if a Control Connection has been established
      *
      * IsConnected() checks is a Control Connection has been established.
      * It can either perform a fast, local check, that just remembers if
      * Connect() has been successfully called for this FlexIOCntrl_c
      * object, without checking if the connection is still alive, or it
      * can actually send a test packet over the network and wait for an
      * acknowledgment to make sure that the network link is still alive.
      * Obviously the later option requires much more time.
      *
      * @param CheckNetStatus In: Do we want to send a test packet over
      *        the network to check that the connection is actually alive
      *        or is it sufficient to remember that we once successfully
      *        established a connection (that may or may not still be alive).
      * @returns Connection Status
      */
    bool IsConnected(bool CheckNetStatus);

    /** @brief Get the IP-Address/UDP-Port of the connected FlexIO Board
      *
      * GetBoardAddr() returns the IP-Address/UDP-Port of the FlexIO Board
      * to which a Control Connection has been established.
      * Note that GetBoardAddr() just reads back locally stored information.
      * It does not check if the FlexIO Board is actually available on
      * the network. Use IsConnected() to check if the Board is actually
      * reachable on the network.
      *
      * @param BoardIPAddr Out: IP-Address of the connected FlexIO Board
      * @param BoardCntrlPort Out: UDP-Port of the connected FlexIO Board
      * @returns Error Code
      */
    FlexIOStatus GetBoardAddr(in_addr_t *BoardIPAddr, in_port_t *BoardCntrlPort);

    /** @brief Dispose of a Control Connection to a FlexIO Board.
      *
      * Disconnect closes a FlexIOLib Connection.
      *
      * @returns Error Code
      */
    FlexIOStatus Disconnect();

    /** @brief Close other(!) Control Connection to a FlexIO Board.
      *
      * DisconnectOthers() closes Control Connection to the FlexIO
      * Board that are in use by other(!) applications. It does NOT
      * close the current connection. The affected applications are NOT
      * notified of their loss (using the assumption that they are dead
      * already anyhow, and just the FlexIO Board was not notified of their
      * death). If this assumption should prove to be wrong and a such
      * deprived application sends another command, it will be ignored
      * and will stumble into a local network timeout, that it will
      * hopefully handle gracefully.\n
      *
      * Using DisconnectOthers() is an extremly rude thing to do and
      * can cause all kinds of trouble for the applications that are
      * kicked out. Every application should always close all open
      * connections before it quits (even if it quits due to an error
      * or a SIGINT), so there should never be stale connections that
      * need to be killed.\n
      *
      * There might be times, when you want to make sure, that your
      * application is the only user to the FlexIO Board, or where
      * an other application did indeed fail to close its connection
      * before quitting, and you need to close that externally. However
      * these should be very rare occasions. Usually there should be no harm
      * if the FlexIO Board is shared between many applications and
      * your application should not take offense at the presence of others!\n
      *
      * Use DisconnectOthers() ONLY if you really see no other way out!
      *
      * @param TargetClient   In: List of Client Connections that shall be killed.
      *        Pass NULL to kill all Client Connection except the current one.
      * @param TargetCnt      In: Number of Entries in TargetClient (or 0)
      * @returns Error Code
      */
    FlexIOStatus DisconnectOthers(UDPAddr_t *TargetClient = NULL, int TargetCnt = 0);

    /** @brief Get the BSD socket used to send/receive commands.
      *
      * GetSocket() provides access to the handle of the BSD-style socket
      * used for the FlexIOLib connection. You can use it if your application has
      * special requirements (e.g. configuration settings) that can not be addressed
      * otherwise. However you should remember that the socket belongs to FlexIOLib
      * and must not be closed etc.
      * @note Most applications will NOT need to use GetSocket().
      * @param CntrlSocket Out: Socket Handle
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus GetSocket(SOCKET *CntrlSocket);

    /** @brief Get the IP-Address/UDP Port used to send/receive commands.
      *
      * GetSocketAddr() provides the IP-Address/UDP Port of the socket
      * used for the FlexIOLib connection.
      * This might be useful for status displays/debugging however most
      * applications will probably never need to call GetSocketAddr().
      * @param LocalIPAddr    Out: IP-Address of the Cntrl-Socket on the local host
      *        Pass NULL if you are not interested in this information.
      * @param LocalCntrlPort Out: UDP Port of the Cntrl-Socket on the local host
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus GetSocketAddr(in_addr_t *LocalIPAddr, in_port_t *LocalCntrlPort);

    /** @brief Set the Input Buffer Size of the local Control Socket
      *
      * The network connection between the FlexIO-Board and the local PC
      * uses BSD sockets, that have a input buffer on the PC to cope with
      * bursts of incomming network packets. Use SetSocketBufferSize() to
      * adjust the size of this buffer.
      * @note  Under Linux the maximal network buffer size is limited by
      *        the value in /proc/sys/net/core/rmem_max. You can temporarily
      *        change this value by editing this file, or you can add the
      *        line "net.core.rmem_max = 8388608" to /etc/sysctl.conf to
      *        make changes that survive a reboot.
      * @note  FlexIOLib automatically sets the buffer size to 2MByte when
      *        creating a new connection. Thus most applications will never
      *        need to call SetSocketBufferSize().
      * @param RecvBufferSize In: Requested Network Receive Buffer Size [Byte] for
      *        the Control Socket. Note that this size is just a suggestion
      *        to the Operating System, that might choose to grant more or
      *        less than was requested. Use 0 if you don't want to change
      *        the Receive Buffer Size.
      * @param SendBufferSize In: Requested Network Send Buffer Size [Byte] for
      *        the Control Socket. Note that this size is just a suggestion
      *        to the Operating System, that might choose to grant more or
      *        less than was requested. Use 0 if you don't want to change
      *        the Send Buffer Size.
      * @returns Error Code
      */
    FlexIOStatus SetSocketBufferSize(uint32_t RecvBufferSize, uint32_t SendBufferSize = 0);

    /** @brief Get the Input Buffer Size of the local Control Socket
      *
      * The network connection between the FlexIO-Board and the local PC
      * uses BSD sockets, that have a input buffer on the PC to cope with
      * bursts of incomming network packets. Use GetSocketBufferSize() to
      * obtain the size of this buffer.
      * @note  FlexIOLib automatically sets the buffer size to 2MByte when
      *        creating a new connection, which is more that sufficient for
      *        most applications. Thus most applications will never need to
      *        call GetSocketBufferSize().
      * @param RecvBufferSize Out: The actual Network Receive Buffer Size [Byte]
      * @param SendBufferSize Out: The actual Network Send Buffer Size [Byte]
      * @returns Error Code
      */
    FlexIOStatus GetSocketBufferSize(uint32_t *RecvBufferSize, uint32_t *SendBufferSize = NULL);

    /** @brief Get Network IP-MTU (Internet Protocol - Max. Transmission Unit) for the FlexIOLib Connection.
      *
      * Each network connection will only carry packets up to a specific size,
      * and will discard packets that are larger than this limit.
      * GetNetMTU() will discover the MTU for the FlexIOCntrl Connection.
      * @note Sending few large packets creates less overhead than sending many
      * small packets to transfer the same amount of payload. Thus it is
      * advantageous to send data packets that are as big as possible
      * (however not bigger than the MTU or the the packets will be dropped...).
      * @note Fast Ethernet generally has a MTU of 1500 Byte. Gigabit Ethernet
      * adapters often support Jumbo frames that can be larger, however
      * how much larger depends on the actual hardware and its configuration.
      * The FlexIO Server can take advantage of an MTU of up to 8KByte for
      * the Data connection. The packets on the FlexIOCntrl connection
      * are usually smaller than 1500Byte, so it it usually not necessary to
      * discover the MTU for the PC-to-FlexIO direction. If some custom
      * application requires large packets in that direction too, the MTU
      * for that direction can be discovered too...
      * @note As stated above GetNetMTU() will get the MTU for the
      * FlexIOCntrl connection. It does not make any statement about the MTU
      * of the FlexIOData connection! If you are running the control and
      * data aquisition applications on the same machine, the MTUs for both
      * applications will be the same, and you can use the FlexIOCntrl MTU
      * value to define the packet size for FlexIOData packets. If you are
      * running these application on different machines, the MTUs might be
      * different and you will need to use a different way (depending on
      * your specific situation) to discover the FlexIOData MTU.
      * @param FlexIOtoPC_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
      *        when sending data from the FlexIO board to the controlling PC.
      *        Pass NULL if you are not interested in this information.
      * @param PCtoFlexIO_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
      *        when sending data from the controlling PC to the FlexIO board.
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus GetNetMTU(uint32_t *FlexIOtoPC_NetMTU, uint32_t *PCtoFlexIO_NetMTU = NULL);

    /** @brief Get Configuration/Status Information of the currently used FlexIO Board.
      *
      * GetBoardInfo() obtains Configuration Information (e.g. Serial Number, Device
      * Type, Network configuration, Firmware Version, Debug Levels etc.) and
      * Status Information (e.g. Partners of active Network-Connections, Measurement
      * Status etc.) of the current FlexIO Board.
      * @param BoardInfo Out: Configuration/Status Information of the FlexIO Board
      * @returns Error Code
      */
    FlexIOStatus GetBoardInfo(FlexIOBoardInfo_t *BoardInfo);

    /** @brief Query the State of the Buttons of the FlexIO-DisplayBoard
      *
      * The FlexIO DisplayBoard contains two buttons. Use QueryKeys() to
      * obtain the current state of these buttons.
      * @note  While there are only two buttons (User & Reset) installed on the
      *        Display Board, there is a connector that allows 6 additional,
      *        external buttons to be connected.
      *        The state of the Reset button is mapped to Bit 0 (Mask 0x01),
      *        the state of the User button to Bit 1 (Mask 0x02). These bits
      *        are '0' when the button is released and '1' when it is pressed.
      * @param KeyBits Out: State of the Keys
      * @returns Error Code
      */
    FlexIOStatus QueryButtons(uint32_t *KeyBits);

    /** @brief Query the State of the Buttons of the FlexIO-DisplayBoard (Depreciated)
      *
      * The FlexIO DisplayBoard contains two buttons. QueryKeys() can still be used
      * obtain the current state of these buttons, however the use of this function
      * is depreciated. Please use QueryButtons() instead.
      */
    #define QueryKeys(KeyBits) QueryButtons(KeyBits)

    /** @brief Set the State of the LEDs on the FlexIO-DisplayBoard
      *
      * The FlexIO DisplayBoard contains 8 Dual-Color LEDs, each containing
      * a red and green Sub-LED, allowing each LED to be dark or to shine red,
      * yellow/orange or green. There are 16 bits controlling these LEDs.\n
      *   Bit 0: Red Sub-LED of LED1\n
      *   Bit 1: Green Sub-LED of LED1\n
      *   Bit 2: Red Sub-LED of LED2\n
      *    ...\n
      *   Bit 15: Green Sub-LED of LED8\n
      * A Sub-LED is lit, if the corresponding bit is set to '1' and dark if
      * it is set to '0'.
      * @note: The leftmost LED of the Display Board is LED1 the rightmost is LED8.
      *
      * @param SetLEDState   In: Requested State of LEDs
      * @param SetLEDMask    In: Only LEDs whose Mask Bit is '1' are set.
      * @param ToggleLEDMask In: LEDs whose ToggleLEDMask bit are set, are toggled.
      * @param CurrLEDState Out: New State of LEDs
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus SetLEDs(uint32_t SetLEDState, uint32_t SetLEDMask = 0x00FFFF,
                         uint32_t ToggleLEDMask = 0x0000, uint32_t *CurrLEDState = NULL);

    /** @brief Set the State of the LEDs on the FlexIO-DisplayBoard
      *
      * The FlexIO DisplayBoard contains 8 Dual-Color LEDs, each containing
      * a red and green Sub-LED, allowing each LED to be dark or to shine red,
      * yellow/orange or green. There are 16 bits controlling these LEDs.\n
      *   Bit 0: Red Sub-LED of LED1\n
      *   Bit 1: Green Sub-LED of LED1\n
      *   Bit 2: Red Sub-LED of LED2\n
      *    ...\n
      *   Bit 15: Green Sub-LED of LED8\n
      * A Sub-LED is lit, if the corresponding bit is set to '1' and dark if
      * it is set to '0'.
      * In addition to the LED Status bit, each LED also has a Active Mask bit.
      * A LED is only lit if the Status Bit is 1 and the Active Mask bit is 1.
      * This feature makes it possible to set the status of a LED to active,
      * without actually emitting light. This is desireable in light sensitive
      * environments, where stray light might confuse the experimental optics.
      *
      * @note: The leftmost LED of the Display Board is LED1 the rightmost is LED8.
      *
      * @param SetLEDState   In: Requested State of LEDs
      * @param SetLEDMask    In: Only LEDs whose Mask Bit is '1' are set.
      * @param ToggleLEDMask In: LEDs whose ToggleLEDMask bit are set, are toggled.
      * @param SetLEDActiveState   In: Requested State of the LED Active Mask
      * @param SetLEDActiveMask    In: Only LED Active Mask Bits whose Mask Bit is '1' are set.
      * @param CurrLEDState Out: New State of LEDs
      *        Pass NULL if you are not interested in this information.
      * @param CurrLEDActiveMask Out: New State of the LED Active Mask
      *        Pass NULL if you are not interested in this information.
      * @returns Error Code
      */
    FlexIOStatus SetLEDExt(uint32_t SetLEDState, uint32_t SetLEDMask = 0x00FFFF,
                       uint32_t ToggleLEDMask = 0x0000,
                       uint32_t SetLEDActiveState = 0x0000, uint32_t SetLEDActiveMask = 0x0000,
                       uint32_t *CurrLEDState = NULL, uint32_t *CurrLEDActiveMask = NULL);

    /** @brief Set the prefered action for the Buttons
      *
      *  The TTM8000 Hardware offers two buttons, that can be used for predefined
      *  actions or for user/application specific purposes. SetButtonAction()
      *  defines the action that the buttons shall be assigned to...
      *
      * @param KeyAction   In: Requested action for the buttons
      * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
      *                        or is it a temporary setting, that shall be forgotten
      *                        at the next reset.
      * @returns Error Code
      */
    FlexIOStatus SetButtonAction(ButtonAction_t KeyAction[2], bool FlashConfig);

    /** @brief Get the prefered action for the Buttons
      *
      *  The TTM8000 Hardware offers two buttons, that can be used for predefined
      *  actions or for user/application specific purposes. GetButtonAction()
      *  fetches the action that the buttons shall be assigned to...
      *
      * @param KeyAction   Out: Requested action for the buttons
      * @returns Error Code
      */
    FlexIOStatus GetButtonAction(ButtonAction_t KeyAction[2]);

    /** @brief Set the function of the User-LEDs
      *
      *  The TTM8000 Hardware offers 8 dual-color LEDs on the front panel. Five of
      *  these LEDs have specific functions, that can not be changed. The other
      *  three LEDs (User-LEDs) can be used for predefined actions (currently only
      *  heartbeat signalling) or for user/application specific purposes.
      *  SetLEDAction() can be used to switch between these uses.
      *
      * @param LEDAction   In: Requested action for the LEDs
      * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
      *                        or is it a temporary setting, that shall be forgotten
      *                        at the next reset.
      * @returns Error Code
      */
    FlexIOStatus SetLEDAction(LEDAction_t LEDAction[3], bool FlashConfig);

    /** @brief Get the function of the User-LEDs
      *
      *  The TTM8000 Hardware offers 8 dual-color LEDs on the front panel. Five of
      *  these LEDs have specific functions, that can not be changed. The other
      *  three LEDs (User-LEDs) can be used for predefined actions (currently only
      *  heartbeat signalling) or for user/application specific purposes.
      *  GetLEDAction() can be used to switch between these uses.
      *
      * @param LEDAction  Out: Requested action for the LEDs
      * @returns Error Code
      */
    FlexIOStatus GetLEDAction(LEDAction_t LEDAction[3]);

    /** @brief Set the PWM Ratio of the Cooling FAN
      *
      * The FlexIO Board can contain an optional Cooling Fan, whose speed can be
      * controlled using PWM (Pulse-Width-Modulation).
      * Note: SetFanPower() is DEPRECIATED! - Use SetFanConfig() instead! // <<<< AIT INTERNAL USE ONLY!!!
      *
      * @param FanPower      In: Fan PWM Ratio (0..100% Off ~ 1000..100% On)
      * @returns Error Code
      */
    FlexIOStatus SetFanPower(uint32_t FanPower);


   /** @brief Get the Input Voltages of the Analog-Digital Converters (ADCs)
     *
     * The FlexIO Board contains 24 12-bit Analog Digital Converters that
     * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
     * Mode.
     * @note Measuring a range of 4.096V, using a 12-bit ADC results in a
     *    resolution of 1mV/LSB.
     * @note For historical reasons GetADC() will only measure some/any of
     *    the first 16 channels.To access all channels use GetExtADC().
     *
     * When used with the TTM8000-Board the 16 Channels measure the following voltages:\n
     *   Channel 0:  3.3V Supply Voltage / 2.0 (3.3V / 2.0 = 1650mV)\n
     *   Channel 1:  1.8V Supply Voltage\n
     *   Channel 2: 12.0V Supply Voltage / 10.1 (12.0V / 10.1 = 1188mV)\n
     *   Channel 3:  5.0V Supply Voltage / 3.1 (5.0V / 3.1 = 1613mV)\n
     *   Channel 4:  ext. Temperature Sensor\n
     *   Channel 5:  int. Temperature Sensor\n
     *   Channel 6:  5.0V Supply Current - Bipolar!!\n
     *   Channel 7: 12.0V Supply Current - Bipolar!!\n
     *   Channel 8:  TTM8000-Board - Analog User Input #0\n
     *   Channel 9:  TTM8000-Board - Analog User Input #1\n
     *   Channel 10: TTM8000-Board - Analog User Input #2\n
     *   Channel 11: TTM8000-Board - Analog User Input #3\n
     *   Channel 12: TTM8000-Board - VDDC\n
     *   Channel 13: TTM8000-Board - VDDC0\n
     *   Channel 14: TTM8000-Board - +5.0V analog / 3.1 (+5.0V / 3.1 = +1613mV)\n
     *   Channel 15: TTM8000-Board - -5.0V analog / 3.1 (-5.0V / 3.1 = -1613mV) - Bipolar!!
     *
     * @param MilliVolt   Out: Measured Voltages [mV]
     * @param BipolarMask  In: Bitmask of Bipolar Channels
     * @param UpdateMask   In: Bitmask of Voltages that are to be measured
     * @returns Error Code
     */
    FlexIOStatus GetADC(int MilliVolt[16], uint32_t BipolarMask, uint32_t UpdateMask = 0x00FFFF);

   /** @brief Get the Input Voltages of the Analog-Digital Converters (ADCs)
     *
     * The FlexIO Board contains 24 12-bit Analog Digital Converters that
     * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
     * Mode.
     * @note Measuring a range of 4.096V, using a 12-bit ADC results in a
     *    resolution of 1mV/LSB. Thus all results are multiples of 1mV.
     *
     * When used with the TTM8000-Board the 24 Channels measure the following voltages:\n
     *   Channel 0:  3.3V Supply Voltage / 2.0 (3.3V / 2.0 = 1650mV)\n
     *   Channel 1:  1.8V Supply Voltage\n
     *   Channel 2: 12.0V Supply Voltage / 10.1 (12.0V / 10.1 = 1188mV)\n
     *   Channel 3:  5.0V Supply Voltage / 3.1 (5.0V / 3.1 = 1613mV)\n
     *   Channel 4:  ext. Temperature Sensor\n
     *   Channel 5:  int. Temperature Sensor\n
     *   Channel 6:  5.0V Supply Current - Bipolar!!\n
     *   Channel 7: 12.0V Supply Current - Bipolar!!\n
     *   Channel 8:  TTM8000-Board - Analog User Input #0\n
     *   Channel 9:  TTM8000-Board - Analog User Input #1\n
     *   Channel 10: TTM8000-Board - Analog User Input #2\n
     *   Channel 11: TTM8000-Board - Analog User Input #3\n
     *   Channel 12: TTM8000-Board - VDDC\n
     *   Channel 13: TTM8000-Board - VDDC0\n
     *   Channel 14: TTM8000-Board - +5.0V analog / 3.1 (+5.0V / 3.1 = +1613mV)\n
     *   Channel 15: TTM8000-Board - -5.0V analog / 3.1 (-5.0V / 3.1 = -1613mV) - Bipolar!!\n
     *   Channel 16: Vcc Aux 2.5V\n
     *   Channel 17: Vcc Int 1.0V\n
     *   Channel 18: AVcc (1,2V or GND)\n
     *   Channel 19: AVtt (1.0V or GND)\n
     *   Channel 20: VRef 4.096V\n
     *   Channel 21: 3.3V Supply Current\n
     *   Channel 22: 2.5V Supply Current\n
     *   Channel 23: Reserved\n
     *   Channel 24..31: Reserved
     *
     * @param MilliVolt   Out: Measured Voltages [mV]
     * @param BipolarMask  In: Bitmask of Bipolar Channels
     * @param UpdateMask   In: Bitmask of Voltages that are to be measured
     * @returns Error Code
     */
    FlexIOStatus GetExtADC(int MilliVolt[32], uint32_t BipolarMask, uint32_t UpdateMask = 0x00FFFFFF);

    /** @brief Set the Configuration for the Averaging ADC measurement
     *
     * @see GetAvgADC()
     *
     * @param UpdateMicroDelay  In: Interval between ADC measurements
     * @param BipolarMask       In: Bitmask of Bipolar Channels
     * @param ActiveChnMask     In: Bitmask of Voltages that are to be measured
     * @returns Error Code
     */
    FlexIOStatus SetAvgADCConfig(uint32_t UpdateMicroDelay = 5000,
                                 uint32_t BipolarMask = 0x0F00,
                                 uint32_t ActiveChnMask = 0x0F00);

    /** @brief Get the Configuration for the Averaging ADC measurement
     *
     * @see GetAvgADC()
     *
     * @param UpdateMicroDelay Out: Interval between ADC measurements
     * @param BipolarMask      Out: Bitmask of Bipolar Channels
     * @param ActiveChnMask    Out: Bitmask of Voltages that are to be measured
     * @returns Error Code
     */
    FlexIOStatus GetAvgADCConfig(uint32_t *UpdateMicroDelay,
                                 uint32_t *BipolarMask, uint32_t *ActiveChnMask);

    /** @brief Obtain average ADC measurements
     *
     * Looking at the current values provided by the ADCs provides a
     * momentary snapshot of the current voltage levels. If the signal
     * itself changes only very slowly, but is contaminated with lots
     * of noise, we can get better measurements by reading the signal
     * repeatedly at regular intervals and then computing the average
     * of all measurements taken recently.
     * The implementation chosen for TTM8000 let the user choose how often
     * the ADCs shall be queried, and which channels shall be updated
     * in bipolar or unipolar mode. The most recent 256 measurements are
     * stored and the (moving!) average of these measurements will be
     * computed upon request and transfered in a single operation (rather
     * than bothering the network for each measurement).
     * Note: Use The Average ADC measurement feature only if you really
     * need it. Querying the ADC often is an expensive operation and
     * should not be use for nought. Keep you UpdateDelay in the range
     * of at least several milliseconds (i.e. 1000s of microseconds!)
     * If you do not need this feature any more, be sure to turn it off
     * again by setting ActiveChnMask to 0.
     *
     * Note: GetAdvADC() reads only ADC channels 0..15. If you need access
     * to channels 16..31 you must use GetExtADC().
     *
     * @see GetExtAvgADC()
     *
     * @param RecentMilliTime In: How old shall the oldest measurement be
     *         that is included in the average? [ms]
     * @param AvgCnt      Out: Number of samples included in the average
     * @param Microvolt   Out: Measured Voltages [uV]
     * @returns Error Code
     */
    FlexIOStatus GetAvgADC(int RecentMilliTime, int *AvgCnt, int Microvolt[16]);

    /** @brief Obtain average ADC measurements
     *
     * Looking at the current values provided by the ADCs provides a
     * momentary snapshot of the current voltage levels. If the signal
     * itself changes only very slowly, but is contaminated with lots
     * of noise, we can get better measurements by reading the signal
     * repeatedly at regular intervals and then computing the average
     * of all measurements taken recently.
     * The implementation chosen for TTM8000 let the user choose how often
     * the ADCs shall be queried, and which channels shall be updated
     * in bipolar or unipolar mode. The most recent 256 measurements are
     * stored and the (moving!) average of these measurements will be
     * computed upon request and transfered in a single operation (rather
     * than bothering the network for each measurement).
     * Note: Use The Average ADC measurement feature only if you really
     * need it. Querying the ADC often is an expensive operation and
     * should not be use for nought. Keep you UpdateDelay in the range
     * of at least several milliseconds (i.e. 1000s of microseconds!)
     * If you do not need this feature any more, be sure to turn it off
     * again by setting ActiveChnMask to 0.
     *
     * @see GetAvgADC()
     *
     * @param RecentMilliTime In: How old shall the oldest measurement be
     *         that is included in the average? [ms]
     * @param AvgCnt      Out: Number of samples included in the average
     * @param Microvolt   Out: Measured Voltages [uV]
     * @returns Error Code
     */
    FlexIOStatus GetExtAvgADC(int RecentMilliTime, int *AvgCnt, int Microvolt[32]);

    /** @brief Set the Output Voltages of the Digital-Analog Converters (DACs)
      *
      * The FlexIO Board contains 16 8-bit Digital Analog Converters (DACs).
      * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
      * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
      * range 0 .. 4096mV (with a resolution of 16mV).
      *
      * When used with the TTM8000 Board the 16 Channels provide the following voltages:\n
      *   Channel 0:  TTM8000-Board - Start Reference Voltage\n
      *   Channel 1:  TTM8000-Board - Stop1 Reference Voltage\n
      *   Channel 2:  TTM8000-Board - Stop2 Reference Voltage\n
      *   Channel 3:  TTM8000-Board - Stop3 Reference Voltage\n
      *   Channel 4:  TTM8000-Board - Stop4 Reference Voltage\n
      *   Channel 5:  TTM8000-Board - Stop5 Reference Voltage\n
      *   Channel 6:  TTM8000-Board - Stop6 Reference Voltage\n
      *   Channel 7:  TTM8000-Board - Stop7 Reference Voltage\n
      *   Channel 8:  TTM8000-Board - Stop8 Reference Voltage\n
      *   Channel 9:  TTM8000-Board - Ext. Clock Reference Voltage\n
      *   Channel 10: TTM8000-Board - OCXO (Oven Controlled Crystal Osc.) Coarse Trim Voltage\n
      *   Channel 11: TTM8000-Board - OCXO (Oven Controlled Crystal Osc.) Fine Trim Voltage\n
      *   Channel 12: TTM8000-Board - Analog User Output #0\n
      *   Channel 13: TTM8000-Board - Analog User Output #1\n
      *   Channel 14: TTM8000-Board - Analog User Output #2\n
      *   Channel 15: TTM8000-Board - Analog User Output #3\n
      *
      * @param MilliVolt   InOut: In: Requested Voltages [mV] - Out: Current Voltage [mV]
      * @param UpdateMask  In: Bitmask of Voltages to be updated
      * @returns Error Code
      */
    FlexIOStatus SetDAC(int MilliVolt[16], uint32_t UpdateMask = 0x00FFFF);

    /** @brief Get the Output Voltages of the Digital-Analog Converters (DACs)
      *
      * The FlexIO Board contains 16 8-bit Digital Analog Converters (DACs).
      * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
      * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
      * range 0 .. 4096mV (with a resolution of 16mV).
      *
      * @note The AnalogOut Channels 0..3 available for user applications at
      *       the TTM8000-Board correspond to DAC Channels 12..15.
      *
      * @param MilliVolt   In: Current Voltages [mV]
      * @returns Error Code
      *
      * @see SetDAC()
      */
    FlexIOStatus GetDAC(int MilliVolt[16]);

    /** @brief Get the Configuration of the FlexIO Board
      *
      * GetBoardConfig() fetches the Configuration (Serial Number/ Board Name/
      * Network Settings/ Debug Level) of the FlexIO Board.
      *
      * @param BoardConfig Out: Configuration of the FlexIO Board
      * @returns Error Code
      */
    FlexIOStatus GetBoardConfig(FlexIOBoardConfig_t *BoardConfig);


    /** @brief Update the Configuration of the FlexIO Board
      *
      * UpdateBoardConfig() updates the Configuration (Board Name/
      * Network Settings/ Debug Level) of the FlexIO Board.
      * Note that the Serial Number and MAC Address are factory defined
      * and can not be updated.
      *
      * @param BoardConfig In: New Configuration of the FlexIO Board
      * @returns Error Code
      */
    FlexIOStatus UpdateBoardConfig(const FlexIOBoardConfig_t *BoardConfig);


    /** @brief Update the PowerPC Firmware of the FlexIO Board
      *
      * UpdateFirmware() writes a new PowerPC Firmware into the FlexIO Board.
      * The Firmware must be in ELF (preferred) or SREC format.
      * The new Firmware will become active after the next reboot.
      *
      * @param Firmware     In: Firmware
      * @param FirmwareSize In: Size of Firmware [Byte]
      * @param Verbose      In: Print Progress Messages to the Console
      * @returns Error Code
      */
    FlexIOStatus UpdateFirmware(uint8_t *Firmware, uint32_t FirmwareSize, bool Verbose);

    /** @brief Update the Xilinx FPGA Bitfile
      *
      * UpdateFPGA() writes a new FPGA Bitfile to the FlexIO Board. Be sure that
      * the FPGA logic has been configured to use the Configuration Clock (CCLK)
      * for system configuration (rather than JTAGClk) and that the BootRAM has
      * been loaded with a valid Bootloader.
      * The new Bitfile will become active after the next reboot.
      *
      * @param Firmware     In: Firmware
      * @param FirmwareSize In: Size of Firmware [Byte]
      * @param Verbose      In: Print Progress Messages to the Console
      * @returns Error Code
      */
    FlexIOStatus UpdateFPGA(uint8_t *Firmware, uint32_t FirmwareSize, bool Verbose);

    /** @brief Reboot the FlexIO Board (breaking the connection)
      *
      * Reboot() tells the FlexIO Board to reboot itself.
      * Obviously this breaks the network connection to the board.
      * @note Warning: Reboot() does not work with the current hardware!
      *
      * @note  Reboot() is designed for AIT Internal Use Only!
      *
      * @returns Error Code
      */
    FlexIOStatus Reboot();


/** @brief Convert an IPAddress to an ASCII String
  *
  * Note: This should actually be handled by using inet_ntoa() or inet_ntop(), however
  *    inet_ntoa() is marked as obsolete (and Visual Studio 2013 gets quite nervous
  *    about that), and inet_ntop() is not supported by Visual Studio 6.
  *    Thus we shall write our own functions that are available on every compiler...
  *
  * @param SockAddr In:  Socket Info (in Network Byte Order!)
  * @param Buffer   Out: Buffer for the Output String (can be NULL to use an internal buffer)
  * @returns Pointer to ASCII String (Buffer if non-NULL or internal static memory)
  */
static char* IPAddrToString(const struct sockaddr_storage *SockAddr, char *Buffer = NULL);

/** @brief Convert an IPAddress to an ASCII String
  *
  * Note: This should actually be handled by using inet_ntoa() or inet_ntop(), however
  *    inet_ntoa() is marked as obsolete (and Visual Studio 2013 gets quite nervous
  *    about that), and inet_ntop() is not supported by Visual Studio 6.
  *    Thus we shall write our own functions that are available on every compiler...
  *
  * @param SockAddr In:  Socket Info (in Network Byte Order!)
  * @param Buffer   Out: Buffer for the Output String (can be NULL to use an internal buffer)
  * @returns Pointer to ASCII String (Buffer if non-NULL or internal static memory)
  */
static char* IPAddrToString(const struct sockaddr_in *SockAddr, char *Buffer = NULL);

/** @brief Convert an IPAddress to an ASCII String
  *
  * Note: This should actually be handled by using inet_ntoa() or inet_ntop(), however
  *    inet_ntoa() is marked as obsolete (and Visual Studio 2013 gets quite nervous
  *    about that), and inet_ntop() is not supported by Visual Studio 6.
  *    Thus we shall write our own functions that are available on every compiler...
  *
  * @param Family In:  Socket Address Family (use AF_INET_IN or AF_INET)
  * @param IPAddr In:  Pointer to an in_addr or in_addr6 (in Network-Byte Order)
  * @param Buffer Out: Buffer for the Output String (can be NULL to use an internal buffer)
  * @returns Pointer to ASCII String (Buffer if non-NULL or internal static memory)
  */
static char* IPAddrToString(sa_family_t Family, void *IPAddr, char *Buffer = NULL);

/** @brief Convert an IPAddress to an ASCII String
  *
  * Note: This should actually be handled by using inet_ntoa() or inet_ntop(), however
  *    inet_ntoa() is marked as obsolete (and Visual Studio 2013 gets quite nervous
  *    about that), and inet_ntop() is not supported by Visual Studio 6.
  *    Thus we shall write our own functions that are available on every compiler...
  *
  * @param IPv4Addr In: IPv4 Address (in Network-Byte Order)
  * @param Buffer Out:  Buffer for the Output String (can be NULL to use an internal buffer)
  * @returns Pointer to ASCII String (Buffer if non-NULL or internal static memory)
  */
static char* IPv4AddrToString(in_addr IPv4Addr, char *Buffer = NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  protected:
    CoreFlexIOCntrl_c *FlexIOCntrlHnd; /**< Opaque Handle to the FlexIOCntrl Connection */
 }; /* class FlexIOCntrl_c */

/**********************************************************************
 *                            FlexIOLib.hpp                           *
 **********************************************************************/
#endif // FlexIOLib_hpp
