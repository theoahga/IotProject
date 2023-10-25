#ifndef RSA_CRYPTO_H
#define RSA_CRYPTO_H

#include <openssl/rsa.h>

RSA *createRSA(unsigned char *key, int public);
int public_encrypt(unsigned char *data, unsigned char *key, unsigned char *encrypted);
int private_decrypt(unsigned char *enc_data, int data_len, unsigned char *key, unsigned char *decrypted);
void printLastError(char *msg);

#endif // RSA_CRYPTO_H
