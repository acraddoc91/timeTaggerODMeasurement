/**********************************************************************
 *                                                                    *
 *                             FlexIOLib.h                            *
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
#ifndef FlexIOLib_h
#define FlexIOLib_h

/**********************************************************************/

#ifndef SysTypes_h
  #include "SysTypes.h"
#endif

/**********************************************************************/

#include "FlexIOLibErrors.h"
#include "FlexIOLibTypes.h"

/**********************************************************************
 *                                                                    *
 **********************************************************************/

#if defined(_WIN32)       // Windows
  #if defined _MSC_VER || defined __GNUC__ // Microsoft Visual Studio & GCC
    #if defined (LIBFLEXIO_DLLEXPORTS)
      #define LIBFLEXIO_API __declspec(dllexport)
    #elif defined (LIBFLEXIO_LIBEXPORTS)
      #define LIBFLEXIO_API
    #else
      #define LIBFLEXIO_API __declspec(dllimport)
    #endif
  #else
    // Qt Creator ...
    // Qt defines Q_DECL_EXPORT as __attribute__((visibility("default")))
        #if defined (LIBFLEXIO_DLLEXPORTS)
          #define LIBFLEXIO_API Q_DECL_EXPORT
        #else
          #define LIBFLEXIO_API Q_DECL_IMPORT
        #endif
  #endif
#elif defined(__linux)    // Linux
  #ifdef LIBFLEXIO_LIBEXPORTS
    #define LIBFLEXIO_API __attribute__((visibility("default")))
  #else
    #define LIBFLEXIO_API
  #endif
#elif defined(__MacOSX__) // Mac OS X
  #ifdef LIBFLEXIO_LIBEXPORTS
    #define LIBFLEXIO_API __attribute__((visibility("default")))
  #else
    #define LIBFLEXIO_API
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

/** @brief Handle to a FlexIOCntrl Object.
  *
  * A FlexIOCntrl_t is a handle to a FlexIOCntrl objects. FlexIOCntrl
  * objects are opaque, they are useable only by passing their handle
  * to one of the functions below. They do not provide any data fields that
  * can be directly read/written with C statements. Use FlexIOCntrl_Connect()
  * to create a new FlexIOCntrl object and remember that you MUST call
  * FlexIOCntrl_Disconnect() to dispose of it after you are done using it.
  * Note that this requirement applies even if you are leaving your
  * application at an unexpected time (e.g. after the user pressed Cntrl-C
  * to abort) as the FlexIO Board would otherwise not be notified of
  * the broken connection.
  *
  * Technical Note: Internally a FlexIOCntrl_t is a *FlexIOCntrl_c, that
  * corresponds to the 'this' pointer used in C++ class-method calls.
  * However since this header can be used in plain C code, that does
  * not understand C++ classes, we can not explicitly use FlexIOCntrl_c
  * here. We could define FlexIOCntrl_t as a void*, however this would
  * not be type-safe, since a void** is automatically converted to a
  * void* as needed by the compiler and mistakes caused by using a
  * *FlexIOCntrl_t when a FlexIOCntrl_t would be needed would not be caught.
  * Thus we shall use a simple struct, that looks a bit oversized but
  * makes things typesafe (and does not incur any cost in code size
  * or performance).
  */
typedef struct {
  void *OpaqueFlexIOCntrlHnd; //!< Pointer to FlexIOCntrl_c masqueraded for use with plain C
 } FlexIOCntrl_t;
/* typedef void* FlexIOCntrl_t; */

/** @brief Get the Version number of FlexIOLib.
  *
  * @return Returns the version number of FlexIOLib at run-time 32-bit integer in format
  * 0xMMNNPPSS (MM = major, NN = minor, PP = patch, SS = sub(0=none, 1='a', ...).
  * For example FlexIOLib 1.4.12b will return 0x01040C02.
  */
uint32_t LIBFLEXIO_API FlexIOCntrl_GetVersion();

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
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetAvailableBoards(uint32_t ApplMagic,
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
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetNearestNetInterface(in_addr_t BoardIPAddr,
                                                              in_addr_t *LocalIPAddr);

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
int LIBFLEXIO_API FlexIOCntrl_SetSockOptBufferSize(SOCKET SocketDescr, int OptName,
                                         uint32_t RequestSize, uint32_t *ActualSize);

/** @brief Get a human-readable String for an Error Code.
  *
  * @param Err In: The Error Code
  * @returns Pointer to a null-terminated string. Note that this string belongs to
  *          FlexIOLib and must not be modified. It is only valid until the next call
  *          to GetErrorName().
  */
const char LIBFLEXIO_API *FlexIOCntrl_GetErrorName(FlexIOStatus Err);

/** @brief Connect the FlexIOCntrl Object to a FlexIO Board.
  *
  * @note Every FlexIOLib Connection that was opened with Connect() must be
  *       closed with a corresponding call to Disconnect()!
  * @param FlexIOCntrl    Out: Handle to the new FlexIOCntrl object
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
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_Connect(FlexIOCntrl_t *FlexIOCntrl,
                  FlexIOBoardInfo_t *BoardInfo, uint32_t ApplMagic,
                  in_addr_t BoardIPAddr, in_port_t BoardCntrlPort,
                  in_addr_t LocalIPAddr, in_port_t LocalCntrlPort,
                  uint32_t  MilliNetTimeout);

/** @brief Check if a Control Connection has been established
  *
  * IsConnected() checks is a Control Connection has been established. It
  * can either perform a fast, local check, that just remembers if Connect()
  * has been successfully called for this FlexIOCntrl_t object, without
  * checking if the connection is still alive, or it can actually send a
  * test packet over the network and wait for an acknowledgment to make
  * sure that the network link is still alive.
  * Obviously the later option requires much more time.
  *
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param CheckNetStatus In: Do we want to send a test packet over
  *        the network to check that the connection is actually alive
  *        or is it sufficient to remember that we once successfully
  *        established a connection (that may or may not still be alive).
  * @returns Connection Status
  */
bool_t LIBFLEXIO_API FlexIOCntrl_IsConnected(FlexIOCntrl_t FlexIOCntrl, bool_t CheckNetStatus);

/** @brief Dispose of a Control Connection to a FlexIO Board.
  *
  * Disconnect closes a FlexIOLib Control Connection.
  *
  * Note: Once a FlexIOLib Connection has been closed it must no longer
  * be used for any FlexIOCntrl_*() function call.
  *
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_Disconnect(FlexIOCntrl_t FlexIOCntrl);


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
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param TargetClient   In: List of Client Connections that shall be killed.
  *        Pass NULL to kill all Client Connection except the current one.
  * @param TargetCnt      In: Number of Entries in TargetClient (or 0)
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_DisconnectOthers(FlexIOCntrl_t FlexIOCntrl,
                                       UDPAddr_t *TargetClient, int TargetCnt);

/** @brief Get the BSD socket used to send/receive commands.
  *
  * GetSocket() provides access to the handle of the BSD-style socket
  * used for the FlexIOLib connection. You can use it if your application has
  * special requirements (e.g. configuration settings) that can not be addressed
  * otherwise. However you should remember that the socket belongs to FlexIOLib
  * and must not be closed etc.
  * @note Most applications will NOT need to use GetSocket().
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param CntrlSocket Out: Socket Handle
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetSocket(FlexIOCntrl_t FlexIOCntrl, SOCKET *CntrlSocket);

/** @brief Get the IP-Address/UDP Port used to send/receive commands.
  *
  * GetSocketAddr() provides the IP-Address/UDP Port of the socket
  * used for the FlexIOLib connection.
  * This might be useful for status displays/debugging however most
  * applications will probably never need to call GetSocketAddr().
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param LocalIPAddr    Out: IP-Address of the Cntrl-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @param LocalCntrlPort Out: UDP Port of the Cntrl-Socket on the local host
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetSocketAddr(FlexIOCntrl_t FlexIOCntrl,
                               in_addr_t *LocalIPAddr, in_port_t *LocalCntrlPort);

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
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param BufferSize In: Requested Network Buffer Size [Byte] for
  *        the Control Socket. Note that this size is just a suggestion
  *        to the Operating System, that might choose to grant more or
  *        less than was requested.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetSocketBufferSize(FlexIOCntrl_t FlexIOCntrl, uint32_t BufferSize);

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
  * @param FlexIOCntrl    In: Handle to the FlexIOCntrl Connection
  * @param BufferSize In: The actual Network Buffer Size [Byte]
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetSocketBufferSize(FlexIOCntrl_t FlexIOCntrl, uint32_t *BufferSize);

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
  * are always smaller than 1500Byte.
  * @note As stated above GetNetMTU() will get the MTU for the
  * FlexIOCntrl connection. It does not make any statement about the MTU
  * of the FlexIOData connection! If you are running the control and
  * data aquisition applications on the same machine, the MTUs for both
  * applications will be the same, and you can use the FlexIOCntrl MTU
  * value to define the packet size for FlexIOData packets. If you are
  * running these application on different machines, the MTUs might be
  * different and you will need to use a different way (depending on
  * your specific situation) to discover the FlexIOData MTU.
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetNetMTU(FlexIOCntrl_t FlexIOCntrl, uint32_t *NetMTU);

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
  * Note: For historic reasons FlexIOCntrl_GetNetMTU() was designed to
  * just fetch the MTU for the FlexIO-to-PC direction. GetNetMTUExt()
  * fetches the MTU individually for both directions.
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param FlexIOtoPC_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
  *        when sending data from the FlexIO board to the controlling PC.
  *        Pass NULL if you are not interested in this information.
  * @param PCtoFlexIO_NetMTU  Out: The Network MTU of the FlexIOCntrl Connection
  *        when sending data from the controlling PC to the FlexIO board.
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetNetMTUExt(FlexIOCntrl_t FlexIOCntrl, uint32_t *FlexIOtoPC_NetMTU, uint32_t *PCtoFlexIO_NetMTU);

/** @brief Get Configuration/Status Information of the currently used FlexIO Board.
  *
  * GetBoardInfo() obtains Configuration Information (e.g. Serial Number, Device
  * Type, Network configuration, Firmware Version, Debug Levels etc.) and
  * Status Information (e.g. Partners of active Network-Connections, Measurement
  * Status etc.) of the current FlexIO Board.
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param BoardInfo  Out: Configuration/Status Information of the FlexIO Board
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetBoardInfo(FlexIOCntrl_t FlexIOCntrl,
                                                    FlexIOBoardInfo_t *BoardInfo);

/** @brief Query the State of the Buttons of the FlexIO-DisplayBoard
  *
  * The FlexIO DisplayBoard contains two buttons. Use QueryButtons() to
  * obtain the current state of these buttons.
  * @note  While there are only two buttons (User & Reset) installed on the
  *        Display Board, there is a connector that allows 6 additional,
  *        external buttons to be connected.
  *        The state of the Reset button is mapped to Bit 0 (Mask 0x01),
  *        the state of the User button to Bit 1 (Mask 0x02). These bits
  *        are '0' when the button is released an '1' when it is pressed.
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param KeyBits Out: State of the Keys
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_QueryButtons(FlexIOCntrl_t FlexIOCntrl, uint32_t *KeyBits);

/** @brief Query the State of the Buttons of the FlexIO-DisplayBoard (Depreciated)
  *
  * The FlexIO DisplayBoard contains two buttons. QueryKeys() can still be used
  * obtain the current state of these buttons, however the use of this function
  * is depreciated. Please use QueryButtons() instead.
  */
#define FlexIOCntrl_QueryKeys(FlexIOCntrl, KeyBits) FlexIOCntrl_QueryButtons(FlexIOCntrl, KeyBits)

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
  * @param FlexIOCntrl   In: Handle to the FlexIOCntrl Connection
  * @param SetLEDState   In: Requested State of LEDs
  * @param SetLEDMask    In: Only LEDs whose Mask Bit is '1' are set.
  * @param ToggleLEDMask In: LEDs whose ToggleLEDMask bit are set, are toggled.
  * @param CurrLEDState Out: New State of LEDs
  *        Pass NULL if you are not interested in this information.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetLEDs(FlexIOCntrl_t FlexIOCntrl,
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
  * @param FlexIOCntrl   In: Handle to the FlexIOCntrl Connection
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
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetLEDExt(FlexIOCntrl_t FlexIOCntrl,
                   uint32_t SetLEDState, uint32_t SetLEDMask,
                   uint32_t ToggleLEDMask,
                   uint32_t SetLEDActiveState, uint32_t SetLEDActiveMask,
                   uint32_t *CurrLEDState, uint32_t *CurrLEDActiveMask);

 /** @brief Set the prefered action for the Buttons
  *
  *  The FlexIO Hardware offers two buttons, that can be used for predefined
  *  actions or for user/application specific purposes. SetButtonAction()
  *  defines the action that the buttons shall be assigned to...
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param KeyAction   In: Requested action for the buttons
  * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
  *                        or is it a temporary setting, that shall be forgotten
  *                        at the next reset.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetButtonAction(FlexIOCntrl_t FlexIOCntrl,
                                             ButtonAction_t KeyAction[2], bool FlashConfig);

 /** @brief Get the prefered action for the Buttons
  *
  *  The FlexIO Hardware offers two buttons, that can be used for predefined
  *  actions or for user/application specific purposes. GetButtonAction()
  *  fetches the action that the buttons shall be assigned to...
  *
  * @param FlexIOCntrl In:  Handle to the FlexIOCntrl Connection
  * @param KeyAction   Out: Requested action for the buttons
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetButtonAction(FlexIOCntrl_t FlexIOCntrl,
                                             ButtonAction_t KeyAction[2]);

 /** @brief Set the function of the User-LEDs
  *
  *  The FlexIO Hardware offers 8 dual-color LEDs on the front panel. Five of
  *  these LEDs have specific functions, that can not be changed. The other
  *  three LEDs (User-LEDs) can be used for predefined actions (currently only
  *  heartbeat signalling) or for user/application specific purposes.
  *  SetLEDAction() can be used to switch between these uses.
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param LEDAction   In: Requested action for the LEDs
  * @param FlashConfig In: Shall this configuration be stored permanetly in Flash
  *                        or is it a temporary setting, that shall be forgotten
  *                        at the next reset.
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetLEDAction(FlexIOCntrl_t FlexIOCntrl,
                                             LEDAction_t LEDAction[3], bool FlashConfig);

 /** @brief Get the function of the User-LEDs
  *
  *  The FlexIO Hardware offers 8 dual-color LEDs on the front panel. Five of
  *  these LEDs have specific functions, that can not be changed. The other
  *  three LEDs (User-LEDs) can be used for predefined actions (currently only
  *  heartbeat signalling) or for user/application specific purposes.
  *  GetLEDAction() can be used to switch between these uses.
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param LEDAction  Out: Requested action for the LEDs
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetLEDAction(FlexIOCntrl_t FlexIOCntrl,
                                             LEDAction_t LEDAction[3]);

 /** @brief Set the PWM Ratio of the Cooling FAN
  *
  * The FlexIO Board can contain an optional Cooling Fan, whose speed can be
  * controlled using PWM (Pulse-Width-Modulation).
  * Note: SetFanPower() is DEPRECIATED! - Use SetFanConfig() instead! // <<<< AIT INTERNAL USE ONLY!!!
  *
  * @param FlexIOCntrl   In: Handle to the FlexIOCntrl Connection
  * @param FanPower      In: Fan PWM Ratio (0..100% Off ~ 1000..100% On)
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetFanPower(FlexIOCntrl_t FlexIOCntrl,
                                                   uint32_t FanPower);

/** @brief Get the Input Voltages of the Analog-Digital Converters (ADCs)
  *
  *
  * The FlexIO Board contains 24 12-bit Analog Digital Converters that
  * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
  * Mode.
  * @note Measuring a range of 4.096V, using a 12-bit ADC results in a
  *    resolution of 1mV/LSB.
  * @note For historical reasons FlexIOCntrl_GetADC() will only measure some/any of
  *    the first 16 channels. To access all channels use FlexIOCntrl_GetExtADC().
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
  * The FlexIO Board contains 24 12-bit Analog Digital Converters that
  * can either convert 0..4.096V in Unipolar Mode or +/-2.048V in Bipolar
  * Mode.
  *
  * @param FlexIOCntrl  In: Handle to the FlexIOCntrl Connection
  * @param MilliVolt   Out: Measured Voltages [mV]
  * @param BipolarMask  In: Bitmask of Bipolar Channels
  * @param UpdateMask   In: Bitmask of Voltages that are to be measured
  * @returns Error Code
  *
  * @see FlexIOCntrl_GetExtADC()
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetADC(FlexIOCntrl_t FlexIOCntrl,
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
  * @param FlexIOCntrl  In: Handle to the FlexIOCntrl Connection
  * @param MilliVolt   Out: Measured Voltages [mV]
  * @param BipolarMask  In: Bitmask of Bipolar Channels
  * @param UpdateMask   In: Bitmask of Voltages that are to be measured
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetExtADC(FlexIOCntrl_t FlexIOCntrl,
               int MilliVolt[32], uint32_t BipolarMask, uint32_t UpdateMask);

/** @brief Set the Output Voltages of the Digital-Analog Converters (DACs)
  *
  * The FlexIO Board contains 16 8-bit Digital Analog Converters (DACs).
  * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
  * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
  * range 0 .. 4096mV (with a resolution of 16mV).
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param MilliVolt   InOut: In: Requested Voltages [mV] - Out: Actual Voltages [mV]
  * @param UpdateMask  In: Bitmask of Voltages to be updated
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_SetDAC(FlexIOCntrl_t FlexIOCntrl,
                                   int MilliVolt[16], uint32_t UpdateMask);

/** @brief Get the Output Voltages of the Digital-Analog Converters (DACs)
  *
  * The FlexIO Board contains 16 8-bit Digital Analog Converters (DACs).
  * Channels 0..9 and 12..15 can generate voltages in the range +/- 4096mV
  * (with a resolution of 32mV). Channels 10/11 can generate voltages in the
  * range 0 .. 4096mV (with a resolution of 16mV).
  *
  * @param FlexIOCntrl In:  Handle to the FlexIOCntrl Connection
  * @param MilliVolt   Out: Actual Voltages [mV]
  * @returns Error Code
  *
  * @see FlexIOCntrl_SetDAC()
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetDAC(FlexIOCntrl_t FlexIOCntrl,
                                              int MilliVolt[16]);

/** @brief Get the Configuration of the FlexIO Board
  *
  * GetBoardConfig() fetches the Configuration (Serial Number/ Board Name/
  * Network Settings/ Debug Level) of the FlexIO Board.
  *
  * @param FlexIOCntrl  In: Handle to the FlexIOCntrl Connection
  * @param BoardConfig Out: Configuration of the FlexIO Board
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_GetBoardConfig(FlexIOCntrl_t FlexIOCntrl,
                                                FlexIOBoardConfig_t *BoardConfig);

/** @brief Update the Configuration of the FlexIO Board
  *
  * UpdateBoardConfig() updates the Configuration (Board Name/
  * Network Settings/ Debug Level) of the FlexIO Board.
  * Note that the Serial Number and MAC Address are factory defined
  * and can not be updated.
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @param BoardConfig In: New Configuration of the FlexIO Board
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_UpdateBoardConfig(FlexIOCntrl_t FlexIOCntrl,
                                            const FlexIOBoardConfig_t *BoardConfig);

/** @brief Update the PowerPC Firmware of the FlexIO Board
  *
  * UpdateFirmware() writes a new PowerPC Firmware into the FlexIO Board.
  * The Firmware must be in ELF (preferred) or SREC format.
  * The new Firmware will become active after the next reboot.
  *
  * @param FlexIOCntrl  In: Handle to the FlexIOCntrl Connection
  * @param Firmware     In: Firmware
  * @param FirmwareSize In: Size of Firmware [Byte]
  * @param Verbose      In: Print Progress Messages to the Console
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_UpdateFirmware(FlexIOCntrl_t FlexIOCntrl,
                           uint8_t *Firmware, uint32_t FirmwareSize, bool_t Verbose);

/** @brief Update the Xilinx FPGA Bitfile
  *
  * UpdateFPGA() writes a new FPGA Bitfile to the FlexIO Board. Be sure that
  * the FPGA logic has been configured to use the Configuration Clock (CCLK)
  * for system configuration (rather than JTAGClk) and that the BootRAM has
  * been loaded with a valid Bootloader.
  * The new Bitfile will become active after the next reboot.
  *
  * @param FlexIOCntrl  In: Handle to the FlexIOCntrl Connection
  * @param Firmware     In: Firmware
  * @param FirmwareSize In: Size of Firmware [Byte]
  * @param Verbose      In: Print Progress Messages to the Console
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_UpdateFPGA(FlexIOCntrl_t FlexIOCntrl,
                           uint8_t *Firmware, uint32_t FirmwareSize, bool_t Verbose);

/** @brief Reboot the FlexIO Board (breaking the connection)
  *
  * Reboot() tells the FlexIO Board to reboot itself.
  * Obviously this breaks the network connection to the board.
  * @note Warning: Reboot() does not work with the current hardware!
  *
  * @note  Reboot() is designed for AIT Internal Use Only!
  *
  * @param FlexIOCntrl In: Handle to the FlexIOCntrl Connection
  * @returns Error Code
  */
FlexIOStatus LIBFLEXIO_API FlexIOCntrl_Reboot(FlexIOCntrl_t FlexIOCntrl);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#if defined __cplusplus
}  /* extern "C" */
#endif

/**********************************************************************
 *                             FlexIOLib.h                            *
 **********************************************************************/
#endif // FlexIOLib_h
