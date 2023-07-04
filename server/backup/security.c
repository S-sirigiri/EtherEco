#include <openssl/aes.h>
#include <string.h>
#include <stdio.h>
#include <openssl/sha.h>


#define BLOCK_SIZE 16

void encrypt(const unsigned char* plaintext, unsigned char* ciphertext) {
    AES_KEY encrypt_key;
    unsigned char key[] = "0123456789abcdef0123456789abcdef"; // 32 bytes for AES-256
    unsigned char iv[] = "0123456789abcdef"; // 16 bytes for AES

    AES_set_encrypt_key(key, 256, &encrypt_key);
    AES_cbc_encrypt(plaintext, ciphertext, strlen((char*)plaintext), &encrypt_key, iv, AES_ENCRYPT);
}

void decrypt(const unsigned char* ciphertext, unsigned char* plaintext) {
    AES_KEY decrypt_key;
    unsigned char key[] = "0123456789abcdef0123456789abcdef";
    unsigned char iv[] = "0123456789abcdef";

    AES_set_decrypt_key(key, 256, &decrypt_key);
    AES_cbc_encrypt(ciphertext, plaintext, strlen((char*)ciphertext), &decrypt_key, iv, AES_DECRYPT);
}


void hash_string(const unsigned char* str, unsigned char* output) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str, strlen((char *)str));
    SHA256_Final(output, &sha256);
}


int main() {
	//encryption
    unsigned char plaintext[] = "This is a test."; // Multiple of 16 bytes
    unsigned char ciphertext[BLOCK_SIZE];
    unsigned char decryptedtext[BLOCK_SIZE];

    encrypt(plaintext, ciphertext);
    decrypt(ciphertext, decryptedtext);

    printf("Original: %s\n", plaintext);
    printf("Encrypted: %s\n", ciphertext);
    printf("Decrypted: %s\n", decryptedtext);


	//hashing
	const unsigned char* str = "hello world";
    unsigned char output[SHA256_DIGEST_LENGTH];
    hash_string(str, output);

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        printf("%02x", output[i]);
    printf("\n");
	printf(output);

    return 0;
}

