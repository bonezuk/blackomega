import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

Rectangle {
    id: lockAppDialog
    anchors.fill: parent
    color: "#60000000"
    z: 2
    visible: false
    
    property string text
    
    Text {
        id: dialogText
        anchors.left: parent.left
        anchors.right: waitImage.left    
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        text: parent.text
        font.pixelSize: 30
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.Wrap
        color: "white"
    }
    Image {
        id: waitImage
        source: Comp.urlRelPath("images/progress/alphaWait01@2x.png")
        fillMode: Image.PreserveAspectFit
        width: 64
        height: 64

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        
        Timer {
            property int imageIndex: 1
            interval: 100
            running: true
            repeat: true
            onTriggered: {
                let imageName = "images/progress/alphaWait";
                imageIndex += 1;
                if(imageIndex > 12)
                {
                    imageIndex = 1;
                }
                imageName += Comp.stringWithDigits(imageIndex, 2);
                imageName += "@2x.png";
                parent.source = Comp.urlRelPath(imageName);
            }
        }
    }
    MouseArea {
        anchors.fill: parent
    }
    TapHandler {
    }
}