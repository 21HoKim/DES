#include "crypto.h"

extern char InitialPermutationTable[64];
extern char FinalPermutationTable[64];
extern char ExpansionPermutationTable[64];
extern char StraightPermutationTable[32];
extern char ParityDropTable[56];
extern char KeyCompressionTable[48];
extern char S_Box[8][4][16];

void PtBintoHex(int inbits, char *bin) {
    int i, j, k, decimal, padding; 
    char hex[100];

    // 입력된 문자열이 8비트로 나누어 떨어지지 않는다면, 패딩을 추가
    if (inbits % 8 != 0) {
        padding = 8 - inbits % 8;
        char temp[100] = "";
        for (i = 0; i < padding; i++) {
            strcat(temp, "0");
        }
        strcat(temp, bin);
        bin = temp;
        inbits = strlen(bin);
    }

    // 8비트씩 끊어서 10진수로 변환하고, 16진수로 변환
    for (i = 0, j = 0; i < inbits; i += 8, j += 2) {
        decimal = 0;
        for (k = 0; k < 8; k++) {
            decimal = decimal * 2 + bin[i + k] - '0';
        }
        sprintf(hex + j, "%02X", decimal);
    }
    printf("%8s",hex);

}

void Cipher(char plainBlock[64],char RoundKeys[16][48],char cipherBlock[64])
{
    char inBlock[64];
    char leftBlock[32], rightBlock[32],outBlock[64];
    permute(64,64,plainBlock,inBlock,InitialPermutationTable);
    printf("After initial permutation : ");PtBintoHex(64,inBlock);puts("");
    split(64,32,inBlock,leftBlock,rightBlock);

    printf("After splitting : L0=");PtBintoHex(32,leftBlock);
    printf("\tR0=");PtBintoHex(32,rightBlock);puts("");
    puts("-----------------------------------------------------------------"); //55개
    puts("|Round\t\t\t| Left\t    | Right    | Round Key    |");
    
    for(char round=0; round<16; round++){
        mixer(leftBlock,rightBlock,RoundKeys[round]);

        if(round!=15){
            swapper(leftBlock,rightBlock);
        }

        printf("|Round %d\t\t| ",round+1);PtBintoHex(32,leftBlock);printf("  | ");PtBintoHex(32,rightBlock);printf(" | ");PtBintoHex(48,RoundKeys[round]);puts(" |");
    }
    puts("-----------------------------------------------------------------"); //55개
    combine(32,64,leftBlock,rightBlock,outBlock);
    printf("After combination : ");PtBintoHex(64,outBlock);puts("");
    permute(64,64,outBlock,cipherBlock,FinalPermutationTable);
    printf("Ciphertext : ");PtBintoHex(64,cipherBlock);puts("");
}

void mixer(char *leftBlock, char *rightBlock,char RoundKeys[48]){

    char T1[32],T2[32],T3[32];
    copy(32,rightBlock,T1);
    function(T1,RoundKeys,T2);
    exclusiveOr(32,leftBlock,T2,T3);
    copy(32,T3,leftBlock); //!!수도코드 오류 rightBlock->leftBlock
}

void permute(char inbits,char outbits,char *inBlock, char *outBlock, char *Table){ // InitialPermutationTable, ExpansionPermutationTable 
    //P-box
    //확장
        for(char i=0;i<outbits;i++){
            outBlock[i]=inBlock[Table[i]-1];
        }

}

void split(char inbits,char outbits,char *inBlock,char *leftBlock,char *rightBlock){
    if(outbits*2!=inbits){//56 -> 28 28
        puts("split error!! inbits's not 2*outbits");
    }
    memcpy(leftBlock,inBlock,outbits);
    memcpy(rightBlock,inBlock+outbits,outbits);
}

void copy(char inbits, char *inBlock,char *outBlock){
    memcpy(outBlock,inBlock,inbits);
}

void combine(char inbits,char outbits, char *leftBlock,char *rightBlock,char *outBlock){
    if(outbits!=2*inbits){//28+28->56
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

void exclusiveOr(char bit, char *Block1, char *Block2, char* endBlock)
{
    for (int i = 0; i < bit; i++)
    {
        endBlock[i] = ((Block1[i] - 48)^(Block2[i]-48))+48;
    }
}


void function(char inBlock[32],char RoundKeys[48],char outBlock[32]){
    unsigned char T1[48],T2[48],T3[32];
    permute(32,48,inBlock,T1,ExpansionPermutationTable);

    exclusiveOr(48,T1,RoundKeys,T2);

    substitute(T2,T3,S_Box);
    permute(32,32,T3,outBlock,StraightPermutationTable);
}

void substitute(char inBlock[48], char outBlock[32], char SubstitutionTable[8][4][16]) {
    int i, j, row, col, val;

    // 6비트 블록으로 분할하여 S-box 대입
    for (i = 0; i < 8; i++) {
        // 행과 열 계산
        row = (inBlock[i * 6] - '0') * 2 + (inBlock[i * 6 + 5] - '0');
        col = (inBlock[i * 6 + 1] - '0') * 8 + (inBlock[i * 6 + 2] - '0') * 4 + (inBlock[i * 6 + 3] - '0') * 2 + (inBlock[i * 6 + 4] - '0');
        val = SubstitutionTable[i][row][col];
        
        for (j = 0; j < 4; j++) {
            outBlock[i * 4 + j] = ((val >> (3 - j)) & 1) + '0';
        }



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

void shiftLeft(char block[28],char numOfShifts){//순환이동, 왼쪽으로 밀기
    for(char i=0;i<numOfShifts;i++){
        char T=block[0];
        for(char j=1;j<28;j++){
            block[j-1]=block[j];
        }
        block[27]=T;
    }
}