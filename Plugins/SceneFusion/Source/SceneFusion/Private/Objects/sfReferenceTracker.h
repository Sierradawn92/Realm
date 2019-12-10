#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "sfReferenceTracker.generated.h"

/**
 * Prevents tracked uobjects from being garbage collected as long as their associated sfObject is referenced by a
 * synced property.
 */
UCLASS()
class UsfReferenceTracker : public UObject
{
	GENERATED_BODY()
public:
    /**
     * @return  UsfReferenceTracker& singleton instance. Create an instance if it does not exist.
     */
    static UsfReferenceTracker& Get();

    /**
     * Destroys the singleton instance.
     */
    static void CleanUp();

    /**
     * Adds an object to the set of tracked objects that will not be garbaged collected as long as their associated
     * sfObject is referenced by a synced property.
     *
     * @param   UObject* uobjPtr to track.
     */
    void Track(UObject* uobjPtr);

    /**
     * Removes objects in the given level from the tracker, allowing them to be garbage collected.
     *
     * @param   ULevel* levelPtr. Objects in this level will be removed from the tracker.
     */
    void RemoveObjectsInLevel(ULevel* levelPtr);

    /**
     * Checks for tracked uobjects with unreferenced sfObjects, and removes them from the tracked set so they can be
     * garbage collected.
     */
    void CollectGarbage();

private:
    UPROPERTY() // Making this a UPROPERTY stops the objects from being garbage collected.
    TSet<UObject*> m_references;
    
    static UsfReferenceTracker* m_instancePtr;
};
