#pragma once

void Print(const FString& Message, const FColor& Color)
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 0), TEXT("Debug Message"), nullptr, FColor::White, 0.0f, true);
}

void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}
