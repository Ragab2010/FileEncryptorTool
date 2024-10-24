#include "MainApp.h"


MainApp::MainApp(QObject* parent)
    : QObject(parent), currentFilePath("") , currentFileInfo(""), password("secretpassword"), encryptor("secretpassword") {}


QString MainApp::formatFileSize(qint64 size) {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double fileSize = size;

    while (fileSize >= 1024 && unitIndex < 4) {
        fileSize /= 1024;
        unitIndex++;
    }

    return QString("%1 %2").arg(fileSize, 0, 'f', 2).arg(units[unitIndex]);
}

void MainApp::selectFile(const QString& fileUrl) {
    QString filePath = QUrl(fileUrl).toLocalFile();
    QFileInfo fileInfo(filePath);
    // QString fileBaseName = fileInfo.completeBaseName();  // Base file name without extension
    // QString fileDir = fileInfo.absolutePath();           // Directory of the file
    // QString outputPath = fileDir + "/" + fileBaseName + ".enc";


    if (fileInfo.exists() && fileInfo.isFile()) {
        // std::cout << "input File Path: " << filePath.toStdString() << std::endl;
        // std::cout << "output File Path: " << outputPath.toStdString() << std::endl;

        try {
            encryptor.setPassword(password); 
            // encryptor.openFile(filePath.toStdString());
            currentFilePath = filePath;
            currentFileInfo = fileInfo;
            encryptor.setInputFilePath(filePath.toStdString());

            encryptor.setSizeInputFile(fileInfo.size());
            encryptor.setInputFileExtension(fileInfo.suffix().toStdString());

            QString outputPath = getOutputFilePath();
            emit fileDetailsUpdated(filePath, fileInfo.size(), fileInfo.suffix() ,outputPath );
            emit operationSuccess("File selected successfully.");
            // std::string filePath;
            // long fileSize;
            // std::string fileExtension;
            // encryptor.showFileDetails(filePath , fileSize , fileExtension)
            //emit fileDetailsUpdated(filePath , fileSize , fileExtension);
        } catch (const std::runtime_error& e) {
            emit errorOccurred(QString::fromStdString(e.what()));
        }
    } else {
        emit errorOccurred("File not found or not a valid file.");
    }
}


void MainApp::encryptFile() {
    emit operationSuccess("");
    if (!currentFilePath.isEmpty()) {
        updateUIState(false);
        QtConcurrent::run([this]() {
            try {
                encryptor.encryptFile([this](double progress) {
                    QMetaObject::invokeMethod(this, "handleProgress", Qt::QueuedConnection, Q_ARG(qreal, progress));
                });
                emit encryptionSuccess("File encrypted successfully");
            } catch (const std::runtime_error& e) {
                emit errorOccurred(QString::fromStdString(e.what()));
            }
            updateUIState(true);
        });
    } else {
        emit errorOccurred("No file selected for encryption.");
    }
}



void MainApp::decryptFile() {
        emit operationSuccess("");
    if (!currentFilePath.isEmpty()) {
        updateUIState(false);
        QtConcurrent::run([this]() {
            try {
                encryptor.decryptFile([this](double progress) {
                    QMetaObject::invokeMethod(this, "handleProgress", Qt::QueuedConnection, Q_ARG(qreal, progress));
                });
                emit decryptionSuccess("File decrypted successfully");
            } catch (const std::runtime_error& e) {
                emit errorOccurred(QString::fromStdString(e.what()));
            }
            updateUIState(true);
        });
    } else {
        emit errorOccurred("No valid encrypted file selected for decryption.");
    }
}

// void MainApp::encryptFile() {
//     if (!currentFilePath.isEmpty()) {
//         QtConcurrent::run([this]() {
//             try {
//                 encryptor.encryptFile([this](double progress) {
//                     QMetaObject::invokeMethod(this, "handleProgress", Qt::QueuedConnection, Q_ARG(qreal, progress));
//                 });
//                 emit encryptionSuccess("File encrypted successfully");
//             } catch (const std::runtime_error& e) {
//                 emit errorOccurred(QString::fromStdString(e.what()));
//             }
//         });
//     } else {
//         emit errorOccurred("No file selected for encryption.");
//     }
// }

// void MainApp::decryptFile() {
//     if (!currentFilePath.isEmpty()) {
//         QtConcurrent::run([this]() {
//             try {
//                 encryptor.decryptFile([this](double progress) {
//                     QMetaObject::invokeMethod(this, "handleProgress", Qt::QueuedConnection, Q_ARG(qreal, progress));
//                 });
//                 emit decryptionSuccess("File decrypted successfully");
//             } catch (const std::runtime_error& e) {
//                 emit errorOccurred(QString::fromStdString(e.what()));
//             }
//         });
//     } else {
//         emit errorOccurred("No valid encrypted file selected for decryption.");
//     }
// }

void MainApp::handleProgress(qreal progress) {
    emit progressUpdated(progress );
}




QString MainApp::getOutputFilePath() {
    return QString::fromStdString(encryptor.getOutputFilePath());
}

void MainApp::updateUIState(bool enabled) {
    emit uiStateChanged(enabled);
}


// void MainApp::encryptFile() {
//     if (!currentFilePath.isEmpty()) {
//         // QString outputFilePath = currentFilePath + ".enc";
//         // std::cout << "Output File Path: " << outputFilePath.toStdString() << std::endl;

//         try {
//             encryptor.encryptFile();  // Call encryptFile on the encryptor
//             //encryptor.saveFile(outputFilePath.toStdString()); // Save the encrypted data
//             emit encryptionSuccess("File encrypted successfully");
//         } catch (const std::runtime_error& e) {
//             emit errorOccurred(QString::fromStdString(e.what()));
//         }
//     } else {
//         emit errorOccurred("No file selected for encryption.");
//     }
// }

// void MainApp::decryptFile() {
//     if (!currentFilePath.isEmpty() ) {
//         // QString outputFilePath = currentFilePath + ".dec";
//         // std::cout << "Output File Path: " << outputFilePath.toStdString() << std::endl;

//         try {
//             encryptor.decryptFile();  // Call decryptFile on the encryptor
//             //encryptor.saveFile(outputFilePath.toStdString()); // Save the decrypted data
//             emit decryptionSuccess("File decrypted successfully");
//         } catch (const std::runtime_error& e) {
//             emit errorOccurred(QString::fromStdString(e.what()));
//         }
//     } else {
//         emit errorOccurred("No valid encrypted file selected for decryption.");
//     }
// }

void MainApp::openSaveFile() {
    QString savePath = QFileDialog::getSaveFileName(nullptr, "Save File As", "", "All Files (*)");
    if (savePath.isEmpty()) return;

    // std::cout << "Current File Path: " << currentFilePath.toStdString() << std::endl;
    // std::cout << "Save File Path: " << savePath.toStdString() << std::endl;

    try {
        //encryptor.saveFile(savePath.toStdString());  // Save the current buffer to the specified path
        encryptor.setOutputFilePath(savePath.toStdString());

        QString filePath = currentFilePath;
        QFileInfo fileInfo =currentFileInfo;
        QString outputPath = getOutputFilePath();
        emit fileDetailsUpdated(filePath, fileInfo.size(), fileInfo.suffix() ,outputPath );
        emit operationSuccess("Saved File Path set successfully.");
    } catch (const std::runtime_error& e) {
        emit errorOccurred(QString::fromStdString(e.what()));
    }
}

// void MainApp::setPassword(const QString& newPassword) {
//     password = newPassword.toStdString();
//     //encryptor.setPassword(newPassword.toStdString()); 
// }
void MainApp::setPassword(const QString& newPassword) {
    if (newPassword.length() < 8) {
        emit errorOccurred("Password must be at least 8 characters long.");
        return;
    }
    password = newPassword.toStdString();
    encryptor.setPassword(password);
    emit operationSuccess("Password Accepted.");
}
