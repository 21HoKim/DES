#include "crypto.h"
extern char ShiftTable[16];
int main(){
    printf("Plaintext : %llX",PLAINTEXT); printf("\tKey : %llX\n",KEY); printf("CipherText : %llX\n",CIPHERTEXT);



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
    //puts("[keyWithParities]");PtBintoHex(64,keyWithParities);puts("");
    Key_Generator(keyWithParities,RoundKeys,ShiftTable);
    Cipher(plainBlock,RoundKeys,cipherBlock);
    return 0;
}
