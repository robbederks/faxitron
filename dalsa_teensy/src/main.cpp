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

#define T_HALF_PIXEL_US 1
#define T_PH_V_PRE_US 5
#define T_PH_V_PULSE_US 20
#define T_PH_V_POST_US 5
#define T_PH_H_TOTAL_US (T_PH_V_PRE_US + 3 * T_PH_V_PULSE_US + T_PH_V_POST_US)

ADC *adc = new ADC();
bool pin_state = false;

typedef struct __attribute__((__packed__)) {
  uint32_t row;
  uint32_t col;
  uint32_t ph_v_counter;
  uint8_t readout_pin;
  bool busy;
  bool done;
  bool rising_edge;
} readout_state;
volatile readout_state state;

EXTMEM uint16_t pixel_buffer[SENSOR_ROWS][SENSOR_COLUMNS]; // External RAM is neccesary to fit the buffer

IntervalTimer pixelTimer;

void pixel_irq(){
  if (state.rising_edge) {
    if (state.busy) {
      if(state.ph_v_counter > 0) {
        // Vertical phase (rows)
        uint32_t v_t_us = (T_PH_H_TOTAL_US - state.ph_v_counter * T_HALF_PIXEL_US);
        if(v_t_us < T_PH_V_PRE_US) {
          PHASE_V(false, false);
        } else if(v_t_us < T_PH_V_PRE_US + T_PH_V_PULSE_US) {
          PHASE_V(true, false);
        } else if(v_t_us < T_PH_V_PRE_US + 2 * T_PH_V_PULSE_US) {
          PHASE_V(false, true);
        } else if(v_t_us < T_PH_V_PRE_US + 3 * T_PH_V_PULSE_US) {
          PHASE_V(true, false);
        } else {
          PHASE_V(false, false);
        }
        state.ph_v_counter--;
      } else {
        // Horizontal phase (columns)
        PHASE_H(true);
        delayNanoseconds(250);

        // Start next read
        adc->adc0->startSingleRead(state.readout_pin);

        // Atomically read pixel value
        noInterrupts();
        while (!adc->adc0->isComplete());
        pixel_buffer[state.row][state.col] = adc->adc0->readSingle();
        interrupts();

        // Next pixel!
        state.col++;
        if (state.col >= SENSOR_COLUMNS) {
          state.col = 0;
          state.row++;

          // Next row!
          state.ph_v_counter = T_PH_H_TOTAL_US;

          if (state.row >= SENSOR_ROWS) {
            state.busy = false; // We're done!
            state.done = true;
            arm_dcache_flush_delete(pixel_buffer, sizeof(pixel_buffer));

            digitalWrite(PIN_LED0, LOW);

            // Reset phases
            PHASE_V(false, false);
            PHASE_H(false);
            PHASE_R(false);
          }
        }
      }
    }
  } else {
    if (state.busy) {
      PHASE_H(false);
    }

    // Pulse reset
    PHASE_R(true);
    delayNanoseconds(150);
    PHASE_R(false);
  }
  state.rising_edge = !state.rising_edge;
}

bool start_readout(bool high_gain){
  if (state.busy) {
    return false;
  }

  // Enable LED
  digitalWrite(PIN_LED0, HIGH);

  // Initialize phases
  PHASE_V(false, false);
  PHASE_H(true);
  PHASE_R(false);

  // Setup analog path
  digitalWrite(PIN_DRV_SW_PH_H21, !high_gain);
  digitalWrite(PIN_DRV_SW_PH_H22, high_gain);

  // Setup read ADC
  adc->adc0->setAveraging(1);
  adc->adc0->setResolution(10);
  adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
  adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
  adc->adc0->wait_for_cal();

  // Setup state
  state.row = 0;
  state.col = 0;
  state.done = false;
  state.rising_edge = false;
  state.ph_v_counter = T_PH_H_TOTAL_US; // Start with a fresh row
  state.readout_pin = high_gain ? PIN_P1_VOUT2 : PIN_P1_VOUT1;
  state.busy = true;

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
    case 0x03: // Start readout
      return_data[0] = start_readout((req->data[0] != 0)) ? 0x00 : 0xFF;
      return_len = 1;
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

  // Sensor readout config
  pinMode(PIN_DRV_SW_PH_H21, OUTPUT);
  pinMode(PIN_DRV_SW_PH_H22, OUTPUT);

  // USB handler
  usb_dalsa_set_handler(usb_handler);

  // Start pixel timer
  pixelTimer.begin(pixel_irq, T_HALF_PIXEL_US);
}

void loop() {
  // if(state.busy == false) {
  //   start_readout(true);
  // }
  // delay(100);
}
