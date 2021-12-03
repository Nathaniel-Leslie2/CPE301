
//on off switch using GPIO on PB1 or digital pin 52

//define port B pointers
volatile unsigned char* port_b = (unsigned char*) 0x25;
volatile unsigned char* ddr_b = (unsigned char*) 0x24;
volatile unsigned char* pin_b = (unsigned char*) 0x23;

//define button counter
int i=0;
void setup() {
  //turn on pull up resistor
  *port_b |= 0b11111111;

  //set PB1 to input
  *ddr_b &= 0b11111110;
}

void loop() {
  if(*pin_b==0){
    i++;
  }
 
   if(i%2==1){
     //running
    }
    else{
    //not running
    }

}
