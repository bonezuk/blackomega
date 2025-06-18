import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

ListView {
    id: albumView
    
    property string currentAlbumName: ""
    property string currentArtistName: ""
    property int currentImageID: -1
    
    signal clicked()
    
    delegate: Rectangle {
        width: parent.width
        height: 50

        gradient: Gradient {
            GradientStop {
                position: 0
                Behavior on color {ColorAnimation { duration: 100 }}
                color: tapHandler.pressed ? "#e0e0e0" : "#fff"
            }
            GradientStop {
                position: 1
                Behavior on color {ColorAnimation { duration: 100 }}
                color: tapHandler.pressed ? "#e0e0e0" : "#f5f5f5"
            }
        }

        RowLayout {
            anchors.fill: parent
            
            Rectangle {
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
                        text: model.album
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
        }
        
        TapHandler {
            id: tapHandler
            onTapped: {
                albumView.currentIndex = index;
                currentAlbumName = model.album;
                currentArtistName = model.artist;
                currentImageID = model.image;
                albumView.clicked();
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
