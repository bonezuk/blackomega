import QtQuick 2.14
import QtQuick.Controls 2.14

import "components.js" as Comp

Rectangle {
    property int navIndex: 0
    property string sourceEnabled: ""
    property string sourceDisabled: ""
    property string label: ""
    
    color: 'black'

    Image {
        id: navButton
        source: (navBar.currentNavIndex === parent.navIndex) ? Comp.urlRelPath(sourceEnabled) : Comp.urlRelPath(sourceDisabled)
        width: ((parent.width < parent.height) ? parent.width : parent.height) / 2.0
        height: ((parent.width < parent.height) ? parent.width : parent.height) / 2.0
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -10.0
    }
    
    Text {
        text: parent.label
        color: (navBar.currentNavIndex == parent.navIndex) ? "white" : "#323232"
        font.pointSize: 20
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: navButton.bottom
    }
    
    MouseArea {
        anchors.fill:parent
        onClicked: parent.clicked()
    }
    
    signal clicked()
    
    onClicked: parent.currentNavIndex = navIndex
}