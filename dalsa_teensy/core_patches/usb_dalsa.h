
extern volatile uint8_t usb_high_speed;

static uint16_t rx_packet_size = 0;
static uint16_t tx_packet_size = 0;

#define RX_NUM  8
static transfer_t rx_transfer[RX_NUM] __attribute__ ((used, aligned(32)));
DMAMEM static uint8_t rx_buffer[RX_NUM * CDC_RX_SIZE_480] __attribute__ ((aligned(32)));

static void rx_queue_transfer(int i) {
	NVIC_DISABLE_IRQ(IRQ_USB1);
	printf("rx queue i=%d\n", i);
	void *buffer = rx_buffer + i * CDC_RX_SIZE_480;
	usb_prepare_transfer(rx_transfer + i, buffer, rx_packet_size, i);
	arm_dcache_delete(buffer, rx_packet_size);
	usb_receive(DALSA_RX_ENDPOINT, rx_transfer + i);
	NVIC_ENABLE_IRQ(IRQ_USB1);
}

static void rx_event(transfer_t *t) {
	int len = rx_packet_size - ((t->status >> 16) & 0x7FFF);
	int i = t->callback_param;
	printf("rx event, len=%d, i=%d\n", len, i);

  // TODO: handle data

  // Put buffer back in
  rx_queue_transfer(i);
}

transfer_t xfer[1];
uint8_t txbuf[512] = {0};

static void tx_event(transfer_t *t) {
	printf("tx event\n");

  usb_prepare_transfer(&xfer[0], txbuf, tx_packet_size, 0);
  arm_dcache_flush_delete(txbuf, tx_packet_size);
  usb_transmit(DALSA_TX_ENDPOINT, &xfer[0]);
}



void usb_dalsa_configure (void) {
  rx_packet_size = 64;
  tx_packet_size = usb_high_speed ? 512 : 64;

  usb_config_rx(DALSA_RX_ENDPOINT, rx_packet_size, 0, rx_event);
  usb_config_tx(DALSA_TX_ENDPOINT, tx_packet_size, 0, tx_event);

  for (int i=0; i < RX_NUM; i++) rx_queue_transfer(i);

  tx_event(NULL);
  printf("Dalsa USB configured\n");
}