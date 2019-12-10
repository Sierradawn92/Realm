#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Misc/Guid.h"
#include "LandscapeInfo.h"
#include "sfLandscapeInfoMapHack.generated.h"

/**
 * This is a hack to expose the members of the private class ULandscapeInfoMap by declaring the same members in the
 * same order.
 */
UCLASS()
class UsfLandscapeInfoMapHack : public UObject
{
    GENERATED_BODY()

public:
    TMap<FGuid, ULandscapeInfo*> Map;
    UWorld* World;
};
