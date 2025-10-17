// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/Inv_InfoMessage.h"
#include "Components/TextBlock.h"

void UInv_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Text_Message->SetText(FText::GetEmpty());
	HideMessage();
}

void UInv_InfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);

	if (!bIsMessageActive)
	{
		ShowMessage();
	}
	bIsMessageActive = true;
	
	TWeakObjectPtr<UInv_InfoMessage> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(MessageTimer, [WeakThis] ()
	{
		
		if (const auto StrongThis = WeakThis.Pin())
		{
		   StrongThis->HideMessage();
		   StrongThis->bIsMessageActive = false;
		}
	}, MessageLifeTime, false);
}
