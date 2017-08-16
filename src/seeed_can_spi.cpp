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

#include "seeed_can_spi.h"

void mcpReset(mcp_can_t *obj) {
  obj->ncs = 0;
  obj->spi.write(MCP_RESET);
  obj->ncs = 1;
  wait_ms(10);
}

uint8_t mcpRead(mcp_can_t *obj, const uint8_t address) {
  uint8_t result;
  obj->ncs = 0;
  obj->spi.write(MCP_READ);
  obj->spi.write(address);
  result = obj->spi.write(0x00);
  obj->ncs = 1;
  return result;
}

void mcpReadMultiple(mcp_can_t *obj, const uint8_t address, uint8_t values[], const uint8_t n) {
  obj->ncs = 0;
  obj->spi.write(MCP_READ);
  obj->spi.write(address);
  for (uint32_t i = 0; i < n; i++) {
    values[i] = obj->spi.write(0x00);
  }
  obj->ncs = 1;
}

void mcpReadBuffer(mcp_can_t *obj, const uint8_t command, uint8_t values[], const uint8_t n) {
  obj->ncs = 0;
  obj->spi.write(command);
  for (uint32_t i = 0; i < n; i++) {
    values[i] = obj->spi.write(0x00);
  }
  obj->ncs = 1;
}

void mcpWrite(mcp_can_t *obj, const uint8_t address, const uint8_t value) {
  obj->ncs = 0;
  obj->spi.write(MCP_WRITE);
  obj->spi.write(address);
  obj->spi.write(value);
  obj->ncs = 1;
}

void mcpWriteMultiple(mcp_can_t *obj, const uint8_t address, const uint8_t values[], const uint8_t n) {
  obj->ncs = 0;
  obj->spi.write(MCP_WRITE);
  obj->spi.write(address);
  for (uint32_t i = 0; i < n; i++) {
    obj->spi.write(values[i]);
  }
  obj->ncs = 1;
}

void mcpWriteBuffer(mcp_can_t *obj, const uint8_t command, uint8_t values[], const uint8_t n) {
  obj->ncs = 0;
  obj->spi.write(command);
  for (uint32_t i = 0; i < n; i++) {
    obj->spi.write(values[i]);
  }
  obj->ncs = 1;
}

void mcpBufferRTS(mcp_can_t *obj, const uint8_t command) {
  obj->ncs = 0;
  obj->spi.write(command);
  obj->ncs = 1;
}

uint8_t mcpStatus(mcp_can_t *obj) {
  uint8_t status;
  obj->ncs = 0;
  obj->spi.write(MCP_READ_STATUS);
  status = obj->spi.write(0x00);
  obj->ncs = 1;
  return status;
}

uint8_t mcpReceiveStatus(mcp_can_t *obj) {
  uint8_t status;
  obj->ncs = 0;
  obj->spi.write(MCP_RX_STATUS);
  status = obj->spi.write(0x00);
  obj->ncs = 1;
  return status;
}

void mcpBitModify(mcp_can_t *obj, const uint8_t address, const uint8_t mask, const uint8_t data) {
  obj->ncs = 0;
  obj->spi.write(MCP_BITMOD);
  obj->spi.write(address);
  obj->spi.write(mask);
  obj->spi.write(data);
  obj->ncs = 1;
}
