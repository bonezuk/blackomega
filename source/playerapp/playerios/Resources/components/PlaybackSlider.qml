import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Window 2.14
import QtQuick.Layouts 1.15

import "components.js" as Comp

Rectangle {
    id: playbackSlider

    property double from: 0.0
    property double to: 60.0
    property double value: 0.0
    property double liveValue: 0.0

    height: 18
    border.color: "#6d90a4"
    border.width: 1
    radius: 2

    signal seek(double v)
    signal display(double v)

    onLiveValueChanged: {
        let newValue = liveValue;
        if(newValue < from)
        {
            newValue = from;
        }
        else if(newValue > to)
        {
            newValue = to;
        }
        if(sliderHandle.sliderState != 2)
        {
            value = newValue;
            display(value);
        }
    }

    function updateValue(newValue)
    {
        if(newValue < from)
        {
            newValue = from;
        }
        else if(newValue > to)
        {
            newValue = to;
        }
        liveValue = newValue;
        if(sliderHandle.sliderState != 2)
        {
            value = newValue;
            display(value);
        }
    }

    function cancelSeek()
    {
        value = liveValue;
    }

    function valueFromMousePosition(mouse)
    {
        let ratio = (mouse.x - (sliderHandle.width / 2.0)) / (width - sliderHandle.width);
        let nValue = (ratio * (to - from)) + from;
        if(nValue < from)
        {
            nValue = from;
        }
        else if(nValue > to)
        {
            nValue = to;
        }
        return nValue;
    }

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#7fc3e8"}
        GradientStop { position: 1.0; color: "#5aa1c5"}
    }

    Image {
        id: sliderHandle

        property int sliderState: 0

        x: {
            let ratio = (parent.value - parent.from) / (parent.to - parent.from);
            return (ratio * (parent.width - width));
        }

        anchors.verticalCenter: parent.verticalCenter
        height: parent.height - 2
        source: switch(sliderState) {
            case 1: return Comp.urlRelPath("images/slide_button_select.png");
            case 2: return Comp.urlRelPath("images/slide_button_push.png");
            default: return Comp.urlRelPath("images/slide_button.png");
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onPressed: (mouse) => {
            parent.value = parent.valueFromMousePosition(mouse);
            sliderHandle.sliderState = 2;
            mouse.accepted = true;
        }
        onReleased: (mouse) => {
            if(sliderHandle.sliderState === 2)
            {
                parent.value = parent.valueFromMousePosition(mouse);
                sliderHandle.sliderState = 0;
                parent.seek(parent.value);
            }
            mouse.accepted = true;
        }
        onPositionChanged: (mouse)=> {
            if(sliderHandle.sliderState === 2)
            {
                parent.value = parent.valueFromMousePosition(mouse);
                sliderHandle.sliderState = 2;
                parent.display(parent.value);
            }
            mouse.accepted = true;
        }
        onExited: {
            if(sliderHandle.sliderState != 0)
            {
                sliderHandle.sliderState = 0;
                parent.cancelSeek();
            }
        }
    }
}
