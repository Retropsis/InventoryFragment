// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ItemDescription/ItemDescription.h"
#include "Components/SizeBox.h"

void UItemDescription::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	for (auto Child : GetChildren())
	{
		Child->Collapse();	
	}
}

FVector2D UItemDescription::GetBoxSize() const
{
	return SizeBox_Root->GetDesiredSize();
}
