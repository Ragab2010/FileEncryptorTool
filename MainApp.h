#ifndef MAINAPP_H
#define MAINAPP_H

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QUrl>
#include <QFileDialog>
#include <iostream>
#include <QThread>
#include <QFuture>
#include <QtConcurrent>
#include "fileEncryptor.h"

class MainApp : public QObject {
    Q_OBJECT
public :
    explicit MainApp(QObject* parent = nullptr);
//public slots:  

    Q_INVOKABLE void selectFile(const QString& fileUrl);
    Q_INVOKABLE void encryptFile();
    Q_INVOKABLE void decryptFile();
    Q_INVOKABLE void openSaveFile();
    Q_INVOKABLE void setPassword(const QString& password);
    Q_INVOKABLE QString formatFileSize(qint64 size);

    Q_INVOKABLE QString getOutputFilePath();
    Q_INVOKABLE void updateUIState(bool enabled);

signals:
    void fileDetailsUpdated(const QString& path, qint64 size, const QString& extension, const QString& outputPath);
    void encryptionSuccess(const QString& message);
    void decryptionSuccess(const QString& message);
    void operationSuccess(const QString& message);   
    void errorOccurred(const QString& error);
    void progressUpdated(qreal progress);
    void uiStateChanged(bool enabled);

private slots:
    void handleProgress(qreal progress);

private:
    QString currentFilePath;
    QFileInfo currentFileInfo;
    std::string password;
    FileEncryptor encryptor;  // Private FileEncryptor variable

};

#endif // MAINAPP_H
