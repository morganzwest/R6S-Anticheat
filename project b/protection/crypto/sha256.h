/*********************************************************************
* Filename:   sha256.h
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding SHA1 implementation.
*********************************************************************/
#ifndef SHA256_H
#define SHA256_H

/*************************** HEADER FILES ***************************/

/****************************** MACROS ******************************/
#define SHA256_BLOCK_SIZE 32  // SHA256 outputs a 32 byte digest

/**************************** DATA TYPES ****************************/

typedef struct {
	unsigned char data[64];
	unsigned datalen;
	unsigned long long bitlen;
	unsigned state[8];
} SHA256_CTX;

/*********************** FUNCTION DECLARATIONS **********************/
void sha256_init(SHA256_CTX* ctx);
void sha256_update(SHA256_CTX* ctx, unsigned char data[], unsigned len);
void sha256_final(SHA256_CTX* ctx, char* hash);

#endif   // SHA256_H