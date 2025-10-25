// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/Spatial/Inv_SpatialInventory.h"
#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"
#include "Widget/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widget/ItemDescription/ItemDescription.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);

	ShowEquippables();
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription)) return;

	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UItemDescription* Description, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);

	FVector2D ClampedPosition = UInv_WidgetUtiliies::GetClampedWidgetPosition(
		UInv_WidgetUtiliies::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer())
	);
	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

FSlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent)
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComponent(ItemComponent))
	{
	case EInv_ItemCategory::Equippable:
		 return Grid_Equippables->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Consumable:
		 return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EInv_ItemCategory::Craftable:
		 return Grid_Craftables->HasRoomForItem(ItemComponent);
	default:
		UE_LOG(LogInventory, Error, TEXT("Item Component doesn't have an Item Category"));
		return FSlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	UItemDescription* ItemDescriptionWidget = GetItemDescription();
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionTimer);

	FTimerDelegate ItemDescriptionDelegate;
	ItemDescriptionDelegate.BindLambda([this] ()
	{
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(ItemDescriptionTimer, ItemDescriptionDelegate, ItemDescriptionDelay, false);
}

void UInv_SpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionTimer);
}

UItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;
	return false;
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);	
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);	
}

void UInv_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid()) ActiveGrid->HideCursor();
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}
