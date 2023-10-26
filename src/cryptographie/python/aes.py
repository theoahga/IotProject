def encrypt(text, key):
    encrypted_text = ""
    for char in text:
        if char.isalpha():
            is_upper = char.isupper()
            char = char.lower()
            char_index = ord(char) - ord('a')
            encrypted_char = key[char_index].upper() if is_upper else key[char_index]
            encrypted_text += encrypted_char
        else:
            encrypted_text += char
    return encrypted_text

def decrypt(encrypted_text, key):
    decrypted_text = ""
    for char in encrypted_text:
        if char.isalpha():
            is_upper = char.isupper()
            char = char.lower()
            char_index = key.index(char)
            decrypted_char = chr(char_index + ord('a')).upper() if is_upper else chr(char_index + ord('a'))
            decrypted_text += decrypted_char
        else:
            decrypted_text += char
    return decrypted_text

# Exemple d'utilisation
key = "bcdefghijklmnopqrstuvwxyza"
text = "Hello, World!"
encrypted_text = encrypt(text, key)
print("Texte chiffré :", encrypted_text)
decrypted_text = decrypt(encrypted_text, key)
print("Texte déchiffré :", decrypted_text)
