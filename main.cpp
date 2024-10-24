// main.cpp
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "MainApp.h" 

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    QQmlApplicationEngine engine;
    MainApp mainApp;

    engine.rootContext()->setContextProperty("mainApp", &mainApp);
    engine.load(QUrl(QStringLiteral("qrc:/qml/MainWindow.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}

#include "main.moc"

/*
it work great , now I have add some modification  , I want  the program automatically when the input folder is is not encrypted (input file  come to encrypt it )  the mOutputFilePath automatically become .enc  , and when the input file is encrypted ( input file com to do decrypt it )  the mOutputFilePath automatically become .enc with the origin decrypted file as we do encrypt  file (video.mp4 as input file )  to produce video (video.enc  as output file) , and when we decrypt (video.enc as input file ) the program  produce video (video.mp4  as output file) this I think can happen when we encrypt video.mp4 to video.enc , we write extension mp4 at the video.enc as IV  , and when we decrypt File we extact the extension if found and place it mOutputFilePath extenstion to produce from video.en  a video.mp4   , handle this situation and this happens  when we select the file to put extention for  mOutputFilePath early
*/