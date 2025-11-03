import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

Rectangle {
    id: digitDisplay
    property int timeInSeconds: 0

    color: "transparent"

    RowLayout {
        anchors.fill: parent
        spacing: 1
        implicitWidth: parent.implicitWidth

        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeHourUpperDigit, digitDisplay.timeInSeconds)
            }
        }
        
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeHourLowerDigit, digitDisplay.timeInSeconds)
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 14

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.urlRelPath("images/digit_colon.png")
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeMinuteUpperDigit, digitDisplay.timeInSeconds)
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeMinuteLowerDigit, digitDisplay.timeInSeconds)
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 14

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.urlRelPath("images/digit_colon.png")
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeSecondUpperDigit, digitDisplay.timeInSeconds)
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width / 7

            Image {
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                fillMode: Image.PreserveAspectFit
                source: Comp.getPlayTimeDigitImageName(Comp.PlayTimeSecondLowerDigit, digitDisplay.timeInSeconds)
            }
        }
    }
}
