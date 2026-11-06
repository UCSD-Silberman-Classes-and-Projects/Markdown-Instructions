#include <FUTABA_SBUS.h>
#include <Streaming.h>


FUTABA_SBUS sBus;


void setup(){
  sBus.begin();
  Serial.begin(115200);
  //Serial << "hello world" << "\n";
}

void loop(){
  sBus.FeedLine();
  //Serial << "values available: " << Serial1.available() << "\n";
  if (sBus.toChannels == 1){
    //Serial << "hello world" << "\n";

    //sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;
    Serial<<"channel values: " <<sBus.channels[0]<<","<<sBus.channels[1]<<","<<sBus.channels[2]<<","<<sBus.channels[3]<<","<<sBus.channels[4]<<","<<sBus.channels[5]
    <<","<<sBus.channels[6]<<","<<sBus.channels[7]<<"\r\n";
  }
}
