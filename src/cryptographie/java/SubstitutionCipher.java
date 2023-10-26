public class SimpleSubstitutionCipher {
    public static String encrypt(String text, int key) {
        StringBuilder encryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char encryptedChar = (char) (c + key);
            encryptedText.append(encryptedChar);
        }
        return encryptedText.toString();
    }

    public static String decrypt(String text, int key) {
        StringBuilder decryptedText = new StringBuilder();
        for (int i = 0; i < text.length(); i++) {
            char c = text.charAt(i);
            char decryptedChar = (char) (c - key);
            decryptedText.append(decryptedChar);
        }
        return decryptedText.toString();
    }

    public static void main(String[] args) {
        String text = "T:26.49;L:115;169832654";
        int key = 3; // Vous pouvez choisir n'importe quelle clé numérique

        // Chiffrement
        String encryptedText = encrypt(text, key);
        System.out.println("Texte chiffré: " + encryptedText);

        // Déchiffrement
        String decryptedText = decrypt(encryptedText, key);
        System.out.println("Texte déchiffré: " + decryptedText);
    }
}
