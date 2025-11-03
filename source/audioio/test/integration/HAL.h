/*==================================================================================================
     File:       CoreAudio/AudioHardwareBase.h

     Copyright:  (c) 1985-2011 by Apple, Inc., all rights reserved.

     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:

                     http://developer.apple.com/bugreporter/

==================================================================================================*/
#if !defined(__AudioHardwareBase_h__)
#define __AudioHardwareBase_h__

//==================================================================================================
#pragma mark -
#pragma mark Overview
/*!
    @header AudioHardwareBase

    This file defines the HAL's object model including the properties and their needed types and
    constants that describe the proeprty values.
*/

//==================================================================================================
//  Includes

#include <CoreAudio/CoreAudioTypes.h>

//==================================================================================================

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint off
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================
#pragma mark -
#pragma mark Basic Types

/*!
    @typedef        AudioObjectID
    @abstract       A UInt32 that provides a handle on a specific AudioObject.
*/
typedef UInt32  AudioObjectID;

/*!
    @typedef        AudioClassID
    @abstract       AudioClassIDs are used to identify the class of an AudioObject.
*/
typedef UInt32  AudioClassID;

/*!
    @typedef        AudioObjectPropertySelector
    @abstract       An AudioObjectPropertySelector is a four char code that identifies, along with
                    the AudioObjectPropertyScope and AudioObjectPropertyElement, a specific piece of
                    information about an AudioObject.
    @discussion     The property selector specifies the general classification of the property such
                    as volume, stream format, latency, etc. Note that each class has a different set
                    of selectors. A subclass inherits its super class's set of selectors, although
                    it may not implement them all.
*/
typedef UInt32  AudioObjectPropertySelector;

/*!
    @typedef        AudioObjectPropertyScope
    @abstract       An AudioObjectPropertyScope is a four char code that identifies, along with the
                    AudioObjectPropertySelector and AudioObjectPropertyElement, a specific piece of
                    information about an AudioObject.
    @discussion     The scope specifies the section of the object in which to look for the property,
                    such as input, output, global, etc. Note that each class has a different set of
                    scopes. A subclass inherits its superclass's set of scopes.
*/
typedef UInt32  AudioObjectPropertyScope;

/*!
    @typedef        AudioObjectPropertyElement
    @abstract       An AudioObjectPropertyElement is an integer that identifies, along with the
                    AudioObjectPropertySelector and AudioObjectPropertyScope, a specific piece of
                    information about an AudioObject.
    @discussion     The element selects one of possibly many items in the section of the object in
                    which to look for the property. Elements are number sequentially where 0
                    represents the master element. Elements are particular to an instance of a
                    class, meaning that two instances can have different numbers of elements in the
                    same scope. There is no inheritance of elements.
*/
typedef UInt32  AudioObjectPropertyElement;

/*!
    @struct         AudioObjectPropertyAddress
    @abstract       An AudioObjectPropertyAddress collects the three parts that identify a specific
                    property together in a struct for easy transmission.
    @field          mSelector
                        The AudioObjectPropertySelector for the property.
    @field          mScope
                        The AudioObjectPropertyScope for the property.
    @field          mElement
                        The AudioObjectPropertyElement for the property.
*/
struct  AudioObjectPropertyAddress
{
    AudioObjectPropertySelector mSelector;
    AudioObjectPropertyScope    mScope;
    AudioObjectPropertyElement  mElement;
};
typedef struct AudioObjectPropertyAddress   AudioObjectPropertyAddress;

//==================================================================================================
#pragma mark Basic Constants

/*!
    @enum           Error Constants
    @abstract       The error constants unique to the HAL.
    @discussion     These are the error constants that are unique to the HAL. Note that the HAL's
                    functions can and will return other codes that are not listed here. While these
                    constants give a general idea of what might have gone wrong during the execution
                    of an API call, if an API call returns anything other than kAudioHardwareNoError
                    it is to be viewed as the same failure regardless of what constant is actually
                    returned.
    @constant       kAudioHardwareNoError
                        The function call completed successfully.
    @constant       kAudioHardwareNotRunningError
                        The function call requires that the hardware be running but it isn't.
    @constant       kAudioHardwareUnspecifiedError
                        The function call failed while doing something that doesn't provide any
                        error messages.
    @constant       kAudioHardwareUnknownPropertyError
                        The AudioObject doesn't know about the property at the given address.
    @constant       kAudioHardwareBadPropertySizeError
                        An improperly sized buffer was provided when accessing the data of a
                        property.
    @constant       kAudioHardwareIllegalOperationError
                        The requested operation couldn't be completed.
    @constant       kAudioHardwareBadObjectError
                        The AudioObjectID passed to the function doesn't map to a valid AudioObject.
    @constant       kAudioHardwareBadDeviceError
                        The AudioObjectID passed to the function doesn't map to a valid AudioDevice.
    @constant       kAudioHardwareBadStreamError
                        The AudioObjectID passed to the function doesn't map to a valid AudioStream.
    @constant       kAudioHardwareUnsupportedOperationError
                        The AudioObject doesn't support the requested operation.
    @constant       kAudioDeviceUnsupportedFormatError
                        The AudioStream doesn't support the requested format.
    @constant       kAudioDevicePermissionsError
                        The requested operation can't be completed because the process doesn't have
                        permission.
*/
enum
{
    kAudioHardwareNoError                   = 0,
    kAudioHardwareNotRunningError           = 'stop',
    kAudioHardwareUnspecifiedError          = 'what',
    kAudioHardwareUnknownPropertyError      = 'who?',
    kAudioHardwareBadPropertySizeError      = '!siz',
    kAudioHardwareIllegalOperationError     = 'nope',
    kAudioHardwareBadObjectError            = '!obj',
    kAudioHardwareBadDeviceError            = '!dev',
    kAudioHardwareBadStreamError            = '!str',
    kAudioHardwareUnsupportedOperationError = 'unop',
    kAudioDeviceUnsupportedFormatError      = '!dat',
    kAudioDevicePermissionsError            = '!hog'
};

/*!
    @enum           Predefined AudioObjectID values
    @abstract       ObjectIDs that are always the same
    @constant       kAudioObjectUnknown
                        This is the sentinel value. No object will have an ID whose value is 0.
*/
enum
{
    kAudioObjectUnknown         = 0
};

/*!
    @enum           Property Address Constants
    @abstract       The valid values for the scope in a property address.
    @constant       kAudioObjectPropertyScopeGlobal
                        The AudioObjectPropertyScope for properties that apply to the object as a
                        whole. All objects have a global scope and for most it is their only scope.
    @constant       kAudioObjectPropertyScopeInput
                        The AudioObjectPropertyScope for properties that apply to the input side of
                        an object.
    @constant       kAudioObjectPropertyScopeOutput
                        The AudioObjectPropertyScope for properties that apply to the output side of
                        an object.
    @constant       kAudioObjectPropertyScopePlayThrough
                        The AudioObjectPropertyScope for properties that apply to the play through
                        side of an object.
    @constant       kAudioObjectPropertyElementMaster
                        The AudioObjectPropertyElement value for properties that apply to the master
                        element or to the entire scope.
*/
enum
{
    kAudioObjectPropertyScopeGlobal         = 'glob',
    kAudioObjectPropertyScopeInput          = 'inpt',
    kAudioObjectPropertyScopeOutput         = 'outp',
    kAudioObjectPropertyScopePlayThrough    = 'ptru',
    kAudioObjectPropertyElementMaster       = 0
};

/*!
    @enum           Wildcard Constants
    @abstract       Constants that are used as wildcards.
    @discussion     Wildcards match any and all values for thier associated type. They are useful
                    when registering to receive notificationss.
    @constant       kAudioObjectPropertySelectorWildcard
                        The wildcard value for AudioObjectPropertySelectors.
    @constant       kAudioObjectPropertyScopeWildcard
                        The wildcard value for AudioObjectPropertyScopes.
    @constant       kAudioObjectPropertyElementWildcard
                        The wildcard value for AudioObjectPropertyElements.
    @constant       kAudioObjectClassIDWildcard
                        The wildcard value for AudioClassIDs.
*/
enum
{
    kAudioObjectPropertySelectorWildcard    = '****',
    kAudioObjectPropertyScopeWildcard       = '****',
    kAudioObjectPropertyElementWildcard     = 0xFFFFFFFF,
    kAudioObjectClassIDWildcard             = '****'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioObject Constants

/*!
    @enum           AudioObject Class Constants
    @abstract       Various constants related to the AudioObject class.
    @discussion     The AudioObject class is the base class for all classes.
    @constant       kAudioObjectClassID
                        The AudioClassID that identifies the AudioObject class.
*/
enum
{
    kAudioObjectClassID = 'aobj'
};

//==================================================================================================
#pragma mark AudioObject Properties

/*!
    @enum           AudioObject Property Selectors
    @abstract       AudioObjectPropertySelector values provided by objects of the AudioObject class.
    @discussion     The AudioObject class is the base class for all classes. As such, all classes
                    inherit this set of properties.
    @constant       kAudioObjectPropertyBaseClass
                        An AudioClassID that identifies the class from which the clase of the
                        AudioObject is derived. This value must always be one of the standard
                        classes.
    @constant       kAudioObjectPropertyClass
                        An AudioClassID that identifies the class of the AudioObject.
    @constant       kAudioObjectPropertyOwner
                        An AudioObjectID that identifies the the AudioObject that owns the given
                        AudioObject. Note that all AudioObjects are owned by some other AudioObject.
                        The only exception is the AudioSystemObject, for which the value of this
                        property is kAudioObjectUnknown.
    @constant       kAudioObjectPropertyName
                        A CFString that contains the human readable name of the object. The caller
                        is responsible for releasing the returned CFObject.
    @constant       kAudioObjectPropertyModelName
                        A CFString that contains the human readable model name of the object. The
                        model name differs from kAudioObjectPropertyName in that two objects of the
                        same model will have the same value for this property but may have different
                        values for kAudioObjectPropertyName.
    @constant       kAudioObjectPropertyManufacturer
                        A CFString that contains the human readable name of the manufacturer of the
                        hardware the AudioObject is a part of. The caller is responsible for
                        releasing the returned CFObject.
    @constant       kAudioObjectPropertyElementName
                        A CFString that contains a human readable name for the given element in the
                        given scope. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioObjectPropertyElementCategoryName
                        A CFString that contains a human readable name for the category of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
    @constant       kAudioObjectPropertyElementNumberName
                        A CFString that contains a human readable name for the number of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
    @constant       kAudioObjectPropertyOwnedObjects
                        An array of AudioObjectIDs that represent all the AudioObjects owned by the
                        given object. The qualifier is an array of AudioClassIDs. If it is
                        non-empty, the returned array of AudioObjectIDs will only refer to objects
                        whose class is in the qualifier array or whose is a subclass of one in the
                        qualifier array.
    @constant       kAudioObjectPropertyIdentify
                        A UInt32 where a value of one indicates that the object's hardware is
                        drawing attention to itself, typically by flashing or lighting up its front
                        panel display. A value of 0 indicates that this function is turned off. This
                        makes it easy for a user to associate the physical hardware with its
                        representation in an application. Typically, this property is only supported
                        by AudioDevices and AudioBoxes.
    @constant       kAudioObjectPropertySerialNumber
                        A CFString that contains the human readable serial number for the object.
                        This property will typically be implemented by AudioBox and AudioDevice
                        objects. Note that the serial number is not defined to be unique in the same
                        way that an AudioBox's or AudioDevice's UID propety are defined. This is
                        purely an informational value. The caller is responsible for releaseing the
                        returned CFObject.
    @constant       kAudioObjectPropertyFirmwareVersion
                        A CFString that contains the human readable firmware version for the object.
                        This property will typically be implemented by AudioBox and AudioDevice
                        objects. Note that this is purely an informational value. The caller is
                        responsible for releaseing the returned CFObject.
*/
enum
{
    kAudioObjectPropertyBaseClass           = 'bcls',
    kAudioObjectPropertyClass               = 'clas',
    kAudioObjectPropertyOwner               = 'stdv',
    kAudioObjectPropertyName                = 'lnam',
    kAudioObjectPropertyModelName           = 'lmod',
    kAudioObjectPropertyManufacturer        = 'lmak',
    kAudioObjectPropertyElementName         = 'lchn',
    kAudioObjectPropertyElementCategoryName = 'lccn',
    kAudioObjectPropertyElementNumberName   = 'lcnn',
    kAudioObjectPropertyOwnedObjects        = 'ownd',
    kAudioObjectPropertyIdentify            = 'iden',
    kAudioObjectPropertySerialNumber        = 'snum',
    kAudioObjectPropertyFirmwareVersion     = 'fwvn'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioPlugIn Constants

/*!
    @enum           AudioPlugIn Class Constants
    @abstract       Various constants related to the AudioPlugIn class.
    @constant       kAudioPlugInClassID
                        The AudioClassID that identifies the AudioPlugIn class.
*/
enum
{
    kAudioPlugInClassID = 'aplg'
};

//==================================================================================================
#pragma mark AudioPlugIn Properties

/*!
    @enum           AudioPlugIn Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioPlugIn class.
    @discussion     The AudioPlugIn class is a subclass of the AudioObject class. The class has just
                    the global scope, kAudioObjectPropertyScopeGlobal, and only a master element.
    @constant       kAudioPlugInPropertyBundleID
                        A CFString that contains the bundle identifier for the AudioPlugIn. The
                        caller is responsible for releasing the returned CFObject.
    @constant       kAudioPlugInPropertyDeviceList
                        An array of AudioObjectIDs that represent all the AudioDevices currently
                        provided by the plug-in.
    @constant       kAudioPlugInPropertyTranslateUIDToDevice
                        This property fetches the AudioObjectID that corresponds to the AudioDevice
                        that has the given UID. The UID is passed in via the qualifier as a CFString
                        while the AudioObjectID for the AudioDevice is returned to the caller as the
                        property's data. Note that an error is not returned if the UID doesn't refer
                        to any AudioDevices. Rather, this property will return kAudioObjectUnknown
                        as the value of the property.
    @constant       kAudioPlugInPropertyBoxList
                        An array of AudioObjectIDs that represent all the AudioBox objects currently
                        provided by the plug-in
    @constant       kAudioPlugInPropertyTranslateUIDToBox
                        This property fetches the AudioObjectID that corresponds to the AudioBox
                        that has the given UID. The UID is passed in via the qualifier as a CFString
                        while the AudioObjectID for the AudioBox is returned to the caller as the
                        property's data. Note that an error is not returned if the UID doesn't refer
                        to any AudioBoxes. Rather, this property will return kAudioObjectUnknown
                        as the value of the property.
*/
enum
{
    kAudioPlugInPropertyBundleID                = 'piid',
    kAudioPlugInPropertyDeviceList              = 'dev#',
    kAudioPlugInPropertyTranslateUIDToDevice    = 'uidd',
    kAudioPlugInPropertyBoxList                 = 'box#',
    kAudioPlugInPropertyTranslateUIDToBox       = 'uidb'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioTransportManager Constants

/*!
    @enum           AudioTransportManager Class Constants
    @abstract       Various constants related to the AudioTransportManager class.
    @discussion     The AudioTransportManager class is a subclass of the AudioPlugIn class.
                    Instances manage an entire tranport mechanism such as AirPlay or an AVB network
                    of devices. AudioTransportManagers present a list of AudioEndPoints that
                    represent all the devices that they manage. They can combine AudioEndPoints into
                    an AudioEndPointDevice that can be used by the system the same as any other
                    AudioDevice.
    @constant       kAudioTransportManagerClassID
                        The AudioClassID that identifies the AudioTransportManager class.
*/
enum
{
    kAudioTransportManagerClassID   = 'trpm'
};

//==================================================================================================
#pragma mark AudioTransportManager Properties

/*!
    @enum           AudioTransportManager Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioTransportManager class.
    @discussion     The AudioTransportManager class is a subclass of the AudioPlugIn class. The
                    class has just the global scope, kAudioObjectPropertyScopeGlobal, and only a
                    master element.
    @constant       kAudioTransportManagerPropertyEndPointList
                        An array of AudioObjectIDs for all the AudioEndPoint objects the transport
                        manager is tracking.
    @constant       kAudioTransportManagerPropertyTranslateUIDToEndPoint
                        This property fetches the AudioObjectID that corresponds to the
                        AudioEndpoint that has the given UID. The UID is passed in via the qualifier
                        as a CFString while the AudioObjectID for the AudioEndpoint is returned to
                        the caller as the property's data. Note that an error is not returned if the
                        UID doesn't refer to any AudioEndpoints. Rather, this property will return
                        kAudioObjectUnknown as the value of the property.
    @constant       kAudioTransportManagerPorpertyTransportType
                        A UInt32 whose value indicates how the transport manager's endpoints and
                        endpoint devices are connected to the CPU. Constants for some of the values
                        for this property can be found in the enum in the AudioDevice Constants
                        section of this file.
*/
enum
{
    kAudioTransportManagerPropertyEndPointList              = 'end#',
    kAudioTransportManagerPropertyTranslateUIDToEndPoint    = 'uide',
    kAudioTransportManagerPropertyTransportType             = 'tran'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioBox Constants

/*!
    @enum           AudioBox Class Constants
    @abstract       Various constants related to the AudioBox class.
    @constant       kAudioBoxClassID
                        The AudioClassID that identifies the AudioBox class.
*/
enum
{
    kAudioBoxClassID    = 'abox'
};

//==================================================================================================
#pragma mark AudioBox Properties

/*!
    @enum           AudioBox Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioBox class.
    @discussion     The AudioBox class is a subclass of the AudioObject class. The class has just
                    the global scope, kAudioObjectPropertyScopeGlobal, and only a master element.
                    An AudioBox is a container for other objects (typically AudioDevice objects). An
                    AudioBox publishes identifying information about itself and can be enabled or
                    disabled. A box's contents are only available to the system when the box is
                    enabled.
    @constant       kAudioBoxPropertyBoxUID
                        A CFString that contains a persistent identifier for the AudioBox. An
                        AudioBox's UID is persistent across boots. The content of the UID string is
                        a black box and may contain information that is unique to a particular
                        instance of an AudioBox's hardware or unique to the CPU. Therefore they are
                        not suitable for passing between CPUs or for identifying similar models of
                        hardware. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioBoxPropertyTransportType
                        A UInt32 whose value indicates how the AudioBox is connected to the system.
                        Constants for some of the values for this property can be found in the enum
                        in the AudioDevice Constants section of AudioHardwareBase.h
    @constant       kAudioBoxPropertyHasAudio
                        A UInt32 where a non-zero value indicates that the box supports audio.
    @constant       kAudioBoxPropertyHasVideo
                        A UInt32 where a non-zero value indicates that the box supports video.
    @constant       kAudioBoxPropertyHasMIDI
                        A UInt32 where a non-zero value indicates that the box supports MIDI.
    @constant       kAudioBoxPropertyIsProtected
                        A UInt32 where a non-zero value indicates that the box requires
                        authentication to use.
    @constant       kAudioBoxPropertyAcquired
                        A UInt32 where a non-zero value indicates that the box's contents are
                        available to the system.
    @constant       kAudioBoxPropertyAcquisitionFailed
                        An OSStatus that indicates the reason for an attempt to acquire a box
                        failed. Note that this property is primarily used for notifications.
    @constant       kAudioBoxPropertyDeviceList
                        An array of AudioObjectIDs that represent all the AudioDevice objects that
                        came out of the given AudioBox. Note that until a box is enabled, this list
                        will be empty.
*/
enum
{
    kAudioBoxPropertyBoxUID             = 'buid',
    kAudioBoxPropertyTransportType      = 'tran',
    kAudioBoxPropertyHasAudio           = 'bhau',
    kAudioBoxPropertyHasVideo           = 'bhvi',
    kAudioBoxPropertyHasMIDI            = 'bhmi',
    kAudioBoxPropertyIsProtected        = 'bpro',
    kAudioBoxPropertyAcquired           = 'bxon',
    kAudioBoxPropertyAcquisitionFailed  = 'bxof',
    kAudioBoxPropertyDeviceList         = 'bdv#'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioDevice Constants

/*!
    @enum           AudioDevice Class Constants
    @abstract       Various constants related to the AudioDevice class.
    @constant       kAudioDeviceClassID
                        The AudioClassID that identifies the AudioDevice class.
*/
enum
{
    kAudioDeviceClassID = 'adev'
};

/*!
    @enum           Transport Type IDs
    @abstract       Commonly used values for kAudioDevicePropertyTransportType and
                    kAudioTransportManagerPropertyTransportType
    @constant       kAudioDeviceTransportTypeUnknown
                        The transport type ID returned when a device doesn't provide a transport
                        type.
    @constant       kAudioDeviceTransportTypeBuiltIn
                        The transport type ID for AudioDevices built into the system.
    @constant       kAudioDeviceTransportTypeAggregate
                        The transport type ID for aggregate devices.
    @constant       kAudioDeviceTransportTypeAutoAggregate
                        The transport type ID for automatically generated aggregate devices.
    @constant       kAudioDeviceTransportTypeVirtual
                        The transport type ID for AudioDevices that don't correspond to real audio
                        hardware.
    @constant       kAudioDeviceTransportTypePCI
                        The transport type ID for AudioDevices connected via the PCI bus.
    @constant       kAudioDeviceTransportTypeUSB
                        The transport type ID for AudioDevices connected via USB.
    @constant       kAudioDeviceTransportTypeFireWire
                        The transport type ID for AudioDevices connected via FireWire.
    @constant       kAudioDeviceTransportTypeBluetooth
                        The transport type ID for AudioDevices connected via Bluetooth.
    @constant       kAudioDeviceTransportTypeBluetoothLE
                        The transport type ID for AudioDevices connected via Bluetooth Low Energy.
    @constant       kAudioDeviceTransportTypeHDMI
                        The transport type ID for AudioDevices connected via HDMI.
    @constant       kAudioDeviceTransportTypeDisplayPort
                        The transport type ID for AudioDevices connected via DisplayPort.
    @constant       kAudioDeviceTransportTypeAirPlay
                        The transport type ID for AudioDevices connected via AirPlay.
    @constant       kAudioDeviceTransportTypeAVB
                        The transport type ID for AudioDevices connected via AVB.
    @constant       kAudioDeviceTransportTypeThunderbolt
                        The transport type ID for AudioDevices connected via Thunderbolt.
*/
enum
{
    kAudioDeviceTransportTypeUnknown        = 0,
    kAudioDeviceTransportTypeBuiltIn        = 'bltn',
    kAudioDeviceTransportTypeAggregate      = 'grup',
    kAudioDeviceTransportTypeVirtual        = 'virt',
    kAudioDeviceTransportTypePCI            = 'pci ',
    kAudioDeviceTransportTypeUSB            = 'usb ',
    kAudioDeviceTransportTypeFireWire       = '1394',
    kAudioDeviceTransportTypeBluetooth      = 'blue',
    kAudioDeviceTransportTypeBluetoothLE    = 'blea',
    kAudioDeviceTransportTypeHDMI           = 'hdmi',
    kAudioDeviceTransportTypeDisplayPort    = 'dprt',
    kAudioDeviceTransportTypeAirPlay        = 'airp',
    kAudioDeviceTransportTypeAVB            = 'eavb',
    kAudioDeviceTransportTypeThunderbolt    = 'thun'
};

//==================================================================================================
#pragma mark AudioDevice Properties

/*!
    @enum           AudioDevice Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioDevice class.
    @discussion     The AudioDevice class is a subclass of the AudioObjectClass. The class has four
                    scopes, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyScopeInput,
                    kAudioObjectPropertyScopeOutput, and kAudioObjectPropertyScopePlayThrough. The
                    class has a master element and an element for each channel in each stream
                    numbered according to the starting channel number of each stream.
    @constant       kAudioDevicePropertyConfigurationApplication
                        A CFString that contains the bundle ID for an application that provides a
                        GUI for configuring the AudioDevice. By default, the value of this property
                        is the bundle ID for Audio MIDI Setup. The caller is responsible for
                        releasing the returned CFObject.
    @constant       kAudioDevicePropertyDeviceUID
                        A CFString that contains a persistent identifier for the AudioDevice. An
                        AudioDevice's UID is persistent across boots. The content of the UID string
                        is a black box and may contain information that is unique to a particular
                        instance of an AudioDevice's hardware or unique to the CPU. Therefore they
                        are not suitable for passing between CPUs or for identifying similar models
                        of hardware. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioDevicePropertyModelUID
                        A CFString that contains a persistent identifier for the model of an
                        AudioDevice. The identifier is unique such that the identifier from two
                        AudioDevices are equal if and only if the two AudioDevices are the exact
                        same model from the same manufacturer. Further, the identifier has to be the
                        same no matter on what machine the AudioDevice appears. The caller is
                        responsible for releasing the returned CFObject.
    @constant       kAudioDevicePropertyTransportType
                        A UInt32 whose value indicates how the AudioDevice is connected to the CPU.
                        Constants for some of the values for this property can be found in the enum
                        in the AudioDevice Constants section of this file.
    @constant       kAudioDevicePropertyRelatedDevices
                        An array of AudioDeviceIDs for devices related to the AudioDevice. For
                        IOAudio-based devices, a AudioDevices are related if they share the same
                        IOAudioDevice object.
    @constant       kAudioDevicePropertyClockDomain
                        A UInt32 whose value indicates the clock domain to which this AudioDevice
                        belongs. AudioDevices that have the same value for this property are able to
                        be synchronized in hardware. However, a value of 0 indicates that the clock
                        domain for the device is unspecified and should be assumed to be separate
                        from every other device's clock domain, even if they have the value of 0 as
                        their clock domain as well.
    @constant       kAudioDevicePropertyDeviceIsAlive
                        A UInt32 where a value of 1 means the device is ready and available and 0
                        means the device is usable and will most likely go away shortly.
    @constant       kAudioDevicePropertyDeviceIsRunning
                        A UInt32 where a value of 0 means the AudioDevice is not performing IO and
                        a value of 1 means that it is. Note that the device can be running even if
                        there are no active IOProcs such as by calling AudioDeviceStart() and
                        passing a NULL IOProc. Note that the notification for this property is
                        usually sent from the AudioDevice's IO thread.
    @constant       kAudioDevicePropertyDeviceCanBeDefaultDevice
                        A UInt32 where 1 means that the AudioDevice is a possible selection for
                        kAudioHardwarePropertyDefaultInputDevice or
                        kAudioHardwarePropertyDefaultOutputDevice depending on the scope.
    @constant       kAudioDevicePropertyDeviceCanBeDefaultSystemDevice
                        A UInt32 where 1 means that the AudioDevice is a possible selection for
                        kAudioHardwarePropertyDefaultSystemOutputDevice.
    @constant       kAudioDevicePropertyLatency
                        A UInt32 containing the number of frames of latency in the AudioDevice. Note
                        that input and output latency may differ. Further, the AudioDevice's
                        AudioStreams may have additional latency so they should be queried as well.
                        If both the device and the stream say they have latency, then the total
                        latency for the stream is the device latency summed with the stream latency.
    @constant       kAudioDevicePropertyStreams
                        An array of AudioStreamIDs that represent the AudioStreams of the
                        AudioDevice. Note that if a notification is received for this property, any
                        cached AudioStreamIDs for the device become invalid and need to be
                        re-fetched.
    @constant       kAudioObjectPropertyControlList
                        An array of AudioObjectIDs that represent the AudioControls of the
                        AudioDevice. Note that if a notification is received for this property, any
                        cached AudioObjectIDs for the device become invalid and need to be
                        re-fetched.
    @constant       kAudioDevicePropertySafetyOffset
                        A UInt32 whose value indicates the number for frames in ahead (for output)
                        or behind (for input the current hardware position that is safe to do IO.
    @constant       kAudioDevicePropertyNominalSampleRate
                        A Float64 that indicates the current nominal sample rate of the AudioDevice.
    @constant       kAudioDevicePropertyAvailableNominalSampleRates
                        An array of AudioValueRange structs that indicates the valid ranges for the
                        nominal sample rate of the AudioDevice.
    @constant       kAudioDevicePropertyIcon
                        A CFURLRef that indicates an image file that can be used to represent the
                        device visually. The caller is responsible for releasing the returned
                        CFObject.
    @constant       kAudioDevicePropertyIsHidden
                        A UInt32 where a non-zero value indicates that the device is not included
                        in the normal list of devices provided by kAudioHardwarePropertyDevices nor
                        can it be the default device. Hidden devices can only be discovered by
                        knowing their UID and using kAudioHardwarePropertyDeviceForUID.
    @constant       kAudioDevicePropertyPreferredChannelsForStereo
                        An array of two UInt32s, the first for the left channel, the second for the
                        right channel, that indicate the channel numbers to use for stereo IO on the
                        device. The value of this property can be different for input and output and
                        there are no restrictions on the channel numbers that can be used.
    @constant       kAudioDevicePropertyPreferredChannelLayout
                        An AudioChannelLayout that indicates how each channel of the AudioDevice
                        should be used.
*/
enum
{
    kAudioDevicePropertyConfigurationApplication        = 'capp',
    kAudioDevicePropertyDeviceUID                       = 'uid ',
    kAudioDevicePropertyModelUID                        = 'muid',
    kAudioDevicePropertyTransportType                   = 'tran',
    kAudioDevicePropertyRelatedDevices                  = 'akin',
    kAudioDevicePropertyClockDomain                     = 'clkd',
    kAudioDevicePropertyDeviceIsAlive                   = 'livn',
    kAudioDevicePropertyDeviceIsRunning                 = 'goin',
    kAudioDevicePropertyDeviceCanBeDefaultDevice        = 'dflt',
    kAudioDevicePropertyDeviceCanBeDefaultSystemDevice  = 'sflt',
    kAudioDevicePropertyLatency                         = 'ltnc',
    kAudioDevicePropertyStreams                         = 'stm#',
    kAudioObjectPropertyControlList                     = 'ctrl',
    kAudioDevicePropertySafetyOffset                    = 'saft',
    kAudioDevicePropertyNominalSampleRate               = 'nsrt',
    kAudioDevicePropertyAvailableNominalSampleRates     = 'nsr#',
    kAudioDevicePropertyIcon                            = 'icon',
    kAudioDevicePropertyIsHidden                        = 'hidn',
    kAudioDevicePropertyPreferredChannelsForStereo      = 'dch2',
    kAudioDevicePropertyPreferredChannelLayout          = 'srnd'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioEndPointDevice Constants

/*!
    @enum           AudioEndPointDevice Class Constants
    @abstract       Various constants related to the AudioEndPointDevice class.
    @constant       kAudioEndPointDeviceClassID
                        The AudioClassID that identifies the AudioEndPointDevice class.
*/
enum
{
    kAudioEndPointDeviceClassID = 'edev'
};

/*!
    @defined        kAudioEndPointDeviceUIDKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the UID
                    of the AudioEndPointDevice.
*/
#define kAudioEndPointDeviceUIDKey  "uid"

/*!
    @defined        kAudioEndPointDeviceNameKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the
                    human readable name of the AudioEndPointDevice.
*/
#define kAudioEndPointDeviceNameKey "name"

/*!
    @defined        kAudioEndPointDeviceEndPointListKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFArray of CFDictionaries that
                    describe each AudioEndPoint in the AudioEndPointDevice. The keys for this
                    CFDictionary are defined in the AudioEndPoint Constants section.
*/
#define kAudioEndPointDeviceEndPointListKey "endpoints"

/*!
    @defined        kAudioEndPointDeviceMasterEndPointKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the UID
                    for the AudioEndPoint that is the master time source for the
                    AudioEndPointDevice.
*/
#define kAudioEndPointDeviceMasterEndPointKey   "master"

/*!
    @defined        kAudioEndPointDeviceIsPrivateKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFNumber where a value of 0
                    means that the AudioEndPointDevice is to be published to the entire system and a
                    value of 1 means that the AudioEndPointDevice is private to the process that
                    created it. Note that a private AudioEndPointDevice is not persistent across
                    launches of the process that created it. Note that if this key is not present,
                    it implies that the AudioEndPointDevice is published to the entire system.
*/
#define kAudioEndPointDeviceIsPrivateKey    "private"

//==================================================================================================
#pragma mark AudioEndPointDevice Properties

/*!
    @enum           AudioEndPointDevice Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioEndPointDevice class.
    @discussion     AudioEndPointDevice is a subclass of AudioDevice and has the same scope and
                    element structure.
    @constant       kAudioEndPointDevicePropertyComposition
                        A CFDictionary that describes the composition of the AudioEndPointDevice.
                        The keys for this CFDicitionary are defined in the AudioEndPointDevice
                        Constants section. The caller is responsible for releasing the returned
                        CFObject.
    @constant       kAudioEndPointDevicePropertyEndPointList
                        An array of AudioObjectIDs for all the AudioEndPoints in the device.
    @constant       kAudioEndPointDevicePropertyIsPrivate
                        A pid_t where a value of 0 idicates that the device is public and a non-zero
                        value indicates the pid of the process that owns the device.
*/
enum
{
    kAudioEndPointDevicePropertyComposition         = 'acom',
    kAudioEndPointDevicePropertyEndPointList        = 'agrp',
    kAudioEndPointDevicePropertyIsPrivate           = 'priv'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioEndPoint Constants

/*!
    @enum           AudioEndPoint Class Constants
    @abstract       Various constants related to the AudioEndPoint class.
    @discussion     The AudioEndPoint class is a subclass of AudioDevice class and has the same
                    scope and element structure. However, AudioEndPoint objects do not implement an
                    IO path of their own and as such do not implmenent any  AudioDevice properties
                    associated with the IO path.
    @constant       kAudioEndPointDeviceClassID
                        The AudioClassID that identifies the AudioEndPointDevice class.
*/
enum
{
    kAudioEndPointClassID   = 'endp'
};

/*!
    @defined        kAudioEndPointUIDKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFString that contains the UID for the AudioEndPoint.
*/
#define kAudioEndPointUIDKey    "uid"

/*!
    @defined        kAudioEndPointNameKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFString that contains the human readable name of the AudioEndPoint.
*/
#define kAudioEndPointNameKey   "name"

/*!
    @defined        kAudioEndPointInputChannelsKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFNumber that indicates the total number of input channels for the
                    AudioEndPoint.
*/
#define kAudioEndPointInputChannelsKey  "channels-in"

/*!
    @defined        kAudioEndPointOutputChannelsKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFNumber that indicates the total number of output channels for the
                    AudioEndPoint.
*/
#define kAudioEndPointOutputChannelsKey "channels-out"

//==================================================================================================
#pragma mark -
#pragma mark AudioStream Types

/*!
    @struct         AudioStreamRangedDescription
    @abstract       This structure allows a specific sample rate range to be associated with an
                    AudioStreamBasicDescription that specifies its sample rate as
                    kAudioStreamAnyRate.
    @discussion     Note that this structure is only used to desicribe the the available formats
                    for a stream. It is not used for the current format.
    @field          mFormat
                        The AudioStreamBasicDescription that describes the format of the stream.
                        Note that the mSampleRate field of the structure will be the same as the
                        the values in mSampleRateRange when only a single sample rate is supported.
                        It will be kAudioStreamAnyRate when there is a range with more elements.
    @field          mSampleRateRange
                        The AudioValueRange that describes the minimum and maximum sample rate for
                        the stream. If the mSampleRate field of mFormat is kAudioStreamAnyRate the
                        format supports the range of sample rates described by this structure.
                        Otherwise, the minimum will be the same as the maximum which will be the
                        same as the mSampleRate field of mFormat.
*/
struct  AudioStreamRangedDescription
{
    AudioStreamBasicDescription     mFormat;
    AudioValueRange                 mSampleRateRange;
};
typedef struct AudioStreamRangedDescription AudioStreamRangedDescription;

//==================================================================================================
#pragma mark AudioStream Constants

/*!
    @enum           AudioStream Class Constants
    @abstract       Various constants related to the AudioStream class.
    @constant       kAudioStreamClassID
                        The AudioClassID that identifies the AudioStream class.
*/
enum
{
    kAudioStreamClassID = 'astr'
};

/*!
    @enum           AudioStream Terminal Types
    @abstract       Various constants that describe the terminal type of an AudioStream.
    @constant       kAudioStreamTerminalTypeUnknown
                        The ID used when the terminal type for the AudioStream is non known.
    @constant       kAudioStreamTerminalTypeLine
                        The ID for a terminal type of a line level stream. Note that this applies to
                        both input streams and output streams
    @constant       kAudioStreamTerminalTypeDigitalAudioInterface
                        The ID for a terminal type of stream from/to a digital audio interface as
                        defined by ISO 60958 (aka SPDIF or AES/EBU). Note that this applies to both
                        input streams and output streams
    @constant       kAudioStreamTerminalTypeSpeaker
                        The ID for a terminal type of a speaker.
    @constant       kAudioStreamTerminalTypeHeadphones
                        The ID for a terminal type of headphones.
    @constant       kAudioStreamTerminalTypeLFESpeaker
                        The ID for a terminal type of a speaker for low frequency effects.
    @constant       kAudioStreamTerminalTypeReceiverSpeaker
                        The ID for a terminal type of a speaker on a telephone handset receiver.
    @constant       kAudioStreamTerminalTypeMicrophone
                        The ID for a terminal type of a microphone.
    @constant       kAudioStreamTerminalTypeHeadsetMicrophone
                        The ID for a terminal type of a microphone attached to an headset.
    @constant       kAudioStreamTerminalTypeReceiverMicrophone
                        The ID for a terminal type of a microhpone on a telephone handset recevier.
    @constant       kAudioStreamTerminalTypeTTY
                        The ID for a terminal type of a device providing a TTY signal.
    @constant       kAudioStreamTerminalTypeHDMI
                        The ID for a terminal type of a stream from/to an HDMI port.
    @constant       kAudioStreamTerminalTypeDisplayPort
                        The ID for a terminal type of a stream from/to an DisplayPort port.
*/
enum
{
    kAudioStreamTerminalTypeUnknown                 = 0,
    kAudioStreamTerminalTypeLine                    = 'line',
    kAudioStreamTerminalTypeDigitalAudioInterface   = 'spdf',
    kAudioStreamTerminalTypeSpeaker                 = 'spkr',
    kAudioStreamTerminalTypeHeadphones              = 'hdph',
    kAudioStreamTerminalTypeLFESpeaker              = 'lfes',
    kAudioStreamTerminalTypeReceiverSpeaker         = 'rspk',
    kAudioStreamTerminalTypeMicrophone              = 'micr',
    kAudioStreamTerminalTypeHeadsetMicrophone       = 'hmic',
    kAudioStreamTerminalTypeReceiverMicrophone      = 'rmic',
    kAudioStreamTerminalTypeTTY                     = 'tty_',
    kAudioStreamTerminalTypeHDMI                    = 'hdmi',
    kAudioStreamTerminalTypeDisplayPort             = 'dprt'
};

//==================================================================================================
#pragma mark AudioStream Properties

/*!
    @enum           AudioStream Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioStream class.
    @discussion     AudioStream is a subclass of AudioObject and has only the single scope,
                    kAudioObjectPropertyScopeGlobal. They have a master element and an element for
                    each channel in the stream numbered upward from 1.
    @constant       kAudioStreamPropertyIsActive
                        A UInt32 where a non-zero value indicates that the stream is enabled and
                        doing IO.
    @constant       kAudioStreamPropertyDirection
                        A UInt32 where a value of 0 means that this AudioStream is an output stream
                        and a value of 1 means that it is an input stream.
    @constant       kAudioStreamPropertyTerminalType
                        A UInt32 whose value describes the general kind of functionality attached
                        to the AudioStream.
    @constant       kAudioStreamPropertyStartingChannel
                        A UInt32 that specifies the first element in the owning device that
                        corresponds to element one of this stream.
    @constant       kAudioStreamPropertyLatency
                        A UInt32 containing the number of frames of latency in the AudioStream. Note
                        that the owning AudioDevice may have additional latency so it should be
                        queried as well. If both the device and the stream say they have latency,
                        then the total latency for the stream is the device latency summed with the
                        stream latency.
    @constant       kAudioStreamPropertyVirtualFormat
                        An AudioStreamBasicDescription that describes the current data format for
                        the AudioStream. The virtual format refers to the data format in which all
                        IOProcs for the owning AudioDevice will perform IO transactions.
    @constant       kAudioStreamPropertyAvailableVirtualFormats
                        An array of AudioStreamRangedDescriptions that describe the available data
                        formats for the AudioStream. The virtual format refers to the data format in
                        which all IOProcs for the owning AudioDevice will perform IO transactions.
    @constant       kAudioStreamPropertyPhysicalFormat
                        An AudioStreamBasicDescription that describes the current data format for
                        the AudioStream. The physical format refers to the data format in which the
                        hardware for the owning AudioDevice performs its IO transactions.
    @constant       kAudioStreamPropertyAvailablePhysicalFormats
                        An array of AudioStreamRangedDescriptions that describe the available data
                        formats for the AudioStream. The physical format refers to the data format
                        in which the hardware for the owning AudioDevice performs its IO
                        transactions.
*/
enum
{
    kAudioStreamPropertyIsActive                    = 'sact',
    kAudioStreamPropertyDirection                   = 'sdir',
    kAudioStreamPropertyTerminalType                = 'term',
    kAudioStreamPropertyStartingChannel             = 'schn',
    kAudioStreamPropertyLatency                     = kAudioDevicePropertyLatency,
    kAudioStreamPropertyVirtualFormat               = 'sfmt',
    kAudioStreamPropertyAvailableVirtualFormats     = 'sfma',
    kAudioStreamPropertyPhysicalFormat              = 'pft ',
    kAudioStreamPropertyAvailablePhysicalFormats    = 'pfta'
};
//==================================================================================================
#pragma mark -
#pragma mark AudioControl Constants

/*!
    @enum           AudioControl Class Constants
    @abstract       Various constants related to the AudioControl class.
    @constant       kAudioControlClassID
                        The AudioClassID that identifies the AudioControl class.
*/
enum
{
    kAudioControlClassID    = 'actl'
};

/*!
    @enum           AudioControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioControl class.
    @discussion     The AudioControl class is a subclass of the AudioObject class. The class has
                    just the global scope, kAudioObjectPropertyScopeGlobal, and only a master
                    element.
    @constant       kAudioControlPropertyScope
                        An AudioServerPlugIn_PropertyScope that indicates which part of a device the
                        control applies to.
    @constant       kAudioControlPropertyElement
                        An AudioServerPlugIn_PropertyElement that indicates which element of the
                        device the control applies to.
*/
enum
{
    kAudioControlPropertyScope      = 'cscp',
    kAudioControlPropertyElement    = 'celm'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioSliderControl Constants

/*!
    @enum           AudioSliderControl Class Constants
    @abstract       Various constants related to the AudioSliderControl class.
    @constant       kAudioSliderControlClassID
                        The AudioClassID that identifies the AudioSliderControl class.
*/
enum
{
    kAudioSliderControlClassID  = 'sldr'
};

/*!
    @enum           AudioSliderControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioSliderControl class.
    @discussion     The AudioSliderControl class is a subclass of the AudioControl class and has the
                    same scope and element structure.
    @constant       kAudioSliderControlPropertyValue
                        A UInt32 that represents the value of the slider control.
    @constant       kAudioSliderControlPropertyRange
                        An array of two UInt32s that represents the inclusive range of values the
                        slider control can take.
*/
enum
{
    kAudioSliderControlPropertyValue    = 'sdrv',
    kAudioSliderControlPropertyRange    = 'sdrr'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioLevelControl Constants

/*!
    @enum           AudioLevelControl Class Constants
    @abstract       Various constants related to the AudioLevelControl class.
    @constant       kAudioLevelControlClassID
                        The AudioClassID that identifies the LevelControl class.
    @constant       kAudioVolumeControlClassID
                        A subclass of the LevelControl class that implements a general
                        gain/attenuation stage.
    @constant       kAudioLFEVolumeControlClassID
                        A subclass of the LevelControl class for an LFE channel that results from
                        bass management. Note that LFE channels that are represented as normal audio
                        channels must use kAudioVolumeControlClassID to manipulate the level.
*/
enum
{
    kAudioLevelControlClassID       = 'levl',
    kAudioVolumeControlClassID      = 'vlme',
    kAudioLFEVolumeControlClassID   = 'subv'
};

/*!
    @enum           AudioLevelControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioLevelControl class.
    @discussion     The AudioLevelControl class is a subclass of the AudioControl class and has the
                    same scope and element structure.
    @constant       kAudioLevelControlPropertyScalarValue
                        A Float32 that represents the value of the volume control. The range is
                        between 0.0 and 1.0 (inclusive). Note that the set of all Float32 values
                        between 0.0 and 1.0 inclusive is much larger than the set of actual values
                        that the hardware can select. This means that the Float32 range has a many
                        to one mapping with the underlying hardware values. As such, setting a
                        scalar value will result in the control taking on the value nearest to what
                        was set.
    @constant       kAudioLevelControlPropertyDecibelValue
                        A Float32 that represents the value of the volume control in dB. Note that
                        the set of all Float32 values in the dB range for the control is much larger
                        than the set of actual values that the hardware can select. This means that
                        the Float32 range has a many to one mapping with the underlying hardware
                        values. As such, setting a dB value will result in the control taking on the
                        value nearest to what was set.
    @constant       kAudioLevelControlPropertyDecibelRange
                        An AudioValueRange that contains the minimum and maximum dB values the
                        control can have.
    @constant       kAudioLevelControlPropertyConvertScalarToDecibels
                        A Float32 that on input contains a scalar volume value for the and on exit
                        contains the equivalent dB value.
    @constant       kAudioLevelControlPropertyConvertDecibelsToScalar
                        A Float32 that on input contains a dB volume value for the and on exit
                        contains the equivalent scalar value.
*/
enum
{
    kAudioLevelControlPropertyScalarValue               = 'lcsv',
    kAudioLevelControlPropertyDecibelValue              = 'lcdv',
    kAudioLevelControlPropertyDecibelRange              = 'lcdr',
    kAudioLevelControlPropertyConvertScalarToDecibels   = 'lcsd',
    kAudioLevelControlPropertyConvertDecibelsToScalar   = 'lcds'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioBooleanControl Constants

/*!
    @enum           AudioBooleanControl Class Constants
    @abstract       Various constants related to the AudioBooleanControl class.
    @constant       kAudioBooleanControlClassID
                        The AudioClassID that identifies the BooleanControl class.
    @constant       kAudioMuteControlClassID
                        A subclass of the AudioBooleanControl class where a true value means that
                        mute is enabled making that element inaudible.
    @constant       kAudioSoloControlClassID
                        A subclass of the AudioBooleanControl class where a true value means that
                        solo is enabled making just that element audible and the other elements
                        inaudible.
    @constant       kAudioJackControlClassID
                        A subclass of the AudioBooleanControl class where a true value means
                        something is plugged into that element.
    @constant       kAudioLFEMuteControlClassID
                        A subclass of the AudioBooleanControl class where true means that mute is
                        enabled making that LFE element inaudible. This control is for LFE channels
                        that result from bass management. Note that LFE channels that are
                        represented as normal audio channels must use an AudioMuteControl.
    @constant       kAudioPhantomPowerControlClassID
                        A subclass of the AudioBooleanControl class where true means that the
                        element's hardware has phantom power enabled.
    @constant       kAudioPhaseInvertControlClassID
                        A subclass of the AudioBooleanControl class where true means that the phase
                        of the signal on the given element is being inverted by 180 degrees.
    @constant       kAudioClipLightControlClassID
                        A subclass of the AudioBooleanControl class where true means that the signal
                        for the element has exceeded the sample range. Once a clip light is turned
                        on, it is to stay on until either the value of the control is set to false
                        or the current IO session stops and a new IO session starts.
    @constant       kAudioTalkbackControlClassID
                        An AudioBooleanControl where true means that the talkback channel is
                        enabled. This control is for talkback channels that are handled outside of
                        the regular IO channels. If the talkback channel is among the normal IO
                        channels, it will use AudioMuteControl.
    @constant       kAudioListenbackControlClassID
                        An AudioBooleanControl where true means that the listenback channel is
                        audible. This control is for listenback channels that are handled outside of
                        the regular IO channels. If the listenback channel is among the normal IO
                        channels, it will use AudioMuteControl.
*/
enum
{
    kAudioBooleanControlClassID         = 'togl',
    kAudioMuteControlClassID            = 'mute',
    kAudioSoloControlClassID            = 'solo',
    kAudioJackControlClassID            = 'jack',
    kAudioLFEMuteControlClassID         = 'subm',
    kAudioPhantomPowerControlClassID    = 'phan',
    kAudioPhaseInvertControlClassID     = 'phsi',
    kAudioClipLightControlClassID       = 'clip',
    kAudioTalkbackControlClassID        = 'talb',
    kAudioListenbackControlClassID      = 'lsnb'
};

/*!
    @enum           AudioBooleanControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioBooleanControl class.
    @discussion     The AudioBooleanControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioBooleanControlPropertyValue
                        A UInt32 where 0 means off/false and non-zero means on/true.
*/
enum
{
    kAudioBooleanControlPropertyValue   = 'bcvl'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioSelectorControl Constants

/*!
    @enum           AudioSelectorControl Class Constants
    @abstract       Various constants related to the AudioSelectorControl class.
    @constant       kAudioSelectorControlClassID
                        The AudioClassID that identifies the AudioSelectorControl class.
    @constant       kAudioDataSourceControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the data
                        for the element is coming from.
    @constant       kAudioDataDestinationControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the data
                        for the element is going.
    @constant       kAudioClockSourceControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the
                        timing info for the object is coming from.
    @constant       kAudioLineLevelControlClassID
                        A subclass of the AudioSelectorControl class that identifies the nominal
                        line level for the element. Note that this is not a gain stage but rather
                        indicating the voltage standard (if any) used for the element, such as
                        +4dBu, -10dBV, instrument, etc.
    @constant       kAudioHighPassFilterControlClassID
                        A subclass of the AudioSelectorControl class that indicates the setting for
                        the high pass filter on the given element.
*/
enum
{
    kAudioSelectorControlClassID        = 'slct',
    kAudioDataSourceControlClassID      = 'dsrc',
    kAudioDataDestinationControlClassID = 'dest',
    kAudioClockSourceControlClassID     = 'clck',
    kAudioLineLevelControlClassID       = 'nlvl',
    kAudioHighPassFilterControlClassID  = 'hipf'
};

/*!
    @enum           AudioSelectorControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioSelectorControl class.
    @discussion     The AudioSelectorControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioSelectorControlPropertyCurrentItem
                        An array of UInt32s that are the IDs of the items currently selected.
    @constant       kAudioSelectorControlPropertyAvailableItems
                        An array of UInt32s that represent the IDs of all the items available.
    @constant       kAudioSelectorControlPropertyItemName
                        This property translates the given item ID into a human readable name. The
                        qualifier contains the ID of the item to be translated and name is returned
                        as a CFString as the property data. The caller is responsible for releasing
                        the returned CFObject.
    @constant       kAudioSelectorControlPropertyItemKind
                        This property returns a UInt32 that identifies the kind of selector item the
                        item ID refers to. The qualifier contains the ID of the item. Note that this
                        property is optional for selector controls and that the meaning of the value
                        depends on the specifc subclass being queried.
*/
enum
{
    kAudioSelectorControlPropertyCurrentItem    = 'scci',
    kAudioSelectorControlPropertyAvailableItems = 'scai',
    kAudioSelectorControlPropertyItemName       = 'scin',
    kAudioSelectorControlPropertyItemKind       = 'clkk'
};

/*!
    @enum           Constants for the value of the property, kAudioSelectorControlPropertyItemKind
                    for any selector control item
    @constant       kAudioSelectorControlItemKindSpacer
                        This ID represents an item in a selector control's range that represents a
                        spacer item in a pop-up menu. Items with this kind are not be selectable.
*/
enum
{
    kAudioSelectorControlItemKindSpacer = 'spcr'
};

/*!
    @enum           Constants for the value of the property, kAudioSelectorControlPropertyItemKind
                    for AudioClockSourceControls.
    @constant       kAudioClockSourceItemKindInternal
                        This ID represents the device's internal clock.
*/
enum
{
    kAudioClockSourceItemKindInternal   = 'int '
};

//==================================================================================================
#pragma mark -
#pragma mark AudioStereoPanControl Constants

/*!
    @enum           AudioStereoPanControl Class Constants
    @abstract       Various constants related to the AudioStereoPanControl class.
    @constant       kAudioStereoPanControlClassID
                        The AudioClassID that identifies the StereoPanControl class.
*/
enum
{
    kAudioStereoPanControlClassID   = 'span'
};

/*!
    @enum           AudioStereoPanControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioStereoPanControl class.
    @discussion     The AudioStereoPanControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioStereoPanControlPropertyValue
                        A Float32 where 0.0 is full left, 1.0 is full right, and 0.5 is center.
    @constant       kAudioStereoPanControlPropertyPanningChannels
                        An array of two UInt32s that indicate which elements of the device the
                        signal is being panned between.
*/
enum
{
    kAudioStereoPanControlPropertyValue             = 'spcv',
    kAudioStereoPanControlPropertyPanningChannels   = 'spcc'
};

//==================================================================================================

#if defined(__cplusplus)
}
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
#endif

#endif  //  __AudioHardwareBase_h__

/*==================================================================================================
     File:       CoreAudio/AudioHardwareDeprecated.h

     Copyright:  (c) 1985-2011 by Apple, Inc., all rights reserved.

     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:

                     http://developer.apple.com/bugreporter/

==================================================================================================*/
#if !defined(__AudioHardwareDeprecated_h__)
#define __AudioHardwareDeprecated_h__

//==================================================================================================
//  Includes

#include <Availability.h>
#include <CoreAudio/CoreAudioTypes.h>
#include <CoreFoundation/CoreFoundation.h>

//==================================================================================================

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint off
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================
#pragma mark -
#pragma mark Property Support Constants

/*!
    @enum           Property Address Constants
    @abstract       The valid values for the scope in a property address.
    @constant       kAudioDevicePropertyScopeInput
                        The AudioObjectPropertyScope for properties that apply to the input side of
                        an object.
    @constant       kAudioDevicePropertyScopeOutput
                        The AudioObjectPropertyScope for properties that apply to the output side of
                        an object.
    @constant       kAudioDevicePropertyScopePlayThrough
                        The AudioObjectPropertyScope for properties that apply to the play through
                        side of an object.
*/
enum
{
    kAudioDevicePropertyScopeInput          = kAudioObjectPropertyScopeInput,
    kAudioDevicePropertyScopeOutput         = kAudioObjectPropertyScopeOutput,
    kAudioDevicePropertyScopePlayThrough    = kAudioObjectPropertyScopePlayThrough
};

/*!
    @enum           Property Wildcard Constants
    @abstract       Constants that are used as wildcards in an AudioObjectPropertyAddress.
    @discussion     Wildcards match any and all values for there associated type. They are
                    especially useful for registering listener procs to receive notifications and
                    for querying an AudioObject's list of AudioControls.
    @constant       kAudioPropertyWildcardPropertyID
                        A synonym for kAudioObjectPropertySelectorWildcard.
    @constant       kAudioPropertyWildcardSection
                        The wildcard value for the isInput argument of AudioDeviceGetPropertyInfo(),
                        AudioDeviceGetProperty(), and AudioDeviceSetProperty().
    @constant       kAudioPropertyWildcardChannel
                        A synonym for kAudioObjectPropertyElementWildcard.
*/
enum
{
    kAudioPropertyWildcardPropertyID        = kAudioObjectPropertySelectorWildcard,
    kAudioPropertyWildcardSection           = 0xFF,
    kAudioPropertyWildcardChannel           = kAudioObjectPropertyElementWildcard
};

//==================================================================================================
#pragma mark -
#pragma mark AudioControl Constants

/*!
    @enum           AudioBooleanControl Subclass IDs
    @abstract       The four char codes that identify the various standard subclasses of
                    AudioBooleanControl.
    @constant       kAudioISubOwnerClassID
                        An AudioBooleanControl where true means that the AudioDevice that
                        ultimately owns the control also owns any iSub attached to the CPU.
*/
enum
{
    kAudioISubOwnerControlClassID       = 'atch'
};

//==================================================================================================
#pragma mark AudioControl Properties

/*!
    @enum           AudioLevelControl Properties
    @abstract       AudioObjectPropertySelector values that apply to all AudioLevelControls.
    @discussion     AudioLevelControl is a subclass of AudioControl and has only the single scope,
                    kAudioObjectPropertyScopeGlobal, and only a master element.
    @constant       kAudioLevelControlPropertyDecibelsToScalarTransferFunction
                        A UInt32 whose value indicates the transfer function the HAL uses to convert
                        between decibel values and scalar values.
*/
enum
{
    kAudioLevelControlPropertyDecibelsToScalarTransferFunction  = 'lctf'
};

/*!
    @enum           Values for kAudioLevelControlPropertyDecibelsToScalarTransferFunction
    @abstract       The following constants are the only supported values for a volume control's
                    transfer function.
    @discussion     The transfer function implemented in the volume control works by raising the
                    scalar value to an exponent to map it into the decibel range. The constants
                    in this enum express the exponent used in the name as a quotient. For example,
                    kAudioLevelControlTranferFunction3Over4 represents the exponent 0.75.
*/
enum
{
    kAudioLevelControlTranferFunctionLinear     = 0,
    kAudioLevelControlTranferFunction1Over3     = 1,
    kAudioLevelControlTranferFunction1Over2     = 2,
    kAudioLevelControlTranferFunction3Over4     = 3,
    kAudioLevelControlTranferFunction3Over2     = 4,
    kAudioLevelControlTranferFunction2Over1     = 5,
    kAudioLevelControlTranferFunction3Over1     = 6,
    kAudioLevelControlTranferFunction4Over1     = 7,
    kAudioLevelControlTranferFunction5Over1     = 8,
    kAudioLevelControlTranferFunction6Over1     = 9,
    kAudioLevelControlTranferFunction7Over1     = 10,
    kAudioLevelControlTranferFunction8Over1     = 11,
    kAudioLevelControlTranferFunction9Over1     = 12,
    kAudioLevelControlTranferFunction10Over1    = 13,
    kAudioLevelControlTranferFunction11Over1    = 14,
    kAudioLevelControlTranferFunction12Over1    = 15
};

//==================================================================================================
#pragma mark -
#pragma mark AudioSystemObject Types

/*!
    @typedef        AudioHardwarePropertyID
    @abstract       An AudioHardwarePropertyID is a integer that identifies a specific piece of
                    information about the AudioSystemObject.
*/
typedef AudioObjectPropertySelector AudioHardwarePropertyID;

/*!
    @typedef        AudioHardwarePropertyListenerProc
    @abstract       Clients register an AudioHardwarePropertyListenerProc with the AudioSystemObject
                    in order to receive notifications when the properties of the object change.
    @discussion     Note that the same functionality is provided by AudioObjectPropertyListenerProc.
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property that changed.
    @param          inClientData
                        A pointer to client data established when the listener proc was registered
                        with the AudioSystemObject.
    @result         The return value is currently unused and should always be 0.
*/
typedef OSStatus
(*AudioHardwarePropertyListenerProc)(   AudioHardwarePropertyID inPropertyID,
                                        void*                   inClientData);

//==================================================================================================
#pragma mark AudioSystemObject Constants

/*!
    @defined        kAudioHardwareRunLoopMode
    @discussion     The name of the run loop mode to which only HAL run loop sources and sources
                    added via AudioHardwareAddRunLoopSource() belong. This is the mode in which to
                    task a run loop in order to ensure that just HAL related events are handled.
*/
#define kAudioHardwareRunLoopMode   "com.apple.audio.CoreAudio"

//==================================================================================================
#pragma mark AudioSystemObject Properties

/*!
    @enum           AudioSystemObject Properties
    @abstract       AudioObjectPropertySelector values that apply to the AudioSystemObject.
    @discussion     The AudioSystemObject has one scope, kAudioObjectPropertyScopeGlobal, and only a
                    master element.
    @constant       kAudioHardwarePropertyRunLoop
                        The CFRunLoopRef the HAL is currently attaching all of its system
                        notification handlers to. In 10.6 and later, the HAL will use the process's
                        run loop (as defined by CFRunLoopGetMain()) for this task. Whereas in
                        previous releases, the HAL created and managed its own thread for the task.
                        Clients can set this property to tell the HAL to use a thread of the
                        client's choosing. If the value for this property is set to NULL, the HAL
                        will return to its pre-10.6 behavior of creating and managing its own
                        thread for notifications. The caller is responsible for releasing the
                        returned CFObject.
    @constant       kAudioHardwarePropertyDeviceForUID
                        Using an AudioValueTranslation structure, this property translates the input
                        CFStringRef containing a UID into the AudioDeviceID that refers to the
                        AudioDevice with that UID. This property will return kAudioDeviceUnknown if
                        the given UID does not match any currently available AudioDevice.
    @constant       kAudioHardwarePropertyPlugInForBundleID
                        Using an AudioValueTranslation structure, this property translates the input
                        CFString containing a bundle ID into the AudioObjectID of the AudioPlugIn
                        that corresponds to it. This property will return kAudioObjectUnkown if the
                        given bundle ID doesn't match any AudioPlugIns.
*/
enum
{
    kAudioHardwarePropertyRunLoop                           = 'rnlp',
    kAudioHardwarePropertyDeviceForUID                      = 'duid',
    kAudioHardwarePropertyPlugInForBundleID                 = 'pibi'
};

/*!
    @enum           AudioSystemObject Properties Implemented via AudioControl objects
    @abstract       AudioObjectPropertySelector values for AudioSystemObject properties that are
                    implemented by AudioControl objects.
    @discussion     These properties are also accessible by locating the AudioControl object
                    attached to the AudioSystemObject and using that object to access the properties
                    of the control.
    @constant       kAudioHardwarePropertyBootChimeVolumeScalar
                        A Float32 that represents the value of the boot chime volume control. The
                        range is between 0.0 and 1.0 (inclusive). This property is implemented by an
                        AudioControl object that is a subclass of AudioBootChimeVolumeControl.
    @constant       kAudioHardwarePropertyBootChimeVolumeDecibels
                        A Float32 that represents the value of the boot chime volume control in dB.
                        This property is implemented by an AudioControl object that is a subclass
                        of AudioBootChimeVolumeControl.
    @constant       kAudioHardwarePropertyBootChimeVolumeRangeDecibels
                        An AudioValueRange that contains the minimum and maximum dB values the
                        boot chime control can have. This property is implemented by an AudioControl
                        object that is a subclass of AudioBootChimeVolumeControl.
    @constant       kAudioHardwarePropertyBootChimeVolumeScalarToDecibels
                        A Float32 that on input contains a scalar volume value for the boot chime
                        and on exit contains the equivalent dB value. This property is implemented
                        by an AudioControl object that is a subclass of AudioBootChimeVolumeControl.
    @constant       kAudioHardwarePropertyBootChimeVolumeDecibelsToScalar
                        A Float32 that on input contains a dB volume value for the boot chime and on
                        exit contains the equivalent scalar value. This property is implemented by
                        an AudioControl object that is a subclass of AudioBootChimeVolumeControl.
    @constant       kAudioHardwarePropertyBootChimeVolumeDecibelsToScalarTransferFunction
                        A UInt32 whose value indicates the transfer function the HAL uses to convert
                        between decibel values and scalar values. This property is implemented by an
                        AudioControl object that is a subclass of AudioBootChimeVolumeControl.
*/
enum
{
    kAudioHardwarePropertyBootChimeVolumeScalar                             = 'bbvs',
    kAudioHardwarePropertyBootChimeVolumeDecibels                           = 'bbvd',
    kAudioHardwarePropertyBootChimeVolumeRangeDecibels                      = 'bbd#',
    kAudioHardwarePropertyBootChimeVolumeScalarToDecibels                   = 'bv2d',
    kAudioHardwarePropertyBootChimeVolumeDecibelsToScalar                   = 'bd2v',
    kAudioHardwarePropertyBootChimeVolumeDecibelsToScalarTransferFunction   = 'bvtf'
};

//==================================================================================================
#pragma mark AudioSystemObject Functions

/*!
    @functiongroup  AudioSystemObject
*/

/*!
    @function       AudioHardwareAddRunLoopSource
    @abstract       Add the given CFRunLoopSource to the the HAL's notification CFRunLoop.
    @discussion     The CFRunLoop the HAL uses for notifications is specified by
                    kAudioHardwarePropertyRunLoop. If kAudioHardwarePropertyRunLoop changes,
                    CFRunLoopSources added with this function will automatically be transferred to
                    the new CFRunLoop.
                    Usage of the HAL's notification run loop is deprecated. Please use libdispatch
                    instead.
    @param          inRunLoopSource
                        The CFRunLoopSource to add.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareAddRunLoopSource(CFRunLoopSourceRef inRunLoopSource)                                   __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_7, __IPHONE_2_0, __IPHONE_4_1);

/*!
    @function       AudioHardwareRemoveRunLoopSource
    @abstract       Remove the given CFRunLoopSource from the the HAL's notification CFRunLoop.
    @discussion     The CFRunLoop the HAL uses for notifications is specified by
                    kAudioHardwarePropertyRunLoop.
                    Usage of the HAL's notification run loop is deprecated. Please use libdispatch
                    instead.
    @param          inRunLoopSource
                        The CFRunLoopSource to remove.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareRemoveRunLoopSource(CFRunLoopSourceRef inRunLoopSource)                                __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_3, __MAC_10_7, __IPHONE_2_0, __IPHONE_4_1);

/*!
    @function       AudioHardwareGetPropertyInfo
    @abstract       Retrieve information about the given property.
    @discussion     Note that the same functionality is provided by the functions
                    AudioObjectHasProperty(), AudioObjectIsPropertySettable(), and
                    AudioObjectGetPropertyDataSize().
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property to query.
    @param          outSize
                        A pointer to a UInt32 that receives the size of the property data in bytes
                        on exit. This can be NULL if the size information is not being requested.
    @param          outWritable
                        A pointer to a Boolean that receives indication of whether or not the given
                        property can be set. This can be NULL if the writability is not being
                        requested.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareGetPropertyInfo(   AudioHardwarePropertyID inPropertyID,
                                UInt32*                 outSize,
                                Boolean*                outWritable)                                __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareGetProperty
    @abstract       Queries an the AudioSystemObject to get the data of the given property and
                    places it in the provided buffer.
    @discussion     Note that the same functionality is provided by the function
                    AudioObjectGetPropertyData().
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property to query.
    @param          ioDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outData
                        The buffer into which the AudioSystemObject will put the data for the given
                        property.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareGetProperty(   AudioHardwarePropertyID inPropertyID,
                            UInt32*                 ioPropertyDataSize,
                            void*                   outPropertyData)                                __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareSetProperty
    @abstract       Tells the AudioSystemObject to change the value of the given property using the
                    provided data.
    @discussion     Note that the value of the property should not be considered changed until the
                    HAL has called the listeners as many properties values are changed
                    asynchronously. Also note that the same functionality is provided by the
                    function AudioObjectGetPropertyData().
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property to change.
    @param          inDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inData
                        The buffer containing the data to be used to change the property's value.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareSetProperty(   AudioHardwarePropertyID inPropertyID,
                            UInt32                  inPropertyDataSize,
                            const void*             inPropertyData)                                 __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareAddPropertyListener
    @abstract       Registers the given AudioHardwarePropertyListenerProc to receive notifications
                    when the given property changes.
    @discussion     Note that the same functionality is provided by AudioObjectAddPropertyListener
                    in conjunction with AudioObjectPropertyListenerProc.
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property to listen to.
    @param          inProc
                        AudioHardwarePropertyListenerProc to call.
    @param          inClientData
                        A pointer to client data that is passed to the listener when it is called.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareAddPropertyListener(   AudioHardwarePropertyID             inPropertyID,
                                    AudioHardwarePropertyListenerProc   inProc,
                                    void*                               inClientData)               __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioHardwareRemovePropertyListener
    @abstract       Unregisters the given AudioHardwarePropertyListenerProc from receive
                    notifications when the given property changes.
    @discussion     Note that the same functionality is provided by
                    AudioObjectRemovePropertyListener in conjunction with
                    AudioObjectPropertyListenerProc.
    @param          inPropertyID
                        The AudioHardwarePropertyID of the property to stop listening to.
    @param          inProc
                        AudioHardwarePropertyListenerProc to unregister.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioHardwareRemovePropertyListener(    AudioHardwarePropertyID             inPropertyID,
                                        AudioHardwarePropertyListenerProc   inProc)                 __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

//==================================================================================================
#pragma mark -
#pragma mark AudioDevice Types

/*!
    @typedef        AudioDeviceID
    @abstract       AudioDevice is the base class for all objects that represent an audio device.
    @discussion     AudioDevice is a subclass of AudioObject. AudioDevices normally contain
                    AudioStreams and AudioControls, but may contain other things depending on the
                    kind of AudioDevice (e.g. aggregate devices contain other AudioDevices).
*/
typedef AudioObjectID   AudioDeviceID;

/*!
    @typedef        AudioDevicePropertyID
    @abstract       An AudioDevicePropertyID is an integer that identifies a specific piece of
                    information about the object.
*/
typedef AudioObjectPropertySelector AudioDevicePropertyID;

/*!
    @typedef        AudioDevicePropertyListenerProc
    @abstract       Clients register an AudioDevicePropertyListenerProc with the AudioDevice object
                    in order to receive notifications when the properties of the object change.
    @discussion     Note that the same functionality is provided by AudioObjectPropertyListenerProc.
    @param          inDevice
                        The AudioDevice whose property has changed.
    @param          inChannel
                        The channel of the property that changed where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice changed.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property that changed.
    @param          inClientData
                        A pointer to client data established when the listener proc was registered
                        with the object.
    @result         The return value is currently unused and should always be 0.
*/
typedef OSStatus
(*AudioDevicePropertyListenerProc)( AudioDeviceID           inDevice,
                                    UInt32                  inChannel,
                                    Boolean                 isInput,
                                    AudioDevicePropertyID   inPropertyID,
                                    void*                   inClientData);

//==================================================================================================
#pragma mark AudioDevice Constants

/*!
    @enum           AudioDevice Class Constants
    @abstract       Various constants related to AudioDevices.
    @constant       kAudioDeviceUnknown
                        The AudioObjectID for a nonexistent AudioObject.
*/
enum
{
    kAudioDeviceUnknown                     = kAudioObjectUnknown
};

/*!
    @enum           Transport Type IDs
    @abstract       Commonly used values for kAudioDevicePropertyTransportType and
                    kAudioTransportManagerPropertyTransportType
    @constant       kAudioDeviceTransportTypeAutoAggregate
                        The transport type ID for automatically generated aggregate devices.
*/
enum
{
    kAudioDeviceTransportTypeAutoAggregate  = 'fgrp',
};

//==================================================================================================
#pragma mark AudioDevice Properties

/*!
    @enum           AudioDevice Properties Implemented via AudioControl objects
    @abstract       AudioObjectPropertySelector values for AudioDevice properties that are
                    implemented by AudioControl objects.
    @discussion     These properties are also accessible by locating the AudioControl object
                    attached to the AudioDevice and using that object to access the properties of
                    the control.
    @constant       kAudioDevicePropertyVolumeDecibelsToScalarTransferFunction
                        A UInt32 whose value indicates the transfer function the HAL uses to convert
                        between decibel values and scalar values. This property is implemented by an
                        AudioControl object that is a subclass of AudioVolumeControl.
    @constant       kAudioDevicePropertyPlayThruVolumeDecibelsToScalarTransferFunction
                        A UInt32 whose value indicates the transfer function the HAL uses to convert
                        between decibel values and scalar values. This property is implemented by an
                        AudioControl object that is a subclass of AudioVolumeControl. Further, the
                        control that implements this property is only available through
                        kAudioDevicePropertyScopePlayThrough.
    @constant       kAudioDevicePropertyDriverShouldOwniSub
                        A UInt32 where a value of 0 means that the AudioDevice should not claim
                        ownership of any attached iSub and a value of 1 means that it should. Note
                        that this property is only available for built-in devices and for USB Audio
                        devices that use the standard class compliant driver. This property is
                        implemented by an AudioControl object that is a subclass of
                        AudioISubOwnerControl.
    @constant       kAudioDevicePropertySubVolumeDecibelsToScalarTransferFunction
                        A UInt32 whose value indicates the transfer function the HAL uses to convert
                        between decibel values and scalar values. This property is implemented by an
                        AudioControl object that is a subclass of AudioLFEVolumeControl.
*/
enum
{
    kAudioDevicePropertyVolumeDecibelsToScalarTransferFunction          = 'vctf',
    kAudioDevicePropertyPlayThruVolumeDecibelsToScalarTransferFunction  = 'mvtf',
    kAudioDevicePropertyDriverShouldOwniSub                             = 'isub',
    kAudioDevicePropertySubVolumeDecibelsToScalarTransferFunction       = 'svtf'
};

/*!
    @enum           AudioDevice Properties That Ought To Some Day Be Deprecated
    @abstract       AudioObjectPropertySelector values whose functionality is better provided by
                    other selectors.
    @discussion     These selectors are still provided for backward compatibility. The description
                    of the property will indicate in parentheses the better selectors to use and
                    why.
    @constant       kAudioDevicePropertyDeviceName
                        A C-string that contains the human readable name of the AudioDevice.
                        (kAudioObjectPropertyName: CFStrings are better for localization.)
    @constant       kAudioDevicePropertyDeviceNameCFString
                        A CFStringRef that contains the human readable name of the AudioDevice. The
                        caller is responsible for releasing the returned CFObject.
                        (kAudioObjectPropertyName: This is just another name for the inherited
                        selector.)
    @constant       kAudioDevicePropertyDeviceManufacturer
                        A C-string that contains the human readable name of the manufacturer of the
                        AudioDevice.
                        (kAudioObjectPropertyManufacturer: CFStrings are better for localization.)
    @constant       kAudioDevicePropertyDeviceManufacturerCFString
                        A CFString that contains the human readable name of the manufacturer of the
                        AudioDevice. The caller is responsible for releasing the returned CFObject.
                        (kAudioObjectPropertyManufacturer: This is just another name for the
                        inherited selector.)
    @constant       kAudioDevicePropertyRegisterBufferList
                        This property allows clients to register a fully populated AudioBufferList
                        that matches the topology described by
                        kAudioDevicePropertyStreamConfiguration for doing input using
                        AudioDeviceRead(). The AudioBufferList will be registered with the call the
                        AudioDeviceSetProperty() and will be unregistered with the call to
                        AudioDeviceGetProperty(). If this property isn't implemented by the
                        AudioDevice, it implies that the AudioDevice also doesn't support
                        AudioDeviceRead().
                        (Aggregate devices make AudioDeviceRead() obsolete for the most part.)
    @constant       kAudioDevicePropertyBufferSize
                        A UInt32 containing the size in bytes of the IO buffer for the AudioStream
                        containing the element.
                        (kAudioDevicePropertyBufferFrameSize: with multiple AudioStreams and the
                        requirement that all streams' buffers represent the same amount of time, it
                        doesn't make sense to set the buffer size in bytes since it will be
                        different for each stream.)
    @constant       kAudioDevicePropertyBufferSizeRange
                        An AudioValueRange specifying the minimum and maximum bytes size for the
                        IO buffer for the AudioStream containing the given element.
                        (kAudioDevicePropertyBufferFrameSizeRange: see
                        kAudioDevicePropertyBufferSize.)
    @constant       kAudioDevicePropertyChannelName
                        A C-string that contains a human readable name for the given element in the
                        given scope. The caller is responsible for releasing the returned CFObject.
                        (kAudioObjectPropertyElementName: CFStrings are better for
                        localization.)
    @constant       kAudioDevicePropertyChannelNameCFString
                        A CFString that contains a human readable name for the given element in the
                        given scope. The caller is responsible for releasing the returned CFObject.
                        (kAudioObjectPropertyElementName: This is just another name for the
                        inherited selector.)
    @constant       kAudioDevicePropertyChannelCategoryName
                        A C-string that contains a human readable name for the category of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
                        (kAudioObjectPropertyElementCategoryName: CFStrings are better for
                        localization.)
    @constant       kAudioDevicePropertyChannelCategoryNameCFString
                        A CFString that contains a human readable name for the category of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
                        (kAudioObjectPropertyElementCategoryName: This is just another name for the
                        inherited selector.)
    @constant       kAudioDevicePropertyChannelNumberName
                        A C-string that contains a human readable name for the number of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
                        (kAudioObjectPropertyElementNumberName: CFStrings are better for
                        localization.)
    @constant       kAudioDevicePropertyChannelNumberNameCFString
                        A CFString that contains a human readable name for the number of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
                        (kAudioObjectPropertyElementNumberName: This is just another name for the
                        inherited selector.)
    @constant       kAudioDevicePropertySupportsMixing
                        A UInt32 where a value of 1 means the AudioDevice supports mixing and a
                        value of 0 means that it doesn't and that all IO is performed in each
                        AudioStream's current physical format. This property is changed indirectly
                        by changing to a format that doesn't support mixing, such as AC-3. (The HAL
                        now vends its format information with a flag indicating the mixability in
                        order to better support devices with streams that are both mixable and non-
                        mixable.)
    @constant       kAudioDevicePropertyStreamFormat
                        An AudioStreamBasicDescription that describes the current data format for
                        the AudioStream that contains the channel referred to by the element number.
                        (kAudioStreamPropertyVirtualFormat: Managing format information is
                        inherently an operation on AudioStreams, rather than AudioDevices. It is
                        confusing for the client to work with formats at the AudioDevice level and
                        has been shown to lead to programming mistakes by clients when working with
                        devices that have multiple streams.)
    @constant       kAudioDevicePropertyStreamFormats
                        An array of AudioStreamBasicDescriptions that describe the available data
                        formats for the AudioStream that contains the channel referred to by the
                        element number.
                        (kAudioStreamPropertyAvailableVirtualFormats: Managing format information is
                        inherently an operation on AudioStreams, rather than AudioDevices. It is
                        confusing for the client to work with formats at the AudioDevice level and
                        has been shown to lead to programming mistakes by clients when working with
                        devices that have multiple streams.)
    @constant       kAudioDevicePropertyStreamFormatSupported
                        An AudioStreamBasicDescription is passed in to query whether or not the
                        format is supported. A kAudioDeviceUnsupportedFormatError will be returned
                        if the format is not supported and kAudioHardwareNoError will be returned if
                        it is supported. AudioStreamBasicDescription fields set to 0 will be ignored
                        in the query, but otherwise values must match exactly.
                        (kAudioStreamPropertyAvailableVirtualFormats: The proper and most robust way
                        to find a format that the AudioStream can support is to get the list of
                        available formats and look through that rather than using this property.)
    @constant       kAudioDevicePropertyStreamFormatMatch
                        An AudioStreamBasicDescription is passed in and the AudioStream will modify
                        it to describe the best match, in the AudioDevice's opinion, for the given
                        format.
                        (kAudioStreamPropertyAvailableVirtualFormats: The proper and most robust way
                        to find a format that the AudioStream can support is to get the list of
                        available formats and look through that rather than using this property.)
    @constant       kAudioDevicePropertyDataSourceNameForID
                        This property translates the given data source item ID into a human readable
                        name using an AudioValueTranslation structure. The input data is the UInt32
                        holding the item ID to be translated and the output data is a buffer to hold
                        the name as a null terminated C-string.
                        (kAudioDevicePropertyDataSourceNameForIDCFString: CFStrings are better for
                        localization.)
    @constant       kAudioDevicePropertyClockSourceNameForID
                        This property translates the given clock source item ID into a human
                        readable name using an AudioValueTranslation structure. The input data is
                        the UInt32 holding the item ID to be translated and the output data is a
                        buffer to hold the name as a null terminated C-string.
                        (kAudioDevicePropertyClockSourceNameForIDCFString: CFStrings are better for
                        localization.)
    @constant       kAudioDevicePropertyPlayThruDestinationNameForID
                        This property translates the given play through destination item ID into a
                        human readable name using an AudioValueTranslation structure. The input data
                        is the UInt32 holding the item ID to be translated and the output data is a
                        buffer to hold the name as a null terminated C-string.
                        (kAudioDevicePropertyPlayThruDestinationNameForIDCFString: CFStrings are
                        better for localization.)
    @constant       kAudioDevicePropertyChannelNominalLineLevelNameForID
                        This property translates the given nominal line level item ID into a human
                        readable name using an AudioValueTranslation structure. The input data is
                        the UInt32 holding the item ID to be translated and the output data is a
                        buffer to hold the name as a null terminated C-string.
                        (kAudioDevicePropertyChannelNominalLineLevelNameForIDCFString: CFStrings are
                        better for localization.)
    @constant       kAudioDevicePropertyHighPassFilterSettingNameForID
                        This property translates the given high pass filter setting item ID into a
                        human readable name using an AudioValueTranslation structure. The input data
                        is the UInt32 holding the item ID to be translated and the output data is a
                        buffer to hold the name as a null terminated C-string.
                        (kAudioDevicePropertyHighPassFilterSettingNameForIDCFString: CFStrings are
                        better for localization.)
*/
enum
{
    kAudioDevicePropertyDeviceName                          = 'name',
    kAudioDevicePropertyDeviceNameCFString                  = kAudioObjectPropertyName,
    kAudioDevicePropertyDeviceManufacturer                  = 'makr',
    kAudioDevicePropertyDeviceManufacturerCFString          = kAudioObjectPropertyManufacturer,
    kAudioDevicePropertyRegisterBufferList                  = 'rbuf',
    kAudioDevicePropertyBufferSize                          = 'bsiz',
    kAudioDevicePropertyBufferSizeRange                     = 'bsz#',
    kAudioDevicePropertyChannelName                         = 'chnm',
    kAudioDevicePropertyChannelNameCFString                 = kAudioObjectPropertyElementName,
    kAudioDevicePropertyChannelCategoryName                 = 'ccnm',
    kAudioDevicePropertyChannelCategoryNameCFString         = kAudioObjectPropertyElementCategoryName,
    kAudioDevicePropertyChannelNumberName                   = 'cnnm',
    kAudioDevicePropertyChannelNumberNameCFString           = kAudioObjectPropertyElementNumberName,
    kAudioDevicePropertySupportsMixing                      = 'mix?',
    kAudioDevicePropertyStreamFormat                        = 'sfmt',
    kAudioDevicePropertyStreamFormats                       = 'sfm#',
    kAudioDevicePropertyStreamFormatSupported               = 'sfm?',
    kAudioDevicePropertyStreamFormatMatch                   = 'sfmm',
    kAudioDevicePropertyDataSourceNameForID                 = 'sscn',
    kAudioDevicePropertyClockSourceNameForID                = 'cscn',
    kAudioDevicePropertyPlayThruDestinationNameForID        = 'mddn',
    kAudioDevicePropertyChannelNominalLineLevelNameForID    = 'cnlv',
    kAudioDevicePropertyHighPassFilterSettingNameForID      = 'chip'
};

//==================================================================================================
#pragma mark AudioDevice Functions

/*!
    @functiongroup  AudioDevice
*/

/*!
    @function       AudioDeviceAddIOProc
    @abstract       Registers the given AudioDeviceIOProc with the AudioDevice.
    @discussion     A client may have multiple IOProcs for a given device, but the device is free to
                    only accept as many as it can handle. Note that it is not recommended for
                    clients to have more than a single IOProc registered at a time as this can be
                    wasteful of system resources. Rather, it is recommended that the client do any
                    necessary mixing itself so that only one IOProc is necessary.
                    This routine has been deprecated in favor of AudioDeviceCreateIOProcID().
    @param          inDevice
                        The AudioDevice to register the IOProc with.
    @param          inProc
                        The AudioDeviceIOProc to register.
    @param          inClientData
                        A pointer to client data that is passed back to the IOProc when it is
                        called.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceAddIOProc(   AudioDeviceID       inDevice,
                        AudioDeviceIOProc   inProc,
                        void*               inClientData)                                           __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceRemoveIOProc
    @abstract       Unregisters the given AudioDeviceIOProc from the AudioDevice.
                    This routine has been deprecated in favor of AudioDeviceDestroyIOProcID().
    @param          inDevice
                        The AudioDevice to unregister the IOProc from.
    @param          inProc
                        The AudioDeviceIOProc to unregister.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceRemoveIOProc(    AudioDeviceID       inDevice,
                            AudioDeviceIOProc   inProc)                                             __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceRead
    @abstract       Read some data from an AudioDevice starting at the given time.
    @discussion     With the advent of aggregate devices, the need for AudioDeviceRead has gone
                    away. Consequently, this function is now deprecated.
    @param          inDevice
                        The AudioDevice to read from.
    @param          inStartTime
                        An AudioTimeStamp indicating the time from which to read the data. In
                        general, the valid range of time (in frames) is from the current time minus
                        the maximum IO buffer size to the current time minus the safety offset.
    @param          outData
                        An AudioBufferList that must be the same size and shape as that returned by
                        kAudioDevicePropertyStreamConfiguration. Further, the AudioBufferList must
                        have been previously registered with the device via
                        kAudioDevicePropertyRegisterBufferList. On exit, the mDataSize fields will
                        be updated with the amount of data read.
    @result         An OSStatus indicating success or failure.
                    kAudioHardwareUnsupportedOperationError will be returned if the AudioDevice does
                    not support direct reading.
*/
extern OSStatus
AudioDeviceRead(    AudioDeviceID           inDevice,
                    const AudioTimeStamp*   inStartTime,
                    AudioBufferList*        outData)                                                __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_5, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceGetPropertyInfo
    @abstract       Retrieve information about the given property of an AudioDevice.
    @discussion     Note that the same functionality is provided by the functions
                    AudioObjectHasProperty(), AudioObjectIsPropertySettable(), and
                    AudioObjectGetPropertyDataSize().
    @param          inDevice
                        The AudioDevice to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to query.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          outSize
                        A pointer to a UInt32 that receives the size of the property data in bytes
                        on exit. This can be NULL if the size information is not being requested.
    @param          outWritable
                        A pointer to a Boolean that receives indication of whether or not the given
                        property can be set. This can be NULL if the writability is not being
                        requested.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceGetPropertyInfo( AudioDeviceID           inDevice,
                            UInt32                  inChannel,
                            Boolean                 isInput,
                            AudioDevicePropertyID   inPropertyID,
                            UInt32*                 outSize,
                            Boolean*                outWritable)                                    __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceGetProperty
    @abstract       Queries an the AudioDevice object to get the data of the given property and
                    places it in the provided buffer.
    @discussion     Note that the same functionality is provided by the function
                    AudioObjectGetPropertyData().
    @param          inDevice
                        The AudioDevice to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to query.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          ioPropertyDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outPropertyData
                        The buffer into which the object will put the data for the given property.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceGetProperty( AudioDeviceID           inDevice,
                        UInt32                  inChannel,
                        Boolean                 isInput,
                        AudioDevicePropertyID   inPropertyID,
                        UInt32*                 ioPropertyDataSize,
                        void*                   outPropertyData)                                    __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceSetProperty
    @abstract       Tells the AudioDevice object to change the value of the given property using the
                    provided data.
    @discussion     Note that the value of the property should not be considered changed until the
                    HAL has called the listeners as many properties values are changed
                    asynchronously. Also note that the same functionality is provided by the
                    function AudioObjectSetPropertyData().
    @param          inDevice
                        The AudioDevice to change.
    @param          inWhen
                        A pointer to an AudioTimeStamp that says when to change the property's value
                        relative to the device's time base. NULL means execute the change
                        immediately.
    @param          inChannel
                        The channel of the property to change where 0 is the master channel.
    @param          isInput
                        Which section of the AudioDevice to change.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to change.
    @param          inPropertyDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inPropertyData
                        The buffer containing the data to be used to change the property's value.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceSetProperty( AudioDeviceID           inDevice,
                        const AudioTimeStamp*   inWhen,
                        UInt32                  inChannel,
                        Boolean                 isInput,
                        AudioDevicePropertyID   inPropertyID,
                        UInt32                  inPropertyDataSize,
                        const void*             inPropertyData)                                     __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceAddPropertyListener
    @abstract       Registers the given AudioDevicePropertyListenerProc to receive notifications
                    when the given property changes.
    @discussion     Note that the same functionality is provided by AudioObjectAddPropertyListener
                    in conjunction with AudioObjectPropertyListenerProc.
    @param          inDevice
                        The AudioDevice with whom to register the listener.
    @param          inChannel
                        The channel of the property to listen to.
    @param          isInput
                        Which section of the AudioDevice to listen to.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to listen to.
    @param          inProc
                        AudioDevicePropertyListenerProc to call.
    @param          inClientData
                        A pointer to client data that is passed to the listener when it is called.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceAddPropertyListener( AudioDeviceID                   inDevice,
                                UInt32                          inChannel,
                                Boolean                         isInput,
                                AudioDevicePropertyID           inPropertyID,
                                AudioDevicePropertyListenerProc inProc,
                                void*                           inClientData)                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioDeviceRemovePropertyListener
    @abstract       Unregisters the given AudioDevicePropertyListenerProc from receiving
                    notifications when the given property changes.
    @discussion     Note that the same functionality is provided by
                    AudioObjectRemovePropertyListener in conjunction with
                    AudioObjectPropertyListenerProc.
    @param          inDevice
                        The AudioDevice with whom to unregister the listener.
    @param          inChannel
                        The channel of the property to unregister from.
    @param          isInput
                        Which section of the AudioDevice to unregister from.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to stop listening to.
    @param          inProc
                        AudioDevicePropertyListenerProc to unregister.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioDeviceRemovePropertyListener(  AudioDeviceID                   inDevice,
                                    UInt32                          inChannel,
                                    Boolean                         isInput,
                                    AudioDevicePropertyID           inPropertyID,
                                    AudioDevicePropertyListenerProc inProc)                         __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_0, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

//==================================================================================================
#pragma mark -
#pragma mark AudioStream Types

/*!
    @typedef        AudioStreamID
    @abstract       AudioStream is the base class for all objects that represent a stream of data on
                    an audio device.
    @discussion     AudioStream is a subclass of AudioObject and can contain AudioControls.
*/
typedef AudioObjectID   AudioStreamID;

/*!
    @typedef        AudioStreamPropertyListenerProc
    @abstract       Clients register an AudioStreamPropertyListenerProc with the AudioStream object
                    in order to receive notifications when the properties of the object change.
    @discussion     Note that the same functionality is provided by AudioObjectPropertyListenerProc.
    @param          inStream
                        The AudioStream whose property has changed.
    @param          inChannel
                        The channel of the property that changed where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property that changed.
    @param          inClientData
                        A pointer to client data established when the listener proc was registered
                        with the object.
    @result         The return value is currently unused and should always be 0.
*/
typedef OSStatus
(*AudioStreamPropertyListenerProc)( AudioStreamID           inStream,
                                    UInt32                  inChannel,
                                    AudioDevicePropertyID   inPropertyID,
                                    void*                   inClientData);

//==================================================================================================
#pragma mark AudioStream Constants

/*!
    @enum           AudioStream Class Constants
    @abstract       Various constants related to AudioStreams.
    @constant       kAudioStreamUnknown
                        The AudioObjectID for a nonexistent AudioObject.
*/
enum
{
    kAudioStreamUnknown = kAudioObjectUnknown
};

//==================================================================================================
#pragma mark AudioStream Properties

/*!
    @enum           AudioStream Properties That Ought To Some Day Be Deprecated
    @abstract       AudioObjectPropertySelector values whose functionality is better provided by
                    other selectors.
    @discussion     These selectors are still provided for backward compatibility. The description
                    of the property will indicate in parentheses the better selectors to use and
                    why.
    @constant       kAudioStreamPropertyOwningDevice
                        The AudioObjectID of the AudioDevice of which this AudioStream is a part.
                        (kAudioObjectPropertyOwner: This is just another name for the inherited
                        selector.)
    @constant       kAudioStreamPropertyPhysicalFormats
                        An array of AudioStreamBasicDescriptions that describe the available data
                        formats for the AudioStream. The physical format refers to the data format
                        in which the hardware for the owning AudioDevice performs its IO
                        transactions.
                        (kAudioStreamPropertyAvailablePhysicalFormats: The new name for this
                        property is much clearer for readers of the API to see what is meant and the
                        AudioStreamRangedDescription structure provides better information.)
    @constant       kAudioStreamPropertyPhysicalFormatSupported
                        An AudioStreamBasicDescription is passed in to query whether or not the
                        format is supported. A kAudioDeviceUnsupportedFormatError will be returned
                        if the format is not supported and kAudioHardwareNoError will be returned if
                        it is supported. AudioStreamBasicDescription fields set to 0 will be ignored
                        in the query, but otherwise values must match exactly. The physical format
                        refers to the data format in which the hardware for the owning AudioDevice
                        performs its IO transactions.
                        (kAudioStreamPropertyAvailablePhysicalFormats: The proper and most robust
                        way to find a format that the AudioStream can support is to get the list of
                        available formats and look through that rather than using this property.)
    @constant       kAudioStreamPropertyPhysicalFormatMatch
                        An AudioStreamBasicDescription is passed in and the AudioStream will modify
                        it to describe the best match, in the AudioDevice's opinion, for the given
                        format. The physical format refers to the data format in which the hardware
                        for the owning AudioDevice performs its IO transactions.
                        (kAudioStreamPropertyAvailablePhysicalFormats: The proper and most robust
                        way to find a format that the AudioStream can support is to get the list of
                        available formats and look through that rather than using this property.)
*/
enum
{
    kAudioStreamPropertyOwningDevice                = kAudioObjectPropertyOwner,
    kAudioStreamPropertyPhysicalFormats             = 'pft#',
    kAudioStreamPropertyPhysicalFormatSupported     = 'pft?',
    kAudioStreamPropertyPhysicalFormatMatch         = 'pftm'
};

//==================================================================================================
#pragma mark AudioStream Functions

/*!
    @functiongroup  AudioStream
*/

/*!
    @function       AudioStreamGetPropertyInfo
    @abstract       Retrieve information about the given property of an AudioStream.
    @param          inStream
                        The AudioStream to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          outSize
                        A pointer to a UInt32 that receives the size of the property data in bytes
                        on exit. This can be NULL if the size information is not being requested.
    @param          outWritable
                        A pointer to a Boolean that receives indication of whether or not the given
                        property can be set. This can be NULL if the writability is not being
                        requested.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioStreamGetPropertyInfo( AudioStreamID           inStream,
                            UInt32                  inChannel,
                            AudioDevicePropertyID   inPropertyID,
                            UInt32*                 outSize,
                            Boolean*                outWritable)                                    __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioStreamGetProperty
    @abstract       Queries an the AudioStream object to get the data of the given property and
                    places it in the provided buffer.
    @discussion     Note that the same functionality is provided by the function
                    AudioObjectGetPropertyData().
    @param          inStream
                        The AudioStream to query.
    @param          inChannel
                        The channel of the property to query where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to query.
    @param          ioPropertyDataSize
                        A UInt32 which on entry indicates the size of the buffer pointed to by
                        outData and on exit indicates how much of the buffer was used.
    @param          outPropertyData
                        The buffer into which the object will put the data for the given property.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioStreamGetProperty( AudioStreamID           inStream,
                        UInt32                  inChannel,
                        AudioDevicePropertyID   inPropertyID,
                        UInt32*                 ioPropertyDataSize,
                        void*                   outPropertyData)                                    __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioStreamSetProperty
    @abstract       Tells the AudioStream object to change the value of the given property using the
                    provided data.
    @discussion     Note that the value of the property should not be considered changed until the
                    HAL has called the listeners as many properties values are changed
                    asynchronously. Also note that the same functionality is provided by the
                    function AudioObjectSetPropertyData().
    @param          inStream
                        The AudioStream to change.
    @param          inWhen
                        A pointer to an AudioTimeStamp that says when to change the property's value
                        relative to the device's time base. NULL means execute the change
                        immediately.
    @param          inChannel
                        The channel of the property to change where 0 is the master channel.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to change.
    @param          inPropertyDataSize
                        A UInt32 indicating the size of the buffer pointed to by inData.
    @param          inPropertyData
                        The buffer containing the data to be used to change the property's value.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioStreamSetProperty( AudioStreamID           inStream,
                        const AudioTimeStamp*   inWhen,
                        UInt32                  inChannel,
                        AudioDevicePropertyID   inPropertyID,
                        UInt32                  inPropertyDataSize,
                        const void*             inPropertyData)                                     __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioStreamAddPropertyListener
    @abstract       Registers the given AudioStreamPropertyListenerProc to receive notifications
                    when the given property changes.
    @discussion     Note that the same functionality is provided by AudioObjectAddPropertyListener
                    in conjunction with AudioObjectPropertyListenerProc.
    @param          inStream
                        The AudioStream with whom to register the listener.
    @param          inChannel
                        The channel of the property to listen to.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to listen to.
    @param          inProc
                        AudioStreamPropertyListenerProc to call.
    @param          inClientData
                        A pointer to client data that is passed to the listener when it is called.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioStreamAddPropertyListener( AudioStreamID                   inStream,
                                UInt32                          inChannel,
                                AudioDevicePropertyID           inPropertyID,
                                AudioStreamPropertyListenerProc inProc,
                                void*                           inClientData)                       __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);

/*!
    @function       AudioStreamRemovePropertyListener
    @abstract       Unregisters the given AudioStreamPropertyListenerProc from receiving
                    notifications when the given property changes.
    @discussion     Note that the same functionality is provided by
                    AudioObjectRemovePropertyListener in conjunction with
                    AudioObjectPropertyListenerProc.
    @param          inStream
                        The AudioStream with whom to unregister the listener.
    @param          inChannel
                        The channel of the property to unregister from.
    @param          inPropertyID
                        The AudioDevicePropertyID of the property to stop listening to.
    @param          inProc
                        AudioStreamPropertyListenerProc to unregister.
    @result         An OSStatus indicating success or failure.
*/
extern OSStatus
AudioStreamRemovePropertyListener(  AudioStreamID                   inStream,
                                    UInt32                          inChannel,
                                    AudioDevicePropertyID           inPropertyID,
                                    AudioStreamPropertyListenerProc inProc)                         __OSX_AVAILABLE_BUT_DEPRECATED(__MAC_10_1, __MAC_10_6, __IPHONE_2_0, __IPHONE_2_0);


//==================================================================================================
#pragma mark -
#pragma mark AudioControl Constants

/*!
    @enum           AudioControl Base Class IDs
    @abstract       The AudioClassIDs that identify the various AudioControl base classes.
    @constant       kAudioBootChimeVolumeControlClassID
                        An AudioLevelControl for the boot chime of the CPU.
*/
enum
{
    kAudioBootChimeVolumeControlClassID = 'pram'
};

//==================================================================================================
#pragma mark AudioControl Properties

/*!
    @enum           AudioControl Properties
    @abstract       AudioObjectPropertySelector values that apply to all AudioControls.
    @discussion     AudioControl is a subclass of AudioObject and has only the single scope,
                    kAudioObjectPropertyScopeGlobal, and only a master element.
    @constant       kAudioControlPropertyVariant
                        A UInt32 that identifies the specific variant of an AudioControl. This
                        allows the owning AudioObject to support controls that are of the same basic
                        class (that is, the values of  kAudioObjectPropertyClass are the same) but
                        may control a part of the object for which the standard controls do not
                        control.
*/
enum
{
    kAudioControlPropertyVariant    = 'cvar'
};

/*!
    @enum           AudioClockSourceControl Properties
    @abstract       AudioObjectPropertySelector values that apply only to AudioClockSourceControls.
    @discussion     These properties supplement the regular AudioSelectorControl Properties.
    @constant       kAudioClockSourceControlPropertyItemKind
                        This property returns a UInt32 that identifies the kind of clock source
                        the item ID refers to. The qualifier contains the ID of the item. Note that
                        this property is a synonym for kAudioSelectorControlPropertyItemKind.
*/
enum
{
    kAudioClockSourceControlPropertyItemKind     = kAudioSelectorControlPropertyItemKind
};

//==================================================================================================

#if defined(__cplusplus)
}
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
#endif

#endif  //  __AudioHardwareDeprecated_h__

/*==================================================================================================
     File:       CoreAudio/AudioHardwareBase.h

     Copyright:  (c) 1985-2011 by Apple, Inc., all rights reserved.

     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:

                     http://developer.apple.com/bugreporter/

==================================================================================================*/
#if !defined(__AudioHardwareBase_h__)
#define __AudioHardwareBase_h__

//==================================================================================================
#pragma mark -
#pragma mark Overview
/*!
    @header AudioHardwareBase

    This file defines the HAL's object model including the properties and their needed types and
    constants that describe the proeprty values.
*/

//==================================================================================================
//  Includes

#include <CoreAudio/CoreAudioTypes.h>

//==================================================================================================

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint off
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

//==================================================================================================
#pragma mark -
#pragma mark Basic Types

/*!
    @typedef        AudioObjectID
    @abstract       A UInt32 that provides a handle on a specific AudioObject.
*/
typedef UInt32  AudioObjectID;

/*!
    @typedef        AudioClassID
    @abstract       AudioClassIDs are used to identify the class of an AudioObject.
*/
typedef UInt32  AudioClassID;

/*!
    @typedef        AudioObjectPropertySelector
    @abstract       An AudioObjectPropertySelector is a four char code that identifies, along with
                    the AudioObjectPropertyScope and AudioObjectPropertyElement, a specific piece of
                    information about an AudioObject.
    @discussion     The property selector specifies the general classification of the property such
                    as volume, stream format, latency, etc. Note that each class has a different set
                    of selectors. A subclass inherits its super class's set of selectors, although
                    it may not implement them all.
*/
typedef UInt32  AudioObjectPropertySelector;

/*!
    @typedef        AudioObjectPropertyScope
    @abstract       An AudioObjectPropertyScope is a four char code that identifies, along with the
                    AudioObjectPropertySelector and AudioObjectPropertyElement, a specific piece of
                    information about an AudioObject.
    @discussion     The scope specifies the section of the object in which to look for the property,
                    such as input, output, global, etc. Note that each class has a different set of
                    scopes. A subclass inherits its superclass's set of scopes.
*/
typedef UInt32  AudioObjectPropertyScope;

/*!
    @typedef        AudioObjectPropertyElement
    @abstract       An AudioObjectPropertyElement is an integer that identifies, along with the
                    AudioObjectPropertySelector and AudioObjectPropertyScope, a specific piece of
                    information about an AudioObject.
    @discussion     The element selects one of possibly many items in the section of the object in
                    which to look for the property. Elements are number sequentially where 0
                    represents the master element. Elements are particular to an instance of a
                    class, meaning that two instances can have different numbers of elements in the
                    same scope. There is no inheritance of elements.
*/
typedef UInt32  AudioObjectPropertyElement;

/*!
    @struct         AudioObjectPropertyAddress
    @abstract       An AudioObjectPropertyAddress collects the three parts that identify a specific
                    property together in a struct for easy transmission.
    @field          mSelector
                        The AudioObjectPropertySelector for the property.
    @field          mScope
                        The AudioObjectPropertyScope for the property.
    @field          mElement
                        The AudioObjectPropertyElement for the property.
*/
struct  AudioObjectPropertyAddress
{
    AudioObjectPropertySelector mSelector;
    AudioObjectPropertyScope    mScope;
    AudioObjectPropertyElement  mElement;
};
typedef struct AudioObjectPropertyAddress   AudioObjectPropertyAddress;

//==================================================================================================
#pragma mark Basic Constants

/*!
    @enum           Error Constants
    @abstract       The error constants unique to the HAL.
    @discussion     These are the error constants that are unique to the HAL. Note that the HAL's
                    functions can and will return other codes that are not listed here. While these
                    constants give a general idea of what might have gone wrong during the execution
                    of an API call, if an API call returns anything other than kAudioHardwareNoError
                    it is to be viewed as the same failure regardless of what constant is actually
                    returned.
    @constant       kAudioHardwareNoError
                        The function call completed successfully.
    @constant       kAudioHardwareNotRunningError
                        The function call requires that the hardware be running but it isn't.
    @constant       kAudioHardwareUnspecifiedError
                        The function call failed while doing something that doesn't provide any
                        error messages.
    @constant       kAudioHardwareUnknownPropertyError
                        The AudioObject doesn't know about the property at the given address.
    @constant       kAudioHardwareBadPropertySizeError
                        An improperly sized buffer was provided when accessing the data of a
                        property.
    @constant       kAudioHardwareIllegalOperationError
                        The requested operation couldn't be completed.
    @constant       kAudioHardwareBadObjectError
                        The AudioObjectID passed to the function doesn't map to a valid AudioObject.
    @constant       kAudioHardwareBadDeviceError
                        The AudioObjectID passed to the function doesn't map to a valid AudioDevice.
    @constant       kAudioHardwareBadStreamError
                        The AudioObjectID passed to the function doesn't map to a valid AudioStream.
    @constant       kAudioHardwareUnsupportedOperationError
                        The AudioObject doesn't support the requested operation.
    @constant       kAudioDeviceUnsupportedFormatError
                        The AudioStream doesn't support the requested format.
    @constant       kAudioDevicePermissionsError
                        The requested operation can't be completed because the process doesn't have
                        permission.
*/
enum
{
    kAudioHardwareNoError                   = 0,
    kAudioHardwareNotRunningError           = 'stop',
    kAudioHardwareUnspecifiedError          = 'what',
    kAudioHardwareUnknownPropertyError      = 'who?',
    kAudioHardwareBadPropertySizeError      = '!siz',
    kAudioHardwareIllegalOperationError     = 'nope',
    kAudioHardwareBadObjectError            = '!obj',
    kAudioHardwareBadDeviceError            = '!dev',
    kAudioHardwareBadStreamError            = '!str',
    kAudioHardwareUnsupportedOperationError = 'unop',
    kAudioDeviceUnsupportedFormatError      = '!dat',
    kAudioDevicePermissionsError            = '!hog'
};

/*!
    @enum           Predefined AudioObjectID values
    @abstract       ObjectIDs that are always the same
    @constant       kAudioObjectUnknown
                        This is the sentinel value. No object will have an ID whose value is 0.
*/
enum
{
    kAudioObjectUnknown         = 0
};

/*!
    @enum           Property Address Constants
    @abstract       The valid values for the scope in a property address.
    @constant       kAudioObjectPropertyScopeGlobal
                        The AudioObjectPropertyScope for properties that apply to the object as a
                        whole. All objects have a global scope and for most it is their only scope.
    @constant       kAudioObjectPropertyScopeInput
                        The AudioObjectPropertyScope for properties that apply to the input side of
                        an object.
    @constant       kAudioObjectPropertyScopeOutput
                        The AudioObjectPropertyScope for properties that apply to the output side of
                        an object.
    @constant       kAudioObjectPropertyScopePlayThrough
                        The AudioObjectPropertyScope for properties that apply to the play through
                        side of an object.
    @constant       kAudioObjectPropertyElementMaster
                        The AudioObjectPropertyElement value for properties that apply to the master
                        element or to the entire scope.
*/
enum
{
    kAudioObjectPropertyScopeGlobal         = 'glob',
    kAudioObjectPropertyScopeInput          = 'inpt',
    kAudioObjectPropertyScopeOutput         = 'outp',
    kAudioObjectPropertyScopePlayThrough    = 'ptru',
    kAudioObjectPropertyElementMaster       = 0
};

/*!
    @enum           Wildcard Constants
    @abstract       Constants that are used as wildcards.
    @discussion     Wildcards match any and all values for thier associated type. They are useful
                    when registering to receive notificationss.
    @constant       kAudioObjectPropertySelectorWildcard
                        The wildcard value for AudioObjectPropertySelectors.
    @constant       kAudioObjectPropertyScopeWildcard
                        The wildcard value for AudioObjectPropertyScopes.
    @constant       kAudioObjectPropertyElementWildcard
                        The wildcard value for AudioObjectPropertyElements.
    @constant       kAudioObjectClassIDWildcard
                        The wildcard value for AudioClassIDs.
*/
enum
{
    kAudioObjectPropertySelectorWildcard    = '****',
    kAudioObjectPropertyScopeWildcard       = '****',
    kAudioObjectPropertyElementWildcard     = 0xFFFFFFFF,
    kAudioObjectClassIDWildcard             = '****'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioObject Constants

/*!
    @enum           AudioObject Class Constants
    @abstract       Various constants related to the AudioObject class.
    @discussion     The AudioObject class is the base class for all classes.
    @constant       kAudioObjectClassID
                        The AudioClassID that identifies the AudioObject class.
*/
enum
{
    kAudioObjectClassID = 'aobj'
};

//==================================================================================================
#pragma mark AudioObject Properties

/*!
    @enum           AudioObject Property Selectors
    @abstract       AudioObjectPropertySelector values provided by objects of the AudioObject class.
    @discussion     The AudioObject class is the base class for all classes. As such, all classes
                    inherit this set of properties.
    @constant       kAudioObjectPropertyBaseClass
                        An AudioClassID that identifies the class from which the clase of the
                        AudioObject is derived. This value must always be one of the standard
                        classes.
    @constant       kAudioObjectPropertyClass
                        An AudioClassID that identifies the class of the AudioObject.
    @constant       kAudioObjectPropertyOwner
                        An AudioObjectID that identifies the the AudioObject that owns the given
                        AudioObject. Note that all AudioObjects are owned by some other AudioObject.
                        The only exception is the AudioSystemObject, for which the value of this
                        property is kAudioObjectUnknown.
    @constant       kAudioObjectPropertyName
                        A CFString that contains the human readable name of the object. The caller
                        is responsible for releasing the returned CFObject.
    @constant       kAudioObjectPropertyModelName
                        A CFString that contains the human readable model name of the object. The
                        model name differs from kAudioObjectPropertyName in that two objects of the
                        same model will have the same value for this property but may have different
                        values for kAudioObjectPropertyName.
    @constant       kAudioObjectPropertyManufacturer
                        A CFString that contains the human readable name of the manufacturer of the
                        hardware the AudioObject is a part of. The caller is responsible for
                        releasing the returned CFObject.
    @constant       kAudioObjectPropertyElementName
                        A CFString that contains a human readable name for the given element in the
                        given scope. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioObjectPropertyElementCategoryName
                        A CFString that contains a human readable name for the category of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
    @constant       kAudioObjectPropertyElementNumberName
                        A CFString that contains a human readable name for the number of the given
                        element in the given scope. The caller is responsible for releasing the
                        returned CFObject.
    @constant       kAudioObjectPropertyOwnedObjects
                        An array of AudioObjectIDs that represent all the AudioObjects owned by the
                        given object. The qualifier is an array of AudioClassIDs. If it is
                        non-empty, the returned array of AudioObjectIDs will only refer to objects
                        whose class is in the qualifier array or whose is a subclass of one in the
                        qualifier array.
    @constant       kAudioObjectPropertyIdentify
                        A UInt32 where a value of one indicates that the object's hardware is
                        drawing attention to itself, typically by flashing or lighting up its front
                        panel display. A value of 0 indicates that this function is turned off. This
                        makes it easy for a user to associate the physical hardware with its
                        representation in an application. Typically, this property is only supported
                        by AudioDevices and AudioBoxes.
    @constant       kAudioObjectPropertySerialNumber
                        A CFString that contains the human readable serial number for the object.
                        This property will typically be implemented by AudioBox and AudioDevice
                        objects. Note that the serial number is not defined to be unique in the same
                        way that an AudioBox's or AudioDevice's UID propety are defined. This is
                        purely an informational value. The caller is responsible for releaseing the
                        returned CFObject.
    @constant       kAudioObjectPropertyFirmwareVersion
                        A CFString that contains the human readable firmware version for the object.
                        This property will typically be implemented by AudioBox and AudioDevice
                        objects. Note that this is purely an informational value. The caller is
                        responsible for releaseing the returned CFObject.
*/
enum
{
    kAudioObjectPropertyBaseClass           = 'bcls',
    kAudioObjectPropertyClass               = 'clas',
    kAudioObjectPropertyOwner               = 'stdv',
    kAudioObjectPropertyName                = 'lnam',
    kAudioObjectPropertyModelName           = 'lmod',
    kAudioObjectPropertyManufacturer        = 'lmak',
    kAudioObjectPropertyElementName         = 'lchn',
    kAudioObjectPropertyElementCategoryName = 'lccn',
    kAudioObjectPropertyElementNumberName   = 'lcnn',
    kAudioObjectPropertyOwnedObjects        = 'ownd',
    kAudioObjectPropertyIdentify            = 'iden',
    kAudioObjectPropertySerialNumber        = 'snum',
    kAudioObjectPropertyFirmwareVersion     = 'fwvn'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioPlugIn Constants

/*!
    @enum           AudioPlugIn Class Constants
    @abstract       Various constants related to the AudioPlugIn class.
    @constant       kAudioPlugInClassID
                        The AudioClassID that identifies the AudioPlugIn class.
*/
enum
{
    kAudioPlugInClassID = 'aplg'
};

//==================================================================================================
#pragma mark AudioPlugIn Properties

/*!
    @enum           AudioPlugIn Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioPlugIn class.
    @discussion     The AudioPlugIn class is a subclass of the AudioObject class. The class has just
                    the global scope, kAudioObjectPropertyScopeGlobal, and only a master element.
    @constant       kAudioPlugInPropertyBundleID
                        A CFString that contains the bundle identifier for the AudioPlugIn. The
                        caller is responsible for releasing the returned CFObject.
    @constant       kAudioPlugInPropertyDeviceList
                        An array of AudioObjectIDs that represent all the AudioDevices currently
                        provided by the plug-in.
    @constant       kAudioPlugInPropertyTranslateUIDToDevice
                        This property fetches the AudioObjectID that corresponds to the AudioDevice
                        that has the given UID. The UID is passed in via the qualifier as a CFString
                        while the AudioObjectID for the AudioDevice is returned to the caller as the
                        property's data. Note that an error is not returned if the UID doesn't refer
                        to any AudioDevices. Rather, this property will return kAudioObjectUnknown
                        as the value of the property.
    @constant       kAudioPlugInPropertyBoxList
                        An array of AudioObjectIDs that represent all the AudioBox objects currently
                        provided by the plug-in
    @constant       kAudioPlugInPropertyTranslateUIDToBox
                        This property fetches the AudioObjectID that corresponds to the AudioBox
                        that has the given UID. The UID is passed in via the qualifier as a CFString
                        while the AudioObjectID for the AudioBox is returned to the caller as the
                        property's data. Note that an error is not returned if the UID doesn't refer
                        to any AudioBoxes. Rather, this property will return kAudioObjectUnknown
                        as the value of the property.
*/
enum
{
    kAudioPlugInPropertyBundleID                = 'piid',
    kAudioPlugInPropertyDeviceList              = 'dev#',
    kAudioPlugInPropertyTranslateUIDToDevice    = 'uidd',
    kAudioPlugInPropertyBoxList                 = 'box#',
    kAudioPlugInPropertyTranslateUIDToBox       = 'uidb'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioTransportManager Constants

/*!
    @enum           AudioTransportManager Class Constants
    @abstract       Various constants related to the AudioTransportManager class.
    @discussion     The AudioTransportManager class is a subclass of the AudioPlugIn class.
                    Instances manage an entire tranport mechanism such as AirPlay or an AVB network
                    of devices. AudioTransportManagers present a list of AudioEndPoints that
                    represent all the devices that they manage. They can combine AudioEndPoints into
                    an AudioEndPointDevice that can be used by the system the same as any other
                    AudioDevice.
    @constant       kAudioTransportManagerClassID
                        The AudioClassID that identifies the AudioTransportManager class.
*/
enum
{
    kAudioTransportManagerClassID   = 'trpm'
};

//==================================================================================================
#pragma mark AudioTransportManager Properties

/*!
    @enum           AudioTransportManager Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioTransportManager class.
    @discussion     The AudioTransportManager class is a subclass of the AudioPlugIn class. The
                    class has just the global scope, kAudioObjectPropertyScopeGlobal, and only a
                    master element.
    @constant       kAudioTransportManagerPropertyEndPointList
                        An array of AudioObjectIDs for all the AudioEndPoint objects the transport
                        manager is tracking.
    @constant       kAudioTransportManagerPropertyTranslateUIDToEndPoint
                        This property fetches the AudioObjectID that corresponds to the
                        AudioEndpoint that has the given UID. The UID is passed in via the qualifier
                        as a CFString while the AudioObjectID for the AudioEndpoint is returned to
                        the caller as the property's data. Note that an error is not returned if the
                        UID doesn't refer to any AudioEndpoints. Rather, this property will return
                        kAudioObjectUnknown as the value of the property.
    @constant       kAudioTransportManagerPorpertyTransportType
                        A UInt32 whose value indicates how the transport manager's endpoints and
                        endpoint devices are connected to the CPU. Constants for some of the values
                        for this property can be found in the enum in the AudioDevice Constants
                        section of this file.
*/
enum
{
    kAudioTransportManagerPropertyEndPointList              = 'end#',
    kAudioTransportManagerPropertyTranslateUIDToEndPoint    = 'uide',
    kAudioTransportManagerPropertyTransportType             = 'tran'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioBox Constants

/*!
    @enum           AudioBox Class Constants
    @abstract       Various constants related to the AudioBox class.
    @constant       kAudioBoxClassID
                        The AudioClassID that identifies the AudioBox class.
*/
enum
{
    kAudioBoxClassID    = 'abox'
};

//==================================================================================================
#pragma mark AudioBox Properties

/*!
    @enum           AudioBox Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioBox class.
    @discussion     The AudioBox class is a subclass of the AudioObject class. The class has just
                    the global scope, kAudioObjectPropertyScopeGlobal, and only a master element.
                    An AudioBox is a container for other objects (typically AudioDevice objects). An
                    AudioBox publishes identifying information about itself and can be enabled or
                    disabled. A box's contents are only available to the system when the box is
                    enabled.
    @constant       kAudioBoxPropertyBoxUID
                        A CFString that contains a persistent identifier for the AudioBox. An
                        AudioBox's UID is persistent across boots. The content of the UID string is
                        a black box and may contain information that is unique to a particular
                        instance of an AudioBox's hardware or unique to the CPU. Therefore they are
                        not suitable for passing between CPUs or for identifying similar models of
                        hardware. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioBoxPropertyTransportType
                        A UInt32 whose value indicates how the AudioBox is connected to the system.
                        Constants for some of the values for this property can be found in the enum
                        in the AudioDevice Constants section of AudioHardwareBase.h
    @constant       kAudioBoxPropertyHasAudio
                        A UInt32 where a non-zero value indicates that the box supports audio.
    @constant       kAudioBoxPropertyHasVideo
                        A UInt32 where a non-zero value indicates that the box supports video.
    @constant       kAudioBoxPropertyHasMIDI
                        A UInt32 where a non-zero value indicates that the box supports MIDI.
    @constant       kAudioBoxPropertyIsProtected
                        A UInt32 where a non-zero value indicates that the box requires
                        authentication to use.
    @constant       kAudioBoxPropertyAcquired
                        A UInt32 where a non-zero value indicates that the box's contents are
                        available to the system.
    @constant       kAudioBoxPropertyAcquisitionFailed
                        An OSStatus that indicates the reason for an attempt to acquire a box
                        failed. Note that this property is primarily used for notifications.
    @constant       kAudioBoxPropertyDeviceList
                        An array of AudioObjectIDs that represent all the AudioDevice objects that
                        came out of the given AudioBox. Note that until a box is enabled, this list
                        will be empty.
*/
enum
{
    kAudioBoxPropertyBoxUID             = 'buid',
    kAudioBoxPropertyTransportType      = 'tran',
    kAudioBoxPropertyHasAudio           = 'bhau',
    kAudioBoxPropertyHasVideo           = 'bhvi',
    kAudioBoxPropertyHasMIDI            = 'bhmi',
    kAudioBoxPropertyIsProtected        = 'bpro',
    kAudioBoxPropertyAcquired           = 'bxon',
    kAudioBoxPropertyAcquisitionFailed  = 'bxof',
    kAudioBoxPropertyDeviceList         = 'bdv#'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioDevice Constants

/*!
    @enum           AudioDevice Class Constants
    @abstract       Various constants related to the AudioDevice class.
    @constant       kAudioDeviceClassID
                        The AudioClassID that identifies the AudioDevice class.
*/
enum
{
    kAudioDeviceClassID = 'adev'
};

/*!
    @enum           Transport Type IDs
    @abstract       Commonly used values for kAudioDevicePropertyTransportType and
                    kAudioTransportManagerPropertyTransportType
    @constant       kAudioDeviceTransportTypeUnknown
                        The transport type ID returned when a device doesn't provide a transport
                        type.
    @constant       kAudioDeviceTransportTypeBuiltIn
                        The transport type ID for AudioDevices built into the system.
    @constant       kAudioDeviceTransportTypeAggregate
                        The transport type ID for aggregate devices.
    @constant       kAudioDeviceTransportTypeAutoAggregate
                        The transport type ID for automatically generated aggregate devices.
    @constant       kAudioDeviceTransportTypeVirtual
                        The transport type ID for AudioDevices that don't correspond to real audio
                        hardware.
    @constant       kAudioDeviceTransportTypePCI
                        The transport type ID for AudioDevices connected via the PCI bus.
    @constant       kAudioDeviceTransportTypeUSB
                        The transport type ID for AudioDevices connected via USB.
    @constant       kAudioDeviceTransportTypeFireWire
                        The transport type ID for AudioDevices connected via FireWire.
    @constant       kAudioDeviceTransportTypeBluetooth
                        The transport type ID for AudioDevices connected via Bluetooth.
    @constant       kAudioDeviceTransportTypeBluetoothLE
                        The transport type ID for AudioDevices connected via Bluetooth Low Energy.
    @constant       kAudioDeviceTransportTypeHDMI
                        The transport type ID for AudioDevices connected via HDMI.
    @constant       kAudioDeviceTransportTypeDisplayPort
                        The transport type ID for AudioDevices connected via DisplayPort.
    @constant       kAudioDeviceTransportTypeAirPlay
                        The transport type ID for AudioDevices connected via AirPlay.
    @constant       kAudioDeviceTransportTypeAVB
                        The transport type ID for AudioDevices connected via AVB.
    @constant       kAudioDeviceTransportTypeThunderbolt
                        The transport type ID for AudioDevices connected via Thunderbolt.
*/
enum
{
    kAudioDeviceTransportTypeUnknown        = 0,
    kAudioDeviceTransportTypeBuiltIn        = 'bltn',
    kAudioDeviceTransportTypeAggregate      = 'grup',
    kAudioDeviceTransportTypeVirtual        = 'virt',
    kAudioDeviceTransportTypePCI            = 'pci ',
    kAudioDeviceTransportTypeUSB            = 'usb ',
    kAudioDeviceTransportTypeFireWire       = '1394',
    kAudioDeviceTransportTypeBluetooth      = 'blue',
    kAudioDeviceTransportTypeBluetoothLE    = 'blea',
    kAudioDeviceTransportTypeHDMI           = 'hdmi',
    kAudioDeviceTransportTypeDisplayPort    = 'dprt',
    kAudioDeviceTransportTypeAirPlay        = 'airp',
    kAudioDeviceTransportTypeAVB            = 'eavb',
    kAudioDeviceTransportTypeThunderbolt    = 'thun'
};

//==================================================================================================
#pragma mark AudioDevice Properties

/*!
    @enum           AudioDevice Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioDevice class.
    @discussion     The AudioDevice class is a subclass of the AudioObjectClass. The class has four
                    scopes, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyScopeInput,
                    kAudioObjectPropertyScopeOutput, and kAudioObjectPropertyScopePlayThrough. The
                    class has a master element and an element for each channel in each stream
                    numbered according to the starting channel number of each stream.
    @constant       kAudioDevicePropertyConfigurationApplication
                        A CFString that contains the bundle ID for an application that provides a
                        GUI for configuring the AudioDevice. By default, the value of this property
                        is the bundle ID for Audio MIDI Setup. The caller is responsible for
                        releasing the returned CFObject.
    @constant       kAudioDevicePropertyDeviceUID
                        A CFString that contains a persistent identifier for the AudioDevice. An
                        AudioDevice's UID is persistent across boots. The content of the UID string
                        is a black box and may contain information that is unique to a particular
                        instance of an AudioDevice's hardware or unique to the CPU. Therefore they
                        are not suitable for passing between CPUs or for identifying similar models
                        of hardware. The caller is responsible for releasing the returned CFObject.
    @constant       kAudioDevicePropertyModelUID
                        A CFString that contains a persistent identifier for the model of an
                        AudioDevice. The identifier is unique such that the identifier from two
                        AudioDevices are equal if and only if the two AudioDevices are the exact
                        same model from the same manufacturer. Further, the identifier has to be the
                        same no matter on what machine the AudioDevice appears. The caller is
                        responsible for releasing the returned CFObject.
    @constant       kAudioDevicePropertyTransportType
                        A UInt32 whose value indicates how the AudioDevice is connected to the CPU.
                        Constants for some of the values for this property can be found in the enum
                        in the AudioDevice Constants section of this file.
    @constant       kAudioDevicePropertyRelatedDevices
                        An array of AudioDeviceIDs for devices related to the AudioDevice. For
                        IOAudio-based devices, a AudioDevices are related if they share the same
                        IOAudioDevice object.
    @constant       kAudioDevicePropertyClockDomain
                        A UInt32 whose value indicates the clock domain to which this AudioDevice
                        belongs. AudioDevices that have the same value for this property are able to
                        be synchronized in hardware. However, a value of 0 indicates that the clock
                        domain for the device is unspecified and should be assumed to be separate
                        from every other device's clock domain, even if they have the value of 0 as
                        their clock domain as well.
    @constant       kAudioDevicePropertyDeviceIsAlive
                        A UInt32 where a value of 1 means the device is ready and available and 0
                        means the device is usable and will most likely go away shortly.
    @constant       kAudioDevicePropertyDeviceIsRunning
                        A UInt32 where a value of 0 means the AudioDevice is not performing IO and
                        a value of 1 means that it is. Note that the device can be running even if
                        there are no active IOProcs such as by calling AudioDeviceStart() and
                        passing a NULL IOProc. Note that the notification for this property is
                        usually sent from the AudioDevice's IO thread.
    @constant       kAudioDevicePropertyDeviceCanBeDefaultDevice
                        A UInt32 where 1 means that the AudioDevice is a possible selection for
                        kAudioHardwarePropertyDefaultInputDevice or
                        kAudioHardwarePropertyDefaultOutputDevice depending on the scope.
    @constant       kAudioDevicePropertyDeviceCanBeDefaultSystemDevice
                        A UInt32 where 1 means that the AudioDevice is a possible selection for
                        kAudioHardwarePropertyDefaultSystemOutputDevice.
    @constant       kAudioDevicePropertyLatency
                        A UInt32 containing the number of frames of latency in the AudioDevice. Note
                        that input and output latency may differ. Further, the AudioDevice's
                        AudioStreams may have additional latency so they should be queried as well.
                        If both the device and the stream say they have latency, then the total
                        latency for the stream is the device latency summed with the stream latency.
    @constant       kAudioDevicePropertyStreams
                        An array of AudioStreamIDs that represent the AudioStreams of the
                        AudioDevice. Note that if a notification is received for this property, any
                        cached AudioStreamIDs for the device become invalid and need to be
                        re-fetched.
    @constant       kAudioObjectPropertyControlList
                        An array of AudioObjectIDs that represent the AudioControls of the
                        AudioDevice. Note that if a notification is received for this property, any
                        cached AudioObjectIDs for the device become invalid and need to be
                        re-fetched.
    @constant       kAudioDevicePropertySafetyOffset
                        A UInt32 whose value indicates the number for frames in ahead (for output)
                        or behind (for input the current hardware position that is safe to do IO.
    @constant       kAudioDevicePropertyNominalSampleRate
                        A Float64 that indicates the current nominal sample rate of the AudioDevice.
    @constant       kAudioDevicePropertyAvailableNominalSampleRates
                        An array of AudioValueRange structs that indicates the valid ranges for the
                        nominal sample rate of the AudioDevice.
    @constant       kAudioDevicePropertyIcon
                        A CFURLRef that indicates an image file that can be used to represent the
                        device visually. The caller is responsible for releasing the returned
                        CFObject.
    @constant       kAudioDevicePropertyIsHidden
                        A UInt32 where a non-zero value indicates that the device is not included
                        in the normal list of devices provided by kAudioHardwarePropertyDevices nor
                        can it be the default device. Hidden devices can only be discovered by
                        knowing their UID and using kAudioHardwarePropertyDeviceForUID.
    @constant       kAudioDevicePropertyPreferredChannelsForStereo
                        An array of two UInt32s, the first for the left channel, the second for the
                        right channel, that indicate the channel numbers to use for stereo IO on the
                        device. The value of this property can be different for input and output and
                        there are no restrictions on the channel numbers that can be used.
    @constant       kAudioDevicePropertyPreferredChannelLayout
                        An AudioChannelLayout that indicates how each channel of the AudioDevice
                        should be used.
*/
enum
{
    kAudioDevicePropertyConfigurationApplication        = 'capp',
    kAudioDevicePropertyDeviceUID                       = 'uid ',
    kAudioDevicePropertyModelUID                        = 'muid',
    kAudioDevicePropertyTransportType                   = 'tran',
    kAudioDevicePropertyRelatedDevices                  = 'akin',
    kAudioDevicePropertyClockDomain                     = 'clkd',
    kAudioDevicePropertyDeviceIsAlive                   = 'livn',
    kAudioDevicePropertyDeviceIsRunning                 = 'goin',
    kAudioDevicePropertyDeviceCanBeDefaultDevice        = 'dflt',
    kAudioDevicePropertyDeviceCanBeDefaultSystemDevice  = 'sflt',
    kAudioDevicePropertyLatency                         = 'ltnc',
    kAudioDevicePropertyStreams                         = 'stm#',
    kAudioObjectPropertyControlList                     = 'ctrl',
    kAudioDevicePropertySafetyOffset                    = 'saft',
    kAudioDevicePropertyNominalSampleRate               = 'nsrt',
    kAudioDevicePropertyAvailableNominalSampleRates     = 'nsr#',
    kAudioDevicePropertyIcon                            = 'icon',
    kAudioDevicePropertyIsHidden                        = 'hidn',
    kAudioDevicePropertyPreferredChannelsForStereo      = 'dch2',
    kAudioDevicePropertyPreferredChannelLayout          = 'srnd'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioEndPointDevice Constants

/*!
    @enum           AudioEndPointDevice Class Constants
    @abstract       Various constants related to the AudioEndPointDevice class.
    @constant       kAudioEndPointDeviceClassID
                        The AudioClassID that identifies the AudioEndPointDevice class.
*/
enum
{
    kAudioEndPointDeviceClassID = 'edev'
};

/*!
    @defined        kAudioEndPointDeviceUIDKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the UID
                    of the AudioEndPointDevice.
*/
#define kAudioEndPointDeviceUIDKey  "uid"

/*!
    @defined        kAudioEndPointDeviceNameKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the
                    human readable name of the AudioEndPointDevice.
*/
#define kAudioEndPointDeviceNameKey "name"

/*!
    @defined        kAudioEndPointDeviceEndPointListKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFArray of CFDictionaries that
                    describe each AudioEndPoint in the AudioEndPointDevice. The keys for this
                    CFDictionary are defined in the AudioEndPoint Constants section.
*/
#define kAudioEndPointDeviceEndPointListKey "endpoints"

/*!
    @defined        kAudioEndPointDeviceMasterEndPointKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFString that contains the UID
                    for the AudioEndPoint that is the master time source for the
                    AudioEndPointDevice.
*/
#define kAudioEndPointDeviceMasterEndPointKey   "master"

/*!
    @defined        kAudioEndPointDeviceIsPrivateKey
    @discussion     The key used in a CFDictionary that describes the composition of an
                    AudioEndPointDevice. The value for this key is a CFNumber where a value of 0
                    means that the AudioEndPointDevice is to be published to the entire system and a
                    value of 1 means that the AudioEndPointDevice is private to the process that
                    created it. Note that a private AudioEndPointDevice is not persistent across
                    launches of the process that created it. Note that if this key is not present,
                    it implies that the AudioEndPointDevice is published to the entire system.
*/
#define kAudioEndPointDeviceIsPrivateKey    "private"

//==================================================================================================
#pragma mark AudioEndPointDevice Properties

/*!
    @enum           AudioEndPointDevice Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioEndPointDevice class.
    @discussion     AudioEndPointDevice is a subclass of AudioDevice and has the same scope and
                    element structure.
    @constant       kAudioEndPointDevicePropertyComposition
                        A CFDictionary that describes the composition of the AudioEndPointDevice.
                        The keys for this CFDicitionary are defined in the AudioEndPointDevice
                        Constants section. The caller is responsible for releasing the returned
                        CFObject.
    @constant       kAudioEndPointDevicePropertyEndPointList
                        An array of AudioObjectIDs for all the AudioEndPoints in the device.
    @constant       kAudioEndPointDevicePropertyIsPrivate
                        A pid_t where a value of 0 idicates that the device is public and a non-zero
                        value indicates the pid of the process that owns the device.
*/
enum
{
    kAudioEndPointDevicePropertyComposition         = 'acom',
    kAudioEndPointDevicePropertyEndPointList        = 'agrp',
    kAudioEndPointDevicePropertyIsPrivate           = 'priv'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioEndPoint Constants

/*!
    @enum           AudioEndPoint Class Constants
    @abstract       Various constants related to the AudioEndPoint class.
    @discussion     The AudioEndPoint class is a subclass of AudioDevice class and has the same
                    scope and element structure. However, AudioEndPoint objects do not implement an
                    IO path of their own and as such do not implmenent any  AudioDevice properties
                    associated with the IO path.
    @constant       kAudioEndPointDeviceClassID
                        The AudioClassID that identifies the AudioEndPointDevice class.
*/
enum
{
    kAudioEndPointClassID   = 'endp'
};

/*!
    @defined        kAudioEndPointUIDKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFString that contains the UID for the AudioEndPoint.
*/
#define kAudioEndPointUIDKey    "uid"

/*!
    @defined        kAudioEndPointNameKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFString that contains the human readable name of the AudioEndPoint.
*/
#define kAudioEndPointNameKey   "name"

/*!
    @defined        kAudioEndPointInputChannelsKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFNumber that indicates the total number of input channels for the
                    AudioEndPoint.
*/
#define kAudioEndPointInputChannelsKey  "channels-in"

/*!
    @defined        kAudioEndPointOutputChannelsKey
    @discussion     The key used in a CFDictionary that describes the state of an AudioEndPoint in a
                    description dictionary for an AudioEndPointDevice. The value for this key is a
                    CFNumber that indicates the total number of output channels for the
                    AudioEndPoint.
*/
#define kAudioEndPointOutputChannelsKey "channels-out"

//==================================================================================================
#pragma mark -
#pragma mark AudioStream Types

/*!
    @struct         AudioStreamRangedDescription
    @abstract       This structure allows a specific sample rate range to be associated with an
                    AudioStreamBasicDescription that specifies its sample rate as
                    kAudioStreamAnyRate.
    @discussion     Note that this structure is only used to desicribe the the available formats
                    for a stream. It is not used for the current format.
    @field          mFormat
                        The AudioStreamBasicDescription that describes the format of the stream.
                        Note that the mSampleRate field of the structure will be the same as the
                        the values in mSampleRateRange when only a single sample rate is supported.
                        It will be kAudioStreamAnyRate when there is a range with more elements.
    @field          mSampleRateRange
                        The AudioValueRange that describes the minimum and maximum sample rate for
                        the stream. If the mSampleRate field of mFormat is kAudioStreamAnyRate the
                        format supports the range of sample rates described by this structure.
                        Otherwise, the minimum will be the same as the maximum which will be the
                        same as the mSampleRate field of mFormat.
*/
struct  AudioStreamRangedDescription
{
    AudioStreamBasicDescription     mFormat;
    AudioValueRange                 mSampleRateRange;
};
typedef struct AudioStreamRangedDescription AudioStreamRangedDescription;

//==================================================================================================
#pragma mark AudioStream Constants

/*!
    @enum           AudioStream Class Constants
    @abstract       Various constants related to the AudioStream class.
    @constant       kAudioStreamClassID
                        The AudioClassID that identifies the AudioStream class.
*/
enum
{
    kAudioStreamClassID = 'astr'
};

/*!
    @enum           AudioStream Terminal Types
    @abstract       Various constants that describe the terminal type of an AudioStream.
    @constant       kAudioStreamTerminalTypeUnknown
                        The ID used when the terminal type for the AudioStream is non known.
    @constant       kAudioStreamTerminalTypeLine
                        The ID for a terminal type of a line level stream. Note that this applies to
                        both input streams and output streams
    @constant       kAudioStreamTerminalTypeDigitalAudioInterface
                        The ID for a terminal type of stream from/to a digital audio interface as
                        defined by ISO 60958 (aka SPDIF or AES/EBU). Note that this applies to both
                        input streams and output streams
    @constant       kAudioStreamTerminalTypeSpeaker
                        The ID for a terminal type of a speaker.
    @constant       kAudioStreamTerminalTypeHeadphones
                        The ID for a terminal type of headphones.
    @constant       kAudioStreamTerminalTypeLFESpeaker
                        The ID for a terminal type of a speaker for low frequency effects.
    @constant       kAudioStreamTerminalTypeReceiverSpeaker
                        The ID for a terminal type of a speaker on a telephone handset receiver.
    @constant       kAudioStreamTerminalTypeMicrophone
                        The ID for a terminal type of a microphone.
    @constant       kAudioStreamTerminalTypeHeadsetMicrophone
                        The ID for a terminal type of a microphone attached to an headset.
    @constant       kAudioStreamTerminalTypeReceiverMicrophone
                        The ID for a terminal type of a microhpone on a telephone handset recevier.
    @constant       kAudioStreamTerminalTypeTTY
                        The ID for a terminal type of a device providing a TTY signal.
    @constant       kAudioStreamTerminalTypeHDMI
                        The ID for a terminal type of a stream from/to an HDMI port.
    @constant       kAudioStreamTerminalTypeDisplayPort
                        The ID for a terminal type of a stream from/to an DisplayPort port.
*/
enum
{
    kAudioStreamTerminalTypeUnknown                 = 0,
    kAudioStreamTerminalTypeLine                    = 'line',
    kAudioStreamTerminalTypeDigitalAudioInterface   = 'spdf',
    kAudioStreamTerminalTypeSpeaker                 = 'spkr',
    kAudioStreamTerminalTypeHeadphones              = 'hdph',
    kAudioStreamTerminalTypeLFESpeaker              = 'lfes',
    kAudioStreamTerminalTypeReceiverSpeaker         = 'rspk',
    kAudioStreamTerminalTypeMicrophone              = 'micr',
    kAudioStreamTerminalTypeHeadsetMicrophone       = 'hmic',
    kAudioStreamTerminalTypeReceiverMicrophone      = 'rmic',
    kAudioStreamTerminalTypeTTY                     = 'tty_',
    kAudioStreamTerminalTypeHDMI                    = 'hdmi',
    kAudioStreamTerminalTypeDisplayPort             = 'dprt'
};

//==================================================================================================
#pragma mark AudioStream Properties

/*!
    @enum           AudioStream Properties
    @abstract       AudioObjectPropertySelector values provided by the AudioStream class.
    @discussion     AudioStream is a subclass of AudioObject and has only the single scope,
                    kAudioObjectPropertyScopeGlobal. They have a master element and an element for
                    each channel in the stream numbered upward from 1.
    @constant       kAudioStreamPropertyIsActive
                        A UInt32 where a non-zero value indicates that the stream is enabled and
                        doing IO.
    @constant       kAudioStreamPropertyDirection
                        A UInt32 where a value of 0 means that this AudioStream is an output stream
                        and a value of 1 means that it is an input stream.
    @constant       kAudioStreamPropertyTerminalType
                        A UInt32 whose value describes the general kind of functionality attached
                        to the AudioStream.
    @constant       kAudioStreamPropertyStartingChannel
                        A UInt32 that specifies the first element in the owning device that
                        corresponds to element one of this stream.
    @constant       kAudioStreamPropertyLatency
                        A UInt32 containing the number of frames of latency in the AudioStream. Note
                        that the owning AudioDevice may have additional latency so it should be
                        queried as well. If both the device and the stream say they have latency,
                        then the total latency for the stream is the device latency summed with the
                        stream latency.
    @constant       kAudioStreamPropertyVirtualFormat
                        An AudioStreamBasicDescription that describes the current data format for
                        the AudioStream. The virtual format refers to the data format in which all
                        IOProcs for the owning AudioDevice will perform IO transactions.
    @constant       kAudioStreamPropertyAvailableVirtualFormats
                        An array of AudioStreamRangedDescriptions that describe the available data
                        formats for the AudioStream. The virtual format refers to the data format in
                        which all IOProcs for the owning AudioDevice will perform IO transactions.
    @constant       kAudioStreamPropertyPhysicalFormat
                        An AudioStreamBasicDescription that describes the current data format for
                        the AudioStream. The physical format refers to the data format in which the
                        hardware for the owning AudioDevice performs its IO transactions.
    @constant       kAudioStreamPropertyAvailablePhysicalFormats
                        An array of AudioStreamRangedDescriptions that describe the available data
                        formats for the AudioStream. The physical format refers to the data format
                        in which the hardware for the owning AudioDevice performs its IO
                        transactions.
*/
enum
{
    kAudioStreamPropertyIsActive                    = 'sact',
    kAudioStreamPropertyDirection                   = 'sdir',
    kAudioStreamPropertyTerminalType                = 'term',
    kAudioStreamPropertyStartingChannel             = 'schn',
    kAudioStreamPropertyLatency                     = kAudioDevicePropertyLatency,
    kAudioStreamPropertyVirtualFormat               = 'sfmt',
    kAudioStreamPropertyAvailableVirtualFormats     = 'sfma',
    kAudioStreamPropertyPhysicalFormat              = 'pft ',
    kAudioStreamPropertyAvailablePhysicalFormats    = 'pfta'
};
//==================================================================================================
#pragma mark -
#pragma mark AudioControl Constants

/*!
    @enum           AudioControl Class Constants
    @abstract       Various constants related to the AudioControl class.
    @constant       kAudioControlClassID
                        The AudioClassID that identifies the AudioControl class.
*/
enum
{
    kAudioControlClassID    = 'actl'
};

/*!
    @enum           AudioControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioControl class.
    @discussion     The AudioControl class is a subclass of the AudioObject class. The class has
                    just the global scope, kAudioObjectPropertyScopeGlobal, and only a master
                    element.
    @constant       kAudioControlPropertyScope
                        An AudioServerPlugIn_PropertyScope that indicates which part of a device the
                        control applies to.
    @constant       kAudioControlPropertyElement
                        An AudioServerPlugIn_PropertyElement that indicates which element of the
                        device the control applies to.
*/
enum
{
    kAudioControlPropertyScope      = 'cscp',
    kAudioControlPropertyElement    = 'celm'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioSliderControl Constants

/*!
    @enum           AudioSliderControl Class Constants
    @abstract       Various constants related to the AudioSliderControl class.
    @constant       kAudioSliderControlClassID
                        The AudioClassID that identifies the AudioSliderControl class.
*/
enum
{
    kAudioSliderControlClassID  = 'sldr'
};

/*!
    @enum           AudioSliderControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioSliderControl class.
    @discussion     The AudioSliderControl class is a subclass of the AudioControl class and has the
                    same scope and element structure.
    @constant       kAudioSliderControlPropertyValue
                        A UInt32 that represents the value of the slider control.
    @constant       kAudioSliderControlPropertyRange
                        An array of two UInt32s that represents the inclusive range of values the
                        slider control can take.
*/
enum
{
    kAudioSliderControlPropertyValue    = 'sdrv',
    kAudioSliderControlPropertyRange    = 'sdrr'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioLevelControl Constants

/*!
    @enum           AudioLevelControl Class Constants
    @abstract       Various constants related to the AudioLevelControl class.
    @constant       kAudioLevelControlClassID
                        The AudioClassID that identifies the LevelControl class.
    @constant       kAudioVolumeControlClassID
                        A subclass of the LevelControl class that implements a general
                        gain/attenuation stage.
    @constant       kAudioLFEVolumeControlClassID
                        A subclass of the LevelControl class for an LFE channel that results from
                        bass management. Note that LFE channels that are represented as normal audio
                        channels must use kAudioVolumeControlClassID to manipulate the level.
*/
enum
{
    kAudioLevelControlClassID       = 'levl',
    kAudioVolumeControlClassID      = 'vlme',
    kAudioLFEVolumeControlClassID   = 'subv'
};

/*!
    @enum           AudioLevelControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioLevelControl class.
    @discussion     The AudioLevelControl class is a subclass of the AudioControl class and has the
                    same scope and element structure.
    @constant       kAudioLevelControlPropertyScalarValue
                        A Float32 that represents the value of the volume control. The range is
                        between 0.0 and 1.0 (inclusive). Note that the set of all Float32 values
                        between 0.0 and 1.0 inclusive is much larger than the set of actual values
                        that the hardware can select. This means that the Float32 range has a many
                        to one mapping with the underlying hardware values. As such, setting a
                        scalar value will result in the control taking on the value nearest to what
                        was set.
    @constant       kAudioLevelControlPropertyDecibelValue
                        A Float32 that represents the value of the volume control in dB. Note that
                        the set of all Float32 values in the dB range for the control is much larger
                        than the set of actual values that the hardware can select. This means that
                        the Float32 range has a many to one mapping with the underlying hardware
                        values. As such, setting a dB value will result in the control taking on the
                        value nearest to what was set.
    @constant       kAudioLevelControlPropertyDecibelRange
                        An AudioValueRange that contains the minimum and maximum dB values the
                        control can have.
    @constant       kAudioLevelControlPropertyConvertScalarToDecibels
                        A Float32 that on input contains a scalar volume value for the and on exit
                        contains the equivalent dB value.
    @constant       kAudioLevelControlPropertyConvertDecibelsToScalar
                        A Float32 that on input contains a dB volume value for the and on exit
                        contains the equivalent scalar value.
*/
enum
{
    kAudioLevelControlPropertyScalarValue               = 'lcsv',
    kAudioLevelControlPropertyDecibelValue              = 'lcdv',
    kAudioLevelControlPropertyDecibelRange              = 'lcdr',
    kAudioLevelControlPropertyConvertScalarToDecibels   = 'lcsd',
    kAudioLevelControlPropertyConvertDecibelsToScalar   = 'lcds'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioBooleanControl Constants

/*!
    @enum           AudioBooleanControl Class Constants
    @abstract       Various constants related to the AudioBooleanControl class.
    @constant       kAudioBooleanControlClassID
                        The AudioClassID that identifies the BooleanControl class.
    @constant       kAudioMuteControlClassID
                        A subclass of the AudioBooleanControl class where a true value means that
                        mute is enabled making that element inaudible.
    @constant       kAudioSoloControlClassID
                        A subclass of the AudioBooleanControl class where a true value means that
                        solo is enabled making just that element audible and the other elements
                        inaudible.
    @constant       kAudioJackControlClassID
                        A subclass of the AudioBooleanControl class where a true value means
                        something is plugged into that element.
    @constant       kAudioLFEMuteControlClassID
                        A subclass of the AudioBooleanControl class where true means that mute is
                        enabled making that LFE element inaudible. This control is for LFE channels
                        that result from bass management. Note that LFE channels that are
                        represented as normal audio channels must use an AudioMuteControl.
    @constant       kAudioPhantomPowerControlClassID
                        A subclass of the AudioBooleanControl class where true means that the
                        element's hardware has phantom power enabled.
    @constant       kAudioPhaseInvertControlClassID
                        A subclass of the AudioBooleanControl class where true means that the phase
                        of the signal on the given element is being inverted by 180 degrees.
    @constant       kAudioClipLightControlClassID
                        A subclass of the AudioBooleanControl class where true means that the signal
                        for the element has exceeded the sample range. Once a clip light is turned
                        on, it is to stay on until either the value of the control is set to false
                        or the current IO session stops and a new IO session starts.
    @constant       kAudioTalkbackControlClassID
                        An AudioBooleanControl where true means that the talkback channel is
                        enabled. This control is for talkback channels that are handled outside of
                        the regular IO channels. If the talkback channel is among the normal IO
                        channels, it will use AudioMuteControl.
    @constant       kAudioListenbackControlClassID
                        An AudioBooleanControl where true means that the listenback channel is
                        audible. This control is for listenback channels that are handled outside of
                        the regular IO channels. If the listenback channel is among the normal IO
                        channels, it will use AudioMuteControl.
*/
enum
{
    kAudioBooleanControlClassID         = 'togl',
    kAudioMuteControlClassID            = 'mute',
    kAudioSoloControlClassID            = 'solo',
    kAudioJackControlClassID            = 'jack',
    kAudioLFEMuteControlClassID         = 'subm',
    kAudioPhantomPowerControlClassID    = 'phan',
    kAudioPhaseInvertControlClassID     = 'phsi',
    kAudioClipLightControlClassID       = 'clip',
    kAudioTalkbackControlClassID        = 'talb',
    kAudioListenbackControlClassID      = 'lsnb'
};

/*!
    @enum           AudioBooleanControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioBooleanControl class.
    @discussion     The AudioBooleanControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioBooleanControlPropertyValue
                        A UInt32 where 0 means off/false and non-zero means on/true.
*/
enum
{
    kAudioBooleanControlPropertyValue   = 'bcvl'
};

//==================================================================================================
#pragma mark -
#pragma mark AudioSelectorControl Constants

/*!
    @enum           AudioSelectorControl Class Constants
    @abstract       Various constants related to the AudioSelectorControl class.
    @constant       kAudioSelectorControlClassID
                        The AudioClassID that identifies the AudioSelectorControl class.
    @constant       kAudioDataSourceControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the data
                        for the element is coming from.
    @constant       kAudioDataDestinationControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the data
                        for the element is going.
    @constant       kAudioClockSourceControlClassID
                        A subclass of the AudioSelectorControl class that identifies where the
                        timing info for the object is coming from.
    @constant       kAudioLineLevelControlClassID
                        A subclass of the AudioSelectorControl class that identifies the nominal
                        line level for the element. Note that this is not a gain stage but rather
                        indicating the voltage standard (if any) used for the element, such as
                        +4dBu, -10dBV, instrument, etc.
    @constant       kAudioHighPassFilterControlClassID
                        A subclass of the AudioSelectorControl class that indicates the setting for
                        the high pass filter on the given element.
*/
enum
{
    kAudioSelectorControlClassID        = 'slct',
    kAudioDataSourceControlClassID      = 'dsrc',
    kAudioDataDestinationControlClassID = 'dest',
    kAudioClockSourceControlClassID     = 'clck',
    kAudioLineLevelControlClassID       = 'nlvl',
    kAudioHighPassFilterControlClassID  = 'hipf'
};

/*!
    @enum           AudioSelectorControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioSelectorControl class.
    @discussion     The AudioSelectorControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioSelectorControlPropertyCurrentItem
                        An array of UInt32s that are the IDs of the items currently selected.
    @constant       kAudioSelectorControlPropertyAvailableItems
                        An array of UInt32s that represent the IDs of all the items available.
    @constant       kAudioSelectorControlPropertyItemName
                        This property translates the given item ID into a human readable name. The
                        qualifier contains the ID of the item to be translated and name is returned
                        as a CFString as the property data. The caller is responsible for releasing
                        the returned CFObject.
    @constant       kAudioSelectorControlPropertyItemKind
                        This property returns a UInt32 that identifies the kind of selector item the
                        item ID refers to. The qualifier contains the ID of the item. Note that this
                        property is optional for selector controls and that the meaning of the value
                        depends on the specifc subclass being queried.
*/
enum
{
    kAudioSelectorControlPropertyCurrentItem    = 'scci',
    kAudioSelectorControlPropertyAvailableItems = 'scai',
    kAudioSelectorControlPropertyItemName       = 'scin',
    kAudioSelectorControlPropertyItemKind       = 'clkk'
};

/*!
    @enum           Constants for the value of the property, kAudioSelectorControlPropertyItemKind
                    for any selector control item
    @constant       kAudioSelectorControlItemKindSpacer
                        This ID represents an item in a selector control's range that represents a
                        spacer item in a pop-up menu. Items with this kind are not be selectable.
*/
enum
{
    kAudioSelectorControlItemKindSpacer = 'spcr'
};

/*!
    @enum           Constants for the value of the property, kAudioSelectorControlPropertyItemKind
                    for AudioClockSourceControls.
    @constant       kAudioClockSourceItemKindInternal
                        This ID represents the device's internal clock.
*/
enum
{
    kAudioClockSourceItemKindInternal   = 'int '
};

//==================================================================================================
#pragma mark -
#pragma mark AudioStereoPanControl Constants

/*!
    @enum           AudioStereoPanControl Class Constants
    @abstract       Various constants related to the AudioStereoPanControl class.
    @constant       kAudioStereoPanControlClassID
                        The AudioClassID that identifies the StereoPanControl class.
*/
enum
{
    kAudioStereoPanControlClassID   = 'span'
};

/*!
    @enum           AudioStereoPanControl Property Selectors
    @abstract       AudioObjectPropertySelector values provided by the AudioStereoPanControl class.
    @discussion     The AudioStereoPanControl class is a subclass of the AudioControl class and has
                    the same scope and element structure.
    @constant       kAudioStereoPanControlPropertyValue
                        A Float32 where 0.0 is full left, 1.0 is full right, and 0.5 is center.
    @constant       kAudioStereoPanControlPropertyPanningChannels
                        An array of two UInt32s that indicate which elements of the device the
                        signal is being panned between.
*/
enum
{
    kAudioStereoPanControlPropertyValue             = 'spcv',
    kAudioStereoPanControlPropertyPanningChannels   = 'spcc'
};

//==================================================================================================

#if defined(__cplusplus)
}
#endif

#if PRAGMA_ENUM_ALWAYSINT
    #pragma enumsalwaysint reset
#endif

#endif  //  __AudioHardwareBase_h__
