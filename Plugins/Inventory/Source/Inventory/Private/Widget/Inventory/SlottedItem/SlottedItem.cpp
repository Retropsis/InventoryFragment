// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/SlottedItem/SlottedItem.h"
#include "Item/Inv_InventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"

FReply USlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void USlottedItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UInv_InventoryStatics::ItemHovered(GetOwningPlayer(), GetInventoryItem());
}

void USlottedItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());	
}

void USlottedItem::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}

void USlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void USlottedItem::UpdateStackCount(int32 StackCount)
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
