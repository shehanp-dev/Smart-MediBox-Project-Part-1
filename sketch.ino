// Project: Smart Medibox
// Author: W.A Shehan Piyumantha-220483D

// Includes libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>

// Definition of OLED display parameters
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Definition of pin numbers
#define BUZZER 27
#define LED_1 26
#define PB_CANCEL 16
#define PB_UP 17
#define PB_DOWN 5
#define PB_OK 4
#define DHTPIN 25


//Definition of NTP Server
#define NTP_SERVER "pool.ntp.org"

//Definition of objects
WiFiUDP ntpUDP; // UDP client for NTP
NTPClient timeClient(ntpUDP, NTP_SERVER); // NTP client
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 
DHTesp dhtSensor; 

//Definition of Alarm and Snooze parameters
const int NUM_ALARMS = 2; // Number of alarms supported
const int SNOOZE_MINUTES = 5; // Snooze duration (minutes)

//Menu options
enum MenuOptions {
  SET_TIMEZONE,
  SET_ALARM_1,
  SET_ALARM_2,
  VIEW_ALARMS,
  DELETE_ALARM_1,
  DELETE_ALARM_2,
  NUM_MENU_OPTIONS 
};

// Global variables for notes
int n_notes = 8; 
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int notes[] = {C, D, E, F, G, A, B, C_H};

// Definition of Global Variables
int current_hour, current_minute, current_second; // Current time
bool alarm_enabled[NUM_ALARMS] = {false, false}; // Alarm enable flags
int alarm_hour[NUM_ALARMS] = {0, 0}; // Alarm hours
int alarm_minute[NUM_ALARMS] = {0, 0}; // Alarm minutes
bool alarm_triggered[NUM_ALARMS] = {false, false}; // Alarm triggered flags
MenuOptions current_menu_option = SET_TIMEZONE; // Current menu option
float UTC_offset = 5.50; // Initial UTC offset for Sri Lanka
float new_UTC_offset = UTC_offset;
bool alarm_ringing = false; // Flag to indicate if the alarm is ringing


//Function prototypes
void print_line(String text, int row, int column, int size);
void update_current_time();
void check_alarm();
void display_current_data();
void handle_alarm();
void go_to_menu();
void set_timezone();
void set_alarm(int alarm_index);
void view_alarms();
void delete_alarm(int alarm_index);
void execute_menu_action(MenuOptions option);
int wait_for_button_press();
void play_alarm();
void stop_alarm();

void setup() {
  //Initializing pins
  pinMode(BUZZER, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(PB_CANCEL, INPUT_PULLUP);
  pinMode(PB_OK, INPUT_PULLUP);
  pinMode(PB_UP, INPUT_PULLUP);
  pinMode(PB_DOWN, INPUT_PULLUP);

  //Initializing DHT Sensor
  dhtSensor.setup(DHTPIN, DHTesp::DHT22);

  //Initialize Serial Communication
  Serial.begin(9600);

  //Initialize the OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;) // Stop if OLED fails to initialize
    ;
  }
  display.display();
  delay(500);

  //Connecting to Wi-Fi
  WiFi.begin("Wokwi-GUEST","",6);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    display.clearDisplay(); // Clear the display
    print_line("Connecting to WiFi", 20, 10, 1);

  }
  display.clearDisplay();
  print_line("Connected to WiFi", 20, 10, 1);


  // Initializing the  NTP Client
  timeClient.begin();
  timeClient.setTimeOffset(UTC_offset * 3600);

  display.clearDisplay();
  print_line("Welcome to", 30, 10, 1);
  print_line("Medibox", 40, 30, 1);
  delay(2000);
  display.clearDisplay();

}

void loop() {
  update_current_time(); 
  check_alarm(); 
  display_current_data(); 
  handle_alarm(); // Handle alarm actions

  //Check for button presses to enter menu
  if (digitalRead(PB_OK) == LOW) {
    delay(200); 
    go_to_menu(); 
  }

  delay(100); 
}

//Function to display text on the OLED screen
void print_line(String text, int column, int row, int text_size) {
  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor( column,row);
  display.println(text);
  display.display();
}

//Fetching the current time using NTP server
void update_current_time() {
  timeClient.update();
  current_hour = timeClient.getHours();
  current_minute = timeClient.getMinutes();
  current_second = timeClient.getSeconds();
}

// Function to Check Alarms
void check_alarm() {
  for (int i = 0; i < NUM_ALARMS; i++) {
    if (alarm_enabled[i] && current_hour == alarm_hour[i] && current_minute == alarm_minute[i] && current_second == 0) {
      alarm_triggered[i] = true;
      alarm_ringing = true;
    }
  }
}

// function to display current time, temperature, and humidity
void display_current_data() {
  display.clearDisplay();

  //Displaying the current time on the OLED screen
  String time_str = String(current_hour / 10) + String(current_hour % 10) + ":" +
                    String(current_minute / 10) + String(current_minute % 10) + ":" +
                    String(current_second / 10) + String(current_second % 10);
  print_line(time_str, 20, 0, 2);

  //  Display the temperature and Humidity on the OLED screen
  TempAndHumidity sensor_values = dhtSensor.getTempAndHumidity();
  float temperature = sensor_values.temperature;
  float humidity = sensor_values.humidity;

  String temp_str = "Temp: " + String(temperature, 1) + " C";
  String humidity_str = "Humidity: " + String(humidity, 1) + " %";
  print_line(temp_str, 0, 20, 1);
  print_line(humidity_str, 0, 30, 1);

  // Temperature and humidity werinings
  if (temperature > 32) {
    print_line("TEMPERATURE IS HIGH", 0, 40, 1);
  } else if (temperature < 24) {
    print_line("TEMPERATURE IS LOW", 0, 40, 1);
  } else {
    print_line("TEMPARATURE IS FINE", 0, 40, 1); 
  }
  if (humidity > 80) {
    print_line("HUMIDITY IS HIGH", 0, 50, 1);
  } else if (humidity < 65) {
    print_line("HUMIDITY IS LOW", 0, 50, 1);
  } else {
    print_line("HUMIDITY IS FINE", 0, 50, 1); 
  }
  
  display.display();
}

// Function to handle alarm actions
void handle_alarm() {
    for (int i = 0; i < NUM_ALARMS; i++) {
        if (alarm_triggered[i]) {
          
            display.clearDisplay();
            //print_line("Press OK to ", 0, 10, 1);
            //print_line("CANCEL to snooze", 0, 20, 1);
            display.display();

            //Playing the alarm  and turning on the LED
            play_alarm();
            digitalWrite(LED_1, HIGH); 

            int button_press = wait_for_button_press();

            if (button_press == PB_OK) {
                alarm_triggered[i] = false;
                alarm_ringing = false; 
                stop_alarm();       
                digitalWrite(LED_1, LOW);  

            } else if (button_press == PB_CANCEL) {
                alarm_triggered[i] = false;
                alarm_ringing = false; // Reset the alarm ringing flag
                stop_alarm();       // Stop playing alarm sound
                digitalWrite(LED_1, LOW);   // Turn off LED

                // Snooze the alarm
                alarm_minute[i] = (alarm_minute[i] + SNOOZE_MINUTES) % 60;
                if ((alarm_minute[i] % 60) < SNOOZE_MINUTES) {
                    alarm_hour[i] = (alarm_hour[i] + 1) % 24;
                }
            }
        } else {
            stop_alarm();
            digitalWrite(LED_1, LOW);
        }
    }
}

// Function to go to the menu
void go_to_menu() {
  while (true) {
    display.clearDisplay();
    String menu_text;

    switch (current_menu_option) {
      case SET_TIMEZONE:
        print_line("1.Set Timezone", 20, 30, 1);
        break;       
      case SET_ALARM_1:
        print_line("2.Set Alarm 1", 20, 30, 1);
        break;
      case SET_ALARM_2:
        print_line("3.Set Alarm 2", 20, 30, 1);
        break;
      case VIEW_ALARMS:
        print_line("4.View Alarms",20, 30, 1);
        break;
      case DELETE_ALARM_1:
        print_line("5.Delete Alarm 1",20, 30, 1);
        break;
      case DELETE_ALARM_2:
      print_line("6.Delete Alarm 2", 20, 30, 1);
      break;
      
    }

    display.display();
    
    // Display the current menu option
    int button_press = wait_for_button_press();

    switch (button_press) {
      case PB_UP:
        current_menu_option = (MenuOptions)(((int)current_menu_option + 1) % NUM_MENU_OPTIONS);
        break;
      case PB_DOWN:
        current_menu_option = (MenuOptions)(((int)current_menu_option - 1 + NUM_MENU_OPTIONS) % NUM_MENU_OPTIONS);
        break;
      case PB_OK:
        execute_menu_action(current_menu_option);
        return; 
      case PB_CANCEL:
        return; 
    }
  }
}

// Function to Set Timezone by user input
void set_timezone() {

  while (true) {
    display.clearDisplay();           
    print_line("Enter UTC Offset", 20, 10, 1);             
    print_line(String(new_UTC_offset, 1),50, 30, 1); 
    display.display();

    int button_press = wait_for_button_press();

    if (button_press == PB_UP) {
      new_UTC_offset += 0.5; 
      if (new_UTC_offset > 14) {
        new_UTC_offset = 14;
      }
    } else if (button_press == PB_DOWN) {
      new_UTC_offset -= 0.5; 
      if (new_UTC_offset < -12) {
        new_UTC_offset = -12; 
      }
    } else if (button_press == PB_OK) {
      UTC_offset = new_UTC_offset;
      timeClient.setTimeOffset(UTC_offset * 3600); 
      display.clearDisplay();
      print_line("Timezone set!",20, 10, 1);
      display.display();
      delay(1000);
      break;
    } 
  }
}

//Function to Set Alarm by user input
void set_alarm(int alarm_index) {

  //Set Alarm Time 
  int temp_hour = alarm_hour[alarm_index];
  int temp_minute = alarm_minute[alarm_index];

  while (true) {
    display.clearDisplay();
    print_line("Set Alarm " + String(alarm_index + 1) + " Hour", 20, 10, 1);
    print_line(String(temp_hour), 60, 30, 1);
    display.display();

    int button_press = wait_for_button_press();
    if (button_press == PB_UP) {
      temp_hour = (temp_hour + 1) % 24;
    } else if (button_press == PB_DOWN) {
      temp_hour = (temp_hour+23) % 24;
    } else if (button_press == PB_OK) {
      alarm_hour[alarm_index] = temp_hour;
      break;
    } 
  }

  
  while (true) {
    display.clearDisplay();
    print_line("Set Alarm " + String(alarm_index + 1) + " Minute", 20, 10, 1);
    print_line(String(temp_minute), 60, 30, 1);
    display.display();

    int button_press = wait_for_button_press();
    if (button_press == PB_UP) {
      temp_minute = (temp_minute + 1) % 60;
    } else if (button_press == PB_DOWN) {
      temp_minute = (temp_minute  + 59) % 60;
    } else if (button_press == PB_OK) {
      alarm_minute[alarm_index] = temp_minute;
      alarm_enabled[alarm_index] = true;
      break;
    }   
  }

  display.clearDisplay();
  print_line("Alarm " + String(alarm_index + 1) + " set!", 20, 20, 1);
  display.display();
  delay(1000);
}

//Function to view alarms through the menu
void view_alarms() {
  Serial.println("Viewing alarms.");
  display.clearDisplay();
  for (int i = 0; i < NUM_ALARMS; i++) {
    String alarm_status = "Alarm " + String(i + 1) + ": ";
    if (alarm_enabled[i]) {
      alarm_status += String(alarm_hour[i]) + ":" + String(alarm_minute[i]);
    } else {
      alarm_status += "Disabled";
    }
    print_line(alarm_status, 0, i * 10, 1);
  }
  display.display();
  delay(2000);
}

// Function to delete specific alarm 
void delete_alarm(int alarm_index) {
  alarm_enabled[alarm_index] = false;
  display.clearDisplay();
  print_line("Alarm " + String(alarm_index + 1) + " deleted!", 20, 20, 1);
  display.display();
  delay(1000);
}

//Function to select menu options
void execute_menu_action(MenuOptions option) {
  switch (option) {
    case SET_TIMEZONE:
      set_timezone();
      break;
    case SET_ALARM_1:
      set_alarm(0);
      break;
    case SET_ALARM_2:
      set_alarm(1);
      break;
    case VIEW_ALARMS:
      view_alarms();
      break;
    case DELETE_ALARM_1:
      delete_alarm(0);
      break;
    case DELETE_ALARM_2:
      delete_alarm(1);
      break;
  }
}

// Function to wait for button press and return the button pressed
int wait_for_button_press() {
    while (true) {
        if (digitalRead(PB_UP) == LOW) {
            delay(50); // Short debounce delay
            while (digitalRead(PB_UP) == LOW); // Wait for release
            return PB_UP;
        }
        if (digitalRead(PB_DOWN) == LOW) {
            delay(50); // Short debounce delay
            while (digitalRead(PB_DOWN) == LOW); // Wait for release
            return PB_DOWN;
        }
        if (digitalRead(PB_OK) == LOW) {
            delay(50); // Short debounce delay
            while (digitalRead(PB_OK) == LOW); // Wait for release
            return PB_OK;
        }
        if (digitalRead(PB_CANCEL) == LOW) {
            delay(50); // Short debounce delay
            while (digitalRead(PB_CANCEL) == LOW); // Wait for release
            return PB_CANCEL;
        }
        delay(10); // Very short delay to prevent busy-waiting
    }
}

//Function to play the alarm
void play_alarm() {
  tone(BUZZER, 1000);
  print_line("MEDICINE!", 10, 10, 2);
  print_line("TIME", 10, 30, 2);

  for (int i = 0; i < n_notes; i++) {
      tone(BUZZER, notes[i]);
      delay(500);
    }
}
//Function to stop the alarm 
void stop_alarm() {
  noTone(BUZZER);
}
