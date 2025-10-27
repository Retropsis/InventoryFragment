// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/SlottedItem/EquippedSlottedItem.h"

FReply UEquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}
