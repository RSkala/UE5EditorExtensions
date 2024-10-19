// Copyright Richard Skala

#include "AssetActions/QuickAssetAction.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "DebugHeader.h"

void UQuickAssetAction::TestFunc()
{
	Print(TEXT("UQuickAssetAction::TestFunc"), FColor::Cyan);
	PrintLog(TEXT("UQuickAssetAction::TestFunc"));
}

void UQuickAssetAction::DuplicateAssets(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		Print(TEXT("Please enter a VALID number"), FColor::Red);
		return;
	}

	TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
	uint32 DuplicatedAssetCounter = 0;

	for (const FAssetData& SelectedAssetData : SelectedAssetsData)
	{
		for (int32 i = 0; i < NumOfDuplicates; ++i)
		{
			// Get the path of the current asset
			const FString SourceAssetPath = SelectedAssetData.ObjectPath.ToString();

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
		Print(TEXT("Successfully duplicated" + FString::FromInt(DuplicatedAssetCounter) + " asset files."), FColor::Green);
	}
	else
	{
		Print(TEXT("No assets duplicated"), FColor::Green);
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
