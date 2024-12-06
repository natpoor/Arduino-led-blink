// For the ICA Denver 2025 Conference Badge project
// Xbox-like controller, with blinky LEDs where the controller buttons are.
// you can digitalRead() the state of a pin set to be an OUTPUT
// https://www.arduino.cc/en/Tutorial/BuiltInExamples/BlinkWithoutDelay
// Fall 2024

// Adafruit Metro setup for breadboard and code testing.

// Global Variables
// YRGB is the order of the lights, starting at noon and going clockwise (based on the buttons on an XBox 360 controller).
// SET PINS HERE
const int led_yellow = 10; // Metro pin numbers
const int led_red    = 9;  // 10, 9, 6, 5 are PWM pins
const int led_green  = 6;
const int led_blue   = 5;
const int button_pin = 12;

// Buttons setup here.
// https://docs.arduino.cc/built-in-examples/digital/StateChangeDetection/
int button_state = 0;  // Variable for reading the pushbutton status.
int button_prev  = 0;  // LOW. For the previous state of the button, helps get one click not a human-length hold/click.

// Pattern varaibles.
const int num_patterns = 13; // How many pattens do I have.
int pattern = 1; // 1 to num_patterns for # patterns of LED blinking.

void setup() {
  //Serial.begin(9600);  // debugging output
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);

  pinMode(button_pin, INPUT);

  init_off(); // Make sure all LEDs are set to LOW to start.
}

void loop() {
  if (pattern == 1)  blink();        // Done
  if (pattern == 2)  pulse();        // Done
  if (pattern == 3)  rotate();       // Done
  if (pattern == 4)  alternate();    // Done
  if (pattern == 5)  snake();        // Done start can be off if button held down(?), is okay though.
  if (pattern == 6)  rave();         // Done
  if (pattern == 7)  rando();        // Done
  if (pattern == 8)  random_path();  // Done
  if (pattern == 9)  bounce();       // Done
  if (pattern == 10) sideside();     // Done
  if (pattern == 11) fire();         // Done
  if (pattern == 12) fireworks();    // Done
  if (pattern == 13) count();        // Done but needs fancy ending.
} // main loop



// SECONDARY FUNCTIONS these are either about the button, are called by the primary functions, or are support (like the inits).
// That is, none of these are the actual pattern functions themselves (but some are required by some of the pattern functions).

// Helper functions that set LEDs to various states.
void init_off() { 
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, LOW);
}
void init_on() {
  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_blue, HIGH);
}

// For the mode/pattern button, which pattern by number.
void increment() {
  pattern++; // Pattern is a global var so it is just set, not returned.
  if (pattern > num_patterns) pattern = 1;
  init_off(); // Because if you're in this function, the mode button was pushed, so start new pattern.
  //Serial.println("Incremented!");
  //Serial.println(pattern);
  delay(750); // Perhaps avoid bounce?
}

// If the LEDs are on turn them off and vice-versa, for the blink pattern.
void reverse_all() {
  if (digitalRead(led_red) == HIGH) init_off(); // If one is on, they are all on, turn all off.
  else init_on();
} // reverse_all

// These two are used by the rave function, if off, turn it on and return an "on until" time, etc. 
unsigned long rave_on(int which_led){
  digitalWrite(which_led, HIGH);
  unsigned long on_until = millis() + random(50, 250);
  return on_until;
}
unsigned long rave_off(int which_led) {
  digitalWrite(which_led, LOW); 
  unsigned long off_until = millis() + random(50, 250);
  return off_until;
}

// All of these four turn one on and make sure the others are off.
void yellow_only() {
  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, LOW);
}
void red_only() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_red, HIGH);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, LOW);
}
void green_only() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_blue, LOW);
}
void blue_only() {
  digitalWrite(led_yellow, LOW);
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, LOW);
  digitalWrite(led_blue, HIGH);
}

// Helper function for the count function. Is given a pointer to the array in that function. 
void count_blinks(bool* marks, int led_count) {   // pointer to the passed-in array
  if (led_count == 0) {marks[0] = 0; marks[1] = 0; marks[2] = 0; marks[3] = 0;}
  if (led_count == 1) {marks[0] = 1; marks[1] = 0; marks[2] = 0; marks[3] = 0;}
  if (led_count == 2) {marks[0] = 1; marks[1] = 1; marks[2] = 0; marks[3] = 0;}
  if (led_count == 3) {marks[0] = 1; marks[1] = 1; marks[2] = 1; marks[3] = 0;}
  if (led_count == 4) {marks[0] = 1; marks[1] = 1; marks[2] = 1; marks[3] = 1;}
}

// Gamma Correction Function (for fading the LEDs).
int gamma_correct(int uncorrected_level) {
  // Convert 0-255 to 0.0-1.0 .
  float decimal_level = uncorrected_level / 255.0; // "If doing math with floats, you need to add a decimal point, otherwise it will be treated as an int."  = 0
  // Gamma adjust.
  float corrected_decimal = decimal_level * decimal_level * decimal_level;
  // Convert back to 0-255 metric and return it.
  return corrected_decimal * 255;
}



// PRIMARY FUNCTIONS these are the patterns (but may call a seconadry function for common actions).

void blink() {
  unsigned long start = millis();
  unsigned long now;
  init_on();
  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - start >= 500) {
      reverse_all();  //
      start = millis();
    }
  } // while
} // blink


void pulse() {
  // Like blink but with a fade-in and fade-out, requires PWM pins or does not work.
  bool increasing = true;
  int x = 0; // Counter for the light level, ont gamma adjusted.
  int y = 0; // The gamma-adjusted 0-255 for the LED light/power level. 
  unsigned long previous_millis = millis();
  unsigned long now = 0;
  const int interval = 50; // Time between change for LEDs in ms.

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - previous_millis >= interval) {  // If so, time to +/- LEDs.
      previous_millis = millis();
      if (x >= 210) increasing = false; // If it reaches the max, then should decrease - moved these two out of the ifs.
      if (x <= 0) increasing = true;    // Similar but for the min, should then increase.
      if (increasing) {
        x = x + 10;
        y = gamma_correct(x);
        analogWrite(led_yellow, y);
        analogWrite(led_red, y);
        analogWrite(led_green, y);
        analogWrite(led_blue, y);
      }
      else {           // else increase is false, so decrease.
        x = x - 10;
        y = gamma_correct(x);
        analogWrite(led_yellow, y);
        analogWrite(led_red, y);
        analogWrite(led_green, y);
        analogWrite(led_blue, y);
      }
    } // if it is time to +/- the LEDs
  } // while
} // pulse


void rotate() {
  unsigned long now = millis();
  unsigned long y_off = now + 450;
  unsigned long r_off = now + 900;
  unsigned long g_off = now + 1350;
  unsigned long b_off = now + 1800;

  digitalWrite(led_yellow, HIGH);

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }

    if (now >= y_off) {
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_red, HIGH);
      y_off = now + 1800;
      r_off = now + 450;
    } 
    else if (now >= r_off) {
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, HIGH);
      r_off = now + 1800;
      g_off = now + 450;
    }
    else if (now >= g_off) {
      digitalWrite(led_green, LOW);
      digitalWrite(led_blue, HIGH);
      g_off = now + 1800;
      b_off = now + 450;
    }
    else if (now >= b_off) {
      digitalWrite(led_blue, LOW);
      digitalWrite(led_yellow, HIGH);
      b_off = now + 1800;
      y_off = now + 450;
    }   
  } // while
} // rotate


void alternate() {
  // Up and down, then left and right.
  const int alternate_delay = 500; // The millis delay for this function.
  unsigned long ud_on = millis();  // Up Down when it has turned on.
  unsigned long lr_on = ud_on + alternate_delay; // Left Right on.
  unsigned long now;

  digitalWrite(led_yellow, HIGH);
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, LOW);
  digitalWrite(led_blue, LOW);

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - ud_on >= alternate_delay) {
      digitalWrite(led_yellow, LOW);
      digitalWrite(led_green, LOW);
      digitalWrite(led_red, HIGH);
      digitalWrite(led_blue, HIGH);
      ud_on = lr_on + alternate_delay; // e.g., 500 + 500
    }
    else if (now - lr_on >= alternate_delay) {
      digitalWrite(led_yellow, HIGH);
      digitalWrite(led_green, HIGH);
      digitalWrite(led_red, LOW);
      digitalWrite(led_blue, LOW);
      lr_on = ud_on + alternate_delay;
    }
  } // while
} // alternate


void snake() {
  // grows and then shrinks    YRGB
  // Y on at   0, on for 1250
  // R on at 250, on for 1250
  // G on at 500, on for 1250
  // B on at 750, on for 1250
  // then pause when all off again

  const int on_for    = 1250;
  const int end_pause = 1250;

  unsigned long y_on  =   0;
  unsigned long r_on  = 250;
  unsigned long g_on  = 500;
  unsigned long b_on  = 750;
  unsigned long y_off = y_on + on_for;
  unsigned long r_off = r_on + on_for;
  unsigned long g_off = g_on + on_for;
  unsigned long b_off = b_on + on_for;
  unsigned long now;

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now >= y_on) {
      digitalWrite(led_yellow, HIGH);
      y_on = y_off + end_pause;
    }
    if (now >= r_on) {
      digitalWrite(led_red, HIGH);
      r_on = r_off + end_pause;
    }
    if (now >= g_on) {
      digitalWrite(led_green, HIGH);
      g_on = g_off + end_pause;
    }
    if (now >= b_on) {
      digitalWrite(led_blue, HIGH);
      b_on = b_off + end_pause;
    }
    if (now > y_off) {
      digitalWrite(led_yellow, LOW);
      y_off = y_on + on_for;
    }
    if (now > r_off) {
      digitalWrite(led_red, LOW);
      r_off = r_on + on_for;
    }
    if (now > g_off) {
      digitalWrite(led_green, LOW);
      g_off = g_on + on_for;
    }
    if (now > b_off) {
      digitalWrite(led_blue, LOW);
      b_off = b_on + on_for;
    }
  } // while
} // snake


void rave() {
  // This is random on all of them: random(min, max+1).
  unsigned long yellow_on_until  = 0;
  unsigned long yellow_off_until = 0;
  unsigned long red_on_until     = 0;
  unsigned long red_off_until    = 0;
  unsigned long green_on_until   = 0;
  unsigned long green_off_until  = 0;
  unsigned long blue_on_until    = 0;
  unsigned long blue_off_until   = 0;
  unsigned long now = 0;

  // Start all on with random rave on values.
  yellow_on_until = rave_on(led_yellow);
  red_on_until = rave_on(led_red);
  green_on_until = rave_on(led_green);
  blue_on_until = rave_on(led_blue);

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    // If on, should it be turned off? If off, should it be turned on?
    if (digitalRead(led_yellow) == HIGH) { if (now > yellow_on_until) yellow_off_until = rave_off(led_yellow); }
    else { if (now > yellow_off_until) yellow_on_until = rave_on(led_yellow); }

    if (digitalRead(led_red) == HIGH) { if (now > red_on_until) red_off_until = rave_off(led_red); }
    else { if (now > red_off_until) red_on_until = rave_on(led_red);  }

    if (digitalRead(led_green) == HIGH) { if (now > green_on_until) green_off_until = rave_off(led_green); }
    else { if (now > green_off_until) green_on_until = rave_on(led_green); }

    if (digitalRead(led_blue) == HIGH) { if (now > blue_on_until) blue_off_until = rave_off(led_blue); }
    else { if (now > blue_off_until) blue_on_until = rave_on(led_blue); }

  } // while
} // rave function


void rando() {
  const int delay = 333;
  int prev   = 0;
  int prev_2 = 0;
  
  unsigned long now   = 0;
  unsigned long start = millis();

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - start >= delay) {
      start = millis();
      int roll = random(1,5); // Gives it a value so you can compare on next line.
      while (roll == prev_2) {roll = random(1,5);} // Reroll -- allows for 1 match but not more than that.
      prev_2 = prev; // Okay passed, so update these two.
      prev = roll;
      if (roll == 1) {
        yellow_only();
      }
      else if (roll == 2) {
        red_only();
      }
      else if (roll == 3) {
        green_only();
      }
      else if (roll == 4) {
        blue_only();
      }
    } // if now - start
  } // while true
} // rando


void random_path() {
  // Random, but only activates a neighbor (a "path"). 
  unsigned long last = 0;
  unsigned long now = 0; 
  int delay = 400; 
  int which = 1; // 1y 2r 3g 4b
  int upit = 0; // Int read as Boolean, random, "up it" up the value or not, beware wrap 1-4.

  digitalWrite(led_yellow, HIGH);
  last = millis();

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - last >= delay) {   // Time to move it one.
      // First get the new LED number.
      upit = random(0,2); // Returns 0,1 can be read as Boolean.
      last = millis(); // Update that.
      if (upit) {
        which++;
        if (which > 4) which = 1;
      }
      else { // else don't up-it, lower it 1-4.
        which--;
        if (which < 1) which = 4;
      }
      if (which == 1) yellow_only();
      else if (which == 2) red_only();
      else if (which == 3) green_only();
      else blue_only();
    } // if time to move it one

  } // while true
} // random_path


void bounce() { // 1, then 2&4, then 3, then 2&4, then 1, etc
  // Similar to sideside function.
  int which_step = 1; // which step of the pattern, so which is on and which is next (1, 2, 3, 4)
  unsigned long now = 0;
  unsigned long last = millis();
  const int delay = 333;

  yellow_only(); // start

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - last >= delay) {
      last = millis();
      which_step++;
      if (which_step > 4) which_step = 1;
      if      (which_step == 1) yellow_only();
      else if (which_step == 2) {digitalWrite(led_red, HIGH); digitalWrite(led_blue, HIGH); digitalWrite(led_yellow, LOW);}
      else if (which_step == 3) green_only();
      else if (which_step == 4) {digitalWrite(led_red, HIGH); digitalWrite(led_blue, HIGH); digitalWrite(led_green, LOW);}
    }
  } // while true
} // bounce


void sideside() { // u, r, d, r, u, l, d, l, u, etc, 1 2 3 4 for position, do where in pattern?
  int which_step = 1; // which step of the pattern, so which is on and which is next
    // 1u 2r 3d 4r 5u 6l 7d 8l 9u that is the start again so 1-8
  unsigned long now = 0;
  unsigned long last = millis();
  const int delay = 333;

  yellow_only(); // start

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - last >= delay) {
      last = millis();
      which_step++;
      if (which_step > 8) which_step = 1;
      if      (which_step == 1) yellow_only();
      else if (which_step == 2) red_only();
      else if (which_step == 3) green_only();
      else if (which_step == 4) red_only();
      else if (which_step == 5) yellow_only();
      else if (which_step == 6) blue_only();
      else if (which_step == 7) green_only();
      else if (which_step == 8) blue_only();
    }
  } // while true
} // sideside


void fire() {
  // Start on blue, blink three times, fire aross to red, bounce back to y/g and b.
  const unsigned long pause_between = 1750;
  // Seven blinks in the pattern. Hard code the entire thing why not.
  unsigned long starter   = 0;
  unsigned long step1_on  = 100; // B blinks three times.
  unsigned long step1_off = 200;
  unsigned long step2_on  = 300; 
  unsigned long step2_off = 400;
  unsigned long step3_on  = 500;
  unsigned long step3_off = 1000;  // longer pause before firing  1000 so far
  unsigned long step4_on  = 1050;   // b fires
  unsigned long step4_off = 1100;
  unsigned long step5_on  = 1150;   // hits r
  unsigned long step5_off = 1200;
  unsigned long step6_on  = 1250;   // bounces back to y/g
  unsigned long step6_off = 1300;
  unsigned long now;

  unsigned long start = millis();
  while (true) {
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    now = millis();
    if (now - start >= starter) {digitalWrite(led_blue, HIGH); starter += (1550 + pause_between);}
    else if (now - start > step1_on) {digitalWrite(led_blue, LOW); step1_on += (1550 + pause_between);}
    else if (now - start > step1_off) {digitalWrite(led_blue, HIGH); step1_off += (1550 + pause_between);}
    else if (now - start > step2_on) {digitalWrite(led_blue, LOW); step2_on += (1550 + pause_between);}
    else if (now - start > step2_off) {digitalWrite(led_blue, HIGH); step2_off += (1550 + pause_between);}
    else if (now - start > step3_on) {digitalWrite(led_blue, LOW); step3_on += (1550 + pause_between);}
    else if (now - start > step3_off) {digitalWrite(led_blue, HIGH); step3_off += (1550 + pause_between);}   // blue fires
    else if (now - start > step4_on) {digitalWrite(led_blue, LOW); step4_on += (1550 + pause_between);}
    else if (now - start > step4_off) {digitalWrite(led_red, HIGH); step4_off += (1550 + pause_between);}  // s4 done, red lights up
    else if (now - start > step5_on) {digitalWrite(led_red, LOW); step5_on += (1550 + pause_between);}
    else if (now - start > step5_off) {digitalWrite(led_yellow, HIGH); digitalWrite(led_green, HIGH); step5_off += (1550 + pause_between);}  // red done, y and g light up
    else if (now - start > step6_on) {digitalWrite(led_yellow, LOW); digitalWrite(led_green, LOW); step6_on += (1550 + pause_between);}
    else if (now - start > step6_off) {
      digitalWrite(led_blue, HIGH); 
      step6_off += (1550 + pause_between);
      int x = 250; // HIGH val for LED rounded down a bit.
      float y = 0.0;
      unsigned long previous_millis = millis();
      while (true){
        now = millis();
        button_prev = button_state; 
        button_state = digitalRead(button_pin);
        if (button_state == HIGH && button_prev == LOW) { // signal from mode button
          increment();
          return; // break out of the function completely
        }
        if (x <= 0) break; // should break out of this inner while but not return
        if (now - previous_millis >= 50) { 
          previous_millis = millis();
          x = x - 10;
          y = gamma_correct(x);
          analogWrite(led_blue, y);
        } 
      } // while inner true / button catch  
    } // If step_6 is completed.
  } // while true
} // fire


void fireworks() {
  // g pops, then y lights up and fades and r/b sparkle (back forth fast, and fade), like a firework. Requires PWM.
  unsigned long now;
  unsigned long start = millis();
  unsigned long previous_millis = millis(); // for fade cycle
  const int fade_interval = 75;
  bool sparkle_lr = 1; // to alternate (sparkle) the two side LEDs
  int x = 250; // for brightness of LED fade
  int y; // gamma corrected version of our good friend x

  const int pause_between   = 2500; // This is important or the green fires too soon. Also interacts with fade_interval and x.
  unsigned long step1_start = 500;  // Initial firing of the firework.
  unsigned long step1_end   = 575;
  unsigned long step2_start = 1000;

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - start > step1_start && now - start < step1_end) digitalWrite(led_green, HIGH);
    else if (now - start > step1_end) {
      digitalWrite(led_green, LOW);
    }
    if (now - start > step2_start && x > 0) { // If in step2, x is the LED fade.
      if (now - previous_millis >= fade_interval) {  // If so, time to +/- LEDs.
        sparkle_lr = !sparkle_lr;
        previous_millis = millis();
          x = x - 7;
          y = gamma_correct(x);
          analogWrite(led_yellow, y);
          if (sparkle_lr) {analogWrite(led_blue, y); digitalWrite(led_red, LOW);}
          else {analogWrite(led_red, y); digitalWrite(led_blue, LOW);}
      } // if it is time to +/- the LEDs
    }  // if in step2
    else if (x <= 0) {
      step1_start = step1_start + pause_between;  // If these step1 aren't here, they get out of sync due to x not being millis-centric. 
      step1_end   = step1_end + pause_between;
      step2_start = step2_start + pause_between;
      start       = start + pause_between;
      init_off();
      x = 250;
    }

  } // while true
} // fireworks


void count() {
  // When they all reach 4, can flash and restart.
  int y_num = 0;  // Number of times they blink each second, 0-4.
  int r_num = 0;
  int g_num = 1;  // Do G B R Y for a different order (is bottom-up).
  int b_num = 0;

  bool y_marks[4];  // Array of 4 bools for y/n LED turning on/off at which marks in the one second cycle. 
  bool r_marks[4];  // So, 4 marks because if on they always blink off after that mark.
  bool g_marks[4];
  bool b_marks[4];

  count_blinks(g_marks, g_num); // Get bools for if it on/offs at which times.
  count_blinks(b_marks, b_num);
  count_blinks(r_marks, r_num);
  count_blinks(y_marks, y_num);

  const int on_for = 125; // 1/8 of a second, so can on/off 4 times per second. Noted but not used.

  unsigned long cycle_start = millis();
  unsigned long now;

  while (true) {
    now = millis();
    button_prev = button_state; 
    button_state = digitalRead(button_pin);
    if (button_state == HIGH && button_prev == LOW) { // signal from mode button
      increment();
      return; // break out of the function
    }
    if (now - cycle_start >= 1000) {
      cycle_start = now;                  // Updates the cycle timing here.
      g_num++;
      if (g_num == 5) {b_num++;}
      if (b_num == 5) {r_num++;}
      if (r_num == 5) {y_num++;}
      if (g_num == 5 && b_num == 5 && r_num == 5 && y_num == 5) {
        // do the success flashing here
        init_on();
        delay(333); // Probably not the best way to do this.
        init_off(); // Who would push the button as this point.
        delay(333);
        init_on();
        delay(333);
        init_off();
        delay(333);
        init_on();
        delay(333);
        init_off();
        delay(333);
      }
      if (g_num == 5) {g_num = 0;} // Green reset to 0 otherwise the 4-then-1 looks like 5. 
      if (b_num == 5) {b_num = 0;}
      if (r_num == 5) {r_num = 0;}
      if (y_num == 5) {y_num = 0;}

      count_blinks(g_marks, g_num); // Get bools for if it on/offs at which times.
      count_blinks(b_marks, b_num); // These here are updated after each 1-second cycle. 
      count_blinks(r_marks, r_num);
      count_blinks(y_marks, y_num);
    } // one-second cycles
    // Main cycle checking here, LEDs on and off. 
    if (now - cycle_start >= 0 && now - cycle_start < 125) { // step 1
      if (g_marks[0]) digitalWrite(led_green, HIGH);
      else digitalWrite(led_green, LOW);
      if (b_marks[0]) digitalWrite(led_blue, HIGH);
      else digitalWrite(led_blue, LOW);
      if (r_marks[0]) digitalWrite(led_red, HIGH);
      else digitalWrite(led_red, LOW);
      if (y_marks[0]) digitalWrite(led_yellow, HIGH);
      else digitalWrite(led_yellow, LOW);
    }
    if (now - cycle_start >= 125 && now - cycle_start < 250) { // step 1 off
      init_off(); 
    }
    if (now - cycle_start >= 250 && now - cycle_start < 375) { // step 2
      if (g_marks[1]) digitalWrite(led_green, HIGH);
      else digitalWrite(led_green, LOW);
      if (b_marks[1]) digitalWrite(led_blue, HIGH);
      else digitalWrite(led_blue, LOW);
      if (r_marks[1]) digitalWrite(led_red, HIGH);
      else digitalWrite(led_red, LOW);
      if (y_marks[1]) digitalWrite(led_yellow, HIGH);
      else digitalWrite(led_yellow, LOW);
    }
    else if (now - cycle_start >= 375 && now - cycle_start < 500) { // step 2 off
      init_off(); 
    }
    else if (now - cycle_start >= 500 && now - cycle_start < 625) { // step 3
      if (g_marks[2]) digitalWrite(led_green, HIGH);
      else digitalWrite(led_green, LOW);
      if (b_marks[2]) digitalWrite(led_blue, HIGH);
      else digitalWrite(led_blue, LOW);
      if (r_marks[2]) digitalWrite(led_red, HIGH);
      else digitalWrite(led_red, LOW);
      if (y_marks[2]) digitalWrite(led_yellow, HIGH);
      else digitalWrite(led_yellow, LOW);
    }
    else if (now - cycle_start >= 625 && now - cycle_start < 750) { // step 3 off
      init_off(); 
    }
    else if (now - cycle_start >= 750 && now - cycle_start < 875) { // step 4
      if (g_marks[3]) digitalWrite(led_green, HIGH);
      else digitalWrite(led_green, LOW);
      if (b_marks[3]) digitalWrite(led_blue, HIGH);
      else digitalWrite(led_blue, LOW);
      if (r_marks[3]) digitalWrite(led_red, HIGH);
      else digitalWrite(led_red, LOW);
      if (y_marks[3]) digitalWrite(led_yellow, HIGH);
      else digitalWrite(led_yellow, LOW);
    }
    else if (now - cycle_start >= 875 && now - cycle_start < 1000) { // step 4 off
      init_off(); 
    }

  } // while true
} // count 

