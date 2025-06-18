import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

Rectangle {
    id: root
    color: "#82E0AA"
    state: "invisible"
    z: 1
    
    property string text
    
    onTextChanged: {
        if(state === "visible")
        {
            timer.restart();
        }
        else
        {
            state = "visible";
            timer.running = true
        }
    }
    
    anchors.bottomMargin: 25
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    width: 2 * parent.width / 3
    height: 50
    border.color: "transparent"
    border.width: 3
    radius: 5
    
    Text {
        id: dialogText
        text: parent.text
        anchors.centerIn: parent
        font.pixelSize: 15
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        width: parent.width
        wrapMode: Text.Wrap
    }
    
    states: [
        State {
            name: "invisible"
            PropertyChanges { target: root; opacity: 0.0 }
        },
        State {
            name: "visible"
            PropertyChanges { target: root; opacity: 1.0 }
        }
    ]
    
    transitions: [
        Transition {
            from: "*"; to: "*"
            PropertyAnimation { properties: "opacity"; duration:500; easing.type: Easing.InOutQuad }
        }
    ]
    
    Timer {
        id: timer
        interval: 3000
        running: false
        onTriggered: state = "invisible"
    }

}