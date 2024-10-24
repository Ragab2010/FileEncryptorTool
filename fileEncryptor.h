#ifndef FILE_ENCRYPTOR_H
#define FILE_ENCRYPTOR_H

#include <vector>
#include <string>
#include <iterator>
#include <algorithm>
#include <functional>

#define CHUNK_SIZE 65536  // Process files in 64 KB chunks
#define SIGNATURE "ENC_FILE"  // Unique signature for encrypted files
#define SIGNATURE_LEN 8       // Length of the signature


class FileEncryptor {
public:
    FileEncryptor(const std::string& password);

    // // Open a file and read its contents into the internal buffer
    // void openFile(const std::string& inputFile);
    
    // // Save the contents of the buffer to the specified output file
    // void saveFile(const std::string& outputFile);

    // Encrypt the currently opened file and store the result in the buffer
    void encryptFile();

    // Decrypt the currently opened file and store the result in the buffer
    void decryptFile();


    // Displays the file details: path, size, and extension
    void showFileDetails(std::string& filePath, long& fileSize, std::string& fileExtension);

    void encryptFile(std::function<void(double)> progressCallback);
    void decryptFile(std::function<void(double)> progressCallback);


    void setPassword(const std::string& newPassword);
    void setInputFilePath(const std::string& inputFilePath);
    void setOutputFilePath(const std::string& outputFilePath);
    std::string getOutputFilePath() const;
    void setSizeInputFile(long long  sizeInputFile);
    void setInputFileExtension(std::string  fileExtension);

    
private:
    std::vector<unsigned char> key;   // key  to hold key
    std::string mInputFilePath; // Path of the input file
    std::string mOutputFilePath; // Path of the  output file
    std::string mInputFileExtension;
    std::string mOuputFileExtension;
    long long  mSizeInputFile ; // size
    std::string password;  // Store the password


    // Generate the encryption key based on the password
    std::vector<unsigned char> generateKey(const std::string& password);

    // Read the content of a file
    std::vector<unsigned char> readFile(const std::string& filename);

    // Write the content to a file
    void writeFile(const std::string& filename, const std::vector<unsigned char>& data);


    // Encrypt the data using AES-256-CBC
    std::vector<unsigned char> aesEncrypt(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& iv);

    // Decrypt the data using AES-256-CBC
    std::vector<unsigned char> aesDecrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& iv);

    // Generate a random Initialization Vector (IV)
    std::vector<unsigned char> generateIV();

    std::vector<unsigned char> readFile(std::ifstream& file, size_t blockSize);

    void writeFile(std::ofstream& file, const std::vector<unsigned char>& buffer);

    bool isFileEncrypted(const std::string& filePath);

    void updateOutputFilePath(const std::string& inputFilePath);
    std::string getInputFileExtension(const std::string& filePath);
    void storeOriginalExtension(std::ofstream& outFile, const std::string& extension);
    std::string retrieveOriginalExtension(std::ifstream& inFile);

};

#endif
