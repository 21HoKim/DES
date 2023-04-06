#include "crypto.h"

extern char InitialPermutationTable[64];
extern char FinalPermutationTable[64];
extern char ExpansionPermutationTable[64];
extern char StraightPermutationTable[32];
extern char ParityDropTable[56];
extern char KeyCompressionTable[48];
extern char S_Box[8][4][16];



void Cipher(char plainBlock[64],char RoundKeys[16][48],char cipherBlock[64]){
    char inBlock[64];
    char leftBlock[32], rightBlock[32],outBlock[64];
    permute(64,64,plainBlock,inBlock,InitialPermutationTable);
    split(64,32,inBlock,leftBlock,rightBlock);
    for(char round=1; round<=16; round++){
        mixer(leftBlock,rightBlock,RoundKeys[round]);
        if(round!=16){
            swapper(leftBlock,rightBlock);
        }
    }
    combine(32,64,leftBlock,rightBlock,outBlock);
    permute(64,64,outBlock,cipherBlock,FinalPermutationTable);
}
void mixer(char* leftBlock, char* rightBlock,char RoundKeys[48]){
    char T1[32],T2[32],T3[32];
    copy(32,rightBlock,T1);
    function(T1,RoundKeys,T2);
    exclusiveOr(32,leftBlock,T2,T3);
    copy(32,T3,rightBlock);
}
void permute(char inbits,char outbits,char *inBlock, char *outBlock, char *Table){ // InitialPermutationTable, ExpansionPermutationTable 
    //P-box
    //확장
    if(inbits==outbits){
        for(char i=0;i<outbits;i++){
            outBlock[i]=inBlock[Table[i]];
        }
    }
}
void split(char inbits,char outbits,char *inBlock,char *leftBlock,char *rightBlock){
    if(outbits*2!=inbits){
        puts("split error!! inbits's not 2*outbits");
    }
    memcpy(leftBlock,inBlock,outbits);
    memcpy(rightBlock,inBlock,outbits);
}
void combine(char inbits,char outbits, char *leftBlock,char *rightBlock,char *outBlock){
    if(outbits!=2*inbits){
        puts("combine error!! outbits's not 2*inbits");
        return;
    }
    memcpy(outBlock,leftBlock,inbits);
    memcpy(outBlock+inbits,rightBlock,inbits);
}
void swapper(char leftBlock[32],char rightBlock[32]){
    char T[32];
    copy(32,leftBlock,T);
    copy(32,rightBlock,leftBlock);
    copy(32,T,rightBlock);
}
void exclusiveOr(char inbits,char *inBlock, char *RoundKeys,char *T2){
    for(int i=0;i<inbits;i++){
        T2[i]=((inBlock[i]-48)^(RoundKeys[i]-48))+48;
    }
}
void copy(char inbits, char *inBlock,char *outBlock){
    memcpy(outBlock,inBlock,inbits);
}
void function(char inBlock[32],char RoundKeys[48],char outBlock[32]){
    char T1[48],T2[48],T3[48];
    permute(32,48,inBlock,T1,ExpansionPermutationTable);
    exclusiveOr(48,T1,RoundKeys,outBlock);
    substitute(T2,T3,S_Box);
    permute(32,32,T3,outBlock,StraightPermutationTable);
}
void substitute(char inBlock[32],char outBlock[48],char SubstitutionTable[8][4][16]){
    for(int i=1;i<=8;i++){
        char row = 2*inBlock[i*6+1] + inBlock[i*6+6];
        char col = 8*inBlock[i*6+2] + 4*inBlock[i*6+3] + 2*inBlock[i*6+4] + inBlock[i*6+5];

        char value = SubstitutionTable[i][row][col];

        outBlock[i*4+1]=value/8; value %= 8;
        outBlock[i*4+2]=value/4; value %= 4;
        outBlock[i*4+3]=value/2; value %= 2;
        outBlock[i*4+4]=value;
    }
    
}

void Key_Generator(char keyWithParities[64],char RoundKeys[16][48],char ShiftTable[16]){
    char cipherKey[56], leftKey[28], rightKey[28], preRoundKey[56];
    permute(64,56,keyWithParities,cipherKey,ParityDropTable);
    split(56,28,cipherKey,leftKey,rightKey);
    for(char round=0;round<16;round++){
        shiftLeft(leftKey,ShiftTable[round]);
        shiftLeft(rightKey,ShiftTable[round]);
        combine(28,56,leftKey,rightKey,preRoundKey);
        permute(56,48,preRoundKey,RoundKeys[round],KeyCompressionTable);
    }
}
void shiftLeft(char block[28],char numOfShifts){
    for(char i=1;i<numOfShifts;i++){
        char T=block[1];
        for(char j=2;j<=28;j++){
            block[j-1]=block[i];
        }
        block[28]=T;
    }
}