#pragma once

#include "sfBaseTranslator.h"
#include "sfActorTranslator.h"
#include <CoreMinimal.h>
#include <Model.h>
#include <Engine/Polys.h>
#include <unordered_set>

/**
 * Manages model syncing.
 */
class sfModelTranslator : public sfBaseTranslator
{
protected:
    /**
     * Initialization. Called after connecting to a session.
     */
    virtual void Initialize() override;

    /**
     * Deinitialization. Called after disconnecting from a session.
     */
    virtual void CleanUp() override;

    /**
     * Creates an sfObject for a uobject. Does not upload or create properties for the object.
     *
     * @param   UObject* uobjPtr to create sfObject for.
     * @param   sfObject::SPtr* outObjPtr created for the uobject.
     * @return  bool true if the uobject was handled by this translator.
     */
    virtual bool Create(UObject* uobjPtr, sfObject::SPtr& outObjPtr) override;

    /**
     * Called when a model is created by another user.
     *
     * @param   sfObject::SPtr objPtr that was created.
     * @param   int childIndex of new object. -1 if object is a root
     */
    virtual void OnCreate(sfObject::SPtr objPtr, int childIndex) override;

    /**
     * Called when an object property changes.
     *
     * @param   sfProperty::SPtr propertyPtr that changed.
     */
    virtual void OnPropertyChange(sfProperty::SPtr propPtr) override;

    /**
     * Called when an object is modified. Checks for model changes.
     *
     * @param   UObject* uobjPtr - modified object
     */
    virtual void OnUObjectModified(sfObject::SPtr objPtr, UObject* uobjptr) override;

private:
    TSharedPtr<sfActorTranslator> m_actorTranslatorPtr;
    std::unordered_set<sfObject::SPtr> m_staleModels;
    std::unordered_set<sfObject::SPtr> m_rebuiltModels;
    float m_checkChangeTimer;
    FDelegateHandle m_onDeselectHandle;
    FDelegateHandle m_onLevelDirtiedHandle;
    FDelegateHandle m_tickHandle;

    /**
     * Updates the model translator.
     *
     * @param   float deltaTime in seconds since last tick.
     */
    void Tick(float deltaTime);

    /**
     * Creates an sfObject for a model.
     *
     * @param   UModel* modelPtr to create object for.
     * @return  sfObject::SPtr object for the model.
     */
    sfObject::SPtr CreateObject(UModel* modelPtr);

    /**
     * Sends model changes to the server, or applies server data to the model if the object is locked.
     *
     * @param   sfObject::SPtr objPtr for the model.
     * @param   UModel* modelPtr to sync.
     */
    void Sync(sfObject::SPtr objPtr, UModel* modelPtr);

    /**
     * Applies server data to a model.
     * @param   UModel* modelPtr to apply data to.
     * @param   sfProperty::SPtr propPtr to get data from.
     */
    void ApplyServerData(UModel* modelPtr, sfProperty::SPtr propPtr);

    /**
     * Called when an actor is deselected. If the actor is a brush and its model is stale, syncs the model.
     *
     * @param   AActor* actorPtr to sync.
     */
    void OnDeselect(AActor* actorPtr);

    /**
     * Called when a level is dirtied. Checks for changes to the selected brush's surface alignment.
     */
    void OnLevelDirtied();

    /**
     * Serializes or deserializes a model.
     * 
     * @param   FArchive& archive to serialize to or deserialize from.
     * @param   UModel* modelPtr to serialize or deserialize.
     */
    void Serialize(FArchive& archive, UModel* modelPtr);

    /**
     * Serializes or deserializes a model vertex.
     *
     * @param   FArchive& archive to serialize to or deserialize from.
     * @param   FModelVertex& vertex to serialize or deserialize.
     */
    void Serialize(FArchive& archive, FModelVertex& vertex);

    /**
     * Serializes or deserializes lightmass primitive settings.
     *
     * @param   FArchive& archive to serialize to or deserialize from.
     * @param   FLightmassPrimitiveSettings& lightmassSettings to serialize or deserialize.
     */
    void Serialize(FArchive& archive, FLightmassPrimitiveSettings& lightmassSettings);
};