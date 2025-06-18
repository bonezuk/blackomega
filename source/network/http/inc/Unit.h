//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_UNIT_H
#define __OMEGA_NETWORK_HTTP_UNIT_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/BOParse.h"
#include "common/inc/BString.h"
#include "network/http/inc/ResponseCodes.h"
#include "network/http/inc/Query.h"

#include <QMutex>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTP_EXPORT Unit
{
    public:
        
        typedef enum
        {
            e_Request = 0,
            e_Response,
            e_Unknown
        } Type;
        
        typedef enum
        {
            e_Options = 0,
            e_Get,
            e_Head,
            e_Post,
            e_Put,
            e_Delete,
            e_Trace,
            e_Connect,
            e_ReqUnknown
        } RequestType;
        
        class Language
        {
            public:
                Language(); //
                
                void lock(); //
                void unlock(); //
                
                common::BOParse m_unitLang;
                tint m_unitState[12];
                
                common::BOParse m_dateLang;
                tint m_dateState[30];
                
                common::BOParse m_urlLang;
                tint m_urlState[5];
                
            protected:
            
                QMutex m_mutex;
        };
    
    public:
        Unit(); //
        Unit(const Unit& rhs); //
        virtual ~Unit(); //
        
        const Unit& operator = (const Unit& rhs); //
        
        virtual Type type() const; //
        
        virtual void request(RequestType t); //
        virtual void request(RequestType t,const tchar *r); //
        virtual void request(RequestType t,const QString& r); //
        virtual RequestType request() const; //
        
        virtual const QString& resource() const; //
        
        virtual void response(tint c); //
        virtual tint response() const; //
        virtual const QString& reason() const; //
        
        virtual Query& query(); //
        virtual const Query& query() const;
        virtual void query(const tchar *x); //
        virtual void query(const QString& x); //
        
        virtual void map(QMap<QString,QString>& m) const; //
        
        virtual bool exist(const tchar *idx) const; //
        virtual bool exist(const QString& idx) const; //
        
        virtual const QString& data(const tchar *idx) const; //
        virtual const QString& data(const QString& idx) const; //
        
        virtual void date(common::TimeStamp& t) const; //
        
        virtual void add(const common::TimeStamp& t); //
        virtual void add(const tchar *h,const tchar *d); //
        virtual void add(const QString& h,const tchar *d); //
        virtual void add(const tchar *h,const QString& d); //
        virtual void add(const QString& h,const QString& d); //
        
        virtual void remove(const tchar *x); //
        virtual void remove(const QString& x); //
        
        virtual bool insertLine(const tchar *line,bool& end); //
        virtual bool insertLine(const QString& line,bool& end); //
        
        virtual void setVersion(tint major,tint minor); //
        virtual void getVersion(tint& major,tint& minor) const; //
        
        virtual void print(QString& str) const; //
        
        virtual void clear(); //
        
        virtual void setProxyConnection(const tchar *host,tint port = 80); //
        virtual void setProxyConnection(const QString& host,tint port = 80); //
        
        static tint processURL(const QString& url,QString& dns,QString& path,QString& query); //
        static tint processURL(const QString& url,QString& dns,QString& path,QString& query,bool& secure); //
        
        virtual void setUsername(const tchar *user);
        virtual void setUsername(const QString& user);
        
        virtual void setPassword(const tchar *pass);
        virtual void setPassword(const QString& pass);
        
    protected:
        
        static Language m_language;
        
        Type m_type;
        
        // HTTP packet version information
        tint m_versionMajor;
        tint m_versionMinor;
        
        // The resource path given for a request
        RequestType m_request;
        QString m_resource;
        Query m_query;
        
        // The code and reason given in a response
        tint m_code;
        QString m_reason;
        
        QMap<QString,QString> m_header;
        QMap<QString,QString> m_data;
        
        common::TimeStamp m_time;
        
        bool m_proxyFlag;
        QString m_host;
        tint m_port;
        
        QString m_username;
        QString m_password;
        
        QString m_lastIndex;
        
        QString m_blank;
        
        virtual void printError(const tchar *strR,const tchar *strE) const; //
        
        virtual void copy(const Unit& rhs); //
        
        virtual bool parseHTTPVersion(const common::BString& line,common::BO_Parse_Unit **pItem,tint& end,tint& major,tint& minor); //
        
        virtual bool parseInLine(const common::BString& line); //
        
        virtual tint asciiToInteger(const tchar *s,tint start,tint end) const; //
        
        // HTTP date string interpreter
        virtual bool parseDate_RFC1123(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const; //
        virtual bool parseDate_RFC1036(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const; //
        virtual bool parseDate_ANSI(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const; //
        virtual bool parseDate(const common::BString& str,common::TimeStamp& t); //
        
        virtual QString buildTimeStamp(const common::TimeStamp& t) const; //
        
        virtual void makeResourceLocal(); //
};

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

