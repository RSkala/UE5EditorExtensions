// Copyright Richard Skala

#include "AssetActions/QuickAssetAction.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DebugHeader.h"

void UQuickAssetAction::TestFunc()
{
	Print(TEXT("UQuickAssetAction::TestFunc"), FColor::Cyan);
	PrintLog(TEXT("UQuickAssetAction::TestFunc"));
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
