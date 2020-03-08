#pragma once
#include "RLTextureData.h"

class RLMaterialData
{
public:
    RLMaterialData() {}
    ~RLMaterialData() {}

    FString m_strMaterialType = "";
    bool  m_bTwoSide = false;
    TArray< float > m_kDiffuseColor = { 255.f, 255.f, 255.f };
    TArray< float > m_kAmbientColor = { 50.f, 50.f, 50.f };
    TArray< float > m_kSpecularColor = { 229.f, 229.f, 229.f };
    float m_fOpacity = 1.f;
    float m_fSelfIllumination = 0.f;
    float m_fSpecular = 0.f;
    float m_fGlossiness = 0.f;
    TMap< FString, RLTextureData > m_kTextureDatas;
};