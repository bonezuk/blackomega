//-------------------------------------------------------------------------------------------
#ifndef OMEGA_COMMON_FACTORY_H
#define OMEGA_COMMON_FACTORY_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QVector>
#include <QMap>
#include <QObject>
#include <QSharedPointer>

#include "common/inc/Exception.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------

#define ABSTRACT_FACTORY_CLASS(EXPORT_NAME, \
                               ABSTRACT_FACTORY_NAME, \
                               ABSTRACT_PRODUCT_NAME) \
\
class EXPORT_NAME ABSTRACT_FACTORY_NAME \
{ \
    public: \
        typedef ABSTRACT_PRODUCT_NAME AbstractProduct; \
        typedef ABSTRACT_PRODUCT_NAME *AbstractProductPtr; \
        typedef QSharedPointer<ABSTRACT_PRODUCT_NAME> AbstractProductQPtr; \
\
        virtual ~ABSTRACT_FACTORY_NAME(); \
\
        static AbstractProductQPtr createShared(const QString& className); \
        static AbstractProductQPtr createShared(const QString& className,QObject *parent); \
        static AbstractProductQPtr createShared(); \
        static AbstractProductQPtr createShared(QObject *parent); \
\
        static AbstractProductPtr createUnmanaged(const QString& className); \
        static AbstractProductPtr createUnmanaged(const QString& className,QObject *parent); \
        static AbstractProductPtr createUnmanaged(); \
        static AbstractProductPtr createUnmanaged(QObject *parent); \
\
        typedef QVector<QString>::const_iterator const_iterator; \
        static QVector<QString> products(); \
        static const_iterator productsBegin(); \
        static const_iterator productsEnd(); \
\
    protected: \
        typedef QMap<QString,ABSTRACT_FACTORY_NAME*> Registry; \
        typedef QVector<QString> RegistryStr; \
\
        ABSTRACT_FACTORY_NAME(const QString& className,bool isDefaultProduct=false); \
\
        virtual AbstractProductQPtr makeShared() const = 0; \
        virtual AbstractProductPtr makeUnmanaged() const = 0; \
\
        virtual AbstractProductQPtr makeQShared(QObject *parent) const = 0; \
        virtual AbstractProductPtr makeQUnmanaged(QObject *parent) const = 0; \
\
        static QString& defaultProduct(); \
        static Registry& registry(); \
        static RegistryStr& registryStr(); \
\
}; \
\

//-------------------------------------------------------------------------------------------

#define ABSTRACT_FACTORY_CLASS_IMPL(ABSTRACT_FACTORY_NAME, \
                                    ABSTRACT_PRODUCT_NAME) \
\
QString& ABSTRACT_FACTORY_NAME::defaultProduct() \
{ \
    static QString s_default; \
    return s_default; \
} \
\
ABSTRACT_FACTORY_NAME::~ABSTRACT_FACTORY_NAME() \
{} \
\
ABSTRACT_FACTORY_NAME::Registry& ABSTRACT_FACTORY_NAME::registry() \
{ \
    static Registry *s_p_registry = new Registry(); \
    return *s_p_registry; \
} \
\
ABSTRACT_FACTORY_NAME::RegistryStr& ABSTRACT_FACTORY_NAME::registryStr() \
{ \
    static RegistryStr *s_p_registry_str = new RegistryStr(); \
    return *s_p_registry_str; \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductQPtr ABSTRACT_FACTORY_NAME::createShared(const QString& className) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes for '" + className; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit = registry().find(className); \
    \
    if(cit!=registry().end()) \
    { \
        return (cit.value()->makeShared()); \
    } \
    else if(!defaultProduct().isEmpty()) \
    { \
        cit = registry().find(defaultProduct()); \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeShared()); \
        } \
    } \
    str = "Abstract factory " #ABSTRACT_FACTORY_NAME " does not have the required product name registered for '" + className; \
    throw common::Exception(str); \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductQPtr ABSTRACT_FACTORY_NAME::createShared(const QString& className,QObject *parent) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes for '" + className; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit = registry().find(className); \
    \
    if(cit!=registry().end()) \
    { \
        return (cit.value()->makeQShared(parent)); \
    } \
    else if(!defaultProduct().isEmpty()) \
    { \
        cit = registry().find(defaultProduct()); \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeQShared(parent)); \
        } \
    } \
    str = "Abstract factory " #ABSTRACT_FACTORY_NAME " does not have the required product name registered for '" + className; \
    throw common::Exception(str); \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductQPtr ABSTRACT_FACTORY_NAME::createShared() \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes."; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit; \
    \
    if(!defaultProduct().isEmpty()) \
    { \
         cit = registry().find(defaultProduct()); \
        \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeShared()); \
        } \
        else \
        { \
            str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered default concrete factory classes."; \
            throw common::Exception(str); \
        } \
    } \
    cit = registry().begin(); \
    return (cit.value()->makeShared()); \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductQPtr ABSTRACT_FACTORY_NAME::createShared(QObject *parent) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes."; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit; \
    \
    if(!defaultProduct().isEmpty()) \
    { \
         cit = registry().find(defaultProduct()); \
        \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeQShared(parent)); \
        } \
        else \
        { \
            str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered default concrete factory classes."; \
            throw common::Exception(str); \
        } \
    } \
    cit = registry().begin(); \
    return (cit.value()->makeQShared(parent)); \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductPtr ABSTRACT_FACTORY_NAME::createUnmanaged(const QString& className) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes for '" + className; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit = registry().find(className); \
    \
    if(cit!=registry().end()) \
    { \
        return (cit.value()->makeUnmanaged()); \
    } \
    else if(!defaultProduct().isEmpty()) \
    { \
        cit = registry().find(defaultProduct()); \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeUnmanaged()); \
        } \
    } \
    str = "Abstract factory " #ABSTRACT_FACTORY_NAME " does not have the required product name registered for '" + className; \
    throw common::Exception(str); \
    return 0; \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductPtr ABSTRACT_FACTORY_NAME::createUnmanaged(const QString& className,QObject *parent) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes for '" + className; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit = registry().find(className); \
    \
    if(cit!=registry().end()) \
    { \
        return (cit.value()->makeQUnmanaged(parent)); \
    } \
    else if(!defaultProduct().isEmpty()) \
    { \
        cit = registry().find(defaultProduct()); \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeQUnmanaged(parent)); \
        } \
    } \
    str = "Abstract factory " #ABSTRACT_FACTORY_NAME " does not have the required product name registered for '" + className; \
    throw common::Exception(str); \
    return 0; \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductPtr ABSTRACT_FACTORY_NAME::createUnmanaged() \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes."; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit; \
    \
    if(!defaultProduct().isEmpty()) \
    { \
         cit = registry().find(defaultProduct()); \
        \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeUnmanaged()); \
        } \
        else \
        { \
            str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered default concrete factory classes."; \
            throw common::Exception(str); \
        } \
    } \
    cit = registry().begin(); \
    return (cit.value()->makeUnmanaged()); \
} \
\
ABSTRACT_FACTORY_NAME::AbstractProductPtr ABSTRACT_FACTORY_NAME::createUnmanaged(QObject *parent) \
{ \
    QString str; \
    \
    if(registry().empty()) \
    { \
        str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered concrete factory classes."; \
        throw common::Exception(str); \
    } \
    \
    Registry::const_iterator cit; \
    \
    if(!defaultProduct().isEmpty()) \
    { \
         cit = registry().find(defaultProduct()); \
        \
        if(cit!=registry().end()) \
        { \
            return (cit.value()->makeQUnmanaged(parent)); \
        } \
        else \
        { \
            str = "Abstract factory " #ABSTRACT_FACTORY_NAME " has no registered default concrete factory classes."; \
            throw common::Exception(str); \
        } \
    } \
    cit = registry().begin(); \
    return (cit.value()->makeQUnmanaged(parent)); \
} \
\
QVector<QString> ABSTRACT_FACTORY_NAME::products() \
{ \
    return registryStr(); \
} \
\
ABSTRACT_FACTORY_NAME::const_iterator ABSTRACT_FACTORY_NAME::productsBegin() \
{ \
    return registryStr().begin(); \
} \
\
ABSTRACT_FACTORY_NAME::const_iterator ABSTRACT_FACTORY_NAME::productsEnd() \
{ \
    return registryStr().end(); \
} \
\
ABSTRACT_FACTORY_NAME::ABSTRACT_FACTORY_NAME(const QString& className,bool isDefaultProduct) \
{ \
    Registry::iterator pos; \
    \
    registry().insert(className,this); \
    registryStr().push_back(className); \
    if(isDefaultProduct) \
    { \
        defaultProduct() = className; \
    } \
} \
\

//-------------------------------------------------------------------------------------------

#define CONCRETE_FACTORY_CLASS_IMPL(ABSTRACT_FACTORY_NAME,ABSTRACT_PRODUCT_NAME, \
                                    CONCRETE_FACTORY_NAME,CONCRETE_PRODUCT_NAME, \
                                    CONCRETE_PRODUCT_STR,IS_DEFAULT_PRODUCT) \
\
class CONCRETE_FACTORY_NAME : public ABSTRACT_FACTORY_NAME \
{ \
    public: \
        CONCRETE_FACTORY_NAME(); \
        ~CONCRETE_FACTORY_NAME(); \
\
    protected: \
        typedef ABSTRACT_PRODUCT_NAME AbstractProduct; \
        typedef ABSTRACT_PRODUCT_NAME* AbstractProductPtr; \
        typedef CONCRETE_PRODUCT_NAME ConcreteProduct; \
        typedef CONCRETE_PRODUCT_NAME* ConcreteProductPtr; \
\
        virtual AbstractProductQPtr makeShared() const; \
        virtual AbstractProductPtr makeUnmanaged() const; \
\
        virtual AbstractProductQPtr makeQShared(QObject *parent) const; \
        virtual AbstractProductPtr makeQUnmanaged(QObject *parent) const; \
\
    private: \
        static const CONCRETE_FACTORY_NAME register_this; \
}; \
\
const CONCRETE_FACTORY_NAME CONCRETE_FACTORY_NAME::register_this; \
\
CONCRETE_FACTORY_NAME::CONCRETE_FACTORY_NAME() : ABSTRACT_FACTORY_NAME(CONCRETE_PRODUCT_STR,IS_DEFAULT_PRODUCT) \
{} \
\
CONCRETE_FACTORY_NAME::~CONCRETE_FACTORY_NAME() \
{} \
\
CONCRETE_FACTORY_NAME::AbstractProductQPtr CONCRETE_FACTORY_NAME::makeShared() const \
{ \
    AbstractProductQPtr productPtr(new ConcreteProduct()); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductPtr CONCRETE_FACTORY_NAME::makeUnmanaged() const \
{ \
    AbstractProductPtr productPtr = new ConcreteProduct(); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductQPtr CONCRETE_FACTORY_NAME::makeQShared(QObject *parent) const \
{ \
    AbstractProductQPtr productPtr(new ConcreteProduct()); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductPtr CONCRETE_FACTORY_NAME::makeQUnmanaged(QObject *parent) const \
{ \
    AbstractProductPtr productPtr = new ConcreteProduct(); \
    return productPtr; \
} \
\

//-------------------------------------------------------------------------------------------

#define CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(ABSTRACT_FACTORY_NAME,ABSTRACT_PRODUCT_NAME, \
                                            CONCRETE_FACTORY_NAME,CONCRETE_PRODUCT_NAME, \
                                            CONCRETE_PRODUCT_STR,IS_DEFAULT_PRODUCT) \
\
class CONCRETE_FACTORY_NAME : public ABSTRACT_FACTORY_NAME \
{ \
    public: \
        CONCRETE_FACTORY_NAME(); \
        ~CONCRETE_FACTORY_NAME(); \
\
    protected: \
        typedef ABSTRACT_PRODUCT_NAME AbstractProduct; \
        typedef ABSTRACT_PRODUCT_NAME* AbstractProductPtr; \
        typedef CONCRETE_PRODUCT_NAME ConcreteProduct; \
        typedef CONCRETE_PRODUCT_NAME* ConcreteProductPtr; \
\
        virtual AbstractProductQPtr makeShared() const; \
        virtual AbstractProductPtr makeUnmanaged() const; \
\
        virtual AbstractProductQPtr makeQShared(QObject *parent) const; \
        virtual AbstractProductPtr makeQUnmanaged(QObject *parent) const; \
\
    private: \
        static const CONCRETE_FACTORY_NAME register_this; \
}; \
\
const CONCRETE_FACTORY_NAME CONCRETE_FACTORY_NAME::register_this; \
\
CONCRETE_FACTORY_NAME::CONCRETE_FACTORY_NAME() : ABSTRACT_FACTORY_NAME(CONCRETE_PRODUCT_STR,IS_DEFAULT_PRODUCT) \
{} \
\
CONCRETE_FACTORY_NAME::~CONCRETE_FACTORY_NAME() \
{} \
\
CONCRETE_FACTORY_NAME::AbstractProductQPtr CONCRETE_FACTORY_NAME::makeShared() const \
{ \
    AbstractProductQPtr productPtr(new ConcreteProduct()); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductPtr CONCRETE_FACTORY_NAME::makeUnmanaged() const \
{ \
    AbstractProductPtr productPtr = new ConcreteProduct(); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductQPtr CONCRETE_FACTORY_NAME::makeQShared(QObject *parent) const \
{ \
    AbstractProductQPtr productPtr(new ConcreteProduct(parent)); \
    return productPtr; \
} \
\
CONCRETE_FACTORY_NAME::AbstractProductPtr CONCRETE_FACTORY_NAME::makeQUnmanaged(QObject *parent) const \
{ \
    AbstractProductPtr productPtr = new ConcreteProduct(parent); \
    return productPtr; \
} \
\

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
