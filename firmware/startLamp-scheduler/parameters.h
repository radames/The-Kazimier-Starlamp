//closest NTP Server
#define NTP_SERVER "0.us.pool.ntp.org"

//GMT Time Zone with sign
#define GMT_TIME_ZONE -7

//MP3 Volume 0-30
#define MP3_VOLUME 25
#define AUDIO_TRACKS 2


//Audio analysis MIN-MAX 0-1024

#define OUT_MIN 0
#define OUT_MAX 1024
#define LED_OSC_PERIOD 10000 // 10 seconds
#define LED_OSC_MAX_AMP 768 // max PWM on Ambient mode


// Audio analisys
#define N_PIXELS  500  // Number of LEDs
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define DC_OFFSET  0  // DC offset in mic signal - if unusure, leave 0
#define NOISE     10  // Noise/hum/interference in mic signal
#define SAMPLES   100  // Length of buffer for dynamic level adjustment
#define TOP       (N_PIXELS + 2) // Allow dot to go slightly off scale
#define PEAK_FALL 10  // Rate of peak falling dot

#define LED_PIN D4

