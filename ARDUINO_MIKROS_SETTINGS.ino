/*
 * 0 - read temperature;
 * 1 - write coeff;
 * 2 - REZ commads;
 * 3 - command for temperature transfer; 
 * 4 - write address; 
 * 5 - search CRC and REZ
 */
#include <OneWire.h>
#include <MegunoLink.h>

byte bufData[9];
char notInvBinaryCodeChar[16];
int binaryCode[16];
int variableList[16];
int commands;
boolean flag= false; //this flag show read temperatu
boolean flag_SET = false;
int iterator = 0;
int collection[512];
char list_coef_all_sxem[1088];
byte list_adreses[100][8];

void readDataFromPort(OneWire ds, bool flag);
void sendingAPackage(OneWire ds);
void giveMeTemperature(OneWire ds);
  //OneWire dq(8);
  OneWire dq(22);
  OneWire dw(23);
  OneWire de(24);
  OneWire dr(25);
  OneWire dt(26);
  OneWire dy(27);
  OneWire du(28);
  OneWire di(29);
  OneWire dp(30);
  OneWire da(31);
  OneWire dd(32);
  OneWire df(33);
  OneWire dg(34);
  OneWire dh(35);
  OneWire dj(36);
  OneWire dk(37);
  OneWire dl(38);
  OneWire dz(39);
  OneWire dx(40);
  OneWire dc(41);
  OneWire dv(42);
  OneWire db(43);
  OneWire dn(44);
  OneWire dm(45);
  OneWire dqq(46);
  OneWire dqw(47);
  OneWire dqe(48);
  OneWire dqr(49);
  OneWire dqt(50);
  OneWire dqy(51);
  OneWire dqu(52);
  OneWire dqi(53);

void setup() {
  Serial.begin(9600);
}

void loop() {
OneWire list[32] = {dq,dw,de,dr,dt,dy,du,di,dp,da,dd,df,dg,dh,dj,dk,dl,dz,dx,dc,dv,db,dn,dm,dqq,dqw,dqe,dqr,dqt,dqy,dqu,dqi};
  if (flag){
    for (int i=0; i<512; i++){
      collection[i]=0;  
    }

    for(int i=0; i<32; i++){
      readDataFromPort(list[i],true);
    }
    int j = 0;
    for (int i=0; i<512; i++){
      if (j<16){
        Serial.print(collection[i]);
        j=j+1;
      } else {
        Serial.println();
        Serial.print(collection[i]);
        j=1;
      } 
    }
    iterator=0;
    flag = false;
  }else{
    commands = -1;
    flag = false;
    commands = Serial.read();
    if(commands=='0'){
      flag = true;
    }else if(commands=='2'){
      for(int i=0; i<32; i++){
        readDataFromPort(list[i],false);
      }
    }else if(commands=='1'){ //1 - write coeff;
      byte number_pacet = 0;
      int flag_one_or_all_chip = 0;
      int int_number_pacet = -1;
      delay(10);
      flag_one_or_all_chip = Serial.read();
      delay(10);
      if (flag_one_or_all_chip == '1'){
        number_pacet = Serial.read();
        delay(10);
        int_number_pacet = (int)number_pacet;
        sending_a_package_one_chip(list[int_number_pacet]); 
      } else {
        sending_a_package_all_chip(list);
      }
    }else if(commands=='3'){
      for(int i=0; i<32; i++){
        giveMeTemperature(list[i]);
      }   
    }else if(commands=='4'){ //  - write address; 
      byte number_pacet = 0;
      int int_number_pacet = -1;
      int set_write = 0;
      delay(10);
      number_pacet = Serial.read();
      delay(10);
      int_number_pacet = (int)number_pacet;
      write_address(list[int_number_pacet]);
    } else if(commands=='5'){
      byte number_pacet = 0;
      int int_number_pacet = -1;
      delay(10);
      number_pacet = Serial.read();
      delay(10);
      int_number_pacet = (int)number_pacet;
      check_crc8_and_FAM(list[int_number_pacet]);
    } else if(commands=='6'){
//      number_pacet = Serial.read();
      delay(10);
     // int_number_pacet = (int)number_pacet;
     // check_real_coef(list[int_number_pacet]);
    }
  }
 }

void write_address(OneWire ds){
  char list_adres[8];
  for (int i=0; i<8; i++){
    delay(10);
    list_adres[i]=Serial.read();
  }
  ds.reset();
  ds.write(0x38);
  for(int i =0; i<8; i++){
    ds.write(list_adres[i]);
  } 
}

void check_real_coef(OneWire ds){
  giveMeTemperature(ds);
  delay(3000);
  int T_kod_check_coef;
  byte bufData_check_coef[9];  // буфер данных
  float temperature_check_coef;
  ds.reset();
  ds.write(0xCC); //skip rom
  ds.write(0xBE); //read_scratch

  bufData_check_coef[1]=ds.read();//low byte T
  bufData_check_coef[2]=ds.read();//high byte T
   
  //T_kod_check_coef = (bufData_check_coef[1] | ((bufData_check_coef[2]) << 8));
  temperature_check_coef = (bufData_check_coef[1] | ((bufData_check_coef[2]) << 8)) * 0.0625;
  Serial.print(temperature_check_coef);
  }
  
void check_crc8_and_FAM(OneWire ds){
  byte get_address_chip[8];
  
  ds.reset();
  ds.write(0x33); //skip rom
  
  get_address_chip[0]=ds.read();
  get_address_chip[1]=ds.read();
  get_address_chip[2]=ds.read();
  get_address_chip[3]=ds.read();
  get_address_chip[4]=ds.read();
  get_address_chip[5]=ds.read();
  get_address_chip[6]=ds.read();
  get_address_chip[7]=ds.read();
 // if(ds.search(get_address_chip)){ 
  if(get_address_chip[7]!=0){ 
    if (OneWire::crc8(get_address_chip, 7) != get_address_chip[7]) {
      Serial.println('0');
    } else {
      Serial.println('1');
    }
    for (int i=0; i<8;i++){
      Serial.println(get_address_chip[i]);
    }
  } else {
    Serial.println('0');
    for (int i=0; i<8;i++){
      Serial.println('0');
    }
  }
  
}

void sending_a_package_one_chip(OneWire ds){
  char list_coef[33];
  for (int i =0; i<33; i++){
    delay(10);
    list_coef[i]=Serial.read();
  }
  ds.reset();
  ds.write(0x18);
  for(int i =0; i<33; i++){
    ds.write(list_coef[i]);
  }
  delay(1);
  ds.write(0);
  ds.write(0x2);
}

void sending_a_package_all_chip(OneWire list[32]){
  char list_coef[1056];
  for (int i =0; i<1056; i++){
    delay(10);
    list_coef[i]=Serial.read();
  }
  int size_list = 0;
  for (int i=0; i<32; i++){
    list[i].reset();
    list[i].write(0x18);
    for (int j=0;j<33; j++){
      list[i].write(list_coef[size_list]);
      size_list = size_list+ j;
    } 
    list[i].write(0);
    list[i].write(0x2);
  }
}

void giveMeTemperature(OneWire ds){
   ds.reset();
   ds.write(0xCC);
   ds.write(0x44);
}

void readDataFromPort(OneWire ds, bool flag){

   
   // unsigned long currentMillis = millis();

    //delay(3000);

    ds.reset();
    ds.write(0xCC);
    ds.write(0xBE);
    
    //ds.read_bytes(bufData,2);
    bufData[1]=ds.read();
    bufData[2]=ds.read();
    int T_kod = 0;
    T_kod = (bufData[1] | ((bufData[2]) << 8));    
    
    for (int i=0;i<16;i++){//reset notInvBinaryCodeChar
      notInvBinaryCodeChar[i] = 0;
    }
    itoa(T_kod,notInvBinaryCodeChar,2);//convert T_kod in char notInvBinaryCodeChar
    
    for (int i=0; i<16; i++){//reset list
      binaryCode[i]=0;
    }
    for (int i=0;i<16;i++){//convert char in int (!-48!)
      binaryCode[i]=int(notInvBinaryCodeChar[i])-48;
    }
    for (int i=0;i<16;i++){//reset variableList
      variableList[i]=0;
    }
    int variable = 0;
    int variableOld = 15;
    for (int i=15;i>=0;i--){//work (-48)
      if (binaryCode[i]<0){
          variableList[variable] = 0;
          variable = variable + 1;
        }else {
          variableList[variableOld] = binaryCode[i];
          variableOld = variableOld - 1;
        }
    }
    for (int i=0;i<16;i++){
      binaryCode[i] = variableList[i];
    }
    for (int i=5; i<16; i++){//invert
      if(binaryCode[i]==0){
        binaryCode[i]=1;
      } else {
        binaryCode[i]=0;  
      }
    }
    
    if (flag){
      for (int i=0; i<16; i++){ // array intput 16
        collection[iterator] = binaryCode[i];
        //Serial.print(binaryCode[i]);
        iterator = iterator + 1;
      }
      //Serial.println();
    }else{
      for (int i=4; i<16; i++){
        if (binaryCode[i]==1){
          ds.reset();
          ds.write(0x78);
          delay(1);
          //ds.write(0x7);
          //Serial.println(i);
              switch(28-i){
                case 23:
                  ds.write(0x0);
                  break;
                case 22:
                  ds.write(0x1);
                  break;
                case 21:
                  ds.write(0x2);
                  break;
                case 20:
                  ds.write(0x3);
                  break;
                case 19:
                  ds.write(0x4);
                  break;
                case 18:
                  ds.write(0x5);
                  break;
                case 17:
                  ds.write(0x6);
                  break;
                case 16:
                  ds.write(0x7);
                  break;
                case 15: 
                  ds.write(0x8);
                  break;
                case 14:
                  ds.write(0x9);
                  break;
                case 13:
                  ds.write(0x10);
                  break;
                case 12:
                  ds.write(0x11);
                  break;
                case 11:
                  ds.write(0x12);
                  break;
              }
              break;
          }
        }
       }
}
       //Serial.print("\n");
