import QtQuick 2.6
import QtQuick.Window 2.2

Item {
    id: bars

    width: 360
    height: 360

    property var levels: [0.3, 0.76, 1, 0.15, 0.4]

    Column {
        spacing: 10
        anchors.centerIn: parent

        Repeater {
            model: 5
            Bar {
                percentage: levels[index]
            }
        }

        Timer{
            interval: 200
            running: true
            repeat: true
            onTriggered:{
                var newlevels = [];
                for (var i = 0; i < levels.length; i++)
                {
                    newlevels.push(Math.random());
                }
                levels = newlevels;
            }
        }
    }
}
