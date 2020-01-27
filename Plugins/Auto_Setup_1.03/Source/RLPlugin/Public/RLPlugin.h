// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetRegistryModule.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "CCImportUI.h"
#include "Editor/UnrealEd/Classes/Factories/FbxFactory.h"
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/Engine/Classes/Engine/Selection.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceConstant.h"
#include "Runtime/Engine/Classes/Engine/SkeletalMesh.h"
#include "RLTextureData.h"
#include "RLMaterialData.h"
#include "Runtime/Engine/Classes/Animation/Rig.h"
class FToolBarBuilder;
class FMenuBuilder;

class FRLPluginModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /** This function will be bound to Command. */
    void PluginButtonClicked();
    void AutoSetup( TArray<FAssetData>& kAssetDatas,
                    FString strShaderType,
                    TArray<FString>& kLODPathList,
                    bool bIsDragFbx );
    bool CheckAutoSetupVersion( FString strJsonFilePath );

private:
    void AddToolbarExtension( FToolBarBuilder& kBuilder );
    void AddMenuExtension( FMenuBuilder& kBuilder );
    void CheckWorldGridMaterial(FString &strFbxName, FString &rootGamePath, FString ccMaterialFolderGamePath, UStaticMesh *pMesh, bool isMaterialInstance);
    bool checkShaderTypeChange(FString &strfbxName, FString shaderType, USkeletalMesh *mesh, UMaterial* material, UMaterialInterface* MaterialInterface, int slotID, FString &rootGamePath, bool isMaterialInstance );
    void AssignGameBaseLODMaterial( TArray<FString> &OutFiles, int k, FString &rootPath, FString &texturesRootPath, FString &texturesGameRootPath, USkeletalMesh * mesh );
    void AssignLODMaterial(FString &strFbxName, TMap< FString, RLMaterialData > &MaterialMap, TArray<FString> &OutFiles, int k, FString &rootPath, FString &texturesRootPath, FString &texturesGameRootPath, USkeletalMesh * mesh, bool isMaterialInstance );
    void DeleteTextureFile( FString & deleteFloderPath, FString &checkFolderPath );
    void showInfo( FString message, float expireDuration );
    bool checkGamePathFileExists( FString path );
    FString checkBoneType( FAssetData assetData );
    bool CreateFolder( FString &path );
    void CreateTexturesPathList( FString &rootGamePath, TArray <FString> & texturesPathList );
    void MoveMaterialFile( FString &sourceFolderPath, FString &targetFolderPath, FString &strFbxName, FString &rootGamePath, USkeletalMesh *mesh, bool isMaterialInstance );

    void MoveStaticMaterialFile( FString &sourceFolderPath, FString &targetFolderPath, FString &strFbxName, FString &rootGamePath, UStaticMesh *mesh, bool isMaterialInstance );
    void MoveTextureFile( FString &targetFolderPath );
    void findLODGroupMaterialJson(TMap< FString, RLMaterialData > &kMaterialMap, FString &strFbxName, FString &strRootGamePath, USkeletalMesh *kMesh, bool bIsMaterialInstance);
    void findLODGroupMaterial(FString &rootGamePath, USkeletalMesh *mesh );
    UMaterialInterface* AssignMaterialInstanceJson( TMap< FString, RLMaterialData > &MaterialMap, TArray <FString> & texturesPathList, UMaterial* material, UMaterialInterface* MaterialInterface, FString texturesFilesGamePath, FString texturesFilesGamePathFbm, FString ccMaterialFolderGamePath, FString boneType, FString shaderType, bool isMaterialInstance );
    void PhysicIniPaser( FString iniPath );
    FString GetTexturePath( RLMaterialData *kMaterialData, FString strKey, FString strTexturesFloderPath, FString materialName );
    void SetTextureParameter( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance );

    void SetUseORM( bool bEnable, UMaterialInstanceConstant* kMaterialInstance );
    void SetBaseColor( RLMaterialData *kMaterialData,
                       FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePathFbm, FString materialName );
    void SetNormal( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePathFbm, FString materialName );
    void SetSpecular( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePathFbm, FString materialName, bool isPBR );
    void SetOpacity( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePathFbm, FString materialName );
    void SetGlow( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName );
    void SetBlend( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName );
    void SetDisplacement( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName );
    void SetAO( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName );
    void SetRoughness( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName, bool isPBR );
    void SetMetallic( RLMaterialData *kMaterialData, FString strKey, UMaterialInstanceConstant* kMaterialInstance, TArray<FString> texturesPathList, FString texturesFilesGamePath, FString materialName, bool isPBR );
    TSharedPtr< class FUICommandList > m_kPluginCommands;
    static URig* m_pEngineHumanoidRig;
    bool m_bIsMaterialInstance = true;
    FString const MATERIAL_FLODER_NAME = "CC_Auto_Setup_Material_Instances";
    //FString const MATERIAL_FLODER_NAME = "RL_Materials";
};