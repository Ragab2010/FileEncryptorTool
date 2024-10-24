import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2

ApplicationWindow {
    visible: true
    width: 800
    height: 540
    title: "File Encryptor"

    property bool uiEnabled: true

    FileDialog {
        id: fileDialog
        title: "Select a file"
        onAccepted: {
            mainApp.selectFile(fileDialog.fileUrl);
        }
    }

    Column {
        spacing: 20
        anchors.fill: parent
        anchors.margins: 20 // Add some margins
        visible: true

        // Title: File Encryption Tool
        Text {
            text: "File Encryption Tool"
            font.pixelSize: 24
            font.weight: Font.Bold
            anchors.horizontalCenter: parent.horizontalCenter
        }

        // Input Field with Floating Label : Password input TextField
        Column {
            spacing: 4
            width: parent.width

            Label {
                id: floatingLabel
                text: "Enter Password"
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: textField.text.length > 0 ? 0 : 1
                font.pixelSize: 14
                color: "grey"
            }

            // Rectangle for border effect
            //TextField for input password
            Rectangle {
                width: parent.width
                height: 40 // Adjust height as needed
                color: "transparent"
                border.color: "lightgrey"
                border.width: 1
                radius: 8

                TextField {
                    id: textField
                    anchors.fill: parent // Fill the Rectangle
                    padding: 10
                    echoMode: TextInput.Password
                    onTextChanged: {
                        mainApp.setPassword(text);  // Call setPassword whenever the text changes
                    }

                    onFocusChanged: {
                        floatingLabel.opacity = focus ? 0 : (text.length === 0 ? 1 : 0);
                    }
                    enabled: uiEnabled
                } // Closing TextField brace
            } // Closing Rectangle brace
            //helperText :Use a strong password for encryption
            Text {
                id: helperText
                text: "Use a strong password for encryption."
                //anchors.top: textField.bottom
                //anchors.left: parent.left
                //anchors.right: parent.right
                anchors.topMargin: 8
                font.pixelSize: 12
                color: "grey"
            }
        }

        // File Selection Button :Choose File
        Button {
            text: "Choose Input File"
            width: parent.width
            padding: 12
            onClicked: fileDialog.open()
            enabled: uiEnabled
        }

        // Labels for File Details(File Path , File Size , File Extension)
        Column {
            spacing: 5
            //anchors.horizontalCenter: parent.horizontalCenter
            anchors.left: parent.left // Align the column to the left of the parent
            //anchors.top: parent.top // Optional: you can also set top alignment
            anchors.margins: 10 // Optional: set some margins if needed


            Text {
                id: filePathLabel
                text: "File Path: "
                font.pixelSize: 14
                font.weight: Font.Normal
            }
                    // Add a new label for output file path
            Text {
                id: outputFilePathLabel
                text: "Output File Path: "
                font.pixelSize: 14
                font.weight: Font.Normal
            }

            Text {
                id: fileSizeLabel
                text: "File Size: "
                font.pixelSize: 14
                font.weight: Font.Normal
            }

            Text {
                id: fileExtensionLabel
                text: "File Extension: "
                font.pixelSize: 14
                font.weight: Font.Normal
            }
        }

        // Save the output File
        Button {
            text: "Save Output File"
            width: parent.width
            padding: 12
            onClicked: {
                if (filePathLabel.text !== "File Path: ") {
                    mainApp.openSaveFile();
                } else {
                    statusLabel.color = "red";
                    statusLabel.text = "No file to save.";
                }
            }
            //enabled: uiEnabled && filePathLabel.text !== "File Path: "
            enabled: uiEnabled
        }

        // Action Buttons :(Encrypt File , )
        //Column {
        Row {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter
            //width: parent.width

            //Encrypt File
            Button {
                text: "Encrypt File"
                width: 120
                //width: parent.width
                padding: 10
                onClicked: {
                    if (filePathLabel.text !== "File Path: ") {
                        //console.log(filePathLabel.text);
                        //mainApp.encryptFile(filePathLabel.text.replace("File Path: ", ""));
                        mainApp.encryptFile();
                    } else {
                        statusLabel.text = "Please select a file first.";
                    }
                }
                //enabled: uiEnabled && filePathLabel.text !== "File Path: "
                enabled: uiEnabled
            }
            //Decrypt File
            Button {
                text: "Decrypt File"
                width: 120
                //width: parent.width
                padding: 10
                onClicked: {
                    if (filePathLabel.text !== "File Path: ") {
                        //mainApp.decryptFile(filePathLabel.text.replace("File Path: ", ""));
                        mainApp.decryptFile();
                    } else {
                        statusLabel.text = "Please select a file first.";
                    }
                }
                //enabled: uiEnabled && filePathLabel.text !== "File Path: "
                enabled: uiEnabled
            }
        }


//        ProgressBar {
//            id: operationProgress
//            width: parent.width
//            value: 0
//          visible: true
//       }
    Rectangle {
        width: parent.width
        height: 10
        color: "lightgrey"  // Background color
        //color: "green"  // Background color

        ProgressBar {
            id: operationProgress
            anchors.fill: parent
            value: 0
            visible: true
        }
    }



        // Status Label
        Text {
            id: statusLabel
            text: ""
            font.pixelSize: 14
            font.weight: Font.Normal
            color: "red"
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }


    Connections {
        target: mainApp


        function onProgressUpdated(progress) {
            operationProgress.value = progress;
            operationProgress.visible = progress > 0 && progress < 1;
        }

        function onFileDetailsUpdated(path, size, extension, outputPath) {
            filePathLabel.text = "Input File Path: " + path;
            fileSizeLabel.text = "File Size: " + mainApp.formatFileSize(size);
            fileExtensionLabel.text = "File Extension: " + extension;
            outputFilePathLabel.text = "Output File Path: " + outputPath;
        }
        function onUiStateChanged(enabled) {
            uiEnabled = enabled;
        }

        function onEncryptionSuccess(message) {
            statusLabel.color = "green";  // Set to green on success
            statusLabel.text = message;
        }

        function onDecryptionSuccess(message) {
            statusLabel.color = "green";  // Set to green on success
            statusLabel.text = message;
        }
        function onOperationSuccess(message) {
            statusLabel.color = "green";  // Set to green on success
            statusLabel.text = message;
        }

        function onErrorOccurred(error) {
            statusLabel.color = "red";  // Set to red on error
            statusLabel.text = error;
        }
    }

}
