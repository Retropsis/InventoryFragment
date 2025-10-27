// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/GridSlot/EquippedGridSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widget/Inventory/SlottedItem/EquippedSlottedItem.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"
#include "Item/Inv_InventoryItem.h"
#include "Item/Fragment/FragmentTags.h"
#include "Item/Fragment/ItemFragment.h"
#include "Widget/Inventory/HoverItem/HoverItem.h"

void UEquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;

	UHoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentType))
	{
		SetOccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UEquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!IsAvailable()) return;

	UHoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentType))
	{
		SetUnoccupiedTexture();
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UEquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentType);
	return FReply::Handled();
}

UEquippedSlottedItem* UEquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item, const FGameplayTag& Type, float TileSize)
{
	// Check EquipmentType
	if (!Type.MatchesTagExact(EquipmentType)) return nullptr;
	
	// Get Grid Dimensions
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(Item, Fragment::Grid);
	if (!GridFragment) return nullptr;
	const FIntPoint GridDimensions = GridFragment->GetGridSize();
	
	// Calculate the Draw Size
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = GridDimensions * IconTileWidth;
	
	// Create the Widget
	EquippedSlottedItem = CreateWidget<UEquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
	
	// Set the slotted item with inventory item, equipment type, hide the stack count
	EquippedSlottedItem->SetInventoryItem(Item);
	EquippedSlottedItem->SetEquipmentType(Type);
	EquippedSlottedItem->SetIsStackable(false);
	EquippedSlottedItem->UpdateStackCount(0);
	
	// Set Inventory Item on this class
	SetInventoryItem(Item);
	
	// Set Image Brush on EquippedSlotted
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(Item, Fragment::Icon);
	if (!ImageFragment) return nullptr;

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);
	
	// Add the slotted item as a child of this widget overlay
	Overlay_Root->AddChildToOverlay(EquippedSlottedItem);
	FGeometry Geometry = Overlay_Root->GetCachedGeometry();
	FVector2f OverlayPosition = Geometry.Position;
	auto OverlaySize = Geometry.GetLocalSize();
	const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
	const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;

	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));
	
	return EquippedSlottedItem;
}
