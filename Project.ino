#include <SPI.h>
#include <MFRC522.h>
#include <HX711.h>


#define SS_PIN 10  /* Slave Select Pin */
#define RST_PIN 9  /* Reset Pin */
#define calibration_factor -1450.0 
#define LOADCELL_DOUT_PIN  3
#define LOADCELL_SCK_PIN  2

MFRC522 mfrc522(SS_PIN, RST_PIN);
HX711 scale;

MFRC522::MIFARE_Key key;          
int a;
int b;
int c=46;
int d;
/* Set the block to which we want to write data */
/* Be aware of Sector Trailer Blocks */
int blockNum = 4;  
/* Create an array of 16 Bytes and fill it with data */
/* This is the actual data which is going to be written into the card */
byte blockData [16] = {0,0,3,11,209,1,7,84,2,101,110,a+48,b+48,c,d+48,254};

/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];

MFRC522::StatusCode status;

void setup() 
{
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); 
  scale.tare();
  Serial.println("Scan a MIFARE 1K Tag to write data...");
  key.keyByte[0]=0xD3;
  key.keyByte[1]=0xF7;
  key.keyByte[2]=0xD3;
  key.keyByte[3]=0xF7;
  key.keyByte[4]=0xD3;
  key.keyByte[5]=0xF7;
  
}

void loop()
{

  mfrc522.PCD_Init();
  Serial.print("Reading: ");
  double current_scale = scale.get_units(20);
  Serial.print(current_scale); //scale.get_units() returns a float
  Serial.print(" g"); //You can change this to kg but you'll need to refactor the calibration_factor
  Serial.println();
 
  a = int(current_scale/10)%10;
  b = int(current_scale)%10;
  d = int(current_scale*10)%10;
  Serial.print(a);
  Serial.print(b);
  Serial.print(d);
  Serial.println();
  byte blockData [16] = {0,0,3,11,209,1,7,84,2,101,110,a+48,b+48,c,d+48,254};
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));
         
   /* Call 'WriteDataToBlock' function, which will write data to the block */
   Serial.print("\n");
   Serial.println("Writing to Data Block...");
   WriteDataToBlock(blockNum, blockData);
   mfrc522.PCD_Reset();
   /* Read data from the same block */
//   Serial.print("\n");
//   Serial.println("Reading from Data Block...");
//   ReadDataFromBlock(blockNum, readBlockData);
   /* If you want to print the full memory dump, uncomment the next line */
   //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
   /* Print the data read from block */
//   Serial.print("\n");
//   Serial.print("Data in Block:");
//   Serial.print(blockNum);
//   Serial.print(" --> ");
//   for (int j=0 ; j<16 ; j++)
//   {
//     Serial.write(readBlockData[j]);
//   }
//   Serial.print("\n");
}



void WriteDataToBlock(int blockNum, byte blockData[]) 
{
  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  
  /* Write data to the block */
  status = mfrc522.MIFARE_Write(blockNum, blockData, 16);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Writing to Block failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Data was written into Block successfully");
  }
  
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{
  /* Authenticating the desired data block for Read access using Key A */
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK)
  {
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  else
  {
    Serial.println("Authentication success");
  }

  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Block was read successfully");  
  }
}
