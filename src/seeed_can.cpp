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

#include "seeed_can.h"

SEEED_CAN::SEEED_CAN(PinName ncs, PinName irq, PinName mosi, PinName miso, PinName clk, int spiBitrate)
    : _spi(mosi, miso, clk), _can(_spi, ncs, irq), _irqpin(irq) {
  // Make sure CS is high
  _can.ncs = 1;
  // Set up the spi interface
  _can.spi.format(8, 3);
  _can.spi.frequency(spiBitrate);
  _irqpin.fall(this, &SEEED_CAN::call_irq);
}

int SEEED_CAN::open(int canBitrate, Mode mode) { return mcpInit(&_can, (uint32_t)canBitrate, (CANMode)mode); }

void SEEED_CAN::monitor(bool silent) { mcpMonitor(&_can, silent); }

int SEEED_CAN::mode(Mode mode) { return mcpMode(&_can, (CANMode)mode); }

int SEEED_CAN::frequency(int canBitRate) { return mcpInit(&_can, (uint32_t)canBitRate, (CANMode)Normal); }

int SEEED_CAN::read(SEEED_CANMessage &msg) { return mcpCanRead(&_can, &msg); }

int SEEED_CAN::write(SEEED_CANMessage msg) { return mcpCanWrite(&_can, msg); }

int SEEED_CAN::mask(int maskNum, int canId, CANFormat format) { return mcpInitMask(&_can, maskNum, canId, format); }

int SEEED_CAN::filter(int filterNum, int canId, CANFormat format) {
  return mcpInitFilter(&_can, filterNum, canId, format);
}

unsigned char SEEED_CAN::rderror(void) { return mcpReceptionErrorCount(&_can); }

unsigned char SEEED_CAN::tderror(void) { return mcpTransmissionErrorCount(&_can); }

int SEEED_CAN::errors(ErrorType type) { return mcpErrorType(&_can, (CANFlags)type); }

unsigned char SEEED_CAN::errorFlags(void) { return mcpErrorFlags(&_can); }

void SEEED_CAN::attach(void (*fptr)(void), IrqType event) {
  if (fptr) {
    _callback_irq.attach(fptr);
    mcpSetInterrupts(&_can, (CANIrqs)event);
  } else {
    mcpSetInterrupts(&_can, (CANIrqs)SEEED_CAN::None);
  }
}

void SEEED_CAN::call_irq(void) { _callback_irq.call(); }

int SEEED_CAN::interrupts(IrqType type) { return mcpInterruptType(&_can, (CANIrqs)type); }

unsigned char SEEED_CAN::interruptFlags(void) { return mcpInterruptFlags(&_can); }
