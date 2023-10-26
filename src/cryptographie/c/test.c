#include <stdio.h>
#include <string.h>

void encrypt(char *text, int key) {
    int textLength = strlen(text);
    for (int i = 0; i < textLength; i++) {
        text[i] = text[i] + key;
    }
}

void decrypt(char *text, int key) {
    int textLength = strlen(text);
    for (int i = 0; i < textLength; i++) {
        text[i] = text[i] - key;
    }
}

int main() {
    char text[] = "T:26.49;L:115;169832654";
    int key = 3; // Vous pouvez choisir n'importe quelle clé numérique

    // Chiffrement
    encrypt(text, key);
    printf("Texte chiffré: %s\n", text);

    // Déchiffrement
    decrypt(text, key);
    printf("Texte déchiffré: %s\n", text);

    return 0;
}
