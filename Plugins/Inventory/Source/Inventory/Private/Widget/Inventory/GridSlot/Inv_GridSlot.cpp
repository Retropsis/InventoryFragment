// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/GridSlot/Inv_GridSlot.h"
#include "Item/Inv_InventoryItem.h"
#include "Widget/ItemPopUp/ItemPopUp.h"
#include "Components/Image.h"

void UInv_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	GridSlotHovered.Broadcast(TileIndex, InMouseEvent);
}

void UInv_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	GridSlotUnhovered.Broadcast(TileIndex, InMouseEvent);
}

FReply UInv_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	GridSlotClicked.Broadcast(TileIndex, InMouseEvent);
	return FReply::Handled();
}

void UInv_GridSlot::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UInv_GridSlot::SetItemPopUp(UItemPopUp* PopUp)
{
	ItemPopUp = PopUp;
	ItemPopUp->SetGridIndex(GetTileIndex());
	ItemPopUp->OnNativeDestruct.AddUObject(this, &ThisClass::OnItemPopUpDestruct);
}

void UInv_GridSlot::OnItemPopUpDestruct(UUserWidget* Menu)
{
	ItemPopUp.Reset();
}

UItemPopUp* UInv_GridSlot::GetItemPopUp() const
{
	return ItemPopUp.Get(); 
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EGridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EGridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
	GridSlotState = EGridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
	GridSlotState = EGridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}
