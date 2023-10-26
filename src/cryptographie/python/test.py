def encrypt(text, key):
    encrypted_text = ''
    for char in text:
        encrypted_char = chr(ord(char) + key)
        encrypted_text += encrypted_char
    return encrypted_text

def decrypt(text, key):
    decrypted_text = ''
    for char in text:
        decrypted_char = chr(ord(char) - key)
        decrypted_text += decrypted_char
    return decrypted_text

text = "T:26.49;L:115;169832654"
key = 3  # Vous pouvez choisir n'importe quelle clé numérique

# Chiffrement
encrypted_text = encrypt(text, key)
print("Texte chiffré:", encrypted_text)

# Déchiffrement
decrypted_text = decrypt(encrypted_text, key)
print("Texte déchiffré:", decrypted_text)