#pragma once

#include "sfUIPanel.h"
#include <Widgets/Views/STableRow.h>

/**
 * Panel that shows the paths of missing assets.
 */
class sfMissingAssetsPanel : public sfUIPanel
{
public:
    /**
     * Constructor
     */
    sfMissingAssetsPanel();

    /**
     * @return  int number of missing assets.
     */
    int NumMissingAssets();

    /**
     * Adds a missing asset path to display.
     *
     * @param   const FString& path
     */
    void AddMissingPath(const FString& path);

    /**
     * Removes a missing asset path from the display.
     *
     * @param   const FString& path
     */
    void RemoveMissingPath(const FString& path);

    /**
     * Refreshes the message box displaying the number of missing assets.
     */
    void RefreshMessageBox();

    /**
     * Clears all missing asset paths.
     */
    void Clear();

private:
    TSharedPtr<SListView<TSharedPtr<FString>>> m_missingAssetsListPtr;
    TArray<TSharedPtr<FString>> m_missingAssets;
    TSharedPtr<sfUIMessageBox> m_messageBoxPtr;

};