#include<avr/io.h>                                                      // I2C PURE C NOTHING MORE NOTHING LESS...
#define CPU 16000000UL    
#define SCL 400000UL
#define twbr ((CPU / SCL) - 16) / 2

int16_t data=0;
int16_t AcX=0;
int16_t AcY=0;
int16_t AcZ=0;
int16_t GyX=0;
int16_t GyY=0;
int16_t GyZ=0;
int16_t Tem=0;

void i2cinit(){
  
  TWBR = twbr;                                                           // Set Baud Rate for transmission 
  TWCR = (1<<TWEN);                                                      // Set Two Wire Enable (not necessary)
  
}

void i2cwrite(uint8_t device,uint8_t address,uint8_t data){
  
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);                                //Send start command
  while (!(TWCR & (1<<TWINT)));
  
  TWDR=device;                                                           //Send device id
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  
  TWDR=address;                                                          //Send address
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

  TWDR=data;                                                             //Send data
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);                                //Send stop command
  while ((TWCR & (1<<TWINT)));
  _delay_ms(10);                                                         //Giving some time to stop (Not always necessary)

}

int16_t i2cread(uint8_t device,uint8_t address){
    
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);                                //Send start command
  while (!(TWCR & (1<<TWINT)));
  
  TWDR=device & 0xFE;                                                    //Send device id
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
  
  TWDR=address;                                                          //Send address
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));
    
  TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTA);                                //Send start command
  while (!(TWCR & (1<<TWINT)));

  TWDR=device;                                                           //Send device id
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

  TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);                       //readack
  while(!(TWCR & (1<<TWINT)));
  data= TWDR<<8;
  
  TWCR = (1 << TWINT) | (1 << TWEN);                                     //readNck
  while(!(TWCR & (1<<TWINT)));
  data|= TWDR;

  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);                            // Sending stop conditon
  while((TWCR & (1<<TWINT)));
  
  return data;
}
      
void i2cstop(){
  
  TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);                            // Sending stop conditon
  while((TWCR & (1<<TWINT)));
  
}

int main(){                                                              //Main Function

  Serial.begin(9600);

  while(1){
   i2cinit();
   i2cwrite(0x68<<1|0, 0x6B,0x00);                                       //Power Management     // ( device_id<<1|0 , address , data )   
   AcX=i2cread(0x68<<1|1,0x3B);                                          //X Axis Accelerometer // (device_id<<1|1, address)
   AcY=i2cread(0x68<<1|1,0x3D);                                          //Y Axis Accelerometer // (device_id<<1|1, address)
   AcZ=i2cread(0x68<<1|1,0x3F);                                          //Z Axis Accelerometer // (device_id<<1|1, address)
   Tem=(i2cread(0x68<<1|1,0x41))/340+36.53;                              //Temperature          // (device_id<<1|1, address)
   GyX=i2cread(0x68<<1|1,0x43);                                          //X Axis Gyroscope     // (device_id<<1|1, address)
   GyY=i2cread(0x68<<1|1,0x45);                                          //Y Axis Gyroscope     // (device_id<<1|1, address)
   GyZ=i2cread(0x68<<1|1,0x47);                                          //Z Axis Gyroscope     // (device_id<<1|1, address)
   
   Serial.print("########################################\n");
   Serial.print("AcX=");Serial.print(AcX);Serial.print("| AcY=");Serial.print(AcY);Serial.print("| AcZ=");Serial.println(AcZ);
   Serial.print("\t Tem=");Serial.println(Tem);
   Serial.print("GyX=");Serial.print(GyX);Serial.print("| GyY=");Serial.print(GyY);Serial.print("| GyZ=");Serial.println(GyZ);

}
}
