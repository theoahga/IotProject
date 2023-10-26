#include <stdio.h>
#include <string.h>

char* encrypt(char* text, char* key) {
    int textLength = strlen(text);
    char* encryptedText = (char*)malloc(textLength + 1);

    for (int i = 0; i < textLength; i++) {
        if (isalpha(text[i])) {
            char isUpperCase = isupper(text[i]);
            text[i] = tolower(text[i]);
            int charIndex = text[i] - 'a';
            char encryptedChar = isUpperCase ? toupper(key[charIndex]) : key[charIndex];
            encryptedText[i] = encryptedChar;
        } else {
            encryptedText[i] = text[i];
        }
    }
    encryptedText[textLength] = '\0';
    return encryptedText;
}

char* decrypt(char* encryptedText, char* key) {
    int textLength = strlen(encryptedText);
    char* decryptedText = (char*)malloc(textLength + 1);

    for (int i = 0; i < textLength; i++) {
        if (isalpha(encryptedText[i])) {
            char isUpperCase = isupper(encryptedText[i]);
            encryptedText[i] = tolower(encryptedText[i]);
            char* charIndex = strchr(key, encryptedText[i]);
            int index = charIndex - key;
            char decryptedChar = index + 'a';
            decryptedChar = isUpperCase ? toupper(decryptedChar) : decryptedChar;
            decryptedText[i] = decryptedChar;
        } else {
            decryptedText[i] = encryptedText[i];
        }
    }
    decryptedText[textLength] = '\0';
    return decryptedText;
}

int main() {
    char key[] = "bcdefghijklmnopqrstuvwxyza";
    char text[] = "Hello, World!";
    char* encryptedText = encrypt(text, key);
    printf("Texte chiffré : %s\n", encryptedText);
    char* decryptedText = decrypt(encryptedText, key);
    printf("Texte déchiffré : %s\n", decryptedText);

    free(encryptedText);
    free(decryptedText);

    return 0;
}
