#include "crypto.h"
extern unsigned char ShiftTable[16];
int main(){
    char RoundKeys[16][48];
    long long OutKeys;

    char plainBlock[64];
    char keyWithParities[64];
    char cipherBlock[64];

    long long Key=KEY;
    long long CipherText=CIPHERTEXT;
    long long Plaintext=PLAINTEXT;


    for (char i = 0; i < 64; i++) {
        int bit = (Key >> (63 - i)) & 1;
        keyWithParities[i] = bit ? '1' : '0';
    }
    for (char i = 0; i < 64; i++) {
        int bit = (CipherText >> (63 - i)) & 1;
        cipherBlock[i] = bit ? '1' : '0';
    }    
    for (char i = 0; i < 64; i++) {
        int bit = (Plaintext >> (63 - i)) & 1;
        plainBlock[i] = bit ? '1' : '0';
    }
    Key_Generator(keyWithParities,RoundKeys,ShiftTable);
    for(int i=0;i<16;i++){
        printf("%.*s\n",48,RoundKeys[i]);
    }
    Cipher(plainBlock,RoundKeys,cipherBlock);
    return 0;
}
