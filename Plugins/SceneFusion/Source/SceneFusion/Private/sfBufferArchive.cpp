#include "sfBufferArchive.h"
#include "SceneFusion.h"
#include "sfObjectMap.h"

#define LOG_CHANNEL "sfBufferArchive"

namespace
{
    // Object types
    enum ObjectType : uint8_t
    {
        NUL = 0,        // the object is null
        UNSYNCED = 1,   // the object cannot be serialized. When deserializing, the reference will be unchanged.
        LEVEL = 2,      // the object is in the level
        ASSET = 3       // the object is an asset 
    };
}

FArchive& sfBufferArchive::operator<<(UObject*& uobjPtr)
{
    if (uobjPtr == nullptr)
    {
        uint8_t type = ObjectType::NUL;
        *this << type;
    }
    else if (uobjPtr->GetTypedOuter<ULevel>() != nullptr)
    {
        // The object is in the level
        sfObject::SPtr objPtr = sfObjectMap::GetSFObject(uobjPtr);
        if (objPtr == nullptr)
        {
            FString str = "Unable to serialize reference to unsynced " + uobjPtr->GetClass()->GetName() + " " +
                uobjPtr->GetName();
            KS::Log::Warning(TCHAR_TO_UTF8(*str), LOG_CHANNEL);
            uint8_t type = ObjectType::UNSYNCED;
            *this << type;
        }
        else
        {
            uint8_t type = ObjectType::LEVEL;
            *this << type;
            uint32_t id = objPtr->Id();
            *this << id;
        }
    }
    else if (uobjPtr->GetOutermost() == GetTransientPackage())
    {
        FString str = "Unable to serialize reference to " + uobjPtr->GetClass()->GetName() + " " +
            uobjPtr->GetName() + " in the transient package.";
        KS::Log::Warning(TCHAR_TO_UTF8(*str), LOG_CHANNEL);
        uint8_t type = ObjectType::UNSYNCED;
        *this << type;
    }
    else
    {
        uint8_t type = ObjectType::ASSET;
        *this << type;
        FString path = uobjPtr->GetPathName();
        *this << path;
    }
    return *this;
}

sfBufferReader::sfBufferReader(void* data, int64_t size) : 
    FBufferReaderBase{ data, size, false }
{

}

FArchive& sfBufferReader::operator<<(UObject*& uobjPtr)
{
    uint8_t type;
    *this << type;
    switch (type)
    {
        case ObjectType::NUL:
        {
            uobjPtr = nullptr;
            break;
        }
        case ObjectType::LEVEL:
        {
            uint32_t id;
            *this << id;
            if (SceneFusion::Service->Session() == nullptr)
            {
                break;
            }
            sfObject::SPtr objPtr = SceneFusion::Service->Session()->GetObject(id);
            UObject* referencePtr = sfObjectMap::GetUObject(objPtr);
            if (referencePtr == nullptr)
            {
                KS::Log::Warning("Unable to deserialize object reference.", LOG_CHANNEL);
            }
            else
            {
                uobjPtr = referencePtr;
            }
            break;
        }
        case ObjectType::ASSET:
        {
            FString path;
            *this << path;
            GIsSlowTask = true;
            uobjPtr = LoadObject<UObject>(nullptr, *path);
            GIsSlowTask = false;
            break;
        }
    }
    return *this;
}

FArchive& sfBufferReader::operator<<(FName& name)
{
    FString str;
    *this << str;
    name = *str;
    return *this;
}

#undef LOG_CHANNEL