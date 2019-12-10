#pragma once

#include "sfBaseActivity.h"
#include <InstancedFoliageActor.h>

/**
 * Activity that removes random foliage.
 */
class sfRemoveFoliageActivity : public sfBaseActivity
{
public:
    /**
     * Constructor
     *
     * @param   const FString& name of activity.
     * @param   float weight the determines how likely this activity is to occur.
     */
    sfRemoveFoliageActivity(const FString& name, float weight);

    /**
     * Selects a random index to remove foliage from.
     */
    virtual void Start() override;

    /**
     * Called every tick to remove foliage.
     *
     * @param   float deltaTime in seconds since the last tick.
     */
    virtual void Tick(float deltaTime) override;

private:
    AInstancedFoliageActor* m_actorPtr;
    UFoliageType* m_typePtr;
    int m_index;
    int m_count;
};