#include <stdio.h>
#include <string.h>

#define MAX_BUF  256

#define IV  0b10110001
#define KEY 0b11001011
#define CTR 0b00110101

void encode(unsigned char*, unsigned char*, int);
void decode(unsigned char*, unsigned char*, int);

unsigned char processCtr(unsigned char, unsigned char);
unsigned char encryptByte(unsigned char, unsigned char, unsigned char);
unsigned char decryptByte(unsigned char, unsigned char, unsigned char);

unsigned char getBit(unsigned char, int);
unsigned char setBit(unsigned char, int);
unsigned char clearBit(unsigned char, int);




int main()
{
  char str[8];
  int  choice;

  printf("\nYou may:\n");
  printf("  (1) Encrypt a message \n");
  printf("  (2) Decrypt a message \n");
  printf("  (0) Exit\n");
  printf("\n  what is your selection: ");
  fgets(str, sizeof(str), stdin);
  sscanf(str, "%d", &choice);

  if (choice == 0)
    return 0;

  switch (choice) {
    case 1:
    {
      //intialize empty arrays
      unsigned char buf[MAX_BUF];
      unsigned char enc[MAX_BUF];
      int len;

      printf("\n  Enter a message to encrypt: ");

      //get message from user
      fgets(buf, sizeof(buf), stdin);
      //length of message
      len = strlen(buf);

      //encode message
      encode(buf,enc,len);

      //print encoded message
      for(int i = 0; i < len-1; i++){
        printf("%d ", enc[i]);
      }



      break;
    }

    case 2:
    
    {
    //decrypt using scanf 
      unsigned char buf[MAX_BUF];
      unsigned char dec[MAX_BUF];
      //intilize variables
      int x = 0;
      int len = 0;
      int i = 0;
      
      printf("\n  Enter a message to decrypt: ");
      
      //get message from user and store in buf
      for (i = 0; i < MAX_BUF; i++){
        scanf("%d", &x);

        //if user enters -1, break
        if ((x) == -1){
          break;
        }
        len++;
        buf[i] = x;
      }

      //decode message
      decode(buf,dec,len);
      for(int i = 0; i < len; i++){
        //print decoded message
        printf("%c", dec[i]);

      }

      break;

    }

 

    }


  return(0);

}



//-----------------------------------------------------------
unsigned char getBit(unsigned char c, int n)
{
  return (c >> n) & 1;
}


unsigned char setBit(unsigned char c, int n)
{
  return c | (1 << n);
}


unsigned char clearBit(unsigned char c, int n)
{
  return c & ~(1 << n);

}
//-----------------------------------------------------------




//processCtr takes in key and a counter and returns the proccessed counter
unsigned char processCtr(unsigned char ctr, unsigned char key)
{
  unsigned char tempCtr = ctr;
  unsigned char value = 0;

  //if ctr is even
  if (tempCtr % 2 != 0) {
  
    
    for (int i = 1 ; i < 8; i+=2)
    {
      //xor the bits
      value = getBit(ctr, i) ^ getBit(key, i);
      
      if (value == 1)
      {
        //set the bit
        tempCtr = setBit(tempCtr, i);
    
      }
      else
      {
        //clear the bit
        tempCtr = clearBit(tempCtr, i);

      }


    }
  }

  //if  ctr is odd
  else
  {
    for (int i = 0; i < 8; i+=2)
    {
      //xor the bits
      value = getBit(ctr, i) ^ getBit(key, i);
      if (value == 1)
      {
        //set the bit
        tempCtr = setBit(tempCtr, i);
    
      }
      else
      {
        //clear the bit
        tempCtr = clearBit(tempCtr, i);

      }

    
    }
  }


  //return the processed counter
  return tempCtr;


}





unsigned char encryptByte(unsigned char pt, unsigned char ctr, unsigned char prev) {

  //tempbyte is the byte that will be returned
  unsigned char tempByte = 0;


  for (int i = 0; i < 8; i++)
  {
    //if the bit is 1 at ctr
    if (getBit(ctr, i) == 1){
      
      //xor the bit with the bit at pt
      if(getBit(pt, i) ^ getBit(prev, i)){

        //set the bit
        tempByte = setBit(tempByte, i);

      } else {
        //clear the bit
        tempByte = clearBit(tempByte, i);
      }
      
    }
    //if the bit is 0 at ctr
    else
    {
      //access mirror bit
      if(getBit(pt, i) ^ getBit(prev, 7-i)){
        tempByte = setBit(tempByte, i);
      } else {
        tempByte = clearBit(tempByte, i);
      }
      }

  
}
//return the encrypted byte
return tempByte;

}




void encode(unsigned char *pt, unsigned char* ct, int numBytes){
  unsigned char ctr = processCtr(CTR, KEY);

  for (int i = 0; i < numBytes ; i++)
  {
    if (i == 0){
      
      //encrypt the first byte using IV
      ct[i] = encryptByte(pt[i], ctr, IV);
      
    }
    else{
      //increment the counter
      ctr++;
      ctr = processCtr(ctr, KEY);
      //encrypt the rest of the bytes using the previous byte
      ct[i] = encryptByte(pt[i], ctr, ct[i-1]);
 
    }


    
  
  }

  
}

//decryptByte takes in a byte, a counter, and the previous byte and returns the decrypted byte
unsigned char decryptByte(unsigned char ct, unsigned char ctr, unsigned char prev) {
  unsigned char tempByte = 0;


  for (int i = 0; i < 8; i++)
  {
    
    if (getBit(ctr, i) == 1){
      if(getBit(ct, i) ^ getBit(prev, i)){

        tempByte = setBit(tempByte, i);

      } else {
        tempByte = clearBit(tempByte, i);
      }
      
    }

    else
    {
        if(getBit(ct, i) ^ getBit(prev, 7-i))
        {
          tempByte = setBit(tempByte, i);
    
         } else {
            tempByte = clearBit(tempByte, i);
          }


  }



  }
  return tempByte;

  
}

//decode takes in a cipher text and a plain text and the number of bytes to decode
void decode(unsigned char *ct, unsigned char* pt, int numBytes){
  unsigned char ctr = processCtr(CTR, KEY);

  for (int i = 0; i < numBytes ; i++)
  {
    if (i == 0){
      
        //decrypt the first byte using IV
      pt[i] = encryptByte(ct[i], ctr, IV);
      
    }
    // decrypt the rest of the bytes using the previous byte
   
    else{
      ctr++;
      ctr = processCtr(ctr, KEY);
      pt[i] = encryptByte(ct[i], ctr, ct[i-1]);
 
    }
   }

  
    
  
  }

