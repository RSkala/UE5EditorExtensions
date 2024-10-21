#pragma once

#include "Misc/MessageDialog.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"


static void Print(const FString& Message, const FColor& Color)
{
	if (GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.0f, Color, Message);
	}
}

static void PrintLog(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

static EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
	if (bShowMsgAsWarning)
	{
		FText MsgTitle = FText::FromString(TEXT("Warning"));

		//return FMessageDialog::Open(MsgType, FText::FromString(Message), &MsgTitle); // DEPRECATED
		return FMessageDialog::Open(MsgType, FText::FromString(Message), MsgTitle);
	}
	else
	{
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}

static void ShowNotifyInfo(const FString& Message)
{
	FNotificationInfo NotifyInfo(FText::FromString(Message));
	NotifyInfo.bUseLargeFont = true;
	NotifyInfo.FadeOutDuration = 4.0f;

	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}
