#include "usb_dev.h"
#include "usb_serial.h"
#include "debug/printf.h"
#include "avr/pgmspace.h" // for PROGMEM, DMAMEM, FASTRUN

#define RX_NUM  8
#define DALSA_RX_SIZE 64

static uint16_t tx_packet_size = 0;
static transfer_t tx_transfer[RX_NUM];
static transfer_t rx_transfer[RX_NUM] __attribute__ ((used, aligned(32)));
DMAMEM static uint8_t rx_buffer[RX_NUM][DALSA_RX_SIZE] __attribute__ ((aligned(32)));
extern volatile uint8_t usb_high_speed;

uint32_t (*control_handler)(uint8_t *control_data, uint32_t len, uint8_t *return_data, uint32_t max_return_len) = NULL;

static void rx_queue_transfer(int i) {
	NVIC_DISABLE_IRQ(IRQ_USB1);
	usb_prepare_transfer(&rx_transfer[i], &rx_buffer[i], DALSA_RX_SIZE, i);
	arm_dcache_delete(&rx_buffer[i], DALSA_RX_SIZE);
	usb_receive(DALSA_RX_ENDPOINT, &rx_transfer[i]);
	NVIC_ENABLE_IRQ(IRQ_USB1);
}

uint8_t return_data[512];
static void rx_event(transfer_t *t) {
	int len = DALSA_RX_SIZE - ((t->status >> 16) & 0x7FFF);
	int i = t->callback_param;

  uint32_t return_len = 0;
  if(control_handler != NULL) {
    return_len = control_handler(rx_buffer[i], len, return_data, tx_packet_size);
    if (return_len > tx_packet_size) {
      printf("return_len > tx_packet_size\n");
      return_len = tx_packet_size;
    }
  } else {
    return_data[0] = 0x00;
    return_len = 1;
    printf("control_handler == NULL\n");
  }

  // queue response transfer
  NVIC_DISABLE_IRQ(IRQ_USB1);
  usb_prepare_transfer(&tx_transfer[i], return_data, return_len, 0);
  arm_dcache_flush_delete(return_data, return_len);
  usb_transmit(DALSA_TX_ENDPOINT, &tx_transfer[i]);
  NVIC_ENABLE_IRQ(IRQ_USB1);

  // Put buffer back in
  rx_queue_transfer(i);
}

void usb_dalsa_set_handler(uint32_t (*handler)(uint8_t *control_data, uint32_t len, uint8_t *return_data, uint32_t max_return_len)) {
  control_handler = handler;
}

void usb_dalsa_configure (void) {
  tx_packet_size = usb_high_speed ? 512 : 64;

  usb_config_rx(DALSA_RX_ENDPOINT, DALSA_RX_SIZE, 0, rx_event);
  usb_config_tx(DALSA_TX_ENDPOINT, tx_packet_size, 0, NULL);

  // init some rx transfers
  for (int i=0; i < RX_NUM; i++) rx_queue_transfer(i);

  printf("Dalsa USB configured\n");
}