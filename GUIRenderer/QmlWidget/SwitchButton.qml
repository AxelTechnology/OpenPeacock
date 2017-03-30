//import QtQuick 2.3
import QtQuick 1.0


Row {

    id: button
    property bool checked: false


    Rectangle {
        id: btnLeft
        color: "grey"
        width: 160
        height: 40
        //anchors.centerIn: parent
        Text {
            id: on
            text: qsTr("On")
            font.bold: true
            anchors.centerIn: parent
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                console.log("On")
                button.checked = true;
                //button.state = "stateOn"
            }

        }
    }

    Rectangle {
        id: btnRight
        color: "darkgrey"
        width: 160
        height: 40
        //anchors.centerIn: parent
        Text {
            id: off
            text: qsTr("Off")
            anchors.centerIn: parent
        }

        MouseArea{
            anchors.fill: parent
            onClicked: {
                button.checked = false;
                console.log("Off")
                //button.state = "stateOff"
            }

        }

    }



    states: [
        State {
            name: "stateOff"
            when: !button.checked
            PropertyChanges {
                target: btnLeft
                color: "darkgrey"
            }
            PropertyChanges {
                target: on
                font.bold: false
            }
            PropertyChanges {
                target: btnRight
                color: "grey"
            }
            PropertyChanges {
                target: off
                font.bold: true
            }
        }
    ]


}
