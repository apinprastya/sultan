import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import com.lekapin.presenter 0.1
import QtQuick.Controls.Material 2.0

Page {
    id: loginpage

    background: Rectangle {
        color: "#F0F0F0"
    }

    LoginPresenter {
        id: loginPresenter
    }

    Rectangle {
        width: parent.width < 500 ? parent.width - 10 : 500
        height: parent.height < 200 ? parent.height - 10 : 200
        color: "white"
        radius: 2
        border.width: 1
        border.color: root.dividerColor
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        RowLayout {
            id: rowLayout
            anchors.fill: parent

            Image {
                id: image
                fillMode: Image.PreserveAspectFit
                Layout.fillHeight: true
                Layout.maximumWidth: 150
                Layout.leftMargin: 10
                Layout.rightMargin: 5
                source: "qrc:/images/icon_256.png"
            }
            Rectangle {
                width: 1
                Layout.fillHeight: true
                color: root.dividerColor
            }
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.leftMargin: 5
                Layout.rightMargin: 10
                Text {
                    text: qsTr("Login")
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    font.pixelSize: 20
                    Layout.fillWidth: true

                }
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: root.dividerColor
                }
                TextField {
                    id: username
                    placeholderText: qsTr("username")
                    Layout.fillWidth: true
                }
                TextField {
                    id: password
                    placeholderText: qsTr("password")
                    Layout.fillWidth: true
                    echoMode: TextInput.Password
                    passwordCharacter: '#'
                }
                Button {
                    text: qsTr("Login")
                    Layout.fillWidth: true
                    Material.background: Material.Green
                    Material.foreground: "white"
                    Binding on enabled {
                        value: username.text != "" && password.text != ""
                    }
                    onClicked: {
                        loginPresenter.login(username.text, password.text)
                    }
                }
            }
        }

    }
}
