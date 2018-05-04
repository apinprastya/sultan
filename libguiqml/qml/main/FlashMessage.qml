import QtQuick 2.7
import QtQuick.Controls 2.0

Popup {
    property string message: "value"

    id: popup
    x: (parent.width - width) / 2
    y: 10
    width: 253
    height: 49
    clip: true
    visible: false
    modal: false
    focus: false

    background: Rectangle {
        color: "red"
    }

    onOpened: {
        timer.start()
    }

    Text {
        text: message
    }

    Timer {
        id: timer
        interval: 1000
        running: false
        repeat: false
        onTriggered: {
            popup.close()
        }
    }
}
