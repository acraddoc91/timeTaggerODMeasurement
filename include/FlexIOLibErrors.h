/***********************************************************************
 *                                                                     *
 *                          FlexIOLibErrors.h                          *
 *                                                                     *
 ***********************************************************************/
/*
 * Project:   ttm (Time Tagging Module)
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
 *
 *//*
 * Tab: 2 spaces
 *
 * Release 4.4.4 - 2016-01-26
 */

#ifndef FlexIOLibErrors_h
#define FlexIOLibErrors_h

/**********************************************************************
 *                                                                    *
 **********************************************************************/
/** @brief Error Codes returned by all functions of TTMLib
  *
  */
typedef enum
 {
  FlexIO_Success            =  0,  //!< Success (No Error)
  FlexIO_Error              = -1,  //!< Generic Error
  FlexIO_BadParam           = -2,  //!< Bad Parameters (e.g NULL pointers or Out of Range)
  FlexIO_FeatureUnimplemented=-3,  //!< Feature Not Implemented
  FlexIO_SyntaxError        = -4,  //!< Syntax Error
  FlexIO_OutOfMemory        = -5,  //!< Out of Memory
  FlexIO_InternalError      = -6,  //!< Internal Error of the FlexIOLib / TTMLib
  FlexIO_BrokenHardware     = -7,  //!< A Hardware Failure was detected
  FlexIO_Quit               = -8,  //!< Quit
  FlexIO_InvalidObject      = -9,  //!< Invalid Object (dangling pointer?)
  FlexIO_InvalidHWType      = -10, //!< Requested Operation does not match Hardware Type

  FlexIO_NotConnected       = -20, //!< A command was sent before the Client was Connected
  // FlexIO_NoCntrlConnectionAvailable  = -20,  //!< Too many Cntrl Connections active
  FlexIO_InvalidTTMCntrl             = -21,  //!< TTMCntrl Parameter not valid

  FlexIO_NoDataConnection   = -40,  //!< No Data Connections
  FlexIO_InvalidTTMData     = -41,  //!< TTMData Parameter not valid
  FlexIO_TooManyTTMDataConnections = -42, //!< Too Many TTM Data Connections
  FlexIO_UnknownDataSource  = -43,  //!< Unknown Data Source (for GetActiveBoardConfig())

  // From sock.h
  FlexIO_UnknownSocket      = -101, //!< Unknown Socket
  FlexIO_SocketNoData       = -102, //!< No Data Available from Socket

  // From sta_net.h
  FlexIO_ARPDifferentTarget = -201, //!< TTMServer: Received ARP Request for a Different Target
  FlexIO_ARPNotRequested    = -202, //!< TTMServer: Received ARP Reply that was not Requested
  FlexIO_ARPReply           = -203, //!< TTMServer: Received Requested ARP Reply (No Error)
  FlexIO_ARPFailed          = -204, //!< TTMServer: ARP Lookup Failed

  // From ttmboardrawremclient
  FlexIO_MissingNetwork     = -300, //!< No Network on Host Computer
  FlexIO_MissingSocket      = -301, //!< Failed to Create Socket
  FlexIO_InvalidSocket      = -302, //!< Trying Network operation on Invalid Socket
  FlexIO_InvalidProtocol    = -303, //!< Protocol must be UDP or TCP - Only UDP is implemented
  FlexIO_NetSendError       = -304, //!< Failed to send network packet
  FlexIO_NetRecvError       = -305, //!< Failed to receive network packet
  FlexIO_InvalidDataTarget  = -306, //!< Failed to perform ARP Resolve on Data Target
  FlexIO_InvalidPacketID    = -307, //!< PacketID not prev. PacketID+1
  FlexIO_NetTimeout         = -308, //!< Network Timeout
  FlexIO_NetPacketLost      = -309, //!< Packet lost during MTU discovery
  FlexIO_InvalidPacketFormat= -310, //!< Invalid Packet Format (CmdAck or Data Packets)
  FlexIO_TooManyOpenSockets = -311, //!< Too many UDP Receive Sockets open at once
  FlexIO_BindSocketFailed   = -312, //!< Socket bind() failed
  FlexIO_EchoDataMismatch   = -313, //!< Result of the Echo command does not match original data
  FlexIO_BufferOverflow     = -614, //!< Buffer Overflow

  // From ttmarc.h
  FlexIO_UnknownMeasurementMode  = -401, //!< Unknown Measurement Mode
  FlexIO_PLLNotLocked            = -402, //!< PLL for Speed Calibration of GPX-TDC not Locked
  FlexIO_HitFIFO_Full            = -403, //!< Hit FIFO of the GPX-TDC full
  FlexIO_InterfaceFIFO_Full      = -404, //!< Interface FIFO of the GPX-TDC full
  FlexIO_InvalidDataPacketFormat = -405, //!< Invalid Timestamp Data Packet Format
  FlexIO_ResetReadWriteIdxFailed = -410, //!< Resetting the ReadIdx or WriteIdx in the FPGA failed

  // Firmware Update
  FlexIO_FlashEraseFailed        = -501, //!< Failed to Erase Program Flash Memory
  FlexIO_FlashWriteFailed        = -502, //!< Failed to Write Program Flash Memmory
  FlexIO_FlashPacketLost         = -503, //!< Flash Data Packet Lost over Network
  FlexIO_InvalidFirmwareImageFormat  = -504,  //!< Firmware Image File is not a valid ELF or SREC File
  FlexIO_InvalidFPGAImageFormat      = -505,  //!< FPGA Bitfile is not a valid Xilinx Bitimage

  // Time Tagging Module Command Acknowledge Codes
  FlexIO_UnknownMaster   =   -1001,  //!< Command Ignored (e.g. Master not connected)
  FlexIO_CmdFormatError  =   -1002,  //!< Bad Command Packet format (Magic Cookies/Size)
  FlexIO_ServerBusy      =   -1003,  //!< Connection Refused - Server Busy
  FlexIO_ServerNotConnected= -1004,  //!< Can't Disconnect since we are not connected
  FlexIO_ChecksumError   =   -1005,  //!< Server firmware transfer with Checksum Error
  FlexIO_UnknownCmd      =   -1006,  //!< Unknwon Command Token

  // FlexIO_CmdNotSupportedbyFPGAImage = -1021,  //!< Command not supported by current FPGA image
  // FlexIO_CmdNotSupportedbyFirmware  = -1022,  //!< Command not supported by current PowerPC Firmware

  FlexIO_ExprTooComplex = -1101,
  FlexIO_DivisionByZero = -1102

}  FlexIOStatus;

/***********************************************************************
 *                          FlexIOLibErrors.h                          *
 ***********************************************************************/
#endif // FlexIOLibErrors_h
