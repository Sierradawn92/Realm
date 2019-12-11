#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include <sfObject.h>
#include <sfSession.h>
#include <sfValueProperty.h>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <Editor/UnrealEd/Classes/Editor/TransBuffer.h>

#include "sfBaseUObjectTranslator.h"
#include "../sfUPropertyInstance.h"
#include "../Consts.h"

using namespace KS::SceneFusion2;
using namespace KS;

/**
 * Manages actor syncing.
 */
class sfActorTranslator : public sfBaseUObjectTranslator
{
public:
    friend class sfBlueprintTranslator;
    friend class sfLevelTranslator;
    friend class sfComponentTranslator;
    friend class sfUObjectTranslator;
    friend class sfModelTranslator;
    friend class AsfMissingActor;
    friend class sfUndoManager;
    friend class SceneFusion;

    /**
     * Types of lock.
     */
    enum LockType
    {
        NotSynced,
        Unlocked,
        PartiallyLocked,
        FullyLocked
    };

    /**
     * Callback to initialize an actor or its sfObject.
     *
     * @param   sfObject::SPtr
     * @param   AActor*
     */
    typedef std::function<void(sfObject::SPtr, AActor*)> Initializer;

    /**
     * Delegate for lock state change.
     *
     * @param   AActor* - pointer to the actor whose lock state changed
     * @param   LockType - lock type
     * @param   sfUser::SPtr - lock owner
     */
    DECLARE_MULTICAST_DELEGATE_ThreeParams(OnLockStateChangeDelegate, AActor*, LockType, sfUser::SPtr);

    /**
     * Lock state change event handler.
     */
    OnLockStateChangeDelegate OnLockStateChange;

    /**
     * Delegate for on deselect.
     *
     * @param   AActor* actorPtr that was deselected.
     */
    DECLARE_MULTICAST_DELEGATE_OneParam(OnDeselectDelegate, AActor*);

    /**
     * Invoked when an actor is deselected.
     */
    OnDeselectDelegate OnDeselect;

    /**
     * Constructor
     */
    sfActorTranslator();

    /**
     * Destructor
     */
    virtual ~sfActorTranslator();

    /**
     * Initialization. Called after connecting to a session.
     */
    virtual void Initialize() override;

    /**
     * Deinitialization. Called after disconnecting from a session.
     */
    virtual void CleanUp() override;

    /**
     * Checks if an actor can be synced.
     *
     * @param   AActor* actorPtr
     * @param   bool allowPendingKill - if true, actors that are pending kill are considered syncable.
     * @return  bool true if the actor can be synced.
     */
    bool IsSyncable(AActor* actorPtr, bool allowPendingKill = false);

    /**
     * @return   int - number of synced actors.
     */
    int NumSyncedActors();

    /**
     * Registers a function to initialize actors of type T.
     *
     * @param   Initializer initializer to register.
     */
    template<typename T>
    void RegisterActorInitializer(Initializer initializer)
    {
        UnregisterActorInitializer<T>();
        m_actorInitializers.emplace(T::StaticClass(), initializer);
    }

    /**
     * Unregisters the actor initializer for actors of type T.
     */
    template<typename T>
    void UnregisterActorInitializer()
    {
        m_actorInitializers.erase(T::StaticClass());
    }

    /**
     * Registers a function to call when creating sfObjects for actors of type T.
     *
     * @param   Initializer initializer funcion to register.
     */
    template<typename T>
    void RegisterObjectInitializer(Initializer initializer)
    {
        UnregisterObjectInitializer<T>();
        m_objectInitializers.emplace(T::StaticClass(), initializer);
    }

    /**
     * Unregisters the object initializer for actors of type T.
     */
    template<typename T>
    void UnregisterObjectInitializer()
    {
        m_objectInitializers.erase(T::StaticClass());
    }

    /**
     * Registers an actor type to sync hidden instances for. By default hidden actors are not synced, but hidden actors
     * of this type will be synced.
     */
    template<typename T>
    void RegisterHiddenSyncType()
    {
        m_hiddenSyncTypes.Add(T::StaticClass());
    }

    /**
     * Unregisters an actor type to sync hidden instances for.
     */
    template<typename T>
    void UnregisterHiddenSyncType()
    {
        m_hiddenSyncTypes.Remove(T::StaticClass());
    }

private:
    FDelegateHandle m_onActorAddedHandle;
    FDelegateHandle m_onActorDeletedHandle;
    FDelegateHandle m_onActorAttachedHandle;
    FDelegateHandle m_onActorDetachedHandle;
    FDelegateHandle m_onFolderChangeHandle;
    FDelegateHandle m_onLabelChangeHandle;
    FDelegateHandle m_onLevelDirtiedHandle;
    FDelegateHandle m_onMoveStartHandle;
    FDelegateHandle m_onMoveEndHandle;
    FDelegateHandle m_onActorMovedHandle;
    FDelegateHandle m_tickHandle;

    TArray<AActor*> m_uploadList;
    std::unordered_set<sfObject::SPtr> m_recreateSet;
    TQueue<AActor*> m_revertFolderQueue;
    TArray<AActor*> m_syncParentList;
    TArray<FString> m_foldersToCheck;

    // Use std map because TSortedMap causes compile errors in Unreal's code
    std::map<AActor*, sfObject::SPtr> m_selectedActors;
    std::unordered_map<UClass*, Initializer> m_actorInitializers;
    std::unordered_map<UClass*, Initializer> m_objectInitializers;
    TSet<UClass*> m_hiddenSyncTypes;
    sfSession::SPtr m_sessionPtr;
    int m_numSyncedActors;
    bool m_movingActors;
    bool m_movingBrush;
    TSet<AActor*> m_movedActors;
    bool m_collectGarbage;
    float m_bspRebuildDelay;

    /**
     * Updates the actor translator.
     *
     * @param   float deltaTime in seconds since last tick.
     */
    void Tick(float deltaTime);

    /**
     * Checks if we should sync hidden actors of the given class.
     *
     * @param   UClass* classPtr to check.
     * @return  bool true if hidden actors of the given class should be synced.
     */
    bool IsHiddenSyncType(UClass* classPtr);

    /**
     * Checks for selection changes and requests locks on newly selected objects and unlocks unselected objects.
     */
    void UpdateSelection();

    /**
     * Destroys an actor.
     *
     * @param   AActor* actorPtr to destroy.
     */
    void DestroyActor(AActor* actorPtr);

    /**
     * Destroys actors that don't exist on the server in the given level.
     *
     * @param   ULevel* levelPtr - level to check
     */
    void DestroyUnsyncedActorsInLevel(ULevel* levelPtr);

    /**
     * Destroys components of an actor that don't exist on the server. Moves components that belong to another actor on
     * the server back to the server's actor.
     *
     * @param   AActor* actorPtr to destroy unsynced components for.
     */
    void DestroyUnsyncedComponents(AActor* actorPtr);

    /**
     * Reverts folders to server values for actors whose folder changed while locked.
     */
    void RevertLockedFolders();

    /**
     * Recreates actors that were deleted while locked.
     */
    void RecreateLockedActors();

    /**
     * Deletes folders that were emptied by other users.
     */
    void DeleteEmptyFolders();

    /**
     * Decreases the rebuild bsp timer and rebuilds bsp if it reaches 0.
     *
     * @param   deltaTime in seconds since the last cick.
     */
    void RebuildBSPIfNeeded(float deltaTime);

    /**
     * Marks a level as needing it's BSP rebuilt, and resets a timer to rebuild BSP.
     *
     * @param   ULevel* levelPtr whose BSP needs to be rebuilt.
     */
    void MarkBSPStale(ULevel* levelPtr);

    /**
     * Recursively removes child components of a deleted actor from the sfObjectMap. Optionally removes child actors or
     * adds them as children of a level object. Optionally removes child uobjects or stores them for reuse if their
     * outer is recreated.
     *
     * @param   sfObject::SPtr objPtr to recursively cleanup children for.
     * @param   sfObject::SPtr levelObjPtr to add actor child objects to. If provided, child uobjects are stored to be
     *          resused if their outers are recreated. Not used if recurseChildActors is true.
     * @param   bool recurseChildActors - if true, child actors and their descendants will also be removed from the
     *          sfObjectMap.
     */
    void CleanUpChildrenOfDeletedObject(
        sfObject::SPtr objPtr,
        sfObject::SPtr levelObjPtr = nullptr,
        bool recurseChildActors = false);

    /**
     * Calls the actor initializer for the actor's class, if one is registered.
     *
     * @param   sfObject::SPtr objPtr for the actor.
     * @param   AActor* actorPtr
     */
    void CallActorInitializer(sfObject::SPtr objPtr, AActor* actorPtr);

    /**
     * Calls the object initializer for the actor's class, if one is registered.
     *
     * @param   sfObject::SPtr objPtr for the actor.
     * @param   AActor* actorPtr
     */
    void CallObjectInitializer(sfObject::SPtr objPtr, AActor* actorPtr);

    /**
     * Called when an actor is added to the level.
     *
     * @param   AActor* actorPtr
     */
    void OnActorAdded(AActor* actorPtr);

    /**
     * Called when an actor is deleted from the level.
     *
     * @param   AActor* actorPtr
     */
    void OnActorDeleted(AActor* actorPtr);

    /**
     * Called when an actor is attached to or detached from another actor.
     *
     * @param   AActor* actorPtr
     * @param   const AActor* parentPtr
     */
    void OnAttachDetach(AActor* actorPtr, const AActor* parentPtr);

    /**
     * Called when an actor's folder changes.
     *
     * @param   const AActor* actorPtr
     * @param   FName oldFolder
     */
    void OnFolderChange(const AActor* actorPtr, FName oldFolder);

    /**
     * Called when an actor's label changes.
     *
     * @param   const AActor* actorPtr
     */
    void OnLabelChanged(AActor* actorPtr);

    /**
     * Called when a level is dirtied. Checks for changes to the selected brush's poly flags.
     */
    void OnLevelDirtied();

    /**
     * Called when an object starts being dragged in the viewport.
     *
     * @param   UObject& obj
     */
    void OnMoveStart(UObject& obj);

    /**
     * Called when an object stops being dragged in the viewport.
     *
     * @param   UObject& object
     */
    void OnMoveEnd(UObject& obj);

    /**
     * Called when an actor moves.
     *
     * @param   AActor* actorPtr
     */
    void OnActorMoved(AActor* actorPtr);

    /**
     * Called for each actor in an undo delete transaction, or redo create transaction. Recreates the actor on the
     * server, or deletes the actor if another actor with the same name already exists.
     *
     * @param   AActor* actorPtr
     */
    void OnUndoDelete(AActor* actorPtr);

    /**
     * Adds an sfObject to the set of objects to recreate actors for.
     *
     * @param   sfObject::SPtr objPtr
     */
    void RecreateActor(sfObject::SPtr objPtr);

    /**
     * Sends new label and name values to the server, or reverts to the server values if the actor is locked.
     *
     * @param   AActor* actorPtr to sync label and name for.
     * @param   sfObject::SPtr objPtr for the actor.
     * @param   sfDictionaryProperty::SPtr propertiesPtr for the actor.
     */
    void SyncLabelAndName(AActor* actorPtr, sfObject::SPtr objPtr, sfDictionaryProperty::SPtr propertiesPtr);

    /**
     * Sends a new folder value to the server, or reverts to the server value if the actor is locked.
     *
     * @param   AActor* actorPtr to sync folder for.
     * @param   sfObject::SPtr objPtr for the actor.
     * @param   sfDictionaryProperty::SPtr propertiesPtr for the actor.
     */
    void SyncFolder(AActor* actorPtr, sfObject::SPtr objPtr, sfDictionaryProperty::SPtr propertiesPtr);

    /**
     * Sends a new parent value to the server, or reverts to the server value if the actor or new parent are locked.
     *
     * @param   AActor* actorPtr to sync parent for.
     * @param   sfObject::SPtr objPtr for the actor.
     */
    void SyncParent(AActor* actorPtr, sfObject::SPtr objPtr);

    /**
     * Creates actor objects on the server.
     *
     * @param   TArray<AActor*>& actors to upload. Actors will be removed from the list, unless we could not upload the
     *          actor because it was deleted and recreated (using undo) and the server hasn't acknowledged the delete
     *          yet.
     */
    void UploadActors(TArray<AActor*>& actors);

    /**
     * Creates an sfObject for a uobject. Does not upload or create properties for the object.
     *
     * @param   UObject* uobjPtr to create sfObject for.
     * @param   sfObject::SPtr* outObjPtr created for the uobject.
     * @return  bool true if the uobject was handled by this translator.
     */
    virtual bool Create(UObject* uobjPtr, sfObject::SPtr& outObjPtr) override;

    /**
     * Recursively creates actor objects for an actor and its children.
     *
     * @param   AActor* actorPtr to create object for.
     * @return  sfObject::SPtr object for the actor.
     */
    sfObject::SPtr CreateObject(AActor* actorPtr);

    /**
     * Creates or finds an actor for an object and initializes it with server values. Recursively initializes child
     * actors for child objects.
     *
     * @param   sfObject::SPtr objPtr to initialize actor for.
     * @param   ULevel* levelPtr the actor belongs to.
     * @return  AActor* actor for the object.
     */
    AActor* InitializeActor(sfObject::SPtr objPtr, ULevel* levelPtr);

    /**
     * Iterates a list of objects and their descendants, looking for child actors whose objects are not attached and
     * attaches those objects.
     *
     * @param   const std::list<sfObject::SPtr>& objects
     */
    void FindAndAttachChildren(const std::list<sfObject::SPtr>& objects);

    /**
     * Checks for and sends transform changes for selected components in an actor to the server.
     *
     * @param   AActor* actorPtr to send transform update for.
     */
    void SyncComponentTransforms(AActor* actorPtr);

    /**
     * Registers property change handlers for server events.
     */
    void RegisterPropertyChangeHandlers();

    /**
     * Locks an actor.
     * 
     * @param   AActor* actorPtr
     * @param   sfObject::SPtr objPtr for the actor.
     */
    void Lock(AActor* actorPtr, sfObject::SPtr objPtr);

    /**
     * Unlocks an actor.
     *
     * @param   AActor* actorPtr
     */
    void Unlock(AActor* actorPtr);

    /**
     * Called when an actor is created by another user.
     *
     * @param   sfObject::SPtr objPtr that was created.
     * @param   int childIndex of new object. -1 if object is a root
     */
    virtual void OnCreate(sfObject::SPtr objPtr, int childIndex) override;

    /**
     * Called when an actor is deleted by another user.
     *
     * @param   sfObject::SPtr objPtr that was deleted.
     */
    virtual void OnDelete(sfObject::SPtr objPtr) override;

    /**
     * Called when an actor is locked by another user.
     *
     * @param   sfObject::SPtr objPtr that was locked.
     */
    virtual void OnLock(sfObject::SPtr objPtr) override;

    /**
     * Called when an actor is unlocked by another user.
     *
     * @param   sfObject::SPtr objPtr that was unlocked.
     */
    virtual void OnUnlock(sfObject::SPtr objPtr) override;

    /**
     * Called when an actor's lock owner changes.
     *
     * @param   sfObject::SPtr objPtr whose lock owner changed.
     */
    virtual void OnLockOwnerChange(sfObject::SPtr objPtr) override;

    /**
     * Called when an actor's parent is changed by another user.
     *
     * @param   sfObject::SPtr objPtr whose parent changed.
     * @param   int childIndex of the object. -1 if the object is a root.
     */
    virtual void OnParentChange(sfObject::SPtr objPtr, int childIndex) override;

    /**
     * Called when an object is modified by an undo or redo transaction.
     *
     * @param   sfObject::SPtr objPtr for the uobject that was modified. nullptr if the uobjPtr is not synced.
     * @param   UObject* uobjPtr that was modified.
     * @return  bool true if event was handled and need not be processed by other handlers.
     */
    virtual bool OnUndoRedo(sfObject::SPtr objPtr, UObject* uobjPtr) override;

    /**
     * Enables the actor attached and dettached event handler.
     */
    void EnableParentChangeHandler();

    /**
     * Disables the actor attached and dettached event handler.
     */
    void DisableParentChangeHandler();

    /**
     * Calls OnLockStateChange event handlers.
     *
     * @param   sfObject::SPtr objPtr whose lock state changed
     * @param   AActor* actorPtr
     */
    void InvokeOnLockStateChange(sfObject::SPtr objPtr, AActor* actorPtr);

    /**
     * Clears collections of actors.
     */
    void ClearActorCollections();

    /**
     * Deletes all actors in the given level.
     *
     * @param   sfObject::SPtr levelObjPtr
     * @param   ULevel* levelPtr
     */
    void OnRemoveLevel(sfObject::SPtr levelObjPtr, ULevel* levelPtr);

    /**
     * Calls OnCreate on every child of the given level sfObject. Destroys all unsynced actors after.
     *
     * @param   sfObject::SPtr sfLevelObjPtr
     * @param   ULevel* levelPtr
     */
    void OnSFLevelObjectCreate(sfObject::SPtr sfLevelObjPtr, ULevel* levelPtr);

    /**
     * Detaches the given actor from its parent if the given sfObject's parent is a level object and returns true.
     * Otherwise, returns false.
     *
     * @param   sfObject::SPtr objPtr
     * @param   AActor* actorPtr
     * @return  bool
     */
    bool DetachIfParentIsLevel(sfObject::SPtr objPtr, AActor* actorPtr);

    /**
     * Logs out an error that the given sfObject has no parent and then leaves the session.
     *
     * @param   sfObject::SPtr objPtr
     */
    void LogNoParentErrorAndDisconnect(sfObject::SPtr objPtr);
};
