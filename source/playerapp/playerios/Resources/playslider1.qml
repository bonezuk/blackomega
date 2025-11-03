import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components" as Component

Window {
    id: window
    visible: true

    width: 400
    height: 75

    Rectangle {
        anchors.fill: parent

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#c6e4f7"}
            GradientStop { position: 1.0; color: "#477996"}
        }

        Rectangle {
            id: rect
            anchors.left: parent.left
            anchors.top: parent.top
            width: 50
            height: 50
            color: "red"
        }

        Component.PlaybackSlider {
            anchors.left: rect.right
            anchors.leftMargin: 5
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5

            onSeek: (v) => {
                console.log("seek -> " + v);
            }

            onDisplay: (v) => {
                console.log("dis -> " + v);
            }

            Timer {
                interval: 100
                running: true
                repeat: true
                onTriggered: {
                    parent.updateValue(parent.liveValue + 0.5);
                }
            }
        }

    }
}
