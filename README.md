# Simplified AES Encryption in C

This repository contains a simplified implementation of the Advanced Encryption Standard (AES) algorithm in the C programming language. The purpose of this project is to provide a clear and educational example of the key components involved in AES encryption and decryption.

## Features

- **Simplified Implementation:** The code focuses on clarity and simplicity, making it a useful resource for educational purposes and understanding the basics of AES.
  
- **Command Line Interface:** The program includes a command-line interface for easy encryption and decryption of data using user-provided keys.
  
- **Modular Structure:** Functions for key expansion, substitution, mixing columns, and other operations are modularized, making the code easy to comprehend and extend.

## Getting Started

Follow these steps to get started with the simplified AES encryption in C:

1. **Clone the repository:**
   ```bash
   git clone https://github.com/engRana404/Simplified_AES
   cd simplified-aes-c
   ```

2. **Compile the program:**
   ```bash
   gcc saes_1901398.c -o saes_1901398
   ```

3. **Run the program:**
   ```bash
   ./saes_1901398 ENC|DEC key data
   ```
   Replace `ENC|DEC`, `key`, and `data` with your desired encryption/decryption mode, key, and data.

## Example

- **Encrypt data:**
   ```bash
   ./saes_1901398 ENC 0011 3344
   ```

- **Decrypt data:**
   ```bash
   ./saes_1901398 DEC 0011 3344
   ```

## Acknowledgments

- This implementation is inspired by the AES encryption algorithm, a widely used symmetric encryption standard.