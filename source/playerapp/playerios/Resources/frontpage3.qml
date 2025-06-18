import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components"
import "components/components.js" as Comp

Window {
    id: window
    visible: true

    width: 400
    height: 600
        
    StackLayout {
        id: mainArea
        currentIndex: navBar.currentNavIndex
        
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: navBar.top
        
        StackLayout {
            id: libraryMain
            currentIndex: 0
            
            AlbumView {
                model: playListModel
                onClicked: {
                    parent.currentIndex = 1;
                }
            }

            Page {
                header: Rectangle {
                    width: parent.width
                    height: 40
                    
                    gradient: Gradient {
                        GradientStop {
                            position: 0
                            color: "#ffffff"
                        }
                        GradientStop {
                            position: 1
                            color: "#e0e0e0"
                        }
                    }
                    
                    Image {
                        source: "images/back.png"
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 5

                        MouseArea {
                            anchors.fill: parent
                            
                            onClicked: {
                                console.log("back");
                                libraryMain.currentIndex = 0;
                            }
                        }
                    }
                    
                    Image {
                        source: addAlbumMouse.pressed ? "images/add_album_pressed.png" : "images/add_album.png"
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 10
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10

                        MouseArea {
                            id: addAlbumMouse
                            anchors.fill: parent
                            
                            onClicked: {
                                console.log("add album");
                            }
                        }
                    }                    
                }
                
                AlbumTrackView {
                    anchors.fill: parent
                    model: playListModel
                    onClicked: {
                        notifyInfo.text = "Added track '" + currentTrack + "' to playlist."
                        notifyInfo.visible = true;
                    }
                    
                    PLNotifyInfo {
                        id: notifyInfo
                        text: ""
                    }

                }

            }
        }
        
        Rectangle {
            anchors.fill: parent
            
            Rectangle {
                id: playControlsContainer
                
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                implicitHeight: 75
                
                gradient: Gradient {
                    GradientStop { position: 0.0; color: "#c6e4f7"}
                    GradientStop { position: 1.0; color: "#477996"}
                }
                
                PlayButton {
                    id: playButton
                    
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 3
                    implicitWidth: implicitHeight
                    implicitHeight: parent.height - 7
                }
                
                
                PlaybackSlider {
                    id: seekSlider
                    
                    anchors.left: playButton.right
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.top: parent.top
                    anchors.topMargin: 5

                    onSeek: (v) => {
                        // console.log("seek -> " + v);
                    }
            
                    onDisplay: (v) => {
                        // console.log("dis -> " + v);
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
                
                Image {
                    id: playingAlbumImage
                    source: "images/note.png"
                    fillMode: Image.PreserveAspectFit
                    
                    anchors.left: playButton.right
                    anchors.top: seekSlider.bottom
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 7
                    anchors.bottomMargin: 7
                }

                ColumnLayout {
                    id: playingTrackInfo
                    
                    anchors.top: seekSlider.bottom
                    anchors.left: playingAlbumImage.right
                    anchors.right: digitDisplay.left
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 7
                    anchors.bottomMargin: 7

                    Text {
                        text: "Track Name"
                        font.pixelSize: 12
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                    Text {
                        text: "Album Name"
                        font.pixelSize: 12
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                    }
                }
                

                DigitDisplay {
                    id: digitDisplay
                    
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.top: seekSlider.bottom
                    anchors.topMargin: 5
                    
                    width: 110
                    height: 25
                }
            }

            PlayListView {
                id: playListView
                model: playListModel
                
                clip: true
                anchors.top: playControlsContainer.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: playlistControlTab.top
                
                onClicked: {
                    console.log(currentIndex);
                }
            }
            
            Rectangle {
                id: playlistControlTab
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                implicitHeight: 40
                color: "#2E4053"
                
                RowLayout {
                    anchors.fill: parent
                    
                    Image {
                        fillMode: Image.PreserveAspectFit
                        source: cleanAllMouse.pressed ? "images/clean_all_pressed.png" : "images/clean_all.png";
                        Layout.preferredHeight: (80 * parent.height) / 100
                        
                        MouseArea {
                            id: cleanAllMouse
                            anchors.fill: parent
                            onDoubleClicked: {
                                console.log("clean all");
                            }
                        }
                    }
                }

            }
        }
        
        Rectangle {
            anchors.fill: parent
            
            ColumnLayout {
                anchors.fill: parent
                anchors.left: parent.left
                anchors.right: parent.right                
                
                Item {
                    Switch {
                        text: "Enable FTP"
                        anchors.centerIn: parent
                    }
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                }
                Item {
                    Text {
                        text: "FTP server is disabled"
                        anchors.centerIn: parent
                    }
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                }
                Item {
                    Button {
                        text: "Rebuild Database"
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.fill: parent
                        onClicked: { 
                            lockAppDialog.text = "<b>Rebuilding database from music directory.\nPlease Wait!</b>"
                            lockAppDialog.visible = true;
                            //settings.onRebuildDatabase();
                            //lockAppDialog.visible = false;
                        }
                    }
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                }
                Item {
                    // spacer item
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Rectangle { anchors.fill: parent; color: "#ffaaaa" } // to visualize the spacer
                }
            }
        }

    }
    
    MutexDialog {
        id: lockAppDialog
        visible: false
        text: ""
    }
    
    RowLayout {
        id: navBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        implicitHeight: 75
        spacing: 0
        
        property int currentNavIndex: 0
        
        NavButton {
            navIndex: 0
            sourceEnabled: "images/album.png"
            sourceDisabled: "images/album_disable.png"
            label: "Library"

            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 4.0
            Layout.minimumHeight: parent.height
        }

        NavButton {
            navIndex: 1
            sourceEnabled: "images/play_icon_toolbar_256x256_1.png"
            sourceDisabled: "images/play_icon_toolbar_disable_256x256_1.png"
            label: "Playback"

            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 4.0
            Layout.minimumHeight: parent.height
        }

        NavButton {
            navIndex: 2
            sourceEnabled: "images/gears.png"
            sourceDisabled: "images/gears_disable.png"
            label: "Settings"

            Layout.fillWidth: true
            Layout.preferredWidth: parent.width / 4.0
            Layout.minimumHeight: parent.height
        }
        
    }

    PlayListModel {
        id: playListModel
    }
    
}
