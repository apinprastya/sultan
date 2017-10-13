import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0

Page {
    id: splashroot

    background: Rectangle {
        color: "#F0F0F0"
    }

    Settings {
        id: settings
        property bool firstRun: true
    }

    Image {
        id: image
        opacity: 1
        clip: false
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit
        source: "qrc:/images/icon_512.png"
    }

    Text {
        id: message
        anchors.horizontalCenter: image.horizontalCenter
        text: "Initializing..."
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        visible: true
    }

    Connections {
        target: CCore
        onSignalShowMessage: {
            message.text = msg
        }
    }

    Connections {
        target: CCore
        onSignalShowLogin: {
            stackView.replace("Login.qml")
        }
    }

    Component.onCompleted: {
    }
}
