import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0

Page {
    id: root

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

    /*Timer {
        id: timer
        interval: 500
        running: false
        repeat: false
        onTriggered: {
            if(settings.firstRun)
                root.StackView.view.replace(Util.getQmlPath("FirstSettingPage.qml"))
            else
                root.StackView.view.replace(Util.getQmlPath("CheckConnectionPage.qml"))
        }
    }*/

    Component.onCompleted: {
        //timer.start()
        console.log("DONE")
    }
}
