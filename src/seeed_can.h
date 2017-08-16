/* Copyright (c) 2017 Akila Perera, Sophie Dexter
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SEEED_CAN_H_
#define _SEEED_CAN_H_

#include "seeed_can_api.h"

// if print debug information
#define DEBUG

/**
 * CANMessage class
 */
class SEEED_CANMessage : public CAN_Message {
 public:
  /**
   * Creates empty CAN message.
   */
  SEEED_CANMessage() {
    id = 0;
    memset(data, 0, 8);
    len = 8;
    type = CANData;
    format = CANStandard;
  }

  /**
   * Creates CAN message with specific content.
   */
  SEEED_CANMessage(int _id, const char *_data, char _len = 8, CANType _type = CANData,
                   CANFormat _format = CANStandard) {
    id = _id;
    memcpy(data, _data, _len);
    len = _len & 0xF;
    type = _type;
    format = _format;
  }

  /**
   * Creates CAN remote message.
   */
  SEEED_CANMessage(int _id, CANFormat _format = CANStandard) {
    id = _id;
    memset(data, 0, 8);
    len = 0;
    type = CANRemote;
    format = _format;
  }
};

/**
 * A can bus client, used for communicating with Seeed Studios' CAN-BUS Arduino Shield.
 */
class SEEED_CAN {
 public:
  /**
   * Seeed Studios CAN-BUS Shield Constructor - Create a SEEED_CAN interface connected to the specified pins.
   *
   * @param ncs Active low chip select
   * @param irq Active low interrupt pin
   * @param mosi SPI Master Out, Slave In pin
   * @param miso SPI Master In, Slave Out pin
   * @param clk SPI Clock pin
   * @param spiBitrate SPI Clock frequency, @b default: @p 1000000 (1 MHz).
   */
  SEEED_CAN(PinName ncs, PinName irq, PinName mosi, PinName miso, PinName clk, int spiBitrate = 1000000);
  //    virtual ~SEEED_CAN(); // !!! Need a de-constructor for the interrrupt pin !!!

  enum Mode { Normal = 0, Sleep, Loopback, Monitor, Config, Reset };

  /**
   * Open initialises the Seeed Studios CAN-BUS Shield.
   *
   * @param canBitrate CAN Bus Clock frequency, @b default: @p 100000 (100 kHz).
   * @param mode The initial operation mode, @b default: @p Normal.
   * @n @p SEEED_CAN::Normal - Normal mode is the standard operating mode,
   * @n @p SEEED_CAN::Monitor - This mode can be used for bus monitor applications.
   * @n @p SEEED_CAN::Sleep - This mode can be used to minimize the current consumption,
   * @n @p SEEED_CAN::Loopback - This mode can be used in system development and testing.
   * @n @p SEEED_CAN::Config - Open with this mode to prevent unwanted messages being received while you configure
   * Filters.
   *
   * @returns 1 if successful, 0 otherwise
   */
  int open(int canBitrate = 100000, Mode mode = Normal);

  /**
   * Puts or removes the Seeed Studios CAN-BUS shield into or from silent monitoring mode.
   *
   * @param silent boolean indicating whether to go into silent mode or not.
   */
  void monitor(bool silent);

  /**
   * Change the Seeed Studios CAN-BUS shield CAN operation mode.
   *
   * @param mode The new operation mode
   * @n @p SEEED_CAN::Normal - Normal mode is the standard operating mode,
   * @n @p SEEED_CAN::Monitor - This mode can be used for bus monitor applications.
   * @n @p SEEED_CAN::Sleep - This mode can be used to minimize the current consumption,
   * @n @p SEEED_CAN::Loopback - This mode can be used in system development and testing.
   * @n @p SEEED_CAN::Reset - Reset the MCP2515 device and stay in Configuration mode.
   *
   * @returns 1 if mode change was successful, 0 if mode change failed or unsupported,
   */
  int mode(Mode mode);

  /**
   * Set the CAN bus frequency (Bit Rate)
   *
   * @param hz The bus frequency in Hertz
   *
   * @returns 1 if successful, 0 otherwise
   */
  int frequency(int canBitRate);

  /**
   * Read a CAN bus message from the MCP2515 (if one has been received)
   *
   * @param msg A CANMessage to read to.
   *
   * @returns 1 if any messages have arrived, 0 if no message arrived,
   */
  int read(SEEED_CANMessage &msg);

  /**
   * Write a CAN bus message to the MCP2515 (if there is a free message buffer)
   *
   * @param msg The CANMessage to write.
   *
   * @returns 1 if write was successful, 0 if write failed,
   */
  int write(SEEED_CANMessage msg);

  /**
   * Configure one of the Accpetance Masks (0 or 1)
   *
   * @param maskNum The number of the Acceptance Mask to configure (Acceptance Mask 0 is associated with Filters 0 and
   * 1, Acceptance Mask 1 is associated with Filters 2 through 5).
   * @param canId CAN Id Mask bits (Acceptance Filters are only compared against bits that are set to '1' in an
   * Acceptance Mask (e.g. mask 0x07F0 and filter 0x03F0 would allow through messages with CAN Id's 0x03F0 through
   * 0x03FF because the 4 LSBs of the CAN Id are not filtered).
   * @param format Describes if the Acceptance Mask is for a standard (CANStandard) or extended (CANExtended) CAN
   * message frame format, @b default: @p CANStandard.
   *
   * @returns 1 if Acceptance Mask was set, 0 if the Acceptance Mask could not be set
   */
  int mask(int maskNum, int canId, CANFormat format = CANStandard);

  /**
   * Configure one of the Acceptance Filters (0 through 5)
   *
   * @param filterNum The number of the Acceptance Filter to configure (Acceptance Filters 0 and 1 are associated with
   * Mask 0, Acceptance Filters 2 through 5 are associated with Mask 1).
   * @param canId CAN Id Filter bits (Acceptance Filters are only compared against bits that are set to '1' in an
   * Acceptance Mask (e.g. mask 0x07F0 and filter 0x03F0 would allow through messages with CAN Id's 0x03F0 through
   * 0x03FF because the 4 LSBs of the CAN Id are not filtered).
   * @param format Describes if the Acceptance Filter is for a standard (CANStandard) or extended (CANExtended) CAN
   * message frame format, @b default: @p CANStandard.
   *
   * @returns 1 if Acceptance Filter was set, 0 if the Acceptance Filter could not be set
   */
  int filter(int filterNum, int canId, CANFormat format = CANStandard);

  /**
   * Returns number of message reception (read) errors to detect read overflow errors.
   *
   * @returns number of reception errors
   */
  unsigned char rderror(void);

  /**
   * Returns number of message transmission (write) errors to detect write overflow errors.
   *
   * @returns number of transmission errors
   */
  unsigned char tderror(void);

  enum ErrorType { AnyError = 0, Errors, Warnings, Rx1Ovr, Rx0Ovr, TxBOff, TxPasv, RxPasv, TxWarn, RxWarn, EWarn };

  /**
   * Check if any type of error has been detected on the CAN bus
   *
   * @param error Specify which type of error to report on, @b default: @p AnyError.
   * @n @p SEEED_CAN::AnyError - Any one or more of the following errors and warnings:
   * @n @p SEEED_CAN::Errors - Any one or more of the 5 errors:
   * @n @p SEEED_CAN::Rx1Ovr - Receive Buffer 1 Overflow Flag bit,
   * @n @p SEEED_CAN::Rx0Ovr - Receive Buffer 0 Overflow Flag bit,
   * @n @p SEEED_CAN::TxBOff - Bus-Off Error Flag bit,
   * @n @p SEEED_CAN::TxPasv - Transmit Error-Passive Flag bit,
   * @n @p SEEED_CAN::RxPasv - Receive Error-Passive Flag bit,
   * @n @p SEEED_CAN::Warnings - Any one or more of the 3 warnings:
   * @n @p SEEED_CAN::TxWarn - Transmit Error Warning Flag bit,
   * @n @p SEEED_CAN::RxWarn - Receive Error Warning Flag bit,
   * @n @p SEEED_CAN::EWarn - Error Warning Flag bit.
   *
   * @returns 1 if specified type of error has been detected, 0 if no errors
   */
  int errors(ErrorType type = AnyError);

  /**
   * Returns the contents of the MCP2515's Error Flag register
   *
   * @returns
   *     @b Bit_7 - RX1OVR: Receive Buffer 1 Overflow Flag bit - Set when a valid message is received for RXB1 and
   * CANINTF.RX1IF = 1 - Must be reset by MCU,
   * @n @b Bit_6 - RX0OVR: Receive Buffer 1 Overflow Flag bit - Set when a valid message is received for RXB0 and
   * CANINTF.RX0IF = 1 - Must be reset by MCU,
   * @n @b Bit_5 - TXBO: Bus-Off Error Flag bit - Bit set when TEC reaches 255 - Reset after a successful bus recovery
   * sequence,
   * @n @b Bit_4 - TXEP: Transmit Error-Passive Flag bit - Set when TEC is >= 128 - Reset when TEC is less than 128,
   * @n @b Bit_3 - RXEP: Receive Error-Passive Flag bit - Set when REC is >= 128 - Reset when REC is less than 128,
   * @n @b Bit_2 - TXWAR: Transmit Error Warning Flag bit - Set when TEC is >= 96 - Reset when TEC is less than 96,
   * @n @b Bit_1 - RXWAR: Receive Error Warning Flag bit - Set when REC is >= 96 - Reset when REC is less than 96,
   * @n @b Bit_0 - EWARN: Error Warning Flag bit - Set when TEC or REC is >= 96 (TXWAR or RXWAR = 1) - Reset when both
   * REC and TEC are < 96.
   */
  unsigned char errorFlags(void);

  enum IrqType {
    None = 0,
    AnyIrq,
    RxAny,
    TxAny,
    Rx0Fill,
    Rx1Full,
    Tx0Free,
    Tx1Free,
    Tx2Free,
    Error,
    Wake,
    MsgError,
  };

  /**
   * Attach a function to call whenever a CAN frame received interrupt is generated.
   *
   * @param fptr A pointer to a void function, or 0 to set as none.
   * @param event Which CAN interrupt to attach the member function to, @b default: @p RxAny
   * @n @p SEEED_CAN::None - Disable all interrupt sources,
   * @n @p SEEED_CAN::AnyIrq - Enable all interrupt sources,
   * @n @p SEEED_CAN::RxAny - Any full RX buffer can generate an interrupt,
   * @n @p SEEED_CAN::TxAny - Any empty TX buffer can generate an interrupt,
   * @n @p SEEED_CAN::Rx0Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Rx1Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Tx0Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx1Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx2Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Error - Error (multiple sources in EFLG register),
   * @n @p SEEED_CAN::Wake - Wakeup,
   * @n @p SEEED_CAN::MsgError - Message Error,
   */
  void attach(void (*fptr)(void), IrqType event = RxAny);

  /**
   * Attach a member function to call whenever a CAN frame received interrupt is generated.
   *
   * @param tptr pointer to the object to call the member function on.
   * @param mptr pointer to the member function to be called.
   * @param event Which CAN interrupt to attach the member function to, @b default: @p RxAny
   * @n @p SEEED_CAN::None - Disable all interrupt sources,
   * @n @p SEEED_CAN::AnyIrq - Enable all interrupt sources,
   * @n @p SEEED_CAN::RxAny - Any full RX buffer can generate an interrupt,
   * @n @p SEEED_CAN::TxAny - Any empty TX buffer can generate an interrupt,
   * @n @p SEEED_CAN::Rx0Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Rx1Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Tx0Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx1Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx2Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Error - Error (multiple sources in EFLG register),
   * @n @p SEEED_CAN::Wake - Wakeup,
   * @n @p SEEED_CAN::MsgError - Message Error,
   */
  template <typename T>
  void attach(T *tptr, void (T::*mptr)(void), IrqType event = RxAny) {
    _callback_irq.attach(tptr, mptr);
    mcpSetInterrupts(&_can, (CANIrqs)event);
    if ((mptr != NULL) && (tptr != NULL)) {
      _callback_irq.attach(tptr, mptr);
      mcpSetInterrupts(&_can, (CANIrqs)event);
    } else {
      mcpSetInterrupts(&_can, (CANIrqs)SEEED_CAN::None);
    }
  }

  void call_irq(void);

  /**
   * Check if the specified interrupt event has occurred
   *
   * @param event Which CAN interrupt to attach the member function to
   * @n @p SEEED_CAN::RxAny - At least 1 RX buffer is full,
   * @n @p SEEED_CAN::TxAny - At least 1 TX buffer is empty,
   * @n @p SEEED_CAN::Rx0Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Rx1Full - Receive buffer 1 full,
   * @n @p SEEED_CAN::Tx0Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx1Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Tx2Free - Transmit buffer 2 empty,
   * @n @p SEEED_CAN::Error - Error (multiple sources in EFLG register),
   * @n @p SEEED_CAN::Wake - Wakeup,
   * @n @p SEEED_CAN::MsgError - Message Error,
   *
   * @returns 1 if specified interrupt event has occurred, 0 if no errors
   */
  int interrupts(IrqType type);

  /**
   * Returns the contents of the MCP2515's Interrupt Flag register
   *
   * @returns
   * @b Bit_7 - MERRF: Message Error Interrupt Flag,
   * @n @b Bit_6 - WAKIF: Wake-up Interrupt Flag,
   * @n @b Bit_5 - ERRIF: Error Interrupt Flag (multiple sources in EFLG register, see errorFlags)
   * @n @b Bit_4 - TX2IF: Transmit Buffer 2 Empty Interrupt Flag
   * @n @b Bit_3 - TX1IF: Transmit Buffer 1 Empty Interrupt Flag
   * @n @b Bit_2 - TX0IF: Transmit Buffer 0 Empty Interrupt Flag
   * @n @b Bit_1 - RX1IF: Receive Buffer 1 Full Interrupt Flag
   * @n @b Bit_0 - RX0IF: Receive Buffer 0 Full Interrupt Flag
   * @n Bits are set (1) when interrupt pending, clear (0) when no interrupt pending.
   * @n Bits must be cleared by MCU to reset interrupt condition.
   */
  unsigned char interruptFlags(void);

 protected:
  SPI _spi;
  mcp_can_t _can;
  InterruptIn _irqpin;
  FunctionPointer _callback_irq;
};

#endif  // SEEED_CAN_H
