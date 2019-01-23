

#include <D7S.h>
#include <Wire.h>
//old earthquake data
float oldSI = 0;
float oldPGA = 0;

//flag variables to handle collapse/shutoff only one time during an earthquake
bool shutoffHandled = false;
bool collapseHandled = false;

//function to handle collapse event
void handleCollapse()
{
  //put here the code to handle the collapse event
  Serial.println("-------------------- COLLAPSE! --------------------");
}

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
/*  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }*/

  Serial.print("Starting D7S communications (it may take some time)...");
  //start D7S connection
  D7S.begin();
  D7S.setAxis(SWITCH_AT_INSTALLATION);
  D7S.initialize();
  
//  Wire.begin();

  //wait until the D7S is ready
  while (!D7S.isReady())
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nListening for earthquakes!");
}

void loop()
{

  //checking if there is an earthquake occuring right now
  if (D7S.isEarthquakeOccuring())
  {

    //check if the shutoff event has been handled and if the shutoff condition is met
    //the call of D7S.isInShutoff() is executed after to prevent useless I2C call
    if (!collapseHandled && D7S.isInCollapse())
    {
      handleCollapse();
      collapseHandled = true;
    }

    //print information about the current earthquake
    float currentSI = D7S.getInstantaneusSI();
    float currentPGA = D7S.getInstantaneusPGA();

    if (currentSI > oldSI || currentPGA > oldPGA)
    {
      //getting instantaneus SI
      Serial.print("\tInstantaneus SI: ");
      Serial.print(currentSI);
      Serial.println(" [m/s]");

      //getting instantaneus PGA
      Serial.print("\tInstantaneus PGA (Peak Ground Acceleration): ");
      Serial.print(currentPGA);
      Serial.println(" [m/s^2]\n");

      //save the current data
      oldSI = currentSI;
      oldPGA = currentPGA;
    }
    else
    {
      //reset the old earthquake data
     oldPGA = 0;
      oldSI = 0;
    //reset the flag of the handled events
      shutoffHandled = false;
      collapseHandled = false;
    //reset D7S events
      Serial.println("reset\n");
      D7S.resetEvents();
      D7S.clearMemory(CLEAR_QUAKE);
    }
  }
}
