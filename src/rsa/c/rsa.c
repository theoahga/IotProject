#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>

int padding = RSA_PKCS1_PADDING;

RSA * createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    if(public)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }

    return rsa;
}

int public_encrypt(unsigned char * data,int data_len, unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted)
{
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

void printLastError(char *msg)
{
    char * err = malloc(130);;
    ERR_load_crypto_strings();
    ERR_error_string(ERR_get_error(), err);
    printf("%s ERROR: %s\n",msg, err);
    free(err);
}

int main(){
    char *plainText = (char *)malloc(2048); 
    strcpy(plainText, "Hello, World!");

    char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"\
    "MIIBOQIBAAJBALLm0TFTmIp6V4soFZmlGdcA4BX1CSJGMx+FHbNCEnW4S9ejJir3\n"\
    "oXx2zHTFKbO5A8/DUqjE5Rxj0xI82b9zkfECAwEAAQJAdu/BD9wVw6qGXPtvMNB0\n"\
    "l137nF1ljyXAgyoKjsxUXXHmWGsngdmEwmSyDZV2iTWAZlnKXhW/hw4tgaDqjX6S\n"\
    "4QIhANg4Xlccum/DgE1hU225t8tfEseLmtl6kHqoqi4B+ypdAiEA09DNlN6X9/1G\n"\
    "nQ04DwS1u6evJAt6XgDb+qyWax4qFKUCIBj/g2C88I0lZDsOCpBADZDUwB4T9OY5\n"\
    "9mRIBczl67z9AiBeGy3LT14bSsbdrerTGVeYqPZyQDs8moshgx5NNxihFQIgCCSw\n"\
    "jkSDDfxvbU0X+/k5eOck7cnDf2G+K/0ZKJCP/HI=\n"\
    "-----END RSA PRIVATE KEY-----\n";


    
    char publicKey[]="-----BEGIN PUBLIC KEY-----\n"\
    "MFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBALLm0TFTmIp6V4soFZmlGdcA4BX1CSJG\n"\
    "Mx+FHbNCEnW4S9ejJir3oXx2zHTFKbO5A8/DUqjE5Rxj0xI82b9zkfECAwEAAQ==\n"\
    "-----END PUBLIC KEY-----\n";
    
    unsigned char  encrypted[4098]={};
    unsigned char decrypted[4098]={};
    int encrypted_length= public_encrypt(plainText,strlen(plainText),publicKey,encrypted);
    if(encrypted_length == -1)
    {
        printLastError("Public Encrypt failed ");
        exit(0);
    }
    printf("Encrypted length =%d\n",encrypted_length);
    int decrypted_length = private_decrypt(encrypted,encrypted_length,privateKey, decrypted);
    if(decrypted_length == -1)
    {
        printLastError("Private Decrypt failed ");
        exit(0);
    }
    printf("Decrypted Text =%s\n",decrypted);
    printf("Decrypted Length =%d\n",decrypted_length);

}

