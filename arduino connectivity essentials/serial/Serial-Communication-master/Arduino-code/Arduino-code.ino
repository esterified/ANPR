char incomingByte ;   // for incoming serial data
int led = 13;
int flag=0;

void setup()
{
  Serial.begin(9600); // // opens serial port, sets data rate to 9600 bps
 
  pinMode(led, OUTPUT);
}
 
void loop()
{ 
  if(flag==0){digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  }
  else if(flag==1){digitalWrite(led,HIGH); 
    }
    
  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();//not using this
   if(incomingByte=='1'){
  flag=0;
    digitalWrite(led, LOW);
   }
   else{
    flag=1;
    digitalWrite(led,HIGH);
    }
    
   
   
  }
  delay(500);
}
