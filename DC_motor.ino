//pins D: 5,4,3

//Variable to be passed in from main
int turn_on =0;

//define port E pointers
volatile unsigned char* port_e = (unsigned char*) 0x2E;
volatile unsigned char* ddr_e = (unsigned char*) 0x2D;
volatile unsigned char* pin_e = (unsigned char*) 0x2C;

//define port G pointers
volatile unsigned char* port_g = (unsigned char*) 0x34;
volatile unsigned char* ddr_g = (unsigned char*) 0x33;
volatile unsigned char* pin_g = (unsigned char*) 0x32;


void setup() {
//Set PE5 to output
  *ddr_e |= 0x20;
  
//set PE3 to output
  *ddr_e |= 0x08;
  
//set PG5 to output
  *ddr_g |= 0x20;

}

void loop() {

if(turn_on == 1){
//write a 1 to the enable bit on PE3
*port_e |= 0x08;
}

else{
//write a 0 to the enable bit on PE3
*port_e &= 0x00;
}


//write a 1 to PE5
*port_e |= 0x20;

//write a 0 to PG5
*port_g &= 0x20;






}
