#pragma once

#include <CoreMinimal.h>
#include <Engine/DecalActor.h>
#include <Components/BillboardComponent.h>
#include <Components/ArrowComponent.h>
#include "sfDecalActor.generated.h"

/**
 * A decal actor that cannot be selected in the editor.
 */
UCLASS()
class AsfDecalActor : public ADecalActor
{
    GENERATED_BODY()
public:

    /**
     * Constructor
     *
     * @param   const FObjectInitializer& objectInitializer
     */
    AsfDecalActor(const FObjectInitializer& objectInitializer);

    /**
     * Overrides IsSelectable to always return false.
     *
     * @return  bool false
     */
    virtual bool IsSelectable() const override;
};