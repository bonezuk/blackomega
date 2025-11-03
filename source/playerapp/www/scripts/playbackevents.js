//-------------------------------------------------------------------------------------------
// Define event source
//-------------------------------------------------------------------------------------------

var eventSource = new EventSource("/event");

//-------------------------------------------------------------------------------------------
// init page
//-------------------------------------------------------------------------------------------

setupEventListener();

//-------------------------------------------------------------------------------------------

function setupEventListener()
{
    eventSource.addEventListener('message', onJsonEvent, false);
}

//-------------------------------------------------------------------------------------------

function onJsonEvent(e)
{
    const evt = JSON.parse(e.data);
    const evtName = evt.event;

    if(evtName !== undefined && evtName !== null)
    {
        if(evtName === "onPlaybackTime")
        {
            onPlaybackTimeEvent(evt.data.time);
        }
        else if(evtName == "onAudioStart")
        {
            setPlaybackButton(true);
            setPlaybackItem(evt.data.id);
        }
        else if(evtName== "onAudioPlaybackEvent")
        {
            setPlaybackButton(evt.data.isPlaying);
            setPlaybackItem(evt.data.id);
        }
        else
        {
            console.log(evt);
        }
    }
}

//-------------------------------------------------------------------------------------------
// Playback Time display
//-------------------------------------------------------------------------------------------

const PlayTimeHourUpperDigit = "pt-hour-upper";
const PlayTimeHourLowerDigit = "pt-hour-lower";
const PlayTimeMinuteUpperDigit = "pt-minute-upper";
const PlayTimeMinuteLowerDigit = "pt-minute-lower";
const PlayTimeSecondUpperDigit = "pt-second-upper";
const PlayTimeSecondLowerDigit = "pt-second-lower";

//-------------------------------------------------------------------------------------------

function onPlaybackTimeEvent(timeValue)
{
    if(timeValue !== undefined && timeValue !== null)
    {
        const tSParts = [
            PlayTimeHourUpperDigit, PlayTimeHourLowerDigit,
            PlayTimeMinuteUpperDigit, PlayTimeMinuteLowerDigit,
            PlayTimeSecondUpperDigit, PlayTimeSecondLowerDigit
        ];
        const timeInSeconds = parseInt(timeValue);

        for(const elementId of tSParts)
        {
            document.getElementById(elementId).src = getPlayTimeDigitImageName(elementId, timeInSeconds);
        }
    }
}

//-------------------------------------------------------------------------------------------

function getPlayTimeDigitImageName(digitType, timeInSeconds)
{
    let imageName = "";
    let tS = -1;

    if(digitType === PlayTimeHourUpperDigit || digitType === PlayTimeHourLowerDigit)
    {
        tS = parseInt(timeInSeconds / (60 * 60));
        if(digitType === PlayTimeHourLowerDigit)
        {
            tS = tS % 10;
        }
        else
        {
            tS = parseInt(tS / 10) % 100;
        }
    }
    else if(digitType === PlayTimeMinuteUpperDigit || digitType === PlayTimeMinuteLowerDigit)
    {
        tS = parseInt(timeInSeconds / 60) % 60;
        if(digitType === PlayTimeMinuteLowerDigit)
        {
            tS = tS % 10;
        }
        else
        {
            tS = parseInt(tS / 10);
        }
    }
    else if(digitType === PlayTimeSecondUpperDigit || digitType === PlayTimeSecondLowerDigit)
    {
        tS = parseInt(timeInSeconds % 60);
        if(digitType === PlayTimeSecondLowerDigit)
        {
            tS = tS % 10;
        }
        else
        {
            tS = parseInt(tS / 10);
        }
    }

    if(tS >= 0.0)
    {
        imageName = "images/digits/digit_" + tS + ".png";
    }
    return imageName;
}

//-------------------------------------------------------------------------------------------
