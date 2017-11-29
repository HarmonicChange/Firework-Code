//Uses default ADC sampling frequency = 9600 Hz

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library
int start_time;
int end_time;

void setup() {
  Serial.begin(115200); // use the serial port
}

void loop() {
  while (1) { // reduces jitter
    //start_time = millis();
    //Serial.println(millis());
    return_freq();
    //Serial.println(millis() - start_time);
    //Serial.println(millis());
    //delay(1000);
    //Serial.println();
  }
}

byte return_freq() {
  cli();  // UDRE interrupt slows this way down on arduino1.0
  for (int i = 0 ; i < 512 ; i += 2) { // save 256 samples
    fft_input[i] = analogRead(A0); // put analog input (pin A0) into even bins
    fft_input[i + 1] = 0; // set odd bins to 0
  }
  fft_window(); // window the data for better frequency response
  fft_reorder(); // reorder the data before doing the fft
  fft_run(); // process the data in the fft
  fft_mag_log(); // take the output of the fft
  sei();

  //Serial.println("start");
  //for (byte i = 0 ; i < FFT_N / 2 ; i++) {
    //Serial.println(fft_log_out[18]); // send out the data
    if(fft_log_out[19] > 68){
      Serial.print("GO!");
    };
  //}
  //while (1); //run once, press reset to run again
}

