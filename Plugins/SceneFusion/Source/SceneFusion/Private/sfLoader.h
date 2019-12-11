#pragma once

#include "sfIStandInGenerator.h"
#include <sfProperty.h>
#include <sfObject.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <CoreMinimal.h>
#include <IInputProcessor.h>
#include <SlateApplication.h>
#include <Engine/AssetManager.h>

using namespace KS;
using namespace KS::SceneFusion2;

/**
 * Utility for loading assets from memory, or from disc when the user is idle. Loading from disc may trigger assets to
 * be baked which can block the main thread (and cannot be done from another thread) for several seconds and interrupt
 * the user, so we wait till the user is idle.
 */
class sfLoader : public IInputProcessor
{
public:
    /**
     * Stand-in generator.
     *
     * @param   const FString& path to missing asset to generate stand-in for.
     * @param   UObject* stand-in to generate data for.
     */
    typedef std::function<void(const FString&, UObject*)> StandInGenerator;

    /**
     * Delegate for on create asset.
     *
     * @param   UObject* asset that was created.
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(OnCreateMissingAssetDelegate, UObject*);

    /**
     * Invoked when the loader creates a missing asset.
     */
    OnCreateMissingAssetDelegate OnCreateMissingAsset;

    /**
     * @return  sfLoader& singleton instance.
     */
    static sfLoader& Get();

    /**
     * Constructor
     */
    sfLoader();

    /**
     * Constructor
     */
    virtual ~sfLoader();

    /**
     * Starts monitoring user activity and loads assets when the user becomes idle.
     */
    void Start();

    /**
     * Stops monitoring user activity and stops loading.
     */
    void Stop();

    /**
     * Registers a generator to generate data for stand-ins of a given class.
     *
     * @param   UClass* classPtr to register generator for.
     * @param   TSharedPtr<sfIStandInGenerator> generatorPtr to register.
     */
    void RegisterStandInGenerator(UClass* classPtr, TSharedPtr<sfIStandInGenerator> generatorPtr);

    /**
     * Registers a generator to generate data for stand-ins of a given class.
     *
     * @param   UClass* classPtr to register generator for.
     * @param   StandInGenerator generator to register.
     */
    void RegisterStandInGenerator(UClass* classPtr, StandInGenerator generator);

    /**
     * Checks if the user is idle.
     *
     * @return  bool true if the user is idle.
     */
    bool IsUserIdle();

    /**
     * Loads the asset for a property when the user becomes idle.
     *
     * @param   sfProperty::SPtr propPtr to load asset for.
     */
    void LoadWhenIdle(sfProperty::SPtr propPtr);

    /**
     * Loads delayed assets referenced by an object or its component children.
     *
     * @param   sfObject::SPtr objPtr to load assets for.
     */
    void LoadAssetsFor(sfObject::SPtr objPtr);

    /**
     * Loads an asset. If the asset could not be found, creates a stand-in to represent the asset.
     *
     * @param   const FString& path of asset to load.
     * @param   const FString& className of asset to load.
     * @return  UObject* asset or stand-in for the asset. nullptr if the asset was not found and a stand-in could not
     *          be created.
     */
    UObject* Load(const FString& path, const FString& className);

    /**
     * Gets the class name and path of the asset a stand-in is representing.
     *
     * @param   UObject* standInPtr
     * @return  FString class name and path seperated by a ';'.
     */
    FString GetPathFromStandIn(UObject* standInPtr);

    /**
     * Loads an asset from memory. Returns null if the asset was not found in memory.
     *
     * @param   FString path to the asset.
     * @return  UObject* loaded asset or nullptr.
     */
    UObject* LoadFromCache(FString path);

    /**
     * Called every tick while the sfLoader is running. Loads assets if the user is idle and sets references to them,
     * and replaces stand-ins with their proper assets if they are available.
     *
     * @param   const float deltaTime
     * @param   FSlateApplication& slateApp
     * @param   TSharedRef<ICursor> cursor
     */
    virtual void Tick(const float deltaTime, FSlateApplication& slateApp, TSharedRef<ICursor> cursor) override;

    /**
     * Called when a mouse button is pressed.
     *
     * @param   FSlateApplication& slateApp
     * @param   const FPointerEvent& mouseEvent
     * @return  bool false to indicate the event was not handled.
     */
    virtual bool HandleMouseButtonDownEvent(FSlateApplication& slateApp, const FPointerEvent& mouseEvent) override;

    /**
     * Called when a mouse button is released.
     *
     * @param   FSlateApplication& slateApp
     * @param   const FPointerEvent& mouseEvent
     * @return  bool false to indicate the event was not handled.
     */
    virtual bool HandleMouseButtonUpEvent(FSlateApplication& slateApp, const FPointerEvent& mouseEvent) override;

    /**
     * Checks if an asset type should be created if it is missing.
     *
     * @param   UClass* classPtr
     * @return  bool true if the asset type should be created if it is missing.
     */
    bool IsCreatableAssetType(UClass* classPtr);

    /**
     * Checks if an asset was a missing asset we created when we tried to load it.
     *
     * @param   UObject* assetPtr
     * @return  bool true if the asset was created when we tried to load it.
     */
    bool WasCreatedOnLoad(UObject* assetPtr);

    /**
     * Registers an asset type to be created if it is missing when we try to load it.
     */
    template<typename T>
    void RegisterCreatableAssetType()
    {
        m_createTypes.Add(T::StaticClass());
    }

    /**
     * Unregisters a creatable asset type.
     */
    template<typename T>
    void UnregisterCreatableAssetType()
    {
        m_createTypes.Remove(T::StaticClass());
    }

private:
    static TSharedPtr<sfLoader> m_instancePtr;

    /**
     * sfIStandInGenerator implementation that wraps a delegate.
     */
    class Generator : public sfIStandInGenerator
    {
    public:
        /**
         * Constructor
         *
         * @param StandInGenerator generator
         */
        Generator(StandInGenerator generator) :
            m_generator{ generator }
        {

        }

        /**
         * Destructor
         */
        virtual ~Generator() {

        }

        /**
         * Calls the generator delegate.
         *
         * @param   const FString* path of missing asset.
         * @param   UObject* uobjPtr stand-in to generate data for.
         */
        virtual void Generate(const FString& path, UObject* uobjPtr) override
        {
            m_generator(path, uobjPtr);
        }

    private:
        StandInGenerator m_generator;
    };

    // Maps objects to a list of their properties that referenced assets to be loaded when the user is idle.
    std::unordered_map<sfObject::SPtr, std::vector<sfProperty::SPtr>> m_delayedAssets;
    // Maps classes to the path to their stand-in asset. If a class is not in the map, a stand-in is created using
    // NewObject.
    TMap<UClass*, FString> m_standInPaths;
    TMap<UClass*, TSharedPtr<sfIStandInGenerator>> m_standInGenerators;
    // Maps missing asset paths to their stand-ins.
    TMap<FString, UObject*> m_standIns;
    TArray<UObject*> m_standInsToReplace;
    // Will create assets of these types when loading if the asset does not exist
    TSet<UClass*> m_createTypes;
    TSet<UObject*> m_createdAssets;
    float m_replaceTimer;
    bool m_isMouseDown;
    bool m_overrideIdle;// if true, IsUserIdle() returns true even if the user isn't idle
    FDelegateHandle m_onNewAssetHandle;

    /**
     * Replaces references to stand-ins with the proper assets.
     */
    void ReplaceStandIns();

    /**
     * Loads delayed assets and sets references to them.
     */
    void LoadDelayedAssets();

    /**
     * Loads the asset for a property and sets the reference to it.
     *
     * @param   sfProperty::SPtr propPtr
     */
    void LoadProperty(sfProperty::SPtr propPtr);

    /**
     * Called when a new asset is created. If the asset has a stand-in, adds the stand-in to a list to replaced with
     * the new asset.
     *
     * @param   const FAssetData& assetData
     */
    void OnNewAsset(const FAssetData& assetData);
};