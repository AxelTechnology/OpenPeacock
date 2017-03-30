import QtQuick 2.6

Rectangle {
    property real percentage: 0.5

    id: bg
    height: 40
    width: 200
    color: "darkgrey"

    Rectangle {
        id: bar
        height: bg.height - 6
        width: (bg.width - 3 -30) * percentage
        color: "red"
        border.color: "darkred"
        border.width: 3
        anchors.verticalCenter: bg.verticalCenter
        anchors.left: bg.left
        anchors.leftMargin: 30

        Behavior on width {
            NumberAnimation {
                //target: object
                duration: 200
                easing.type: Easing.Linear
            }
        }
    }

    Text {
        text: percentage.toFixed(2)
        anchors.verticalCenter: bg.verticalCenter
        anchors.left: bg.left
        anchors.leftMargin: 2
    }
}
