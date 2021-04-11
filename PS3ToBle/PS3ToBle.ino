/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_64 
 * 
 * Possible DPAD/HAT switch position values are: 
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * (or HAT_CENTERED, HAT_UP etc)
 *
 * bleGamepad.setAxes takes the following int16_t parameters for the Left/Right Thumb X/Y, uint16_t for the Left/Right Triggers plus slider1 and slider2, and hat switch position as above: 
 * (Left Thumb X, Left Thumb Y, Right Thumb X, Right Thumb Y, Left Trigger, Right Trigger, Hat switch positions (hat1, hat2, hat3, hat4));
 */
 
#include <BleGamepad.h> 
#include <Ps3Controller.h> 

BleGamepad bleGamepad;

#include "esp_bt.h"
//#include "bt_app_core.h"
//#include "bt_app_av.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"

#include "esp_spp_api.h"

int battery = 0;
void ps3_spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
#define PS3_TAG "PS3_SPP"
#define PS3_DEVICE_NAME "PS3 Host"
#define PS3_SERVER_NAME "PS3_SERVER" 

    ESP_LOGE(PS3_TAG, "%s begin event %i\n", __func__, event);

    if (event == ESP_SPP_INIT_EVT) {
        ESP_LOGE(PS3_TAG, "ESP_SPP_INIT_EVT");
        esp_bt_dev_set_device_name(PS3_DEVICE_NAME);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 0, 0)
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
        ESP_LOGE(PS3_TAG, "esp_bt_gap_set_scan_mode");
#else
        esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE);
        ESP_LOGE(PS3_TAG, "esp_bt_gap_set_scan_mode");
#endif

        esp_spp_start_srv(ESP_SPP_SEC_NONE,ESP_SPP_ROLE_SLAVE, 0, PS3_SERVER_NAME);
    }
}

bool printBTAddress()
{
  Serial.println("printBTAddress()");

/*  
    Serial.print("Getting BT address...");
    const uint8_t* point = esp_bt_dev_get_address();
    if (point=NULL) {
      Serial.println("ERR!");
      return false;   
    }
    
    {
      char str[256];
      sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X\n", (int)point[0], (int)point[1], (int)point[2], (int)point[3], (int)point[4], (int)point[5]);
      Serial.println(str);
    }
*/    
    return true;
}

unsigned long startMillis = 0;
unsigned long currentMillis;
const unsigned long period = 10;
void notify()
{
/*
  if (startMillis ==0)
    startMillis = millis();

  currentMillis = millis();  //get the current "time" (actually the number of milliseconds since the program started)
  if (currentMillis - startMillis >= period)  //test whether the period has elapsed
  {    
    startMillis = currentMillis;  //IMPORTANT to save the start time of the current LED state.
  }
  else
  {
    return;
  }

*/
  bool updateBle = false;
  
  if(bleGamepad.isConnected()) 
  {    
     
    //--- Digital cross/square/triangle/circle button events ---
    if( Ps3.event.button_down.cross )
    {
        //Serial.println("Started pressing the cross button");
        bleGamepad.press(BUTTON_1);
        updateBle = true;
    }
        
    if( Ps3.event.button_up.cross )
    {
        //Serial.println("Released the cross button");
        bleGamepad.release(BUTTON_1);
        updateBle = true;
    }
    
    if( Ps3.event.button_down.square )
    {
        //Serial.println("Started pressing the square button");
        bleGamepad.press(BUTTON_2);
        updateBle = true;
    }
    if( Ps3.event.button_up.square )
    {
        //Serial.println("Released the square button");
        bleGamepad.release(BUTTON_2);
        updateBle = true;
    }
    
    if( Ps3.event.button_down.triangle )
    {      
        //Serial.println("Started pressing the triangle button");
        bleGamepad.press(BUTTON_3);        
        updateBle = true;
    }
    if( Ps3.event.button_up.triangle )
    {
        //Serial.println("Released the triangle button");
        bleGamepad.release(BUTTON_3);
        updateBle = true;
    }
    
    if( Ps3.event.button_down.circle )
    {
        //Serial.println("Started pressing the circle button");
        bleGamepad.press(BUTTON_4);        
        updateBle = true;
    }
    if( Ps3.event.button_up.circle )
    {
        //Serial.println("Released the circle button");
        bleGamepad.release(BUTTON_4);
        updateBle = true;
    }
    
    //---------------- Analog stick value events ---------------
    if( abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2 ){
       Serial.print("Moved the left stick:");
       Serial.print(" x="); Serial.print(Ps3.data.analog.stick.lx, DEC);
       Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ly, DEC);
       Serial.println();
       updateBle = true;
    }
    
    if( abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2 ){
       Serial.print("Moved the right stick:");
       Serial.print(" x="); Serial.print(Ps3.data.analog.stick.rx, DEC);
       Serial.print(" y="); Serial.print(Ps3.data.analog.stick.ry, DEC);
       Serial.println();
       updateBle = true;
    }


    bleGamepad.setAxes(Ps3.data.analog.stick.lx, Ps3.data.analog.stick.ly, Ps3.data.analog.stick.rx, Ps3.data.analog.stick.ry, 0 , 0);


    //--------------- Digital D-pad button events --------------

    static uint32_t dpad = 0;
    
    if( Ps3.event.button_down.up )
    {
        //Serial.println("Started pressing the up button");
        updateBle = true;
        dpad |= 1<<0;
    }
    if( Ps3.event.button_up.up )
    {
        //Serial.println("Released the up button");
        updateBle = true;
        dpad &= ~(1<<0);
    }
    
    if( Ps3.event.button_down.right )
    {
        //Serial.println("Started pressing the right button");
        updateBle = true;
        dpad |= 1<<1;
    }
    if( Ps3.event.button_up.right )
    {
        //Serial.println("Released the right button");
        updateBle = true;
        dpad &= ~(1<<1);
    }
    
    
    if( Ps3.event.button_down.down )
    {
        //Serial.println("Started pressing the down button");
        updateBle = true;
        dpad |= 1<<2;
    }
    if( Ps3.event.button_up.down )
    {
        //Serial.println("Released the down button");
        updateBle = true;
        dpad &= ~(1<<2);
    }
    
    if( Ps3.event.button_down.left )
    {
        //Serial.println("Started pressing the left button");
        updateBle = true;
        dpad |= 1<<3;
        
    }
    if( Ps3.event.button_up.left )
    {
        //Serial.println("Released the left button");
        updateBle = true;
        dpad &= ~(1<<3);
    }


    int v = 0;
    switch(dpad)
    {
      case 1 : v= 1; break;
      case 2 : v= 3; break;
      case 4 : v= 5; break;
      case 8 : v= 7; break;

      case 3 : v= 2; break;
      case 6 : v= 4; break;
      case 12 : v= 6; break;
      case 9 : v= 8; break;
    }
   
    bleGamepad.setHat1(v);
        
    /*
    //------------- Digital shoulder button events -------------
    if( Ps3.event.button_down.l1 )
        Serial.println("Started pressing the left shoulder button");
    if( Ps3.event.button_up.l1 )
        Serial.println("Released the left shoulder button");
    
    if( Ps3.event.button_down.r1 )
        Serial.println("Started pressing the right shoulder button");
    if( Ps3.event.button_up.r1 )
        Serial.println("Released the right shoulder button");
    
    //-------------- Digital trigger button events -------------
    if( Ps3.event.button_down.l2 )
        Serial.println("Started pressing the left trigger button");
    if( Ps3.event.button_up.l2 )
        Serial.println("Released the left trigger button");
    
    if( Ps3.event.button_down.r2 )
        Serial.println("Started pressing the right trigger button");
    if( Ps3.event.button_up.r2 )
        Serial.println("Released the right trigger button");
    
    //--------------- Digital stick button events --------------
    if( Ps3.event.button_down.l3 )
        Serial.println("Started pressing the left stick button");
    if( Ps3.event.button_up.l3 )
        Serial.println("Released the left stick button");
    
    if( Ps3.event.button_down.r3 )
        Serial.println("Started pressing the right stick button");
    if( Ps3.event.button_up.r3 )
        Serial.println("Released the right stick button");
    */
    //---------- Digital select/start/ps button events --------- //5,6,7
    if( Ps3.event.button_down.select )
    {
        Serial.println("Started pressing the select button");
        bleGamepad.setSelect(true);
        updateBle = true;
    }
    if( Ps3.event.button_up.select )
    {
        Serial.println("Released the select button");
        bleGamepad.setSelect(false);
        updateBle = true;
    }
    if( Ps3.event.button_down.start )
    {
        Serial.println("Started pressing the start button");
        bleGamepad.setStart(true);
        updateBle = true;
    }
    if( Ps3.event.button_up.start )
    {
        Serial.println("Released the start button");
        bleGamepad.setStart(false);
        updateBle = true;
    }
    
    if( Ps3.event.button_down.ps )
    {
        Serial.println("Started pressing the Playstation button");
        bleGamepad.setMode(true);
        updateBle = true;
    }
    if( Ps3.event.button_up.ps )
    {
        Serial.println("Released the Playstation button");
        bleGamepad.setMode(false);
        updateBle = true;
    }
    
    
    if (updateBle == true)
      bleGamepad.sendReport();
    
    /*
    //--------------- Analog D-pad button events ----------------
    if( abs(Ps3.event.analog_changed.button.up) ){
       Serial.print("Pressing the up button: ");
       Serial.println(Ps3.data.analog.button.up, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.right) ){
       Serial.print("Pressing the right button: ");
       Serial.println(Ps3.data.analog.button.right, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.down) ){
       Serial.print("Pressing the down button: ");
       Serial.println(Ps3.data.analog.button.down, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.left) ){
       Serial.print("Pressing the left button: ");
       Serial.println(Ps3.data.analog.button.left, DEC);
    }
    
    //---------- Analog shoulder/trigger button events ----------
    if( abs(Ps3.event.analog_changed.button.l1)){
       Serial.print("Pressing the left shoulder button: ");
       Serial.println(Ps3.data.analog.button.l1, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.r1) ){
       Serial.print("Pressing the right shoulder button: ");
       Serial.println(Ps3.data.analog.button.r1, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.l2) ){
       Serial.print("Pressing the left trigger button: ");
       Serial.println(Ps3.data.analog.button.l2, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.r2) ){
       Serial.print("Pressing the right trigger button: ");
       Serial.println(Ps3.data.analog.button.r2, DEC);
    }
    
    //---- Analog cross/square/triangle/circle button events ----
    if( abs(Ps3.event.analog_changed.button.triangle)){
       Serial.print("Pressing the triangle button: ");
       Serial.println(Ps3.data.analog.button.triangle, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.circle) ){
       Serial.print("Pressing the circle button: ");
       Serial.println(Ps3.data.analog.button.circle, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.cross) ){
       Serial.print("Pressing the cross button: ");
       Serial.println(Ps3.data.analog.button.cross, DEC);
    }
    
    if( abs(Ps3.event.analog_changed.button.square) ){
       Serial.print("Pressing the square button: ");
       Serial.println(Ps3.data.analog.button.square, DEC);
    }
    */
  }
  else
  {
    Serial.print("#");
    //delay(1000);
  }


   //---------------------- Battery events ---------------------
    if( battery != Ps3.data.status.battery ){
        battery = Ps3.data.status.battery;
        Serial.print("The controller battery is ");
        if( battery == ps3_status_battery_charging )      Serial.println("charging");
        else if( battery == ps3_status_battery_full )     Serial.println("FULL");
        else if( battery == ps3_status_battery_high )     Serial.println("HIGH");
        else if( battery == ps3_status_battery_low)       Serial.println("LOW");
        else if( battery == ps3_status_battery_dying )    Serial.println("DYING");
        else if( battery == ps3_status_battery_shutdown ) Serial.println("SHUTDOWN");
        else Serial.println("UNDEFINED");
    }

   
}

void onConnect(){
    Serial.println("Connected.");
    Ps3.setPlayer(1);     

  Serial.println("Gamepad Begin.");
  bleGamepad.setAutoReport(false);
  bleGamepad.begin();  
    
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  //InitBluetooth();

  Serial.println("PS3 Begin.");
  Ps3.attach(notify);
  Ps3.attachOnConnect(onConnect);
  Ps3.begin();

}

void loop() 
{ 
  if(bleGamepad.isConnected()) 
  {
    /*
    Serial.println("Press buttons 5 and 32. Move all axes to max. Set DPAD (hat 1) to down right.");
    bleGamepad.press(BUTTON_5);
    bleGamepad.press(BUTTON_32);
    bleGamepad.setAxes(32767, 32767, 32767, 32767, 65535, 65535, 65535, 65535, DPAD_DOWN_RIGHT); //(can also optionally set hat2/3/4 after DPAD/hat1 as seen below)
    // All axes, sliders, hats can also be set independently. See the IndividualAxes.ino example
    delay(500);

    Serial.println("Release button 5. Move all axes to min. Set DPAD (hat 1) to centred.");
    bleGamepad.release(BUTTON_5);
    bleGamepad.setAxes(-32767, -32767, -32767, -32767, 0, 0, 0, 0, DPAD_CENTERED, HAT_CENTERED, HAT_CENTERED, HAT_CENTERED);
    */
  }
  else
  {
  Serial.print(".");
  }
  delay(1000);  
}
