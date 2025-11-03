import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

ListView {
    id: playListView

    property var playbackState

    signal clicked()

    delegate: SwipeDelegate {
        id: playListItem

        property bool isDeleted: false

        implicitWidth: parent.width
        implicitHeight: 60

        swipe.right: Label {
            id: deleteLabel
            text: qsTr("Delete")
            color: "white"
            verticalAlignment: Label.AlignVCenter
            padding: 12
            height: parent.height
            anchors.right: parent.right

            SwipeDelegate.onClicked: {
                isDeleted = true;
                playListView.model.remove(index);
            }

            background: Rectangle {
                color: deleteLabel.SwipeDelegate.pressed ? Qt.darker("tomato", 1.1) : "tomato"
            }
        }

        SequentialAnimation {
            id: animOnDelete

            PropertyAction {
                target: playListItem
                property: "ListView.delayRemove"
                value: true
            }
            NumberAnimation {
                target: playListItem
                property: "height"
                to: 0
                easing.type: Easing.InOutQuad
            }
            PropertyAction {
                target: playListItem
                property: "ListView.delayRemove"
                value: false
            }
        }

        ListView.onRemove: animOnDelete.start()

           onClicked: {
            if(!isDeleted)
            {
                if(playListDClkTimer.running === true)
                {
                    console.log("dd = " + index);
                    playListView.currentIndex = index;
                    playListView.clicked();
                    playListDClkTimer.running = false;
                }
                else
                {
                    playListDClkTimer.running = true;
                }
            }
        }

        Timer {
            id: playListDClkTimer
            interval: 500
            running: false
            repeat: false
        }

        background: Rectangle {
            id: playListInformation

            color: "white"

            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: {
                        if(index === playbackState.index)
                        {
                            SwipeDelegate.pressed ? "#e0e0e0" : "#d8ffed"
                        }
                        else
                        {
                            SwipeDelegate.pressed ? "#e0e0e0" : "#fff"
                        }
                    }
                }
                GradientStop {
                    position: 1
                    color: {
                        if(index === playbackState.index)
                        {
                            SwipeDelegate.pressed ? "#e0e0e0" : "#82ffca"
                        }
                        else
                        {
                            SwipeDelegate.pressed ? "#e0e0e0" : "#f5f5f5"
                        }
                    }
                }
            }

            RowLayout {
                anchors.fill: parent

                Rectangle {
                    color: "transparent"
                    Layout.leftMargin: 1
                    Layout.preferredWidth: parent.height - 2
                    Layout.minimumHeight: parent.height - 2

                    Image {
                        source: "image://db/" + model.image
                        fillMode: Image.PreserveAspectFit
                        anchors.fill: parent
                    }
                }

                Rectangle {
                    color: "transparent"

                    Layout.leftMargin: 10
                    Layout.fillWidth: true
                    Layout.minimumHeight: parent.height

                    ColumnLayout {
                        Text {
                            text: model.title
                            font.pixelSize: 18
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }
                        Text {
                            text: model.artist
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Rectangle {
                    color: "transparent"

                    Layout.preferredWidth: 70
                    Layout.minimumHeight: parent.height

                    Text {
                        text: Comp.getDisplayTime(model.length)
                        font.pixelSize: 18
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                    }

                    Rectangle {
                        width: 1
                        color: "#ccc"
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                    }
                }
            }
        }

        Rectangle {
            height: 1
            color: "#ccc"
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
