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
    char hex[100]; // 16진수로 변환된 결과를 저장할 문자열 (static 변수)

    // 입력된 문자열이 8비트로 나누어 떨어지지 않는다면, 패딩을 추가합니다.
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

    // 8비트씩 끊어서 10진수로 변환하고, 16진수로 변환합니다.
    for (i = 0, j = 0; i < inbits; i += 8, j += 2) {
        decimal = 0;
        for (k = 0; k < 8; k++) {
            decimal = decimal * 2 + bin[i + k] - '0';
        }
        sprintf(hex + j, "%02X", decimal);
    }
    printf("%16s",hex);
    //return hex;
}

unsigned long long binaryToDecimal(char *binaryString) {
    int i;
    unsigned long long decimal = 0;
    int len = strlen(binaryString);

    for (i = 0; i < len; i++) {
        if (binaryString[i] == '1') {
            decimal = decimal * 2 + 1;
        } else if (binaryString[i] == '0') {
            decimal = decimal * 2;
        } else {
            printf("Invalid binary string.\n");
            return -1;
        }
    }

    return decimal;
}

void Cipher(char plainBlock[64],char RoundKeys[16][48],char cipherBlock[64]){
    char inBlock[64];
    char leftBlock[32], rightBlock[32],outBlock[64];
    permute(64,64,plainBlock,inBlock,InitialPermutationTable);

    printf("After initial permutation : ");PtBintoHex(64,inBlock);puts("");

    split(64,32,inBlock,leftBlock,rightBlock);
        printf("left:[%.*s]\n",28,leftBlock);
        printf("right:[%.*s]\n",28,rightBlock);

    printf("After splitting : L0=");PtBintoHex(32,leftBlock);
    printf("\tR0=");PtBintoHex(32,rightBlock);puts("");
    puts("-------------------------------------------------------------------------------------"); //75개
    puts("|Round\t\t\t|\tLeft\t\t|\tRight\t  |\tRound Key  |");
    for(char round=0; round<16; round++){
        mixer(leftBlock,rightBlock,RoundKeys[round]);
        if(round!=15){
            swapper(leftBlock,rightBlock);
        }
        printf("|Round %d\t\t|",round+1);PtBintoHex(32,leftBlock);printf("\t|");PtBintoHex(32,rightBlock);printf(" |");PtBintoHex(48,RoundKeys[round]);puts("|");
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

void exclusiveOr(char inbits,char *inBlock, char *T2,char *T3){
    // for(int i=0;i<inbits;i++){
    //     T2[i]=((inBlock[i]-48)^(RoundKeys[i]-48))+48;
    // }

    // 문자열을 비트로 변환
    unsigned long long block = 0;
    for (int i = 0; i < inbits; i++) {
        block = (block << 1) | (inBlock[i] - '0');
    }

    unsigned long long t2 = 0;
    for (int i = 0; i < inbits; i++) {
        t2 = (t2 << 1) | (T2[i] - '0');
    }

    // XOR 연산
    unsigned long long result = block ^ t2;

    // 결과 비트를 문자열로 변환
    for (int i = inbits - 1; i >= 0; i--) {
        T3[i] = (result & 1) + '0';
        result = result >> 1;
    }
}

void copy(char inbits, char *inBlock,char *outBlock){
    memcpy(outBlock,inBlock,inbits);
}

void function(char inBlock[32],char RoundKeys[48],char outBlock[32]){
    char T1[48],T2[48],T3[32];
    permute(32,48,inBlock,T1,ExpansionPermutationTable);
    exclusiveOr(48,T1,RoundKeys,T2);
    substitute(T2,T3,S_Box);
    permute(32,32,T3,outBlock,StraightPermutationTable);
}

void substitute(char inBlock[48],char outBlock[32],char SubstitutionTable[8][4][16]){
    //S-box 48->32 !!수도코드 오류 inBlock[32]->[48], outBlock[48]->[32]
    for(int i=0;i<8;i++){
        int row = 2*inBlock[i*6] + inBlock[i*6+5];
        int col = 8*inBlock[i*6+1] + 4*inBlock[i*6+2] + 2*inBlock[i*6+3] + inBlock[i*6+4];

        char value = SubstitutionTable[i][row][col];

        outBlock[i*4]=value/8; value %= 8;
        outBlock[i*4+1]=value/4; value %= 4;
        outBlock[i*4+2]=value/2; value %= 2;
        outBlock[i*4+3]=value;
    }
    
}

void Key_Generator(char keyWithParities[64],char RoundKeys[16][48],char ShiftTable[16]){
    //puts("[Key_Generator]");PtBintoHex(64,keyWithParities);puts("");
    char cipherKey[56], leftKey[28], rightKey[28], preRoundKey[56];
    permute(64,56,keyWithParities,cipherKey,ParityDropTable);
    //puts("[cipherKey]");PtBintoHex(56,cipherKey);puts("");
    //printf("cipherKey:[%.*s]\n",56,cipherKey);
    split(56,28,cipherKey,leftKey,rightKey);
    //printf("L:[%.*s] R:[%.*s]\n",28,leftKey,28,rightKey);
    //puts("[split]");printf("L : ");PtBintoHex(28,leftKey);printf("\t R : ");PtBintoHex(28,rightKey); puts("");
    for(char round=0;round<16;round++){
        shiftLeft(leftKey,ShiftTable[round]);
        shiftLeft(rightKey,ShiftTable[round]);
        combine(28,56,leftKey,rightKey,preRoundKey);
        permute(56,48,preRoundKey,RoundKeys[round],KeyCompressionTable);
        //PtBintoHex(48,RoundKeys[round]);puts("");
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