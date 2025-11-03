function urlRelPath(path)
{
    return "../" + path;
}

function playButtonImageSource(isEnabled, isPause, isPressed, isHovering)
{
    let imageName = "images/pc_";
    imageName += (!isPause || !isEnabled) ? "play" : "pause";
    imageName += "_";
    if(!isEnabled)
    {
        imageName += "disabled";
    }
    else if(isPressed)
    {
        imageName += "press";
    }
    else
    {
        imageName += (isHovering) ? "hover" : "normal";
    }
    imageName += ".png";
    console.log(isEnabled + " " + isPause + " " +  isPressed + " " +  isHovering  + " " + imageName);
    return urlRelPath(imageName);
}

//-------------------------------------------------------------------------------------------
// PlayTime enums
//-------------------------------------------------------------------------------------------

const PlayTimeHourUpperDigit = "PlayTimeHourUpperDigit";
const PlayTimeHourLowerDigit = "PlayTimeHourLowerDigit";
const PlayTimeMinuteUpperDigit = "PlayTimeMinuteUpperDigit";
const PlayTimeMinuteLowerDigit = "PlayTimeMinuteLowerDigit";
const PlayTimeSecondUpperDigit = "PlayTimeSecondUpperDigit";
const PlayTimeSecondLowerDigit = "PlayTimeSecondLowerDigit";

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
        imageName = urlRelPath("images/digit_" + tS + ".png");
    }
    return imageName;
}

//-------------------------------------------------------------------------------------------

function getDisplayTime(timeStamp)
{
    let dstime = "";

    let hour = parseInt(timeStamp / 3600);
    timeStamp -= hour * 3600;
    let mins = parseInt(timeStamp / 60);
    timeStamp -= mins * 60;
    let secs = parseInt(timeStamp);
    
    
    if(hour > 0)
    {
        dstime += hour.toString() + ":";
    }
    
    if(mins > 0)
    {
        if(mins > 10)
        {
            let uMin = parseInt(mins / 10);
            dstime += uMin.toString();
            let lMin = parseInt(mins % 10);
            dstime += lMin.toString();
        }
        else
        {
            if(hour > 0)
            {
                dstime += "0";
            }
            dstime += mins.toString();
        }
        dstime += ":";
    }
    else if(hour > 0)
    {
        dstime += "00:";
    }
    else
    {
        dstime += "0:";
    }
    
    if(secs > 10)
    {
        let uSec = parseInt(secs / 10);
        dstime += uSec.toString();
        let lSec = parseInt(secs % 10);
        dstime += lSec.toString();
    }
    else
    {
        dstime += "0" + secs.toString();
    }
    return dstime;
}

//-------------------------------------------------------------------------------------------

function reverseString(str) {
    // Step 1. Use the split() method to return a new array
    var splitString = str.split(""); // var splitString = "hello".split("");
    // ["h", "e", "l", "l", "o"]
 
    // Step 2. Use the reverse() method to reverse the new created array
    var reverseArray = splitString.reverse(); // var reverseArray = ["h", "e", "l", "l", "o"].reverse();
    // ["o", "l", "l", "e", "h"]
 
    // Step 3. Use the join() method to join all elements of the array into a string
    var joinArray = reverseArray.join(""); // var joinArray = ["o", "l", "l", "e", "h"].join("");
    // "olleh"
    
    //Step 4. Return the reversed string
    return joinArray; // "olleh"
}

function stringWithDigits(value, noDigits)
{
    let s = "";
    while(value > 0 || noDigits > 0)
    {
        let mod = parseInt(value % 10);
        s += mod.toString();
        value = parseInt(value / 10);
        noDigits--;
    }
    return reverseString(s);
}

//-------------------------------------------------------------------------------------------
