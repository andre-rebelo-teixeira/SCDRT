#include "can.hpp"
#include <hardware/flash.h>

CanCom::CanCom(uint16_t pin, uint8_t CSn, uint8_t Tx,
               uint8_t RX, uint8_t SCK, uint32_t CLK_SPEED)
    : CSn(CSn), Tx(Tx), RX(RX), SCK(SCK), CLK_SPEED(CLK_SPEED), pin(pin) {
  // TODO :  Get current rpi pico id from flash

  // initialize can0
  can0 = MCP2515{spi0, CSn, Tx, RX, SCK, CLK_SPEED};

  can0.reset();
  can0.setBitrate(CAN_1000KBPS, MCP_16MHZ);
  can0.setNormalMode();
}

ERROR CanCom::read_msg() {
  MCP2515::ERROR status_;

  if (!data_available) {
    return {ERROR::ERROR_NO_MESSAGE};
  };

  status_ = can0.readMessage(&msg);
  data_available = false;

  if (status_ != MCP2515::ERROR::ERROR_OK) {
    return {ERROR::ERROR_BAD_MESSAGE};
  }
  // decode a message and send to controller
}

ERROR CanCom::send_msg(uint16_t msg_id, uint8_t dlc,
                       std::vector<uint8_t> data) {
  if (dlc > 8 || dlc < 0) {
    return {ERROR::ERROR_BAD_MESSAGE};
  }

  MCP2515::ERROR status_;
  struct can_frame msg;

  msg.can_id = create_msg_ssid(msg_id);
  msg.can_dlc = dlc;
  for (int i = 0; i < dlc; i++) {
    msg.data[i] = data[i];
  }

  status_ = can0.sendMessage(&msg);

  if (status_ != MCP2515::ERROR::ERROR_OK) {
    return {ERROR::ERROR_HARDWARE};
  }

  return {ERROR::ERROR_OK};
}

ERROR CanCom::send_msg(uint32_t msg_ssid, uint8_t dlc,
                       std::vector<uint8_t> data) {
  if (dlc > 8 || dlc < 0) {
    return {ERROR::ERROR_BAD_MESSAGE};
  }

  MCP2515::ERROR status_;
  struct can_frame msg;

  msg.can_id = msg_ssid;
  msg.can_dlc = dlc;
  for (int i = 0; i < dlc; i++) {
    msg.data[i] = data[i];
  }

  status_ = can0.sendMessage(&msg);

  if (status_ != MCP2515::ERROR::ERROR_OK) {
    return {ERROR::ERROR_HARDWARE};
  }

  return {ERROR::ERROR_OK};
}

ERROR CanCom::send_msg(struct can_frame msg) {
  MCP2515::ERROR status_;

  status_ = can0.sendMessage(&msg);

  if (status_ != MCP2515::ERROR::ERROR_OK) {
    return {ERROR::ERROR_HARDWARE};
  }

  return {ERROR::ERROR_OK};
}
