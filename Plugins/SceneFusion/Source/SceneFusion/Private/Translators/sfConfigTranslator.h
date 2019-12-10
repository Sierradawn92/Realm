#pragma once

#include "sfBaseTranslator.h"

using namespace KS::SceneFusion2;

/**
 * Manages syncing of shared config settings that are set by the session creator.
 */
class sfConfigTranslator : public sfBaseTranslator
{
protected:
    /**
     * Initialization. Called after connecting to a session.
     */
    virtual void Initialize() override;

    /**
     * Called when a config object is created by another user.
     *
     * @param   sfObject::SPtr objPtr that was created.
     * @param   int childIndex of new object. -1 if object is a root.
     */
    virtual void OnCreate(sfObject::SPtr objPtr, int childIndex) override;
};