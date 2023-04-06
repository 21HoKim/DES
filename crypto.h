#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define PLAINTEXT 0x123456ABCD132536ull
#define KEY 0xAABB09182736CCDDull
#define CIPHERTEXT 0xC0B7A8D05F3A829Cull

void PtBintoHex(int inbits, char *bin);

void bitStringToNum(int bits, char *bitString, int *result);

void Cipher(char plainBlock[64],char RoundKeys[16][48],char cipherBlock[64]);
void permute(char inbits,char outbits,char *inBlock, char *outBlock, char *Table); // InitialPermutationTable, ExpansionPermutationTable 
void split(char inbits,char outbits,char *inBlock,char *leftBlock,char *rightBlock);
void mixer(char* leftBlock, char* rightBlock,char RoundKeys[48]);
void combine(char inbits,char outbits, char *leftBlock,char *rightBlock,char *outBlock);
void swapper(char leftBlock[32],char rightBlock[32]);
void exclusiveOr(int inBlockLength, char* inBlock_A, char* inBlock_B, char* outBlock);
void copy(char inbits, char *inBlock,char *outBlock);
void function(char inBlock[32],char RoundKey[48],char outBlock[32]);
void substitute(char inBlock[48],char outBlock[32],char SubstitutionTable[8][4][16]);

void Key_Generator(char keyWithParities[64],char RoundKey[16][48],char ShiftTable[16]);
void shiftLeft(char block[28],char numOfShifts);