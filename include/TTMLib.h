/**********************************************************************
 *                                                                    *
 *                              TTMLib.h                              *
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
#ifndef TTMLib_h
#define TTMLib_h

/**********************************************************************/

#ifndef SysTypes_h
  #include "SysTypes.h"
#endif

#ifndef FlexIOLib_h
  #include "FlexIOLib.h"
#endif

/**********************************************************************
 *                                                                    *
 **********************************************************************/

#if defined(_WIN32)       // Windows
  #if defined _MSC_VER || defined __GNUC__ // Microsoft Visual Studio & GCC
    #if defined (LIBTTM_DLLEXPORTS)
      #define LIBTTM_API __declspec(dllexport)
    #elif defined (LIBTTM_LIBEXPORTS)
      #define LIBTTM_API
    #else
      #define LIBTTM_API __declspec(dllimport)
    #endif
  #else
    // Qt Creator ...
    // Qt defines Q_DECL_EXPORT as __attribute__((visibility("default")))
        #if defined (LIBTTM_DLLEXPORTS)
          #define LIBTTM_API Q_DECL_EXPORT
        #else
          #define LIBTTM_API Q_DECL_IMPORT
        #endif
  #endif
#elif defined(__linux)    // Linux
  #ifdef LIBTTM_LIBEXPORTS
    #define LIBTTM_API __attribute__((visibility("default")))
  #else
    #define LIBTTM_API
  #endif
#elif defined(__MacOSX__) // Mac OS X
  #ifdef LIBTTM_LIBEXPORTS
    #define LIBTTM_API __attribute__((visibility("default")))
  #else
    #define LIBTTM_API
  #endif
#else
  #error Unknown Operating System - Please use Windows, Linux or MacOS X
#endif

/**********************************************************************
 *                                                                    *
 **********************************************************************/

#if defined __cplusplus
extern "C" {
#endif

/**********************************************************************/

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                             TTMCntrl                             **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Handle to a TTMCntrl Object.
  *
  * A TTMCntrl_t is a handle to a TTMCntrl objects. TTMCntrl objects are
  * opaque, they are useable only by passing their handle to one of the
  * functions below. They do not provide any data fields that can be
  * directly read/written with C statements. Use TTMCntrl_Connect()
  * to create a new TTMCntrl object and remember that you MUST call
  * TTMCntrl_Disconnect() to dispose of it after you are done using it.
  * Note that this requirement applies even if you are leaving your
  * application at an unexpected time (e.g. after the user pressed Cntrl-C
  * to abort) as the TTM8000-Board would otherwise not be notified of
  * the broken connection.
  *
  * Technical Note: Internally a TTMCntrl_t is a *TTMCntrl_c, that
  * corresponds to the 'this' pointer used in C++ class-method calls.
  * However since this header can be used in plain C code, that does
  * not understand C++ classes, we can not explicitly use TTMCntrl_c
  * here. We could define TTMCntrl_t as a void*, however this would
  * not be type-safe, since a void** is automatically converted to a
  * void* as needed by the compiler and mistakes caused by using a
  * *TTMCntrl_t when a TTMCntrl_t would be needed would not be caught.
  * Thus we shall use a simple struct, that looks a bit oversized but
  * makes things typesafe (and does not incur any cost in code size
  * or performance).
  */
typedef struct {
  void *OpaqueTTMCntrlHnd; //!< Pointer to TTMCntrl_c masqueraded for use with plain C
 } TTMCntrl_t;

/** @brief Get the Version number of TTMLib.
  *
  * @return Returns the version number of TTMLib at run-time 32-bit integer in format
  * 0xMMNNPPSS (MM = major, NN = minor, PP = patch, SS = sub(0=none, 1='a', ...).
  * For example TTMLib 1.4.12b will return 0x01040C02.
  */
uint32_t LIBTTM_API TTMCntrl_GetVersion();

/** @brief Get a list of all TTM8000-Boards available on the net.
  *
  * GetAvailableBoards() queries a single board, all boards in a specific subnet
  * or all boards available on all subnets connected to the local host and reports
  * their configuration and current status in an array of FlexIOBoardInfos.
  *
  * @param ApplMagic      In: Look only for FlexIO-/TTM8000-Boards that run a specific
  *        application. Use AnyApplCookie to search for Boards running any application.
  *        Use TTM8ApplCookie to search for Boards running TTM8000.
  * @param BoardSubNet    In: The broadcast address of the subnet that shall be
  *        searched for boards. Use INADDR_ANY if you want to look at the subnets
  *        of all network interfaces on the local host. Use the broadcast address
  *        of a specific subnet to search just this subnet and use the IP-address
  *        of a specific FlexIO board if you just want to check the availability
  *        and obtain configuration/status data for a single board.
  * @param BoardCntrlPort In: The Control Port used by the FlexIO-/TTM8000-boards.
  *        Usually you will use FlexIOCntrlPort.
  * @param BoardInfo     Out: Array of FlexIOBoardInfos
  * @param MaxBoardCnt    In: Capacity of BoardInfo (Number of Entries available)
  * @param BoardCnt      Out: Number of FlexIO-Boards actually discovered
  * @return Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetAvailableBoards(uint32_t ApplMagic,
                                in_addr_t BoardSubNet, in_port_t BoardCntrlPort,
                                FlexIOBoardInfo_t *BoardInfo, int MaxBoardCnt, int *BoardCnt);

/** @brief Get the local network interface, whose subnet contains a given TTM8000-Board.
  *
  * The TTM8000-Boards only operate in the local network. They do not support ethernet routing
  * with gateways. Thus when we need a destination address of the application data, it must
  * be in the same subnet as the TTM8000-Board. GetNearestNetInterface() will determine the
  * IP-Address of the network interface on the local host, that is closest to a given board.
  * @param BoardIPAddr   In: IP-address of the TTM8000-Board
  * @param LocalIPAddr  Out: IP-address of the network interface closest to the TTM8000-Board
  *                          i.e. containing the IP-address of the TTM8000-Board in its subnet.
  *                          The IP-address of any network interface, if none matches the
  *                          above criterion.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetNearestNetInterface(in_addr_t BoardIPAddr,
                                                              in_addr_t *LocalIPAddr);

/** @brief Get a human-readable String for an Error Code.
  *
  * @param Err In: The Error Code
  * @returns Pointer to a null-terminated string. Note that this string belongs to
  *          FlexIOLib and must not be modified. It is only valid until the next call
  *          to GetErrorName().
  */
const char LIBTTM_API *TTMCntrl_GetErrorName(FlexIOStatus Err);

/** @brief Connect the TTMCntrl Object to a TTM8000-Board.
  *
  * @note Every TTMCntrl Connection that was opened with Connect() must be
  *       closed with a corresponding call to Disconnect()!
  * @param TTMCntrl       Out: Handle to the new TTMCntrl object
  * @param BoardInfo      Out: Configuration/Status Information of the TTM8000-Board.
  *        Pass NULL if you are not interested in this information.
  * @param ApplMagic       In:  Only connect to the FlexIO-/TTM8000-Board if it runs a
  *        given application. Use AnyApplCookie to connect to the FlexIO-Board
  *        regardless of the application it is running. Use TTM8ApplCookie to
  *        search for Boards running TTM8000.
  * @param BoardIPAddr     In: IP-Address of the requested TTM8000-Board
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
FlexIOStatus LIBTTM_API TTMCntrl_Connect(TTMCntrl_t *TTMCntrl,
                  FlexIOBoardInfo_t *BoardInfo, uint32_t ApplMagic,
                  in_addr_t BoardIPAddr, in_port_t BoardCntrlPort,
                  in_addr_t LocalIPAddr, in_port_t LocalCntrlPort,
                  uint32_t  MilliNetTimeout);

/** @brief Check if a Control Connection to a TTM8000-Board has been established
  *
  * IsConnected() checks is a Control Connection has been established. It
  * can either perform a fast, local check, that just remembers if Connect()
  * has been successfully called for this TTMCntrl_t object, without
  * checking if the connection is still alive, or it can actually send a
  * test packet over the network and wait for an acknowledgment to make
  * sure that the network link is still alive.
  * Obviously the later option requires much more time.
  *
  * @param TTMCntrl       In: Handle to the TTMCntrl Connection
  * @param CheckNetStatus In: Do we want to send a test packet over
  *        the network to check that the connection is actually alive
  *        or is it sufficient to remember that we once successfully
  *        established a connection (that may or may not still be alive).
  * @returns Connection Status
  */
bool_t LIBTTM_API TTMCntrl_IsConnected(TTMCntrl_t TTMCntrl, bool_t CheckNetStatus);

/** @brief Dispose of a Control Connection to a TTM8000-Board.
  *
  * Disconnect closes a TTMCntrl Control Connection.
  *
  * Note: Once a TTMCntrl Connection has been closed it must no longer
  * be used for any TTMCntrl_*() function call.
  *
  * @param TTMCntrl  In: Handle to the TTMCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_Disconnect(TTMCntrl_t TTMCntrl);

/** @brief Close other(!) Control Connection to a TTM8000-Board.
  *
  * DisconnectOthers() closes Control Connection to the TTM8000-
  * Board that are in use by other(!) applications. It does NOT
  * close the current connection. The affected applications are NOT
  * notified of their loss (using the assumption that they are dead
  * already anyhow, and just the TTM8000-Board was not notified of their
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
  * application is the only user to the TTM8000-Board, or where
  * an other application did indeed fail to close its connection
  * before quitting, and you need to close that externally. However
  * these should be very rare occasions. Usually there should be no harm
  * if the TTM8000-Board is shared between many applications and
  * your application should not take offense at the presence of others!\n
  *
  * Use DisconnectOthers() ONLY if you really see no other way out!
  *
  * @param TTMCntrl       In: Handle to the TTM8Cntrl Connection
  * @param TargetClient   In: List of Client Connections that shall be killed.
  *        Pass NULL to kill all Client Connection except the current one.
  * @param TargetCnt      In: Number of Entries in TargetClient (or 0)
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_DisconnectOthers(TTMCntrl_t TTMCntrl,
                                       UDPAddr_t *TargetClient, int TargetCnt);

/** @brief Get the BSD socket used to send/receive commands.
  *
  * GetSocket() provides access to the handle of the BSD-style socket
  * used for the TTMCntrl connection. You can use it if your application has
  * special requirements (e.g. configuration settings) that can not be addressed
  * otherwise. However you should remember that the socket belongs to TTMLib
  * and must not be closed etc.
  * @note Most applications will NOT need to use GetSocket().
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param CntrlSocket Out: Socket Handle
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetSocket(TTMCntrl_t TTMCntrl, SOCKET *CntrlSocket);

/** @brief Get the IP-Address/UDP Port used to send/receive commands.
  *
  * GetSocketAddr() provides the IP-Address/UDP Port of the socket
  * used for the TTMCntrl connection.
  * This might be useful for status displays/debugging however most
  * applications will probably never need to call GetSocketAddr().
  * @param TTMCntrl        In: Handle to the TTMCntrl Connection
  * @param LocalIPAddr    Out: IP-Address of the Cntrl-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @param LocalCntrlPort Out: UDP Port of the Cntrl-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetSocketAddr(TTMCntrl_t TTMCntrl,
                               in_addr_t *LocalIPAddr, in_port_t *LocalCntrlPort);

/** @brief Set the Input Buffer Size of the local Control Socket
  *
  * The network connection between the TTM8000-Board and the local PC
  * uses BSD sockets, that have a input buffer on the PC to cope with
  * bursts of incomming network packets. Use SetSocketBufferSize() to
  * adjust the size of this buffer.
  * @note  Under Linux the maximal network buffer size is limited by
  *        the value in /proc/sys/net/core/rmem_max. You can temporarily
  *        change this value by editing this file, or you can add the
  *        line "net.core.rmem_max = 8388608" to /etc/sysctl.conf to
  *        make changes that survive a reboot.
  * @note  TTMLib automatically sets the buffer size to 2MByte when
  *        creating a new connection. Thus most applications will never
  *        need to call SetSocketBufferSize().
  * @param TTMCntrl   In: Handle to the TTMCntrl Connection
  * @param BufferSize In: Requested Network Buffer Size [Byte] for
  *        the Control Socket. Note that this size is just a suggestion
  *        to the Operating System, that might choose to grant more or
  *        less than was requested.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetSocketBufferSize(TTMCntrl_t TTMCntrl, uint32_t BufferSize);

/** @brief Get the Input Buffer Size of the local Control Socket
  *
  * The network connection between the TTM8000-Board and the local PC
  * uses BSD sockets, that have a input buffer on the PC to cope with
  * bursts of incomming network packets. Use GetSocketBufferSize() to
  * obtain the size of this buffer.
  * @note  TTMLib automatically sets the buffer size to 2MByte when
  *        creating a new connection, which is more that sufficient for
  *        most applications. Thus most applications will never need to
  *        call GetSocketBufferSize().
  * @param TTMCntrl   In: Handle to the TTMCntrl Connection
  * @param BufferSize In: The actual Network Buffer Size [Byte]
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetSocketBufferSize(TTMCntrl_t TTMCntrl, uint32_t *BufferSize);

/** @brief Get Network IP-MTU (Internet Protocol - Max. Transmission Unit) for the TTMLib Connection.
  *
  * Each network connection will only carry packets up to a specific size,
  * and will discard packets that are larger than this limit.
  * GetNetMTU() will discover the MTU for the TTMCntrl Connection.
  * @note Sending few large packets creates less overhead than sending many
  * small packets to transfer the same amount of payload. Thus it is
  * advantageous to send data packets that are as big as possible
  * (however not bigger than the MTU or the the packets will be dropped...).
  * @note Fast Ethernet generally has a MTU of 1500 Byte. Gigabit Ethernet
  * adapters often support Jumbo frames that can be larger, however
  * how much larger depends on the actual hardware and its configuration.
  * The FlexIO Server can take advantage of an MTU of up to 8KByte for
  * the Data connection. The packets on the TTMCntrl connection
  * are always smaller than 1500Byte.
  * @note As stated above GetNetMTU() will get the MTU for the
  * TTMCntrl connection. It does not make any statement about the MTU
  * of the TTMData connection! If you are running the control and
  * data aquisition applications on the same machine, the MTUs for both
  * applications will be the same, and you can use the TTMCntrl MTU
  * value to define the packet size for TTMData packets. If you are
  * running these application on different machines, the MTUs might be
  * different and you will need to use a different way (depending on
  * your specific situation) to discover the TTMData MTU.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param NetMTU  Out: The Network MTU of the TTMCntrl Connection
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetNetMTU(TTMCntrl_t TTMCntrl, uint32_t *NetMTU);

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
  *
  * Note: For historic reasons TTMCntrl_GetNetMTU() was designed to
  * just fetch the MTU for the TTM-to-PC direction. GetNetMTUExt()
  * fetches the MTU individually for both directions.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param TTMtoPC_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
  *        when sending data from the FlexIO board to the controlling PC.
  *        Pass NULL if you are not interested in this information.
  * @param PCtoTTM_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
  *        when sending data from the controlling PC to the FlexIO board.
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetNetMTUExt(TTMCntrl_t TTMCntrl, uint32_t *TTMtoPC_NetMTU, uint32_t *PCtoTTM_NetMTU);

/** @brief Get Configuration/Status Information of the currently used TTM8000-Board.
  *
  * GetBoardInfo() obtains Configuration Information (e.g. Serial Number, Device
  * Type, Network configuration, Firmware Version, Debug Levels etc.) and
  * Status Information (e.g. Partners of active Network-Connections, Measurement
  * Status etc.) of the current TTM8000-Board.
  * @param TTMCntrl   In: Handle to the TTMCntrl Connection
  * @param BoardInfo  Out: Configuration/Status Information of the TTM8000-Board
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetBoardInfo(TTMCntrl_t TTMCntrl,
                                                    FlexIOBoardInfo_t *BoardInfo);

/** @brief Query the State of the Buttons of the FlexIO-DisplayBoard
  *
  * The TTM8000-DisplayBoard contains two buttons. Use QueryButtons() to
  * obtain the current state of these buttons.
  * @note  While there are only two buttons (User & Reset) installed on the
  *        Display Board, there is a connector that allows 6 additional,
  *        external buttons to be connected.
  *        The state of the Reset button is mapped to Bit 0 (Mask 0x01),
  *        the state of the User button to Bit 1 (Mask 0x02). These bits
  *        are '0' when the button is released an '1' when it is pressed.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param KeyBits Out: State of the Keys
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_QueryKeys(TTMCntrl_t TTMCntrl, uint32_t *KeyBits);

/** @brief Query the State of the Buttons of the FlexIO-DisplayBoard (Depreciated)
  *
  * The FlexIO DisplayBoard contains two buttons. QueryKeys() can still be used
  * obtain the current state of these buttons, however the use of this function
  * is depreciated. Please use QueryButtons() instead.
  */
#define TTMCntrl_QueryKeys(TTMCntrl, KeyBits) TTMCntrl_QueryButtons(TTMCntrl, KeyBits)

/** @brief Set the State of the LEDs on the FlexIO-DisplayBoard
  *
  * The TTM8000-DisplayBoard contains 8 Dual-Color LEDs, each containing
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
  * @note: LEDs 1-5 are used/updated by the TTM8000-Server application. Any
  *  value written to these LEDs will soon be overwritten by the TTM8000-Server.
  *  Only LEDs 6-8 are available to user applications.
  *
  * @param TTMCntrl      In: Handle to the TTMCntrl Connection
  * @param SetLEDState   In: Requested State of LEDs
  * @param SetLEDMask    In: Only LEDs whose Mask Bit is '1' are set.
  * @param ToggleLEDMask In: LEDs whose ToggleLEDMask bit are set, are toggled.
  * @param CurrLEDState Out: New State of LEDs
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetLEDs(TTMCntrl_t TTMCntrl,
                    uint32_t SetLEDState,   uint32_t SetLEDMask,
                    uint32_t ToggleLEDMask, uint32_t *CurrLEDState);

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
  * @param TTMCntrl      In: Handle to the TTMCntrl Connection
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
FlexIOStatus LIBTTM_API TTMCntrl_SetLEDExt(TTMCntrl_t TTMCntrl,
                   uint32_t SetLEDState, uint32_t SetLEDMask,
                   uint32_t ToggleLEDMask,
                   uint32_t SetLEDActiveState, uint32_t SetLEDActiveMask,
                   uint32_t *CurrLEDState, uint32_t *CurrLEDActiveMask);

 /** @brief Set the prefered action for the Buttons
  *
  *  The TTM8000 Hardware offers two buttons, that can be used for predefined
  *  actions or for user/application specific purposes. SetButtonAction()
  *  defines the action that the buttons shall be assigned to...
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param KeyAction   In: Requested action for the buttons
  * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
  *                        or is it a temporary setting, that shall be forgotten
  *                        at the next reset.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetButtonAction(TTMCntrl_t TTMCntrl,
                                             ButtonAction_t KeyAction[2], bool FlashConfig);

 /** @brief Get the prefered action for the Buttons
  *
  *  The TTM8000 Hardware offers two buttons, that can be used for predefined
  *  actions or for user/application specific purposes. GetButtonAction()
  *  fetches the action that the buttons shall be assigned to...
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param KeyAction   Out: Requested action for the buttons
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetButtonAction(TTMCntrl_t TTMCntrl,
                                             ButtonAction_t KeyAction[2]);

 /** @brief Set the function of the User-LEDs
  *
  *  The TTM8000 Hardware offers 8 dual-color LEDs on the front panel. Five of
  *  these LEDs have specific functions, that can not be changed. The other
  *  three LEDs (User-LEDs) can be used for predefined actions (currently only
  *  heartbeat signalling) or for user/application specific purposes.
  *  SetLEDAction() can be used to switch between these uses.
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param LEDAction   In: Requested action for the LEDs
  * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
  *                        or is it a temporary setting, that shall be forgotten
  *                        at the next reset.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetLEDAction(TTMCntrl_t TTMCntrl,
                                             LEDAction_t LEDAction[3], bool FlashConfig);

 /** @brief Get the function of the User-LEDs
  *
  *  The TTM8000 Hardware offers 8 dual-color LEDs on the front panel. Five of
  *  these LEDs have specific functions, that can not be changed. The other
  *  three LEDs (User-LEDs) can be used for predefined actions (currently only
  *  heartbeat signalling) or for user/application specific purposes.
  *  GetLEDAction() can be used to switch between these uses.
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param LEDAction  Out: Requested action for the LEDs
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetLEDAction(TTMCntrl_t TTMCntrl,
                                             LEDAction_t LEDAction[3]);

/** @brief Set the PWM Ratio of the Cooling FAN
  *
  * The FlexIO Board can contain an optional Cooling Fan, whose speed can be
  * controlled using PWM (Pulse-Width-Modulation).
  * Note: SetFanPower() is DEPRECIATED! - Use SetFanConfig() instead! // <<<< AIT INTERNAL USE ONLY!!!
  *
  * @param TTMCntrl      In: Handle to the TTMCntrl Connection
  * @param FanPower      In: Fan PWM Ratio (0..100% Off ~ 1000..100% On)
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetFanPower(TTMCntrl_t TTMCntrl,
                                                   uint32_t FanPower);
/** @brief Get the Input Voltages of the Analog-Digital Converters (ADCs)
  *
  * The FlexIO Board contains 24 12-bit Analog Digital Converters that
  * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
  * Mode.
  * @note Measuring a range of 4.096V, using a 12-bit ADC results in a
  *    resolution of 1mV/LSB.
  * @note For historical reasons TTMCntrl_GetADC() will only measure some/any of
  *    the first 16 channels. To access all channels use TTMCntrl_GetExtADC().
  *
  * The AnalogIn Channels 0..3 available for user applications at
  *       the TTM8000-Board correspond to ADC Channels 8..11.
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
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param MilliVolt   Out: Measured Voltages [mV]
  * @param BipolarMask  In: Bitmask of Bipolar Channels
  * @param UpdateMask   In: Bitmask of Voltages that are to be measured
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetADC(TTMCntrl_t TTMCntrl,
               int MilliVolt[16], uint32_t BipolarMask, uint32_t UpdateMask);

/** @brief Get the Input Voltages of the Analog-Digital Converters (ADCs)
  *
  * The FlexIO Board contains 24 12-bit Analog Digital Converters that
  * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
  * Mode.
  * @note Measuring a range of 4.096V, using a 12-bit ADC results in a
  *    resolution of 1mV/LSB.
  *
  * The AnalogIn Channels 0..3 available for user applications at
  *       the TTM8000-Board correspond to ADC Channels 8..11.
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
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param MilliVolt   Out: Measured Voltages [mV]
  * @param BipolarMask  In: Bitmask of Bipolar Channels
  * @param UpdateMask   In: Bitmask of Voltages that are to be measured
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetExtADC(TTMCntrl_t TTMCntrl,
               int MilliVolt[32], uint32_t BipolarMask, uint32_t UpdateMask);

/** @brief Set the Output Voltages of the Digital-Analog Converters (DACs)
  *
  * The TTM8000-Board contains 16 8-bit Digital Analog Converters (DACs).
  * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
  * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
  * range 0 .. 4096mV (with a resolution of 16mV).
  *
  * @note The AnalogOut Channels 0..3 available for user applications at
  *       the TTM8000-Board correspond to DAC Channels 12..15.
  *
  * @param TTMCntrl    In: Handle to the TTMCntrl Connection
  * @param MilliVolt   InOut: In: Requested Voltages [mV] - Out: Current Voltages [mV]
  * @param UpdateMask  In: Bitmask of Voltages to be updated
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetDAC(TTMCntrl_t TTMCntrl,
                                   int MilliVolt[16], uint32_t UpdateMask);

/** @brief Get the Output Voltages of the Digital-Analog Converters (DACs)
  *
  * The TTM8000-Board contains 16 8-bit Digital Analog Converters (DACs).
  * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
  * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
  * range 0 .. 4096mV (with a resolution of 16mV).
  *
  * @note The AnalogOut Channels 0..3 available for user applications at
  *       the TTM8000-Board correspond to DAC Channels 12..15.
  *
  * @param TTMCntrl    In:  Handle to the TTMCntrl Connection
  * @param MilliVolt   Out: Current Voltages [mV]
  * @returns Error Code
  *
  * @see TTMCntrl_SetDAC()
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetDAC(TTMCntrl_t TTMCntrl, int MilliVolt[16]);

/** @brief Get the Configuration of the TTM8000-Board
  *
  * GetBoardConfig() fetches the Configuration (Serial Number/ Board Name/
  * Network Settings/ Debug Level) of the TTM8000-Board.
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param BoardConfig Out: Configuration of the TTM8000-Board
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetBoardConfig(TTMCntrl_t TTMCntrl,
                                                FlexIOBoardConfig_t *BoardConfig);

/** @brief Update the Configuration of the TTM8000-Board
  *
  * UpdateBoardConfig() updates the Configuration (Board Name/
  * Network Settings/ Debug Level) of the TTM8000-Board.
  * Note that the Serial Number and MAC Address are factory defined
  * and can not be updated.
  *
  * @param TTMCntrl    In: Handle to the TTMCntrl Connection
  * @param BoardConfig In: New Configuration of the TTM8000-Board
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_UpdateBoardConfig(TTMCntrl_t TTMCntrl,
                                            const FlexIOBoardConfig_t *BoardConfig);

/** @brief Update the PowerPC Firmware of the TTM8000-Board
  *
  * UpdateFirmware() writes a new PowerPC Firmware into the TTM8000-Board.
  * The Firmware must be in ELF (preferred) or SREC format.
  * The new Firmware will become active after the next reboot.
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param Firmware     In: Firmware
  * @param FirmwareSize In: Size of Firmware [Byte]
  * @param Verbose      In: Print Progress Messages to the Console
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_UpdateFirmware(TTMCntrl_t TTMCntrl,
                           uint8_t *Firmware, uint32_t FirmwareSize, bool_t Verbose);

/** @brief Update the Xilinx FPGA Bitfile
  *
  * UpdateFPGA() writes a new FPGA Bitfile to the TTM8000-Board. Be sure that
  * the FPGA logic has been configured to use the Configuratin Clock (CCLK)
  * for system configuration (rather than JTAGClk) and that the BootRAM has
  * been loaded with a valid Bootloader.
  * The new Bitfile will become active after the next reboot.
  *
  * @param TTMCntrl     In: Handle to the TTMCntrl Connection
  * @param Firmware     In: Firmware
  * @param FirmwareSize In: Size of Firmware [Byte]
  * @param Verbose      In: Print Progress Messages to the Console
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_UpdateFPGA(TTMCntrl_t TTMCntrl,
                           uint8_t *Firmware, uint32_t FirmwareSize, bool_t Verbose);

/** @brief Reboot the TTM8000-Board (breaking the connection)
  *
  * Reboot() tells the TTM8000-Board to reboot itself.
  * Obviously this breaks the network connection to the board.
  * @note Warning: Reboot() does not work with the current hardware!
  *
  * @note  Reboot() is designed for AIT Internal Use Only!
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_Reboot(TTMCntrl_t TTMCntrl);

/**********************************************************************
 **********************************************************************/

/** @brief Configure the TTM8000 Board for a Measurement.
  *
  * ConfigMeasurement() configures the board for a new measurement
  * as defined in the fields of MeasureConfig. Note that ConfigMeasurement()
  * stops the previously running measurement (if any), but does not automatically
  * start the new measurement. It can not be used for 'on-the-fly' reconfiguration.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param MeasureConfig In: Description of the reqeusted Measurement Configuration
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_ConfigMeasurement(TTMCntrl_t TTMCntrl, TTMMeasConfig_t *MeasureConfig);

/** @brief Check if the GPX PLL is Locked
  *
  * If the 40MHz clock driving the acam GPX is unstable or not available
  * the PLL of the acam GPX does sometimes loose its lock, causing the
  * TTM module to fail. Use GetGPXPLLState() to check the stability of
  * the reference clock.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param PLLLocked Out: Is the GPX PLL Locked?
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetGPXPLLState(TTMCntrl_t TTMCntrl, bool_t *PLLLocked);

/** @brief Start a Measurement.
  *
  * StartMeasurement() starts the measurement that was set
  * up using TTMCntrlConfig::Measurement().
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param PermitAutoPulseGenStart In: Allow the use of the PulseGen to
  *        create a Start Pulse in continuous I-Mode.
  * @returns Error Code
  * @see StopMeasurement()
  */
FlexIOStatus LIBTTM_API TTMCntrl_StartMeasurement(TTMCntrl_t TTMCntrl, bool_t PermitAutoPulseGenStart);

/** @brief Stop the current Measurement.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  * @see StartMeasurement()
  */
FlexIOStatus LIBTTM_API TTMCntrl_StopMeasurement(TTMCntrl_t TTMCntrl);

/** @brief Pause the current Measurement.
  *
  * PauseMeasurement() disables all external signal input, however
  * the internal time measurement engine continues operation. This
  * is can be useful when running in continuous I-Mode when measurement
  * shall be paused, but the global time shall continue.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  * @see ResumeMeasurement()
  */
FlexIOStatus LIBTTM_API TTMCntrl_PauseMeasurement(TTMCntrl_t TTMCntrl);

/** @brief Resume the current Measurement.
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  * @see PauseMeasurement()
  */
FlexIOStatus LIBTTM_API TTMCntrl_ResumeMeasurement(TTMCntrl_t TTMCntrl);

/** @brief Send all data, that is stored on the TTM8000 Board to the Network.
  *
  * @note  FlushData() shall ONLY be called when the current Measurement
  * is stopped. It is not intended to speed up/enforce data transfer in
  * situations with low event rates. Use the DataIdleTimeout field in the
  * MeasureConfig structure to enforce periodic transfer of available data
  * even in situations with low event rates.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_FlushData(TTMCntrl_t TTMCntrl);

/** @brief Restart the Global Time for the current Measurement.
  *
  * The 60-bit Timestamp counter used for continuous I-Mode is reset.
  * This can be used to (extremely crudely!) synchronize the timers of
  * several TTM8000-boards.
  * @note The Counter will NOT actually be reset to 0. Rather it will
  * perform a modulo (256 * 77760) operation (bringing the counter
  * value into the range of (0..19906560). Thus there is a slack of
  * up to 1.6384ms (@ 82.3045ps/Tick)
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_RestartTime(TTMCntrl_t TTMCntrl);

/** @brief Reconfigure the active Trigger Edges on a TTM8000 Board.
  *
  * SetEnabledEdges() reconfigures the active edges of the current
  * measurement. This is done on-the-fly without interruption of the running
  * measurement or loss of data.
  * @note Even so a complete Measurement Configuration is passed to
  * SetEnabledEdges() only the enabled edges are reconfigured.
  * All other settings are expected to be valid, but are not updated!
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param MeasureConfig In: Description of the reqeusted Measurement Configuration
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetEnabledEdges(TTMCntrl_t TTMCntrl,
                                   TTMMeasConfig_t *MeasureConfig);

/** @brief Set the Threshold Voltage for the Start/Stop Signal Inputs and the External Clock.
  *
  * In order to accomodate various logic families the threshold voltage that
  * is used to distinguish 'low' from 'high' states on signal and clock inputs
  * is adjustable in the range of [-4096mV...+4095mV] on the TTM8000 Board.
  * The analog voltage is created using an 8-bit DAC (Digital-to-Analog Converter).
  * Thus there is a conversion resolution of 32mV (8192mV Range / 2^8 values).
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ChannelMask   In: Select Levels to Adjust (Bit 0..Start, Bits 1..8: Stop1..8, Bit 9..Clock)
  * @param MilliVolt     InOut: Input: Requested Threshold Voltage for External Signals [mV]\n
  *                             Output: Actual Threshhold Voltage for External Signals [mV]
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetTriggerLevel(TTMCntrl_t TTMCntrl,
                                   uint32_t ChannelMask, int32_t MilliVolt[10]);

/** @brief Get the Threshold Voltage for the Start/Stop Signal Inputs and the External Clock.
  *
  * In order to accomodate various logic families the threshold voltage that
  * is used to distinguish 'low' from 'high' states on signal and clock inputs
  * is adjustable in the range of [-4096mV...+4095mV] on the TTM8000 Board.
  * The analog voltage is created using an 8-bit DAC (Digital-to-Analog Converter).
  * Thus there is a conversion resolution of 32mV (8192mV Range / 2^8 values).
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param MilliVolt   Out: Threshold Voltage for Signal Inputs\n
  *        MilliVolt[0]  ..   Start Input [mV]\n
  *        MilliVolt[1..8] .. Stop1..8 Input [mV]\n
  *        MilliVolt[9]  ..   External Clock [mV]\n
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetTriggerLevel(TTMCntrl_t TTMCntrl, int32_t MilliVolt[10]);

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
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ChannelMask    In: Select Channel Delays to Adjust (Bit 0..Start, Bits 1..8: Stop1..8)
  * @param ChannelDelay   In: Channel Delays\n
  *        ChannelDelay[0]  ..   Unused\n
  *        ChannelDelay[1..8] .. Channel Delay for Stop1..8 [Ticks]\n
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetChannelDelay(TTMCntrl_t TTMCntrl,
                                   uint32_t ChannelMask, int ChannelDelay[9]);

/** @brief Get the Channel Delay for Stop Signal Inputs
  *
  * Get the Channel Delays for all Stop Signals
  *
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ChannelDelay   Out: Channel Delays\n
  *        ChannelDelay[0]  ..   Unused (const. 0)\n
  *        ChannelDelay[1..8] .. Channel Delay for Stop1..8 [Ticks]\n
  * @returns Error Code
  * @see SetChannelDelay()
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetChannelDelay(TTMCntrl_t TTMCntrl,
                                   int ChannelDelay[9]);

/** @brief Get the Number of Events recorded for each Channel
  *
  * This is a conveniance function that provides the number of events
  * that was recorded on each channel since the beginning of time. It
  * would be easy to compute this information from the stream of timetags,
  * however it is delivered here as preprocessed information.
  *
  * @param TTMCntrl In:  Handle to the TTMCntrl Connection\n
  * @param EventCnt Out: Number of Events for each Channel\n
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetEventCnt(TTMCntrl_t TTMCntrl, TTMEventCnt_t *EventCnt);

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
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ExternClkConfig In: Configuration of the External Clock Input/Output
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetExternClockConfig(TTMCntrl_t TTMCntrl,
                                const TTMExternClkConfig_t *ExternClkConfig);

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
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ExternClkConfig Out: Configuration of the External Clock Input/Output
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetExternClockConfig(TTMCntrl_t TTMCntrl,
                                TTMExternClkConfig_t *ExternClkConfig);

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
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param ExternClkConfig In: Configuration of the External Clock Input/Output
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_UpdateExternClockConfig(TTMCntrl_t TTMCntrl,
                                const TTMExternClkConfig_t *ExternClkConfig);

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
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param QueryCnt In: How often shall the clocks be observed.
  * @param ClkState Out: State of the Internal and External Clock
  * @returns Error Code
  */
FlexIOStatus TTMCntrl_GetClockSourceState(TTMCntrl_t TTMCntrl,
                               int QueryCnt, TTMClockSourceState_t *ClkState);

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
  * SetStartSource() to adjust the start source for a running measurement.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param Enable   In: Enable/Disable Pulse Generator
  * @param FrameCnt In: Number of Frames to Generate
  * @param FrameLen In: Duration of a Frame [10ns]
  * @param StopPulsePos    In: Delay between the Start-of-Frame and the first Stop-Pulse [10ns]
  * @param BurstPulseCnt   In: Number of Stop Pulses within a Frame
  * @param BurstPulseDelay In: Delay between the rising-edge of one Stop-Pulse and the
  *        rising-edge of the next Stop-Pulse within a burst of Stop-Pulses. [10ns]
  * @param SendFirstStartPulse In: Generate a Start-Pulse for the first Frame?
  * @param SendNextStartPulse  In: Generate a Start-Pulse for the next (i.e. non-first) Frame?
  * @returns Error Code
  * @see GetPulseGenProgress(), SetStartSource(), GetStartSource()
  */
FlexIOStatus LIBTTM_API TTMCntrl_ConfigPulseGen(TTMCntrl_t TTMCntrl, bool_t Enable,
                            uint32_t FrameCnt, uint32_t FrameLen, uint32_t StopPulsePos,
                            uint16_t BurstPulseCnt, uint16_t BurstPulseDelay,
                            bool_t SendFirstStartPulse, bool_t SendNextStartPulse);

/** @brief Check the Progress of the Pulse Generator.
  *
  * If we want to wait until the Pulse Generator has finished its sequence,
  * it is useful to check its progress (e.g. to display a progress bar to the
  * user).
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
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
FlexIOStatus LIBTTM_API TTMCntrl_GetPulseGenProgress(TTMCntrl_t TTMCntrl,
                                   bool_t *Enabled, bool_t *Done, uint32_t *FramesToGo);

/** @brief Set the Signal Source for Start
  *
  * The internal Start signal of the TTM8000 Board can be driven by
  * either a signal from the external Start connector or by the
  * Pulse Generator built into the TTM8000 Board. SetStartSource()
  * selects the signal source for Start.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param UsePulseGenStart In: Use the Pulse Generator (TRUE) or External Connector
  *        (FALSE) to drive the Start signal
  * @returns Error Code
  * @see ConfigPulseGen(), GetPulseGenProgress(), GetStartSource()
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetStartSource(TTMCntrl_t TTMCntrl, bool_t UsePulseGenStart);

/** @brief Get the Signal Source for Start
  *
  * The internal Start signal of the TTM8000 Board can be driven by
  * either a signal from the external Start connector or by the
  * Pulse Generator built into the TTM8000 Board. SetStartSource()
  * selects the signal source for Start.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param UsePulseGenStart Out: Use the Pulse Generator (TRUE) or External Connector
  *        (FALSE) to drive the Start signal
  *        Pass NULL if you do not require this information.
  * @returns Error Code
  * @see ConfigPulseGen(), GetPulseGenProgress(), SetStartSource()
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetStartSource(TTMCntrl_t TTMCntrl, bool_t *UsePulseGenStart);

/** @brief Reconfigure the User Data.
  *
  * SetUserData() set the User Data field. This is done on-the-fly without
  * interruption of the running measurement or loss of data.\n
  * The UserData field will be copied unchanged to each Timetag Data packet
  * and can be used to pass signaling information from the control application
  * to the data processing application.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param UserData In: The User Data
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetUserData(TTMCntrl_t TTMCntrl, uint16_t UserData);

/** @brief Set the State of the DigitalIO Pins
  *
  * The TTM8000 Board has an 8-bit wide DigitalIO port using 3.3V TTL logic.
  * Each bit is individually configurable as input or output.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param DataOut  In: Bitmask of Output Data
  * @param DataDir  In: Bitmask of Data Direction (0..Out, 1..In)
  * @returns Error Code
  * @see TTMCntrlGetUserIO
  */
FlexIOStatus LIBTTM_API TTMCntrl_SetUserIO(TTMCntrl_t TTMCntrl,
                                   uint8_t DataOut, uint8_t DataDir);

/** @brief Get the State of the DigitalIO Pins
  *
  * The TTM8000 Board has an 8-bit wide DigitalIO port using 3.3V TTL logic.
  * Each bit is individually configurable as input or output.
  * @param TTMCntrl In: Handle to the TTMCntrl Connection
  * @param DataOut Out: Bitmask of Output Data
  *        Pass NULL if you do not require this information.
  * @param DataDir Out: Bitmask of Data Direction (0..Out, 1..In)
  *        Pass NULL if you do not require this information.
  * @param DataIn  Out: Bitmask of Current State of IOPort
  *        Pass NULL if you do not require this information.
  * @returns Error Code
  * @see TTMCntrlSetUserIO
  */
FlexIOStatus LIBTTM_API TTMCntrl_GetUserIO(TTMCntrl_t TTMCntrl,
                   uint8_t *DataIn, uint8_t *DataOut, uint8_t *DataDir);

/** @brief Enable the External UART Link
  *
  * The DigitalIO pins GPIO0 and GPIO1 of the TTM8000 can be used as
  * UART link to control external devices.
  *
  * The UART will operate at 115200 baud, using 8 bits/word, no parity
  * and one stop bit.
  * It shall be noted that these are still regular GPIO Pins that work
  * with 3.3V TTL levels. They are NOT 12V safe!!! Thus you must NEVER
  * directly connect GPIO pins to a standard RS232 signal. If you need
  * to connect a RS232 compliant signal, you must use a RS232 transceiver
  * (e.g. Maxim MAX3221) to avoid permanent damage to the TTM8000.
  *
  * UART requires two data pins (+ Ground). GPIO0 is used for the RX pin
  * (data travelling from the external device to the TTM8000) and GPIO1
  * is used as TX pin (data travelling from the TTM8000 to the external
  * device). When the UART is enabled, these pins are dedicated as UART
  * pins and can not be used as general purpose IO pins.
  *
  * If the external UART and the external SPI are both active, the UART
  * takes control over pins GPIO0/1 and SPI has control over the other
  * 6 GPIO Pins, thus reducing the number of Slave Select pins of the
  * SPI from 5 to 3.
  *
  * Pin Usage:
  *   GPIO1: UART TX
  *   GPIO0: UART RX
  *
  * @param TTMCntrl   In: Handle to the TTMCntrl Connection
  * @param Enable In: Enable external UART
  * @returns Error Code
  * @see TTMCntrl_ExecExtUART()
  */
FlexIOStatus TTMCntrl_EnableExtUART(TTMCntrl_t TTMCntrl, bool_t Enable);
/* <<< AIT INTERNAL USE ONLY!!! <<< */

/**********************************************************************
 **********************************************************************
 **                                                                  **
 **                             TTMData                              **
 **                                                                  **
 **********************************************************************
 **********************************************************************/

/** @brief Data Connection between a TTM8000 Board and the local host.
  *
  * FIXME - More Documentation required here - FIXME
  *
  * Technical Note: Internally a TTMData_t is a *TTMData_c, that
  * corresponds to the 'this' pointer used in C++ class-method calls.
  * However since this header can be used in plain C code, that does
  * not understand C++ classes, we can not explicitly use TTMData_c
  * here. We could define TTMData_t as a void*, however this would
  * not be type-safe, since a void** is automatically converted to a
  * void* as needed by the compiler and mistakes caused by using a
  * *TTMData_t when a TTMData_t would be needed would not be caught.
  * Thus we shall use a simple struct, that looks a bit oversized but
  * makes things typesafe (and does not incur any cost in code size
  * or performance).
  */
typedef struct {
  void *OpaqueTTMDataHnd; //!< Pointer to TTMData_c masqueraded for use with plain C
 } TTMData_t;
// typedef void* TTMData_t;

/** @brief Create a new TTMData Connection.
  *
  * @note Every TTMData Connection that was opened with Connect() must be
  *       closed with a corresponding call to Disconnect()!
  * @param TTMData       Out: Handle to the TTMData Connection
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
FlexIOStatus LIBTTM_API TTMData_Connect(TTMData_t *TTMData,
                        in_addr_t BoardIPAddr, in_port_t BoardDataPort,
                        in_addr_t LocalIPAddr, in_port_t LocalDataPort,
                        uint32_t  SocketBufferSize,
                        SOCKET DataSocketIn);

/** @brief Check if a Data Connection has been established
  *
  * TTMData_IsConnected() checks is a Data Connection has been established.
  * Note that TTMData_IsConnected() only checks if TTMData_ Connect() has been called.
  * We can not know if our data source is still alive and willing to send data to us.
  *
  * @param TTMData In: Handle to the TTMData Connection
  * @returns Connection Status
  */
bool_t LIBTTM_API TTMData_IsConnected(TTMData_t TTMData);

/** @brief Dispose of a Data Connection to a TTM8000 Board.
  *
  * Disconnect closes a TTMData Connection.
  * @param TTMData In: Handle to the TTMData Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_Disconnect(TTMData_t TTMData);


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
  * @param TTMData In: Handle to the TTMData Connection
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_SendDummyPacket(TTMData_t TTMData);

/** @brief Send a dummy Network Packet from the PC to the TTM8000 Data Port
  *
  * Some users can not use TTMData_c::Connect() to build the network
  * connection. To make it possible to punch a hole into the firewall
  * even for these users, we shall offer SendDummyPacket as a static
  * function too. Applications that use TTMData_c objects and member
  * functions don't need this static function.
  *
  * @param DataSocket In: The Socket to be used for sending the Network Packet
  * @param BoardIPAddr   In: Board IP-Address (Host Byte Order)
  * @param BoardDataPort In: UDP-Port for the Data Port of the Board (Host Byte Order)
  * @returns Error Code
  *
  * @see SendDummyPacket()
  */
FlexIOStatus LIBTTM_API TTMData_SendDummyPacketToSocket(SOCKET DataSocket, in_addr_t BoardIPAddr, in_port_t BoardDataPort);
/** @brief Get the BSD socket used to receive data.
  *
  * GetSocket() provides access to the handle of the BSD-style socket
  * used for the TTMData connection. You can use it if your application has
  * special requirements (e.g. configuration settings) that can not be addressed
  * otherwise. However you should remember that the socket belongs to TTMLib
  * and must not be closed etc.
  * @note Most applications will NOT need to use GetSocket().
  * @param TTMData In: Handle to the TTMData Connection
  * @param DataSocket Out: Socket Handle
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_GetSocket(TTMData_t TTMData, SOCKET *DataSocket);

/** @brief Get the IP-Address/UDP Port used to receive data.
  *
  * GetSocketAddr() provides the IP-Address/UDP Port of the socket
  * used for the TTMData connection.
  * This might be useful for status displays/debugging however most applications
  * will probably never need to call GetSocketAddr().
  * @param TTMData In: Handle to the TTMData Connection
  * @param LocalIPAddr Out: IP-Address of the Data-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @param LocalDataPort Out: UDP Port of the Data-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_GetSocketAddr(TTMData_t TTMData,
                           in_addr_t *LocalIPAddr, in_port_t *LocalDataPort);

/** @brief Set Network Buffer Size of the Recv. Socket
  *
  * SetSocketBufferSize() sets the buffer size of the Timetag Receivce
  * Socket at the Local Host.\n
  * @note Running at full speed, the TTM8000 Board can generate up to 80MByte/s of
  *  Timetags. Since the lost host will (probably) run a interactive multitasking
  *  operating system, we can make no guarantees about sceduling and it is quite
  *  likely that the data receiving application is not sceduled for several 100ms
  *  at a time. In order to avoid packet loss during such times, we must make sure
  *  that the network input buffer is sufficiently big. It is recommended to make
  *  the buffer at least 8MByte or better 32MByte large.
  * @param TTMData In: Handle to the TTMData Connection
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
FlexIOStatus LIBTTM_API TTMData_SetSocketBufferSize(TTMData_t TTMData, uint32_t BufferSize);

/** @brief Get Network Buffer Size of the Recv. Socket
  *
  * GetSocketBufferSize() gets the buffer size of the Timetag Receivce
  * Socket at the Local Host.\n
  * @note Running at full speed, the TTM8000 Board can generate up to 80MByte/s of
  *  Timetags. Since the lost host will (probably) run a interactive multitasking
  *  operating system, we can make no guarantees about sceduling and it is quite
  *  likely that the data receiving application is not sceduled for several 100ms
  *  at a time. In order to avoid packet loss during such times, we must make sure
  *  that the network input buffer is sufficiently big. It is recommended to make
  *  the buffer at least 8MByte or better 32MByte large.
  * @param TTMData In: Handle to the TTMData Connection
  * @param BufferSize Out: Actual Network Buffer Size [Byte] - Note that the
  *  operating system will generally round up the BufferSize when setting it.
  *  Thus it is likely that GetSocketBufferSize() does NOT exactly report the
  *  value requested when calling by SetSocketBufferSize() but a significantly
  *  higher value (a factor of 2 is quite common!).
  * @returns Error Code
  * @see SetSocketBufferSize()
  */
FlexIOStatus LIBTTM_API TTMData_GetSocketBufferSize(TTMData_t TTMData, uint32_t *BufferSize);

/** @brief Check if Timetag Data Packets are available
  *
  * @param TTMData In: Handle to the TTMData Connection
  * @param DataAvailable Out: Data is available (TRUE) or not available (FALSE)
  * @param MilliTimeout   In: If no data is immediatly available, how long do we want
  *        to wait for data to arrive before we concede failure. [ms]
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_DataAvailable(TTMData_t TTMData,
                                   bool_t *DataAvailable, uint32_t MilliTimeout);

/** @brief Fetch a Packet of Timetag Data Packets
  *
  * @note  FetchData() already takes care of all endian issues for both the
  *        packet header and the packet data. You MUST NOT call TimeTagPacketNetToHost()
  *        for packets received by TTMDataFetchData().
  * @param TTMData In: Handle to the TTMData Connection
  * @param TimetagBuffer Out: Received Timetag Data Packet
  * @param MilliTimeout   In: If no data is immediatly available, how long do we want
  *        to wait for data to arrive before we concede failure. [ms]
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_FetchData(TTMData_t TTMData,
                           TTMDataPacket_t *TimetagBuffer, uint32_t MilliTimeout);

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
FlexIOStatus LIBTTM_API TTMData_TimeTagPacketNetToHost(TTMDataPacket_t *TimetagBuffer);

/** @brief Adjust Byte Order of Timetag Data Packets
  *
  * TimeTagPacketHostToNet() converts the Header of a TTMDataPacketfrom
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
FlexIOStatus LIBTTM_API TTMData_TimeTagPacketHostToNet(TTMDataPacket_t *TimetagBuffer);

/** @brief Magic Constant indicating that no High Packet has arrived */
#define TTMData_TimeHighUndefined (1LL << 60)

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
  *        Use TTMData_TimeHighUndefined if there has not been a High Bit field, and
  *        any other value if a High bit field has come by.
  * @param MaxEventCnt   In: How many events do we want at most. If there are more than
  *        MaxEventCnt events in the packet, the additional events will be discarded. If
  *        there are less events, the packet will be unchanged.
  * @param EventCnt     Out: How many events are in the packet after clipping.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_TimeTagClipPacket(TTMDataPacket_t *TimetagBuffer,
                                   uint64_t *TimeHigh, int MaxEventCnt, int *EventCnt);

/** @brief Expand Timetag Data from IMode_Ext64_Packed Format to IMode_Ext64_Flat Format
  *
  * Handling 60 Bits of Time information for each Timetag provides extended protection
  * against overflow, however it also requires 8 Byte/Timetag to be handled. Since the
  * high bits of the 60 Bit Time information change only rarely, there is a packed data
  * format that transmits these high bits only when they change and manages to encode
  * most Timetags in just 4 Byte/Timetag (reducing network bandwidth and storage requirement
  * by 50%). ExpandData() converts Timetag Data in this packed format to flat,
  * @note  The Source Data MUST be in IMode_Ext64_Packed Format. All other formats
  *  generate an error of TTM_InvalidPacketFormat.
  * @param Dest Out: Destination Timetag Data Packet
  * @param Src   In: Source Timetag Data Packet - It is safe to use
  *        the same area of memory for Src and Dest!
  * @param TimeHigh In/Out: Most recently transmitted field of High Bits
  *        Initialize with TTMData_TimeHighUndefined before the first call to TTMData_ExpandData(),
  *        then reuse the same variable for each further call to TTMData_ExpandData().
  * @returns Error Code
  *
  * @see TTMData_CompressData()
  */
FlexIOStatus LIBTTM_API TTMData_ExpandData(TTMDataPacket_t *Dest,
                                   const TTMDataPacket_t *Src, uint64_t *TimeHigh);

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
  *        Initialize with TimeHighUndefined before the first call to TTMData_CompressData(),
  *        then reuse the same variable for each further call to TTMData_CompressData().
  * @returns Error Code
  *
  * @see TTMData_ExpandData()
  */
FlexIOStatus LIBTTM_API TTMData_CompressData(TTMDataPacket_t *Dest,
                                      const TTMDataPacket_t *Src, uint64_t *TimeHigh);


 /** @brief Eliminate Duplicate Events from IMode_Ext64_Flat Format Data
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
FlexIOStatus LIBTTM_API TTMData_RemoveDuplicateEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src,
                                        uint64_t PrevEvent[16], const uint64_t DeadTime[16],
                                        bool_t ExtendDeadTimeOnRetrigger);

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
  * TTMData_TimeShiftEvents() will add a channel- and edge-dependent Offset to every timetag
  * to align all Timetags according to the delays of their sources.
  *
  * Note: TTMData_TimeShiftEvents() will only work for Data in TTFormat_IMode_EXT64_FLAT format.
  *
  * Note: Timetag Data that comes from the TTM8000 module is chronologically sorted
  * according to the time the events were detected by the TTM8000. After adding an
  * offset the data will generally no longer be chronologically sorted. If you need
  * sorted data, you should use TTMData_SortEvents() (and friends).
  *
  * @param Dest Out: Destination Timetag Data Packet
  * @param Src   In: Source Timetag Data Packet - It is safe to use
  *        the same area of memory for Src and Dest!
  * @param OffsetTicks In: Number of ticks to add to each measurement result (by channel/edge)
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMData_TimeShiftEvents(TTMDataPacket_t *Dest, const TTMDataPacket_t *Src,
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
FlexIOStatus LIBTTM_API TTMData_ChannelExchange(TTMDataPacket_t *Dest,
                        const TTMDataPacket_t *Src, int16_t ExchangeRule[256]);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
/** @brief Handle to a TTMEvtSort Object.
  *
  */
typedef struct {
  void *OpaqueTTMEvtSortHnd; //!< Pointer to TTMEvtSort_c masqueraded for use with plain C
 } TTMEvtSort_t;

/** @brief Create a Timetag Buffer for Sorting a Stream of Timetag Event Data
  *
  * For a description of TTMEvtSort_Create() please see the entry for
  * TTMData_SortEvents().
  *
  * Remember to call TTMEvtSort_Dispose() when you are done using the SortBuffer!
  *
  * @param TTMEvtSort     Out: Handle to a Initialised SortBuffer for sorting
  * @returns Error Code
  *
  * @see TTMEvtSort_SortEvents()
  */
FlexIOStatus LIBTTM_API TTMEvtSort_Create(TTMEvtSort_t *TTMEvtSort);

/** @brief Dispose of a Timetag Buffer for Sorting a Stream of Timetag Event Data
  *
  * For a description of TTMEvtSort_Dispose() please see the entry for
  * TTMData_SortEvents().
  *
  * @param TTMEvtSort     In: Handle to the SortBuffer to be disposed
  * @returns Error Code
  *
  * @see TTMEvtSort_SortEvents()
  */
FlexIOStatus LIBTTM_API TTMEvtSort_Dispose(TTMEvtSort_t TTMEvtSort);

/** @brief Define the max. amount of Shuffling in a Stream of Timetag Event Data
  *
  * For a description of TTMEvtSort_SetMaxShuffleTicks() please see the entry for
  * TTMData_SortEvents().
  *
  * Since we are processing an infinite stream of Timetag Event Data, we
  * need some criterion to recognize that a given timetag can no longer
  * be preceeded by any timetag to come in future event packets. We shall
  * therefore define MaxShuffleTicks to denote the maximal amount of
  * missorting in the original stream of events. Any event that is older
  * than the most recent event by at least MaxShuffleTicks ticks, can
  * no longer be overtaken by any future event. It is therefore safe to
  * commit these events to the output stream.
  *
  * @param TTMEvtSort     In: Handle to the SortBuffer
  * @param MaxShuffleTicks In: Max. Number of Ticks an event can be missorted.
  * @returns Error Code
  *
  * @see TTMEvtSort_SortEvents()
  */
FlexIOStatus LIBTTM_API TTMEvtSort_SetMaxShuffleTicks(TTMEvtSort_t TTMEvtSort, uint32_t MaxShuffleTicks);

/** @brief Sort a Stream of Timetag Event Data
  *
  * Timetag Data that comes directly from the TTM8000 module is
  * chonologically sorted. However if you add offsets to each timetag
  * to compensate for channel-dependent delays in the event creation
  * system (e.g. cable delays) using TimeShiftEvents() the events are
  * no longer sorted. SortEvents() can be used to obtain a stream of
  * sorted timetags again.
  *
  * Note that TTMData_SortEvents() sorts a (conceptually infinite) stream
  * of timetags, not just a finite array of timetags. Thus it needs to
  * buffer where it can store timetags that might be preceeded by timetags
  * that arrive in laters packets.
  * Also note that due to this buffering the number of timetag returned
  * by a single call of TTMData_SortEvents() will generally not be the same as
  * the number of timetags provided for this call. (Seen for the entire
  * streams the numbers of provided and returned timetags will of course
  * match). Be sure that the destination buffer can hold at least 2048
  * samples more than provided by the source buffer.
  *
  * Note: TTMEvtSort_SortEvents() will only work for Data in TTFormat_IMode_EXT64_FLAT format.
  *
  * SortEvents() is the central core of a set of functions:
  *   .) Call TTMEvtSort_Create() once to create a SortBuffer
  *   .) Call TTMEvtSort_SetMaxShuffleTicks() to tell the SortBuffer much
  *      earlier (in Ticks) than the latest processed timetag any future
  *      timetag can be.
  *   .) Call TTMEvtSort_SortEvents() once for each TTMDataPacket to do the
  *      the actual sorting
  *   .) Call TTMEvtSort_Flush() to retrieve all timetags still
  *      stored in the SortBuffer when no fresh timetags are available.
  *   .) Dall TTMEvtSort_Dispose() to release the SortBuffer
  *
  * @param TTMEvtSort In: SortBuffer obtained with TTMEvtSort_Create()
  * @param Dest Out: Destination Timetag Data Packet
  * @param Src   In: Source Timetag Data Packet - It is safe to use
  *        the same area of memory for Src and Dest, however performance
  *        is better if separate memorys are used!
  * @returns Error Code
  *
  * @see TTMEvtSort_Create(), TTMEvtSort_Dispose(),
  *      TTMEvtSort_SetMaxShuffleTicks(), TTMEvtSort_Flush()
  */
FlexIOStatus LIBTTM_API TTMEvtSort_SortEvents(TTMEvtSort_t TTMEvtSort,
                     TTMDataPacket_t *Dest, const TTMDataPacket_t *Src);

/** @brief Obtain remaining Events from the Sorting Buffer
  *
  * For a description of TTMEvtSort_Flush() please see the entry for
  * TTMData_SortEvents().
  *
  * After processing each packet of timetag events, there are always some
  * events that can not be safely commited to the output stream at once,
  * because there is always a chance that they will be overtaken by events
  * in a later packet. Usually these events will be commited to the output
  * stream when the next packet is processed and no further user-action
  * is required. However for the last packet of a stream it is necessary
  * to flush the queue to obtain access to all packets.
  *
  * @param TTMEvtSort     In: Handle to the SortBuffer
  * @param Dest          Out: Remaining Timetags
  * @returns Error Code
  *
  * @see TTMEvtSort_SortEvents()
  */
FlexIOStatus LIBTTM_API TTMEvtSort_Flush(TTMEvtSort_t TTMEvtSort, TTMDataPacket_t *Dest);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** @brief Handle to a TTMSoloEvtFilter Object.
  *
  */
typedef struct {
  void *OpaqueTTMSoloEvtFilterHnd; //!< Pointer to TTMSoloEvtFilter_c masqueraded for use with plain C
 } TTMSoloEvtFilter_t;

/** @brief Create a Buffer for the Removal of Solitary Events from a Stream of Timetag Event Data
  *
  * For a description of TTMSoloEvtFilter_Create() please see the entry for
  * TTMSoloEvtFilter_FilterEvents().
  *
  * Remember to call TTMSoloEvtFilter_Dispose() when you are done using the SolitaryEventFilterBuffer!
  *
  * @param TTMSoloEvtFilter  Out: Handle to a Initialised SolitaryEventFilterBuffer for sorting
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_FilterEvents()
  */
FlexIOStatus LIBTTM_API TTMSoloEvtFilter_Create(TTMSoloEvtFilter_t *TTMSoloEvtFilter);

/** @brief Dispose of a Solitary Event Filter Buffer
  *
  * For a description of TTMSoloEvtFilter_Dispose() please see the entry for
  * TTMSoloEvtFilter_FilterEvents().
  *
  * @param TTMSoloEvtFilter   In: Handle to the SolitaryEventFilterBuffer to be disposed
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_FilterEvents()
  */
FlexIOStatus LIBTTM_API TTMSoloEvtFilter_Dispose(TTMSoloEvtFilter_t TTMSoloEvtFilter);

/** @brief Define the maximal acceptable distance between two neighbors
  *
  * For a description of TTMSoloEvtFilter_SetMaxDeltaTicks() please see the entry for
  * TTMSoloEvtFilter_FilterEvents().
  *
  * Note: TTMSoloEvtFilter_SetMaxDeltaTicks() is just a shorthand for
  *   TTMSoloEvtFilter_SetMaxDeltaTicks(MaxChnDeltaTicks, true) where all elements of
  *   MaxChnDeltaTicks[] are initialised with MaxDeltaTicks.
  *
  * @param TTMSoloEvtFilter  In: Handle to the SolitaryEventFilterBuffer
  * @param MaxDeltaTicks  In: Maximal number of Ticks (12.15GHz) that two events can
  *                           be separated and still be considered neighbors.
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_FilterEvents(), TTMSoloEvtFilter_SetMaxChnDeltaTicks()
  */
FlexIOStatus LIBTTM_API TTMSoloEvtFilter_SetMaxDeltaTicks(TTMSoloEvtFilter_t TTMSoloEvtFilter,
                                               uint32_t MaxDeltaTicks);

/** @brief Define the maximal acceptable distance between two neighbors
  *
  * For a description of TTMSoloEvtFilter_SetMaxChnDeltaTicks() please see the entry for
  * TTMSoloEvtFilter_FilterEvents().
  *
  * We want to remove events that do not have neighbors. Thus we need to
  * define what a neighbor is. Two events A and B are neighbors of each other, iff
  * .) TimeStamp[B] > TimeStamp[A]
  * .) (TimeStamp[B] - TimeStamp[A]) <= MaxChnDeltaTicks[Chn[A]][Chn[B]].
  *
  * Note: That MaxChnDeltaTicks[X][Y] need not be the same as MaxChnDeltaTicks[Y][X]!
  * Thus we can have trigger events that allow other events to pass for quite some
  * time after they occured but not before.
  *
  * There is an option for faster processing: If we know that every event will be
  * neighbor of at most one other event on every channel, we need only look at
  * the most recent events of every channel when looking for neighbors. If we have
  * no such guarantee, and have long MaxChnDeltaTicks[] we might need to look at
  * a lot of events to be sure to discover all neighbor relations.
  *
  * @param TTMSoloEvtFilter  In: Handle to the SolitaryEventFilterBuffer
  * @param MaxChnDeltaTicks  In: Maximal number of Ticks (12.15GHz) that two events can
  *                           be separated and still be considered neighbors.
  * @param PerformDeepSearch In: Do we need to look at the full history of
  *                events to find a neighbor relations (true) or just at
  *                the last event from every channel (false)?
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_FilterEvents()
  */
FlexIOStatus LIBTTM_API TTMSoloEvtFilter_SetMaxChnDeltaTicks(TTMSoloEvtFilter_t TTMSoloEvtFilter,
                                               uint32_t MaxChnDeltaTicks[16][16], bool_t PerformDeepSearch);

/** @brief Delete all solitary Timetags from a Stream of Timetag Events
  *
  * Sometimes we are only interested in groups of events that occurr
  * at (approximately) the same time. Solitary events, that do not
  * have a neighbor within a given amount of time are of no interest.
  * TTMSoloEvtFilter_FilterEvents() will scan a stream of Timetags and will
  * remove all timetags that have no neighbor within a specified
  * interval.
  *
  * Neighbors can either be defined independent of the channel or a specific
  * acceptable distance can be defined for each combination of two channels.
  *
  * TTMSoloEvtFilter_FilterEvents() will only work correctly if the timetags of
  * the stream are chronologically sorted.
  *
  * Note: TTMSoloEvtFilter_FilterEvents() will only work for Data in
  * TTFormat_IMode_EXT64_FLAT format.
  *
  * @param TTMSoloEvtFilter   In: Handle to the SolitaryEventFilterBuffer
  * @param Dest Out: Destination Timetag Data Packet
  * @param Src   In: Source Timetag Data Packet - It is safe to use
  *        the same area of memory for Src and Dest!
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_Create(), TTMSoloEvtFilter_Dispose(),
  *      TTMSoloEvtFilter_SetMaxDeltaTicks(), TTMSoloEvtFilter_SetMaxChnDeltaTicks()
  *      TTMSoloEvtFilter_Flush()
  */
 FlexIOStatus LIBTTM_API TTMSoloEvtFilter_FilterEvents(TTMSoloEvtFilter_t TTMSoloEvtFilter,
                     TTMDataPacket_t *Dest, const TTMDataPacket_t *Src);

/** @brief Obtain remaining Events from the Solitary Event Filter Buffer
  *
  * For a description of TTMSoloEvtFilter_Flush() please see the entry for
  * TTMSoloEvtFilter_FilterEvents().
  *
  * After processing each packet of timetag events, there are always some
  * events that can not be safely commited to the output stream at once,
  * because there is always a chance that they will be needed as partners
  * for future events or will and cannot be discarded because a future
  * event might become their partner. Usually these events will be commited
  * to the output stream or discarded when the next packet is processed
  * and no further user-action is required. However for the last packet
  * of a stream it is necessary to flush the queue to correctly process
  * the final elements
  *
  * @param TTMSoloEvtFilter   In: Handle to the SolitaryEventFilterBuffer
  * @param Dest              Out: Remaining Timetags
  * @returns Error Code
  *
  * @see TTMSoloEvtFilter_FilterEvents()
  */
FlexIOStatus LIBTTM_API TTMSoloEvtFilter_Flush(TTMSoloEvtFilter_t TTMSoloEvtFilter, TTMDataPacket_t *Dest);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

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
  * Technical Note: Internally a TTMEvtCnt_t is a *TTMEvtCnt_c, that
  * corresponds to the 'this' pointer used in C++ class-method calls.
  * However since this header can be used in plain C code, that does
  * not understand C++ classes, we can not explicitly use TTMEvtCnt_c
  * here. We could define TTMEvtCnt_t as a void*, however this would
  * not be type-safe, since a void** is automatically converted to a
  * void* as needed by the compiler and mistakes caused by using a
  * *TTMEvtCnt_t when a TTMEvtCnt_t would be needed would not be caught.
  * Thus we shall use a simple struct, that looks a bit oversized but
  * makes things typesafe (and does not incur any cost in code size
  * or performance).
  */
typedef struct {
  void *OpaqueTTMEvtCntHnd; //!< Pointer to TTMEvtCnt_c masqueraded for use with plain C
 } TTMEvtCnt_t;
/* typedef void* TTMEvtCnt_t; */

/** @brief Create a new Event Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @note Every EventCounter that was created with TTMEventCnt_Create() must be
  *       disposed with a corresponding call to TTMEventCnt_Dispose()!
  * @param TTMEvtCnt      Out: Handle to the new TTMEvtCnt EventCounter
  * @returns Error Code
  * @see TTMEventCnt_Dispose()
  */
FlexIOStatus LIBTTM_API TTMEventCnt_Create(TTMEvtCnt_t *TTMEvtCnt);

/** @brief Dispose of an Event Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMEvtCnt  In: Handle to the EventCounter to dispose of
  * @returns Error Code
  * @see TTMEventCnt_Create()
  */
FlexIOStatus LIBTTM_API TTMEventCnt_Dispose(TTMEvtCnt_t TTMEvtCnt);

/** @brief Reset all counters of an Event Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @param SlotMilliDuration In: Duration of a Timeslot in Milliseconds
  * @returns Error Code
  * @see TTMEvtCnt_t()
  */
FlexIOStatus LIBTTM_API TTMEventCnt_Reset(TTMEvtCnt_t TTMEvtCnt, uint32_t SlotMilliDuration);

/** @brief Process Timetag Events / Count Events
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @param DataPacket In: Timetags to Process
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_ProcessEvents(TTMEvtCnt_t TTMEvtCnt, const TTMDataPacket_t *DataPacket);

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
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_ForceNewSlot(TTMEvtCnt_t TTMEvtCnt);

/** @brief Get the Timestamp of the most recent Event
  *
  * When using Continous I-Mode TTMEventCnt automatically records the
  * most recent processed timestamp.
  * When using other (non-I-Mode-continous) modes, there is no concept
  * of global time and GetCurrTime() always returns 0.
  *
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @param CurrTime  Out: Current Time (in I-Mode Ticks @ 82.3045ps)
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_GetCurrTime(TTMEvtCnt_t TTMEvtCnt, uint64_t *CurrTime);

/** @brief Get the number of Timeslots processed
  *
  * When using Continous I-Mode TTMEventCnt automatically starts a new
  * timeslot after the timetag clock has ticked for SlotMilliDuration
  * milliseconds. In all other modes a new timeslot is started by calling
  * ForceNewSlot() manually.
  *
  * TTMEventCnt_GetSlotCnt() fetches the number of Timeslots that have
  * been completed. It can be used to determine if it is worth looking a
  * new frequency measurements.
  *
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @param SlotCnt   Out: Number of Slots
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_GetSlotCnt(TTMEvtCnt_t TTMEvtCnt, uint32_t *SlotCnt);

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
  * @param TTMEvtCnt  In: Handle to the EventCounter
  * @param EventCnt  Out: Number of Timetagging Events/Channel
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_GetCounter(TTMEvtCnt_t TTMEvtCnt, uint64_t EventCnt[16]);

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
  * @param TTMEvtCnt   In: Handle to the EventCounter
  * @param TimeSlotCnt In: How many Timeslots form one second?
  * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
  * @param Freq       Out: Average (NOT Total!) Number of Timetagging Events/Channel
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMEventCnt_GetFrequency(TTMEvtCnt_t TTMEvtCnt,
                                    int TimeSlotCnt, int AvgSlotCnt, uint32_t Freq[16]);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** @brief An Event and Correlation Counter for Timetagging Events from an TTM8000 Board
  *
  * An EventCounter is an object that processes Timetags and counts how
  * often each type of event occurs. Furthermore, if the Timetags are
  * measured in continuous I-Mode, TTMCorrEvtCnt will also count
  * correlations between two or more events. A correlation is detected
  * if all selected events occur within a small time window. The width
  * of this time window can be selected. Furthermore a fixed, channel
  * dependent time offset can be added to the events from each channel
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
  *
  * Technical Note: Internally a TTMCorrEvtCnt_t is a *TTMCorrelationCnt_c, that
  * corresponds to the 'this' pointer used in C++ class-method calls.
  * However since this header can be used in plain C code, that does
  * not understand C++ classes, we can not explicitly use TTMCorrelationCnt_c
  * here. We could define TTMCorrEvtCnt_t as a void*, however this would
  * not be type-safe, since a void** is automatically converted to a
  * void* as needed by the compiler and mistakes caused by using a
  * *TTMCorrEvtCnt_t when a TTMCorrEvtCnt_t would be needed would not be caught.
  * Thus we shall use a simple struct, that looks a bit oversized but
  * makes things typesafe (and does not incur any cost in code size
  * or performance).
  */
typedef struct {
  void *OpaqueTTMCorrEvtCntHnd; //!< Pointer to TTMCorrelationCnt_c masqueraded for use with plain C
 } TTMCorrEvtCnt_t;
/* typedef void* TTMCorrEvtCnt_t; */

/** @brief Maximal number of combinations of Timetag types whose (Multi-)Correlations
  *        Events that can be counted
  *
  * Note: Assuming that that we shall only use the rising _or_ falling edge
  * of each Stop-input of the TTM8000 there are just 8 different signals that
  * can create timetags. Thus there are 2^8 = 256 possible different combinations
  * of these 8 events (including the NULL-event). Thus allowing 256 different
  * combinations should be more that sufficient ;)
  *
  * Note: This should match the C++ definition of TTMCorrelationCnt_c::MultiCorrMaxDefCnt
  */
#define CMaxMultiCorrDefCnt 256

/** @brief Create a new Event and Correlation Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @note Every EventCounter that was created with TTMCorrEvtCnt_Create() must be
  *       disposed with a corresponding call to TTMCorrEvtCnt_Dispose()!
  * @param TTMCorrEvtCnt      Out: Handle to the new TTMEvtCnt CorrEventCounter
  * @returns Error Code
  * @see TTMCorrEvtCnt_Dispose()
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_Create(TTMCorrEvtCnt_t *TTMCorrEvtCnt);

/** @brief Dispose of an Event and Correlation Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter to dispose of
  * @returns Error Code
  * @see TTMCorrEvtCnt_Create()
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_Dispose(TTMCorrEvtCnt_t TTMCorrEvtCnt);

/** @brief Reset all counters of an Event and Correlation Counter
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param SlotMilliDuration In: Duration of a Timeslot in Milliseconds
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_Reset(TTMCorrEvtCnt_t TTMCorrEvtCnt, uint32_t SlotMilliDuration);

/** @brief Configure the Timing of the Correlation Detectors
  *
  * A correlation is detected if all selected events occur within a
  * small time window. Before the logic checks for coincidence a fixed,
  * channel dependent time offset can be added to the events from each
  * channel to compensate for different cable length, delays in logic elements etc.
  *
  * TMCorrEvtCnt_ConfigTiming() can be used to define the delays that
  * are added to the events from each channel and the width of the time
  * window. In order to trigger an event the time difference between the
  * earliest event and the latest event of the event group must not exceed
  * the time window.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param ChannelPicoDelay In: Delay to be added to each event in Picoseconds.
  *         ChannelPicoDelay[0..7] correspond to the rising edges of Stop1..8.
  *         ChannelPicoDelay[8..15] correspond to the falling edges of Stop1..8.
  * @param WindowPicoWidth In: Width of the Correlation Time Window in Picoseconds.
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ConfigTiming(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                        int32_t ChannelPicoDelay[16], uint32_t WindowPicoWidth);

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
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
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
FlexIOStatus TTMCorrEvtCnt_ConfigTimingExt(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                        int32_t ChannelPicoDelay[16], uint32_t WindowPicoWidth[16]);

/** @brief Configure the Interesting Correlations
  *
  * Detecting Correlations is a fairly CPU-expensive operation, thus it
  * should only be enabled, if the correlations are actually required.
  * Once counting of correlations is enabled, we shall always count all
  * possible pairs of two events. In additions to that we shall count
  * up to CMaxMultiCorrDefCnt arbitrary combinations of any of the 16 possible events.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param EnableCorr In: Do we want to count correlations at all?
  * @param MultiCorrDef[][] In: Define up to CMaxMultiCorrDefCnt correlation patterns
  *    using arbitrary combinations of the events.
  * @param MultiCorrDefCnt In: How many of the CMaxMultiCorrDefCnt entries in MultiCorrDef
  *    are actually valid?
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ConfigCorr(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                  bool_t EnableCorr,
                  bool_t MultiCorrDef[CMaxMultiCorrDefCnt][16], int MultiCorrDefCnt);

/** @brief Configure the Interesting Correlations
  *
  * Detecting Correlations is a fairly CPU-expensive operation, thus it
  * should only be enabled, if the correlations are actually required.
  * Once counting of correlations is enabled, we shall always count all
  * possible pairs of two events. In additions to that we shall count
  * up to CMaxMultiCorrDefCnt arbitrary combinations of any of the 16 possible events.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param EnableCorr In: Do we want to count correlations at all?
  * @param MultiCorrDef In: Define up to CMaxMultiCorrDefCnt correlation patterns
  *    using arbitrary combinations of the events.
  * @param MultiCorrDefCnt In: How many of the CMaxMultiCorrDefCnt entries in MultiCorrDef
  *    are actually valid?
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ConfigCorrExt(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                  bool_t EnableCorr,
                  struct MultiCorrInfo MultiCorrDef[CMaxMultiCorrDefCnt], int MultiCorrDefCnt);

/** @brief Process Timetag Events / Count Events and Correlations
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param DataPacket In: Timetags to Process
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ProcessEvents(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                                               const TTMDataPacket_t *DataPacket);

/** @brief Process Timetag Events / Count Events and Correlations
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param DataPacket In: Timetags to Process
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ProcessEvents(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                                               const TTMDataPacket_t *DataPacket);

/** @brief Process queued Timetag Events / Count Events and Correlations
  *
  * ProcessEvents will always keep a queue of events that have been received
  * but not yet processed, since events that arrive later might need to be
  * processed earlier due to delay compensation. However if we know that no
  * earlier events can arrive any more (e.g. because we have stopped processing)
  * this queue should be emptied.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_FlushEventQueue(TTMCorrEvtCnt_t TTMCorrEvtCnt);

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
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_ForceNewSlot(TTMCorrEvtCnt_t TTMCorrEvtCnt);

/** @brief Get the Timestamp of the most recent Event
  *
  * When using Continous I-Mode TTMEventCnt automatically records the
  * most recent processed timestamp.
  * When using other (non-I-Mode-continous) modes, there is no concept
  * of global time and GetCurrTime() always returns 0.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param CurrTime  Out: Current Time (in I-Mode Ticks @ 82.3045ps)
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetCurrTime(TTMCorrEvtCnt_t TTMCorrEvtCnt, uint64_t *CurrTime);

/** @brief Get the number of Timeslots processed
  *
  * When using Continous I-Mode TTMEventCnt automatically starts a new
  * timeslot after the timetag clock has ticked for SlotMilliDuration
  * milliseconds. In all other modes a new timeslot is started by calling
  * ForceNewSlot() manually.
  *
  * TTMCorrEvtCnt_GetSlotCnt() fetches the number of Timeslots that have
  * been completed. It can be used to determine if it is worth looking for a
  * new frequency measurements.
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param SlotCnt   Out: Number of Slots
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetSlotCnt(TTMCorrEvtCnt_t TTMCorrEvtCnt, uint32_t *SlotCnt);

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
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param EventCnt  Out: Number of Timetagging Events/Channel
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetEvtCounter(TTMCorrEvtCnt_t TTMCorrEvtCnt, uint64_t EventCnt[16]);

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
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param TimeSlotCnt In: How many Timeslots form one second?
  * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
  * @param EventFreq  Out: Frequency of Timetagging Events/Channel
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetEvtFreq(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                         int TimeSlotCnt, int AvgSlotCnt, uint32_t EventFreq[16]);

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
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param CorrCnt  Out: Number of correlation between two channels
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetCorrCounter(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                                                         uint64_t CorrCnt[16][16]);

/** @brief Fetch the Frequency of Pairwise Correlation Events
  *
  * FIXME - More Documentation required here - FIXME
  *
  * Note: CorrFreq[] is an array of 16x16 elements. Indeces 0..7 in each matrix
  * direction correspond to the rising edges of Stop1..Stop8, indices 8..15
  * correspond to the falling edges of Stop1..Stop8.
  *
  * Note: Correlation are always counted for both contributing events. Thus
  * CorrFreq[x][y] is always equal to CorrFreq[y][x].
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param TimeSlotCnt In: How many Timeslots form one second?
  * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
  * @param CorrFreq  Out: Frequency of correlation between two channels
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetCorrFreq(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                       int TimeSlotCnt, int AvgSlotCnt, uint32_t CorrFreq[16][16]);

/** @brief Fetch the Number of MultiChannel Correlation Events
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param MultiCorrCnt  Out: Number of MultiEvent Correlations
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetMultiCorrCounter(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                                                        uint64_t MultiCorrCnt[CMaxMultiCorrDefCnt]);

/** @brief Fetch the Frequency of MultiChannel Correlation Events
  *
  * FIXME - More Documentation required here - FIXME
  *
  * @param TTMCorrEvtCnt  In: Handle to the CorrEventCounter
  * @param TimeSlotCnt In: How many Timeslots form one second?
  * @param AvgSlotCnt  In: Number of Timeslots to use for averaging
  * @param MultiCorrFreq  Out: Frequency of MultiEvent Correlations
  * @returns Error Code
  */
FlexIOStatus LIBTTM_API TTMCorrEvtCnt_GetMultiCorrFreq(TTMCorrEvtCnt_t TTMCorrEvtCnt,
                      int TimeSlotCnt, int AvgSlotCnt, uint32_t MultiCorrFreq[CMaxMultiCorrDefCnt]);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/** @brief Underflow Avoidance Offset for the GPX.
  *
  * The acam GPX contains an internal ALU that performs preliminary
  * computations on the timestamps. This ALU is not designed to handle
  * underflow if Stop and Start events occur very closely together. Thus
  * an Offset is added to all Stop events. This offset becomes part of
  * the results generated by the GPX and must be removed by the user
  * application to obtain unbiased results.\n
  * Depending on the Measurement Mode GPXUnderflowAvoidanceOffset will
  * be added once or several times:\n
  * Looking at a timing resolution of 82.3045ps \n
  *  .) I-Mode: 1 x GPXUnderflowAvoidanceOffset   // 6075 * 82.3045ps       = 500ns \n
  *  .) G-Mode: 1 x GPXUnderflowAvoidanceOffset   // 6075 * (82.3045ps/2)   = 250ns (!)\n
  *  .) R-Mode: 3 x GPXUnderflowAvoidanceOffset   // 3*6075 * (82.3045ps/3) = 500ns \n
  *  .) M-Mode: 3*MModeDiv x GPXUnderflowAvoidanceOffset // 3*MDiv*6075 * (82.3045ps/(3*MDiv)) = 500ns \n
  * Note: 6075 = 3^5 * 5^2 = 5 * (3^5 * 5)
  *       TBin = (25ns * 128) / (216 * 180) = (25ns * 2^7) / ((2^3 * 3^3) * (2^2 * 3^2 * 5)) =
  *            = (25ns * 2^7) / (2^5 * 3^5 * 5) = (25ns * 2^2) / (3^5 * 5) = 100ns / (3^5 * 5) =
  *            = (5 * 100ns) / (5 * (3^5 * 5)) = 500ns / (3^5 * 5^2)
  *
  * Looking at a timing resolution of 100.1001ps \n
  *  .) I-Mode: 1 x GPXUnderflowAvoidanceOffset   // 4995 * 100.1001ps       = 500ns \n
  *  .) G-Mode: 1 x GPXUnderflowAvoidanceOffset   // 4995 * (100.1001ps/2)   = 250ns (!)\n
  *  .) R-Mode: 3 x GPXUnderflowAvoidanceOffset   // 3*4995 * (100.1001ps/3) = 500ns \n
  *  .) M-Mode: 3*MModeDiv x GPXUnderflowAvoidanceOffset // 3*MDiv*4995 * (100.1001ps/(3*MDiv)) = 500ns \n
  * Note: 4995 = 5 * 999 = 5 * (3^3 * 37)
  *       TBin = (25ns * 128) / (216 * 148) = (25ns * 2^7) / ((2^3 * 3^3) * (2^2 * 37)) =
  *            = (25ns * 2^7) / (2^5 * 3^3 * 37) = (25ns * 2^2) / (3^3 * 37) = 100ns / (3^3 * 37)
  *
  * @see GPXChnCalibrateOffset, GPXCableAdjOffset
  */
#define GPXUnderflowAvoidanceOffset 5000     // Neat Value when editing Offset Files
// #define GPXUnderflowAvoidanceOffset 6075  // Technically correct for 500ns @ 82,3045ps/Tick
// #define GPXUnderflowAvoidanceOffset 4995  // Technically correct for 500ns @ 100,1001ps/Tick

/** @brief Channel Calibration Offset
  *
  * The GPX chip does produce measurements that (for a given start/stop
  * signal pair) depend on the channel, the measurement mode and the
  * individual GPX chip. During manufacturing it is therefore necessary
  * to measure the actual offsets of each channel/measurement mode for
  * each individual TTM8000 produced and store the offset values on the
  * board where they will be automatically added before the measurement
  * is delivered to the user.
  * The offset can be both positive and negative. To avoid having to deal
  * with negative numbers in the FPGA, we shall add a fixed offset to
  * make all offsets positive. Of course this fixed offset needs to be
  * subtracted before the numbers are presented to the user...
  *
  * @see GPXUnderflowAvoidanceOffset, GPXCableAdjOffset
  */
#define GPXChnCalibrateOffset         8192

/** @brief Cable Length Adjustment Offset
  *
  * In addition to the 'technical' offsets above, we shall also provide
  * a user defined offset, that can be used to compensate for different
  * cable length etc. This is a just an 8-bit value (to avoid trouble
  * with sorting) that can be chosen in the range +/-125. Again we want
  * to avoid the trouble of working with signed numbers, so we shall add
  * GPXCableAdjOffset and always add a positive quantity. Of course this
  * offset needs to be subtracted before the numbers are presented to the
  * user...
  *
  * @see GPXUnderflowAvoidanceOffset, GPXChnCalibrateOffset
  */
#define GPXCableAdjOffset              128

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined __cplusplus
}  /* extern "C" */
#endif

/**********************************************************************
 *                              TTMLib.h                              *
 **********************************************************************/
#endif // TTMLib_h
