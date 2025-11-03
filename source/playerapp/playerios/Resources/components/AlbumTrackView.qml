import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

ListView {
    id: albumTrackView

    property string currentTrack: ""
    property string currentAlbumName: ""
    property string currentArtistName: ""
    property int currentImageID: -1

    signal clicked

    header: Rectangle {
        width: parent.width
        height: 100

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "#fff"
            }
            GradientStop {
                position: 1
                color: "#f5f5f5"
            }
        }

        RowLayout {
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.left: parent.left


            Image {
                source: "image://db/" + currentImageID
                fillMode: Image.PreserveAspectFit
                Layout.fillWidth: true
                Layout.leftMargin: parent.spacing
                Layout.preferredWidth: 100
                Layout.preferredHeight: parent.height
            }

            Rectangle {
                color: "transparent"
                Layout.fillWidth: true
                Layout.preferredWidth: parent.width - 100
                Layout.preferredHeight: parent.height

                ColumnLayout {
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.left: parent.left

                    Text {
                        text: albumTrackView.currentAlbumName
                        font.pixelSize: 18
                        Layout.leftMargin: 10
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                    Text {
                        text: albumTrackView.currentArtistName
                        font.pixelSize: 16
                        Layout.leftMargin: 10
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }
                }

            }
        }
    }

    delegate: Rectangle {
        width: parent.width
        height: 40

        gradient: Gradient {
            GradientStop {
                position: 0
                Behavior on color {ColorAnimation { duration: 100 }}
                color: trackTapHandler.pressed ? "#e0e0e0" : "#fff";
            }
            GradientStop {
                position: 1
                Behavior on color {ColorAnimation { duration: 100 }}
                color: trackTapHandler.pressed ? "#e0e0e0" : "#f5f5f5"
            }
        }

        TapHandler {
            id: trackTapHandler

            onTapped: {
                albumTrackView.currentIndex = index;
                albumTrackView.currentTrack = model.track;
                albumTrackView.clicked();
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                color: "transparent"
                Layout.fillWidth: true
                Layout.preferredWidth: 40
                Layout.minimumHeight: parent.height
                Text {
                    text: index + 1
                    font.pixelSize: 16
                    anchors.centerIn: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Rectangle {
                    width: 1
                    color: "#ccc"
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.top: parent.top
                }
            }

            Rectangle {
                color: "transparent"
                Layout.fillWidth: true
                Layout.preferredWidth: parent.width - (40 + 70)
                Layout.minimumHeight: parent.height
                Text {
                    text: model.track
                    font.pixelSize: 16
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }
                Rectangle {
                    width: 1
                    color: "#ccc"
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right
                    anchors.top: parent.top
                }
            }

            Rectangle {
                color: "transparent"
                Layout.fillWidth: true
                Layout.preferredWidth: 70
                Layout.minimumHeight: parent.height
                Text {
                    text: Comp.getDisplayTime(model.length)
                    font.pixelSize: 16
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                }
            }

        }
    }
}
