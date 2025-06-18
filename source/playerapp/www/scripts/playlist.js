//-------------------------------------------------------------------------------------------
// Global
//-------------------------------------------------------------------------------------------

const playListElement = document.getElementById('pl-list');
const playListIdMap = new Map();
var currentPlaybackId = 0;

//-------------------------------------------------------------------------------------------
// init page
//-------------------------------------------------------------------------------------------

loadPlaylist();

//-------------------------------------------------------------------------------------------

function loadPlaylist()
{
    var playlistURL = window.location.protocol + "//" + window.location.host + "/playlist";

    getJSON(playlistURL, function(err, data) {
        if(err === null) 
        {
            data.forEach(createPlaylistElement);
            loadPlaybackState();
        }
    });
}

//-------------------------------------------------------------------------------------------

function loadPlaybackState()
{
    var stateURL = window.location.protocol + "//" + window.location.host + "/state";

    getJSON(stateURL, function(err, data) {
        if(err === null) 
        {
            setPlaybackItem(data.playingId);
            onPlaybackTimeEvent(data.playbackTime);
            setPlaybackButton(data.isPlaying);
        }
    });
}

//-------------------------------------------------------------------------------------------

function onPlayPressed()
{
    var url = window.location.protocol + "//" + window.location.host + "/pressplay";
    var req = new XMLHttpRequest();
    req.open('GET', url);
    req.onload = function() {
        var status = req.status;
        console.log(status);
    };
    req.send();
}

//-------------------------------------------------------------------------------------------

function onStartPlayback(id)
{
    var url = window.location.protocol + "//" + window.location.host + "/startplayback?id=" + id;
    console.log(url);
    console.log(id);
    var req = new XMLHttpRequest();
    req.open('GET', url);
    req.onload = function() {
        var status = req.status;
        console.log(status);
    };
    req.send();
}

//-------------------------------------------------------------------------------------------

function getJSON(url, callback)
{
    var xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.responseType = 'json';
    xhr.onload = function() {
        var status = xhr.status;
        if (status === 200) 
        {
            callback(null, xhr.response);
        } 
        else 
        {
            callback(status, xhr.response);
        }
    };
    xhr.send();    
}

//-------------------------------------------------------------------------------------------

function setPlaybackItem(currentId)
{
    if(currentPlaybackId !== 0)
    {
        const rListItem = playListIdMap.get(currentPlaybackId);
        rListItem.classList.remove('isplaying');
    }
    if(currentId !== 0)
    {
        const aListItem = playListIdMap.get(currentId);
        aListItem.classList.add('isplaying');
    }
    currentPlaybackId = currentId;
    if(currentId === 0)
    {
        onPlaybackTimeEvent(0);
    }
}

//-------------------------------------------------------------------------------------------

function setPlaybackButton(isPlaying)
{
    const pbButton = document.getElementById('pt-play-button');
    if(isPlaying)
    {
        pbButton.src = "images/pc_pause_normal.png";
    }
    else
    {
        pbButton.src = "images/pc_play_normal.png";
    }
}

//-------------------------------------------------------------------------------------------

function createPlaylistElement(value, index, array)
{
    var idValue = value.id.toString();
    const listItem = document.createElement('li');
    listItem.setAttribute('data-id', idValue);
    listItem.innerHTML = generatePlaylistHTMLFromJSON(index, value);
    playListElement.appendChild(listItem);
    playListIdMap.set(value.id, listItem);
}

//-------------------------------------------------------------------------------------------

function generatePlaylistHTMLFromJSON(index, value)
{
    let idx = index + 1;
    let h = '<div class="pl-item-index" ondblclick="onDblClickPlaylistItem(\'' + value.id + '\')">' + idx.toString() + '</div>';
    h += '<div class="pl-item-name" ondblclick="onDblClickPlaylistItem(\'' + value.id + '\')">' + value.title + '</div>';
    h += '<div class="pl-item-time" ondblclick="onDblClickPlaylistItem(\'' + value.id + '\')">' + timeStampAsString(value.length) + '</div>';
    return h;
}

//-------------------------------------------------------------------------------------------

function onDblClickPlaylistItem(id)
{
    onStartPlayback(id);
}

//-------------------------------------------------------------------------------------------

function timeStampAsString(timeValue)
{
    let sec = parseInt(timeValue) % 60;
    let min = parseInt(timeValue / 60);
    let hour = parseInt(min / 60);
    let tS = "";
    
    if(hour > 0)
    {
        tS += hour.toString();
        tS += ":";
        if(min < 10)
        {
            tS += "0";
        }
        tS += min.toString();
    }
    else
    {
        tS += min.toString();
    }
    
    tS += ":";
    if(sec < 10)
    {
        tS += "0";
    }
    tS += sec.toString();
    
    return tS;
}

//-------------------------------------------------------------------------------------------
