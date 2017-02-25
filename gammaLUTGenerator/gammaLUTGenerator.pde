// Generate an LED gamma-correction table for Arduino sketches.
// Written in Processing (www.processing.org), NOT for Arduino!
// Copy-and-paste the program's output into an Arduino sketch.

float gamma   = 2.8; // Correction factor
int   max_in  = 100, // Top end of INPUT range
      max_out = 941; // Top end of OUTPUT range

void setup() {
  print("const uint8_t PROGMEM gamma[] = {");
  for(int i=0; i<=max_in; i++) {
    if(i > 0) print(',');
    if((i & 15) == 0) print("\n  ");
    System.out.format("%3d",
      (int)(pow((float)i / (float)max_in, gamma) * max_out + 0.5));
  }
  println(" };");
  exit();
}