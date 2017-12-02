//Uses default ADC sampling frequency = 9600 Hz

#define LOG_OUT 1 // use the log output function
#define FFT_N 128 // set to 256 point fft

#include <FFT.h> // include the library
uint8_t heard = 0;

void setup() {
  Serial.begin(115200); // use the serial port
}

void loop() {
  while (1) { // reduces jitter
    return_freq();
    
    if(heard == 1) {
      delay(700);
    }
    else if (heard == 2){
      Serial.println("GO!");
      while(1);
    }
  }
}

byte return_freq() {
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 128 ; i += 2) { // save 256 samples
    fft_input[i] = analogRead(A5); // put analog input (pin A0) into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();

  /*for (byte i = 0 ; i < FFT_N / 2 ; i++) {
    Serial.print(i);
    Serial.println("...........");
    Serial.println(fft_log_out[i]); // send out the data*/
    
      if(fft_log_out[9] > 70){
      heard++;
      Serial.println("GO!");
      }
  //}
}

