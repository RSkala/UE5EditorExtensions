// Copyright Richard Skala

#include "AssetActions/QuickAssetAction.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "AssetViewUtils.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/MessageDialog.h"
#include "ObjectTools.h"

#include "DebugHeader.h"

void UQuickAssetAction::TestFunc()
{
	DebugHeader::Print(TEXT("UQuickAssetAction::TestFunc"), FColor::Cyan);
	DebugHeader::PrintLog(TEXT("UQuickAssetAction::TestFunc"));
}

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a VALID number"));
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 DuplicatedAssetCounter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; ++i)
		{
			// Get the path of the current asset
			//const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString(); // DEPRECATED
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();

			// Get the name of the current asset and append "_x" to the end
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);

			// Get the full path of the current asset folder. Need to combine so the path has slashes.
			const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

			// Duplicate the asset
			UObject* NewDuplicatedAsset = UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName);
			if (NewDuplicatedAsset != nullptr)
			{
				// We have successfully duplicated the asset, save the asset immediately
				UEditorAssetLibrary::SaveAsset(NewPathName, false);

				// Increment the counter
				++DuplicatedAssetCounter;
			}
		}
	}

	if (DuplicatedAssetCounter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully duplicated" + FString::FromInt(DuplicatedAssetCounter) + " asset files."));
	}
}

void UQuickAssetAction::TestCallInEditorString(FString TestString)
{
	UE_LOG(LogTemp, Warning, TEXT("User entered: %s"), *TestString);
}

void UQuickAssetAction::PrintViewportAspectRatio()
{
	if (GEditor == nullptr)
	{
		return;
	}

	FViewport* ActiveViewport = GEditor->GetActiveViewport(); // Need "UnrealEd" as dependency in order to use GEditor and GetActiveViewport()
	if (ActiveViewport != nullptr)
	{
		FIntPoint ViewportSizeXY = ActiveViewport->GetSizeXY();
		UE_LOG(LogTemp, Warning, TEXT("ViewportSizeXY: %s"), *ViewportSizeXY.ToString());

		float ViewportAspectRatio = 0.0f;
		if (ViewportSizeXY.Y != 0.0f)
		{
			ViewportAspectRatio = (float)ViewportSizeXY.X / (float)ViewportSizeXY.Y;
		}
		UE_LOG(LogTemp, Warning, TEXT("ViewportAspectRatio: %f"), ViewportAspectRatio);
	}

	// --------------------------
	FWorldContext& EditorWorldContext = GEditor->GetEditorWorldContext();
	UWorld* EditorWorld = EditorWorldContext.World();
	if (EditorWorld != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Editor World Name:    %s"), *EditorWorld->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("Current Editor World MapName: %s"), *EditorWorld->GetMapName());

		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(EditorWorld, ACameraActor::StaticClass(), FoundActors);
		UE_LOG(LogTemp, Warning, TEXT("FoundActors: %d"), FoundActors.Num());
		for (AActor* FoundActor : FoundActors)
		{
			FString FoundCameraActorName = FoundActor != nullptr ? FoundActor->GetName() : "(invalid)";
			UE_LOG(LogTemp, Warning, TEXT("Found Camera Actor: %s"), *FoundCameraActorName);

			ACameraActor* FoundCameraActor = Cast<ACameraActor>(FoundActor);
			if (FoundCameraActor != nullptr)
			{
				UCameraComponent* CameraComponent = FoundCameraActor->GetCameraComponent();
				if (CameraComponent != nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("Camera ProjectionMode: %d"), CameraComponent->ProjectionMode);
					UE_LOG(LogTemp, Warning, TEXT("Camera FOV:            %f"), CameraComponent->FieldOfView);
					UE_LOG(LogTemp, Warning, TEXT("Camera AspectRatio:    %f"), CameraComponent->AspectRatio);

					//float TestNewAspectRatio = CameraComponent->AspectRatio + 1.0f;
					//CameraComponent->SetAspectRatio(TestNewAspectRatio);

					//float TestNewFOV = CameraComponent->FieldOfView + 5.0f;
					//CameraComponent->SetFieldOfView(TestNewFOV);
				}
			}
		}
	}
}

void UQuickAssetAction::AddPrefixes()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;

	for (UObject* SelectedAsset : SelectedAssets)
	{
		if (SelectedAsset != nullptr)
		{
			FString* FoundPrefix = PrefixMap.Find(SelectedAsset->GetClass());
			if (FoundPrefix == nullptr || FoundPrefix->IsEmpty())
			{
				// This means we do not have a pre-set value for this class
				DebugHeader::Print(TEXT("Failed to find prefix for class ") + SelectedAsset->GetClass()->GetName(), FColor::Red);
			}
			else
			{
				FString OldName = SelectedAsset->GetName();
				if (OldName.StartsWith(*FoundPrefix))
				{
					DebugHeader::Print(OldName + TEXT(" already has prefix added"), FColor::Red);
				}
				else
				{
					// Now, we have found a prefix AND the asset does not have the appropriate prefix

					// Handle special case for MaterialInstance assets
					if (SelectedAsset->IsA<UMaterialInstanceConstant>())
					{
						if (OldName.StartsWith("M_"))
						{
							OldName.RemoveFromStart("M_");
						}

						if (OldName.EndsWith("_Inst"))
						{
							OldName.RemoveFromEnd("_Inst");
						}
					}

					// Create the new name using the appropriate prefix prepended to the old name
					const FString NewNameWithPrefix = *FoundPrefix + OldName;

					// Rename the asset
					UEditorUtilityLibrary::RenameAsset(SelectedAsset, NewNameWithPrefix);

					// Increment counter
					++Counter;
				}
			}
		}
	}

	if (Counter > 0)
	{
		DebugHeader::ShowNotifyInfo(TEXT("Successfully renamed ") + FString::FromInt(Counter) + " assets");
	}
}

void UQuickAssetAction::RemoveUnusedAssets()
{
	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	TArray<FAssetData> UnusedAssetsData;

	// First, fix up Redirectors
	FixUpRedirectors();

	// NOTE: This does NOT work in UE5.4. The standard delete dialog appears.
	/*
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FString> AssetReferencers;
		UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString(), false);
		if (AssetReferencers.Num() == 0)
		{
			// The current asset has NO assets referencing it. Add it to the "Unused" asset data list
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}
	*/
	
	FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		TArray<FName> AssetDependencies;
		AssetRegistry.Get().GetDependencies(SelectedAssetData.PackageName, AssetDependencies);
		if (AssetDependencies.Num() == 0)
		{
			UnusedAssetsData.Add(SelectedAssetData);
		}
	}

	if (UnusedAssetsData.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found among selected assets"), false);
		return;
	}

	// At least one unused asset has been found

	// Note: We CAN use the UEditorAssetLibrary DeleteAsset functions, but they are FORCE deletes without any user confirmation
	const int32 NumAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData, true);
	if (NumAssetsDeleted == 0)
	{
		// No assets were deleted
		return;
	}

	// At least one asset was deleted
	DebugHeader::ShowNotifyInfo(TEXT("Successfully deleted ") + FString::FromInt(NumAssetsDeleted) + " unused assets.");
}

void UQuickAssetAction::FixUpRedirectors()
{
	//FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();

	// Create an Asset Registry Filter from the paths
	FARFilter ARFilter;
	ARFilter.bRecursivePaths = true;
	ARFilter.PackagePaths.Emplace("/Game");

	ARFilter.ClassPaths.Add(UObjectRedirector::StaticClass()->GetClassPathName());

	// Query for a list of assets in the selected paths
	TArray<FAssetData> AssetList;
	AssetRegistry.GetAssets(ARFilter, AssetList);

	if (AssetList.Num() == 0)
	{
		return;
	}

	TArray<FString> ObjectPaths;
	for (const FAssetData& Asset : AssetList)
	{
		ObjectPaths.Add(Asset.GetObjectPathString());
	}

	TArray<UObject*> Objects;
	const bool bAllowToPromptToLoadAssets = true;
	const bool bLoadRedirects = true;

	AssetViewUtils::FLoadAssetsSettings LoadAssetsSettings;
	LoadAssetsSettings.bFollowRedirectors = false;
	LoadAssetsSettings.bAllowCancel = true;

	AssetViewUtils::ELoadAssetsResult LoadAssetsResult = AssetViewUtils::LoadAssetsIfNeeded(ObjectPaths, Objects, LoadAssetsSettings);

	if (LoadAssetsResult != AssetViewUtils::ELoadAssetsResult::Cancelled)
	{
		// Transform Objects array to ObjectRedirectors array
		TArray<UObjectRedirector*> RedirectorsToFix;
		for (UObject* Object : Objects)
		{
			RedirectorsToFix.Add(CastChecked<UObjectRedirector>(Object));
		}

		// Load the asset tools module (used for fixing up the Redirectors)
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		AssetToolsModule.Get().FixupReferencers(RedirectorsToFix);
	}
}
