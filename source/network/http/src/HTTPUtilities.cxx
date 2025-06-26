#include "network/http/inc/HTTPUtilities.h"
#include "engine/inc/Codec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------
// source : https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
//-------------------------------------------------------------------------------------------

void populateContentExtensionMap(QMap<QString, QString>& map)
{
    map.insert("aac", "audio/aac");
    map.insert("abw", "application/x-abiword");
    map.insert("arc", "application/x-freearc");
    map.insert("aiff", "audio/x-aiff");
    map.insert("aifc", "audio/x-aiff");
    map.insert("aif", "audio/x-aiff");
    map.insert("avif", "image/avif");
    map.insert("avi", "video/x-msvideo");
    map.insert("azw", "application/vnd.amazon.ebook");
    map.insert("bin", "application/octet-stream");
    map.insert("bmp", "image/bmp");
    map.insert("bz", "application/x-bzip");
    map.insert("bz2", "application/x-bzip2");
    map.insert("cda", "application/x-cdf");
    map.insert("csh", "application/x-csh");
    map.insert("css", "text/css");
    map.insert("csv", "text/csv");
    map.insert("doc", "application/msword");
    map.insert("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
    map.insert("eot", "application/vnd.ms-fontobject");
    map.insert("epub", "application/epub+zip");
    map.insert("gz", "application/gzip");
    map.insert("gif", "image/gif");
    map.insert("htm", "text/html");
    map.insert("html", "text/html");
    map.insert("ico", "image/vnd.microsoft.icon");
    map.insert("ics", "text/calendar");
    map.insert("jar", "application/java-archive");
    map.insert("jpeg", "image/jpeg");
    map.insert("jpg", "image/jpeg");
    map.insert("js", "text/javascript");
    map.insert("json", "application/json");
    map.insert("jsonld", "application/ld+json");
    map.insert("m3u", "audio/x-mpegurl");
    map.insert("mid", "audio/midi");
    map.insert("midi", "audio/midi");
    map.insert("mjs", "text/javascript");
    map.insert("mp3", "audio/mpeg");
    map.insert("mp4", "audio/mp4");
    map.insert("m4a", "audio/mp4");
    map.insert("mpeg", "video/mpeg");
    map.insert("mpkg", "application/vnd.apple.installer+xml");
    map.insert("odp", "application/vnd.oasis.opendocument.presentation");
    map.insert("ods", "application/vnd.oasis.opendocument.spreadsheet");
    map.insert("odt", "application/vnd.oasis.opendocument.text");
    map.insert("oga", "audio/ogg");
    map.insert("ogg", "audio/ogg");
    map.insert("ogv", "video/ogg");
    map.insert("ogx", "application/ogg");
    map.insert("opus", "audio/opus");
    map.insert("otf", "font/otf");
    map.insert("png", "image/png");
    map.insert("pdf", "application/pdf");
    map.insert("php", "application/x-httpd-php");
    map.insert("ppt", "application/vnd.ms-powerpoint");
    map.insert("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation");
    map.insert("rar", "application/vnd.rar");
    map.insert("rtf", "application/rtf");
    map.insert("sh", "application/x-sh");
    map.insert("svg", "image/svg+xml");
    map.insert("swf", "application/x-shockwave-flash");
    map.insert("tar", "application/x-tar");
    map.insert("tif", "image/tiff");
    map.insert("tiff", "image/tiff");
    map.insert("ts", "video/mp2t");
    map.insert("ttf", "font/ttf");
    map.insert("txt", "text/plain");
    map.insert("vsd", "application/vnd.visio");
    map.insert("wav", "audio/wav");
    map.insert("weba", "audio/webm");
    map.insert("webm", "video/webm");
    map.insert("webp", "image/webp");
    map.insert("woff", "font/woff");
    map.insert("woff2", "font/woff2");
    map.insert("xhtml", "application/xhtml+xml");
    map.insert("xls", "application/vnd.ms-excel");
    map.insert("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
    map.insert("xml", "application/xml");
    map.insert("xul", "application/vnd.mozilla.xul+xml");
    map.insert("zip", "application/zip");
    map.insert("3gp", "video/3gpp");
    map.insert("3g2", "video/3gpp2");
    map.insert("7z", "application/x-7z-compressed");
}

//-------------------------------------------------------------------------------------------

QString contentTypeFromExtension(const QString& fileName)
{
    static QMap<QString, QString> extMap;
    QString ext = engine::Codec::getFileExtension(fileName);
    QMap<QString,QString>::const_iterator ppI;
    QString type;

    if(extMap.isEmpty())
    {
        populateContentExtensionMap(extMap);
    }
    ppI = extMap.find(ext);
    if(ppI != extMap.constEnd())
    {
        type = ppI.value();
    }
    return type;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
