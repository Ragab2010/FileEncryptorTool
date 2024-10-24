#include "fileEncryptor.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/stat.h> 
#include <algorithm>

FileEncryptor::FileEncryptor(const std::string& newPassword):key(),mInputFilePath("") ,
                                mOutputFilePath(""), mInputFileExtension(""), mOuputFileExtension(""),
                                mSizeInputFile(0), password("")
                                {
    password = newPassword;  
    key = generateKey(password);
}

std::vector<unsigned char> FileEncryptor::generateKey(const std::string& password) {
    std::vector<unsigned char> key(32); // AES-256 key size is 32 bytes
    const unsigned char* salt = nullptr; // No salt
    PKCS5_PBKDF2_HMAC(password.c_str(), password.size(), salt, 0, 10000, EVP_sha256(), 32, key.data());
    return key;
}

std::vector<unsigned char> FileEncryptor::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }
    std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::move(buffer);
}


// void FileEncryptor::openFile(const std::string& inputFile) {
//     buffer.clear();
//     //std::vector<unsigned char>().swap(buffer); to make capacity = 0
//     mInputFilePath = inputFile;
//     buffer = readFile(inputFile); // Read the file contents into the buffer
// }

void FileEncryptor::writeFile(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    // std::copy(std::begin(data), std::end(data), std::ostreambuf_iterator<char>(file));

}



std::vector<unsigned char> FileEncryptor::generateIV() {
    std::vector<unsigned char> iv(16); // AES-CBC IV size is 16 bytes
    RAND_bytes(iv.data(), iv.size());
    return iv;
}

std::vector<unsigned char> FileEncryptor::aesEncrypt(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len;
    std::vector<unsigned char> ciphertext(plaintext.size() + EVP_CIPHER_block_size(EVP_aes_256_cbc()));

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size());
    int ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len);
    return std::move(ciphertext);
}

std::vector<unsigned char> FileEncryptor::aesDecrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    int len;
    std::vector<unsigned char> plaintext(ciphertext.size());

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size());
    int plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    plaintext.resize(plaintext_len);
    return std::move(plaintext);
}




// void FileEncryptor::saveFile(const std::string& outputFile) {
//     writeFile(outputFile, buffer); // Write the buffer contents to the output file
// }

// Show details of the file
void FileEncryptor::showFileDetails(std::string& filePath, long& fileSize, std::string& fileExtension) {
    filePath = mInputFilePath;

    // Get file size
    // fileSize =std::distance(begin(buffer), end(buffer));

    // Get file size
    struct stat statBuf;
    if (stat(filePath.c_str(), &statBuf) == 0) {
        fileSize = statBuf.st_size;
    } else {
        fileSize = -1; // Error in getting file size
    }

    // Get file extension
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        fileExtension = filePath.substr(dotPos + 1);
    } else {
        fileExtension = ""; // No extension
    }
}

void FileEncryptor::setPassword(const std::string& newPassword) {
    password = newPassword;  
    key = generateKey(password);
}



void FileEncryptor::setOutputFilePath(const std::string& outputFilePath){
    mOutputFilePath = outputFilePath;
}

void FileEncryptor::setSizeInputFile(long long  sizeInputFile){
    mSizeInputFile = sizeInputFile;
}

void FileEncryptor::setInputFileExtension(std::string  fileExtension){
    mInputFileExtension=fileExtension ; 
}


// Reads `blockSize` bytes from a file and returns them in a vector
std::vector<unsigned char> FileEncryptor::readFile(std::ifstream& file, size_t blockSize) {
    std::vector<unsigned char> buffer(blockSize);
    file.read(reinterpret_cast<char*>(buffer.data()), blockSize);
    buffer.resize(file.gcount());  // Adjust size in case fewer bytes are read
    return buffer;
}

// Writes the buffer to a file
void FileEncryptor::writeFile(std::ofstream& file, const std::vector<unsigned char>& buffer) {
    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
}


// Function to check if the file is already encrypted (contains the signature)
bool FileEncryptor::isFileEncrypted(const std::string& filePath) {
    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) throw std::runtime_error("Could not open file: " + filePath);

    char signature[SIGNATURE_LEN] = {0};
    inFile.read(signature, SIGNATURE_LEN);

    return std::string(signature) == SIGNATURE;
}


void FileEncryptor::encryptFile() {
    // Check if the file is already encrypted
    if (isFileEncrypted(mInputFilePath)) {
        throw std::runtime_error("File is already encrypted.");
    }
    // Open input and output files
    std::ifstream inFile(mInputFilePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open input file for encryption: " + mInputFilePath);
    }
    // Open the output file for writing
    std::ofstream outFile(mOutputFilePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open input file for decryption: " + mOutputFilePath);
    }

    // Write the signature to the output file
    outFile.write(SIGNATURE, SIGNATURE_LEN);

    // Generate and write the IV after the signature
    std::vector<unsigned char> iv = generateIV();
    outFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    std::vector<unsigned char> buffer(CHUNK_SIZE);
    std::vector<unsigned char> ciphertext(CHUNK_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len, ciphertext_len;

    while (inFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || inFile.gcount() > 0) {
        int readBytes = inFile.gcount();
        EVP_EncryptUpdate(ctx, ciphertext.data(), &len, buffer.data(), readBytes);
        outFile.write(reinterpret_cast<const char*>(ciphertext.data()), len);
    }

    EVP_EncryptFinal_ex(ctx, ciphertext.data(), &len);
    outFile.write(reinterpret_cast<const char*>(ciphertext.data()), len);
    
    EVP_CIPHER_CTX_free(ctx);

    inFile.close();
    outFile.close();
}


void FileEncryptor::decryptFile() {
    // Check if the file is already encrypted
    if (!isFileEncrypted(mInputFilePath)) {
        throw std::runtime_error("File is already decrypted.");
    }
    // Open input and output files
    std::ifstream inFile(mInputFilePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open input file for encryption: " + mInputFilePath);
    }
    // Open the output file for writing
    std::ofstream outFile(mOutputFilePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open input file for decryption: " + mOutputFilePath);
    }


    // Skip the signature
    inFile.seekg(SIGNATURE_LEN, std::ios::beg);

    // Read the IV
    std::vector<unsigned char> iv(16);
    inFile.read(reinterpret_cast<char*>(iv.data()), iv.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    std::vector<unsigned char> buffer(CHUNK_SIZE);
    std::vector<unsigned char> plaintext(CHUNK_SIZE);
    int len, plaintext_len;

    while (inFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || inFile.gcount() > 0) {
        int readBytes = inFile.gcount();
        EVP_DecryptUpdate(ctx, plaintext.data(), &len, buffer.data(), readBytes);
        outFile.write(reinterpret_cast<const char*>(plaintext.data()), len);
    }

    EVP_DecryptFinal_ex(ctx, plaintext.data(), &len);
    outFile.write(reinterpret_cast<const char*>(plaintext.data()), len);

    EVP_CIPHER_CTX_free(ctx);

    inFile.close();
    outFile.close();
}



void FileEncryptor::encryptFile(std::function<void(double)> progressCallback) { 

    // Check if the file is already encrypted
    if (isFileEncrypted(mInputFilePath)) {
        throw std::runtime_error("File is already encrypted.");
    }
    // Open input and output files
    std::ifstream inFile(mInputFilePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open input file for encryption: " + mInputFilePath);
    }
    // Open the output file for writing
    std::ofstream outFile(mOutputFilePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open input file for decryption: " + mOutputFilePath);
    }

    // Write the signature to the output file
    outFile.write(SIGNATURE, SIGNATURE_LEN);

    // Store the original file extension
    //std::string originalExtension = mInputFileExtension;
    std::string originalExtension = getInputFileExtension(mInputFilePath);
    storeOriginalExtension(outFile, originalExtension);

    // Generate and write the IV after the signature
    std::vector<unsigned char> iv = generateIV();
    outFile.write(reinterpret_cast<const char*>(iv.data()), iv.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    std::vector<unsigned char> buffer(CHUNK_SIZE);
    std::vector<unsigned char> ciphertext(CHUNK_SIZE + EVP_CIPHER_block_size(EVP_aes_256_cbc()));
    int len, ciphertext_len;

    long long totalSize = mSizeInputFile;
    long long processedSize = 0;

    while (inFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || inFile.gcount() > 0) {
        int readBytes = inFile.gcount();
        EVP_EncryptUpdate(ctx, ciphertext.data(), &len, buffer.data(), readBytes);
        outFile.write(reinterpret_cast<const char*>(ciphertext.data()), len);

        processedSize += readBytes;
        progressCallback(static_cast<double>(processedSize) / totalSize);
    }

    EVP_EncryptFinal_ex(ctx, ciphertext.data(), &len);
    outFile.write(reinterpret_cast<const char*>(ciphertext.data()), len);
    
    EVP_CIPHER_CTX_free(ctx);

    inFile.close();
    outFile.close();
}

void FileEncryptor::decryptFile(std::function<void(double)> progressCallback) {

        // Check if the file is already encrypted
    if (!isFileEncrypted(mInputFilePath)) {
        throw std::runtime_error("File is already decrypted.");
    }
    // Open input and output files
    std::ifstream inFile(mInputFilePath, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Could not open input file for encryption: " + mInputFilePath);
    }
    // Open the output file for writing
    std::ofstream outFile(mOutputFilePath, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not open input file for decryption: " + mOutputFilePath);
    }


    // Skip the signature
    inFile.seekg(SIGNATURE_LEN, std::ios::beg);

    // Retrieve the original file extension
    std::string originalExtension = retrieveOriginalExtension(inFile);

    //Update the output file path with the original extension
    // if (!originalExtension.empty()) {
    //     mOutputFilePath += "." + originalExtension;
    // }
    // std::cout<<"originalExtension :"<<originalExtension<<std::endl;

    // Read the IV
    std::vector<unsigned char> iv(16);
    inFile.read(reinterpret_cast<char*>(iv.data()), iv.size());

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.data(), iv.data());

    std::vector<unsigned char> buffer(CHUNK_SIZE);
    std::vector<unsigned char> plaintext(CHUNK_SIZE);
    int len, plaintext_len;


    long long totalSize = mSizeInputFile - SIGNATURE_LEN - 16; // Subtract signature and IV size
    long long processedSize = 0;

    while (inFile.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || inFile.gcount() > 0) {
        int readBytes = inFile.gcount();
        EVP_DecryptUpdate(ctx, plaintext.data(), &len, buffer.data(), readBytes);
        outFile.write(reinterpret_cast<const char*>(plaintext.data()), len);

        processedSize += readBytes;
        progressCallback(static_cast<double>(processedSize) / totalSize);
    }

    EVP_DecryptFinal_ex(ctx, plaintext.data(), &len);
    outFile.write(reinterpret_cast<const char*>(plaintext.data()), len);

    EVP_CIPHER_CTX_free(ctx);

    inFile.close();
    outFile.close();
    // outFile.open(mOutputFilePath, std::ios::binary);
    // if (!outFile.is_open()) {
    //     throw std::runtime_error("Could not open output file for decryption: " + mOutputFilePath);
    // }

}





void FileEncryptor::setInputFilePath(const std::string& inputFilePath) {
    mInputFilePath = inputFilePath;
    updateOutputFilePath(inputFilePath);
}

std::string FileEncryptor::getOutputFilePath() const {
    return mOutputFilePath;
}

void FileEncryptor::updateOutputFilePath(const std::string& inputFilePath) {
    if (isFileEncrypted(mInputFilePath)) {
        // Input file is encrypted, so we're decrypting
        size_t lastDotPos = mInputFilePath.find_last_of('.');
        if (lastDotPos != std::string::npos ) {

            mOutputFilePath = mInputFilePath.substr(0, lastDotPos);
                // Open input and output files
            std::ifstream inFile(mInputFilePath, std::ios::binary);
            if (!inFile.is_open()) {
                throw std::runtime_error("Could not open input file for encryption: " + mInputFilePath);
            }
            // Skip the signature
            inFile.seekg(SIGNATURE_LEN, std::ios::beg);
            // Retrieve the original file extension
            std::string originalExtension = retrieveOriginalExtension(inFile);
            std::cout<<"originalExtension :"<<originalExtension<<std::endl;

            //Update the output file path with the original extension
            if (!originalExtension.empty()) {
                mOutputFilePath += "." + originalExtension;
            }
            inFile.close();
            //std::cout<<"isFileEncrypted  mOutputFilePath:"<<mOutputFilePath<<std::endl;
        }
        // The original extension will be retrieved during decryption
    } else {
        // Input file is not encrypted, so we're encrypting
        size_t lastDotPos = mInputFilePath.find_last_of('.');
        // add ".enc" extension
        mOutputFilePath = mInputFilePath.substr(0, lastDotPos) +  ".enc";
        //std::cout<<"!isFileEncrypted  mOutputFilePath:"<<mOutputFilePath<<std::endl;
       // mOutputFilePath = mInputFilePath + ".enc";

    }
}

std::string FileEncryptor::getInputFileExtension(const std::string& filePath) {
    size_t lastDotPos = filePath.find_last_of('.');
    if (lastDotPos != std::string::npos) {
        std::string extension = filePath.substr(lastDotPos + 1);
        // Check if the extension is "enc" (for encrypted files)
        // if (extension != "enc") {
        //     return extension;
        // }
        return extension;
    }
    return "   ";
}

void FileEncryptor::storeOriginalExtension(std::ofstream& outFile, const std::string& extension) {
    uint8_t extensionLength = extension.length();
    outFile.write(reinterpret_cast<const char*>(&extensionLength), sizeof(extensionLength));
    outFile.write(extension.c_str(), extensionLength);
}

std::string FileEncryptor::retrieveOriginalExtension(std::ifstream& inFile) {
    uint8_t extensionLength;
    inFile.read(reinterpret_cast<char*>(&extensionLength), sizeof(extensionLength));
    std::string extension(extensionLength, '\0');
    inFile.read(&extension[0], extensionLength);
    return extension;
}