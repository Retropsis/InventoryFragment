// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/HoverItem/HoverItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/Inv_InventoryItem.h"

void UHoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UHoverItem::UpdateStackCount(const int32 Count)
{
	StackCount = Count;
	if (Count > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(Count));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FGameplayTag UHoverItem::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag();
}

void UHoverItem::SetIsStackable(bool bStackable)
{
	bIsStackable = bStackable;
	if (!bStackable)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UInv_InventoryItem* UHoverItem::GetInventoryItem() const
{
	return InventoryItem.Get(); 
}

void UHoverItem::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}
