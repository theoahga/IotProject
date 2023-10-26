public class SubstitutionCipher {
    public static String encrypt(String text, String key) {
        StringBuilder encryptedText = new StringBuilder();
        for (char c : text.toCharArray()) {
            if (Character.isLetter(c)) {
                boolean isUpperCase = Character.isUpperCase(c);
                c = Character.toLowerCase(c);
                int charIndex = c - 'a';
                char encryptedChar = Character.toUpperCase(key.charAt(charIndex)) 
                                        if isUpperCase 
                                        else key.charAt(charIndex);
                encryptedText.append(encryptedChar);
            } else {
                encryptedText.append(c);
            }
        }
        return encryptedText.toString();
    }

    public static String decrypt(String encryptedText, String key) {
        StringBuilder decryptedText = new StringBuilder();
        for (char c : encryptedText.toCharArray()) {
            if (Character.isLetter(c)) {
                boolean isUpperCase = Character.isUpperCase(c);
                c = Character.toLowerCase(c);
                int charIndex = key.indexOf(c);
                char decryptedChar = (char) (charIndex + 'a');
                decryptedChar = isUpperCase ? Character.toUpperCase(decryptedChar) : decryptedChar;
                decryptedText.append(decryptedChar);
            } else {
                decryptedText.append(c);
            }
        }
        return decryptedText.toString();
    }

    public static void main(String[] args) {
        String key = "bcdefghijklmnopqrstuvwxyza";
        String text = "Hello, World!";
        String encryptedText = encrypt(text, key);
        System.out.println("Texte chiffré : " + encryptedText);
        String decryptedText = decrypt(encryptedText, key);
        System.out.println("Texte déchiffré : " + decryptedText);
    }
}
