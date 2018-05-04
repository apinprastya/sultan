import QtQuick 2.7
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0

Page {
    id: loginpage

    Component.onCompleted: {
    }

    background: Rectangle {
        color: "#F0F0F0"
    }

    FlashMessage {
        id: popup
    }

    Frame {
        id: frame
        x: 220
        y: 140
        width: 500
        height: 205
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        background: Rectangle {
            color: "transparent"
            border.color: "#808080"
            radius: 10
        }

        Image {
            id: image
            y: 38
            width: 150
            height: 150
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 0
            source: "../../sultan/images/icon_512.png"
        }

        Rectangle {
            id: rectangle
            y: 0
            width: 2
            height: 176
            color: "#808080"
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: image.right
            anchors.leftMargin: 6
            border.color: "#808080"
        }

        Text {
            id: text1
            x: 164
            y: 0
            width: 312
            height: 40
            text: qsTr("Sultan Login")
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 32
        }

        TextField {
            id: username
            x: 164
            y: 46
            width: 312
            height: 40
            placeholderText: "Username"
        }

        TextField {
            id: password
            x: 164
            y: 92
            width: 312
            height: 40
            placeholderText: "Password"
            echoMode: TextInput.Password
        }

        Button {
            id: loginButton
            x: 164
            y: 138
            width: 312
            height: 40
            text: qsTr("Login")
            onClicked: {
                if(username.text == "" || password.text == "") {
                    popup.message = "HOLA"
                    popup.open()
                    return
                }

                console.log("YES")
            }
        }
    }
}
