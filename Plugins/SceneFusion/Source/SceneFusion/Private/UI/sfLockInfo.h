#pragma once

#include <CoreMinimal.h>
#include <Widgets/SBoxPanel.h>

#include "../Translators/sfActorTranslator.h"

/**
 * Lock info.
 */
class sfLockInfo
{
public:
    sfActorTranslator::LockType LockType;
    sfUser::SPtr LockOwner;
    TSharedPtr<SHorizontalBox> Icon;

    /**
     * Constructor.
     */
    sfLockInfo();

private:
    /**
     * Gets lock icon for the given actor.
     *
     * @return  const FSlateBrush*
     */
    const FSlateBrush* GetLockIcon() const;

    /**
     * Gets lock color for the given actor.
     *
     * @return  FSlateColor
     */
    FSlateColor GetLockColor() const;

    /**
     * Gets lock tool tip for the given actor.
     *
     * @return  FText
     */
    FText GetLockTooltip() const;
};
