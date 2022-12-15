#include <Arduino.h>
#include <ADC.h>
#include <usb_dalsa.h>

// Pin definitions
#define PIN_DRV_PH_V1 0
#define PIN_DRV_PH_V2 1
#define PIN_DRV_PH_H1 2
#define PIN_DRV_PH_H2 3
#define PIN_DRV_PH_R 4
#define PIN_DRV_SW_PH_H21 5
#define PIN_DRV_SW_PH_H22 6

#define PIN_P1_VOUT1 14
#define PIN_P1_VOUT2 15
#define PIN_P2_VOUT1 16
#define PIN_P2_VOUT2 17

#define PIN_LED_TEENSY 13
#define PIN_LED0 28
#define PIN_LED1 29
#define PIN_LED2 30
#define PIN_LED3 31

// Sensor definitions
#define SENSOR_RESOLUTION 1024
#define SENSOR_DARK_ROWS 4
#define SENSOR_ROWS (SENSOR_RESOLUTION + 2 * (SENSOR_DARK_ROWS))
#define SENSOR_JUNK_COLS_PRE 4
#define SENSOR_DARK_COLS_PRE 4
#define SENSOR_JUNK_COLS_POST 2
#define SENSOR_DARK_COLS_POST 8
#define SENSOR_COLUMNS (SENSOR_JUNK_COLS_PRE + SENSOR_DARK_COLS_PRE + SENSOR_RESOLUTION + SENSOR_DARK_COLS_POST + SENSOR_JUNK_COLS_POST)

// Macros
#define PHASE_V(state1, state2) {digitalWrite(PIN_DRV_PH_V1, state1); digitalWrite(PIN_DRV_PH_V2, state2);}
#define PHASE_H(state) {digitalWrite(PIN_DRV_PH_H1, state); digitalWrite(PIN_DRV_PH_H2, !state);}
#define PHASE_R(state) {digitalWrite(PIN_DRV_PH_R, state);}

ADC *adc = new ADC();
bool pin_state = false;

typedef struct __attribute__((__packed__)) {
  uint32_t row;
  uint32_t col;
  uint8_t readout_pin;
  bool busy;
  bool done;
} readout_state;
readout_state state;

EXTMEM uint16_t pixel_buffer[SENSOR_ROWS][SENSOR_COLUMNS]; // External RAM is neccesary to fit the buffer

void next_row() {
  delayMicroseconds(1);
  PHASE_V(true, false);
  delayMicroseconds(5);
  PHASE_V(false, true);
  delayMicroseconds(5);
  PHASE_V(true, false);
  delayMicroseconds(5);
  PHASE_V(false, false);
  delayMicroseconds(1);
}

void adc_irq() {
  // Read pixel value
  pixel_buffer[state.row][state.col] = adc->adc0->readSingle();

  // Next pixel!
  state.col++;
  if (state.col >= SENSOR_COLUMNS) {
    state.col = 0;
    state.row++;

    next_row();

    if (state.row >= SENSOR_ROWS) {
      state.busy = false; // We're done!
      state.done = true;
    }
  }

  // Do some clocking


  if (state.busy) {
    // Start next read
    adc->adc0->startSingleRead(state.readout_pin);
  }
}

bool start_readout(){
  if (state.busy) {
    return false;
  }

  // Setup state
  state.row = 0;
  state.col = 0;
  state.busy = true;
  state.done = false;
  state.readout_pin = PIN_P1_VOUT1;

  // Initialize phases
  PHASE_V(false, false);
  PHASE_H(false);
  PHASE_R(false);

  // Setup read ADC
  adc->adc0->enableInterrupts(adc_irq);
  adc->adc0->setAveraging(1);
  adc->adc0->setResolution(10);
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED);
  adc->adc0->wait_for_cal();

  // Start readout
  next_row();

  adc->adc0->startSingleRead(state.readout_pin);

  return true;
}

typedef struct __attribute__((__packed__)) {
  uint8_t command;
  uint32_t data_len;
  uint8_t data[64 - 5];
} control_req_t;

uint32_t usb_handler(uint8_t *control_data, uint32_t len, uint8_t *return_data, uint32_t max_return_len) {
  uint32_t return_len = 0;
  control_req_t *req = (control_req_t *)control_data;
  if (len < 5) {
    Serial.write("Invalid command with len %d\n", len);
    return_len = 0;
    goto end;
  }

  switch (req->command) {
    case 0x00: // Ping
      Serial.write("Ping!\n");
      return_data[0] = 0xA5;
      return_len = 1;
      break;
    case 0x01: // Check state
      memcpy(return_data, &state, sizeof(state));
      return_len = sizeof(state);
      break;
    case 0x02: // Get pixel buffer
      // setup bulk transfer
      usb_dalsa_init_bulk_transfer((uint8_t *) pixel_buffer, sizeof(pixel_buffer), 0);

      // return the size of the buffer
      *((uint32_t *)return_data) = sizeof(pixel_buffer);
      return_len = sizeof(uint32_t);
      break;

    default:
      Serial.write("Invalid command %d\n", req->command);
      return_len = 0;
      break;
  }

end:
  return return_len;
}

void setup() {
  // Setup Serial
  Serial.begin(115200);

  // LEDs
  pinMode(PIN_LED_TEENSY, OUTPUT);
  pinMode(PIN_LED0, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  // Phases
  pinMode(PIN_DRV_PH_V1, OUTPUT);
  pinMode(PIN_DRV_PH_V2, OUTPUT);
  pinMode(PIN_DRV_PH_H1, OUTPUT);
  pinMode(PIN_DRV_PH_H2, OUTPUT);
  pinMode(PIN_DRV_PH_R, OUTPUT);

  usb_dalsa_set_handler(usb_handler);

  // Start readout
  start_readout();
}

void loop() {
  start_readout();
  delay(2000);
}
