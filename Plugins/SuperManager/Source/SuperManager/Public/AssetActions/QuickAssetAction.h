// Copyright Richard Skala

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"

#include "Blueprint/UserWidget.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceConstant.h"
#include "NiagaraSystem.h"
#include "NiagaraEmitter.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundWave.h"

#include "QuickAssetAction.generated.h"

UCLASS()
class SUPERMANAGER_API UQuickAssetAction : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor)
	void TestFunc();

private:
	// NOTE:
	// * I tried Removing the private to see if these functions refresh without having to recompile the Blueprint child asset.
	// * The TestFunc() always seems to appear, but I wonder if that's due to it 
	//   being the first function created before the blueprint child class was created
	// * Update: Doesn't matter, you still have to re-compile the Blueprint.

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void DuplicateAssets(int32 NumOfDuplicates);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void TestCallInEditorString(FString TestString);

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true)) // AllowPrivateAccess is needed for the dialog to show up when private
	void PrintViewportAspectRatio();

	UFUNCTION(CallInEditor, meta = (AllowPrivateAccess = true))
	void AddPrefixes();

private:
	TMap<UClass*, FString> PrefixMap =
	{
		{UBlueprint::StaticClass(),TEXT("BP_")},
		{UStaticMesh::StaticClass(),TEXT("SM_")},
		{UMaterial::StaticClass(), TEXT("M_")},
		{UMaterialInstanceConstant::StaticClass(),TEXT("MI_")},
		{UMaterialFunctionInterface::StaticClass(), TEXT("MF_")},
		{UParticleSystem::StaticClass(), TEXT("PS_")},
		{USoundCue::StaticClass(), TEXT("SC_")},
		{USoundWave::StaticClass(), TEXT("SW_")},
		{UTexture::StaticClass(), TEXT("T_")},
		{UTexture2D::StaticClass(), TEXT("T_")},
		{UUserWidget::StaticClass(), TEXT("WBP_")},
		{USkeletalMeshComponent::StaticClass(), TEXT("SK_")},
		{UNiagaraSystem::StaticClass(), TEXT("NS_")},
		{UNiagaraEmitter::StaticClass(), TEXT("NE_")}
	};
};
