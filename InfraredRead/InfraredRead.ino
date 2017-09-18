// do #defines BEFORE #includes
#define LOG_OUT 1 // use the log output functio
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // include the library

int numSamples=0;
int i = 0;
long t, t0;

void setup()
{
  Serial.begin(115200);

  ADCSRA = 0;             // clear ADCSRA register
  ADCSRB = 0;             // clear ADCSRB register
  ADMUX |= (0 & 0x07);    // set A0 analog input pin
  ADMUX |= (1 << REFS0);  // set reference voltage
  ADMUX |= (1 << ADLAR);  // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0);    // 32 prescaler for 38.5 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}

ISR(ADC_vect)
{
  if (i<512) {
    byte m = ADCL; // fetch adc data
    byte j = ADCH;

    // form into a 16b signed int
    int k = (j << 8) | m;
    k -= 0x0200;
    k <<= 6;
    
    fft_input[i] = k; // put real data into even bins
    fft_input[i+1] = 0; // set odd bins to 0
    i=i+2;
  }
}
  
void loop()
{
  if (i>=512)
  {
    // window data, then reorder, then run, then take output
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log();
    //fft_mag_lin8(); // take the output of the fft

    /*for (int k = 0; k < 128; k++)
      Serial.println(fft_log_out[k]); // print FFT data
    while(1){}*/
    
    delay(1000);
    // restart
    i = 0;
    for(int k = 47; k < 50; k++)
      if(fft_log_out[k] > 170)Serial.println("7kHz Treasure");
    for(int k = 81; k < 84; k++)
      if(fft_log_out[k] > 170)Serial.println("12kHz Treasure");
  }
}
