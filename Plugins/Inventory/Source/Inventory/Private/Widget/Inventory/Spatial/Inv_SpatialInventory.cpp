// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/Spatial/Inv_SpatialInventory.h"
#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Component/Inv_InventoryComponent.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"
#include "Item/Inv_InventoryItem.h"
#include "Widget/Inventory/GridSlot/EquippedGridSlot.h"
#include "Widget/Inventory/HoverItem/HoverItem.h"
#include "Widget/Inventory/SlottedItem/EquippedSlottedItem.h"
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

	WidgetTree->ForEachWidget([this] (UWidget* Widget)
	{
		if (UEquippedGridSlot* EquippedGridSlot = Cast<UEquippedGridSlot>(Widget); IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

void UInv_SpatialInventory::EquippedGridSlotClicked(UEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentType)
{
	// Check to see if we can equip the Hover Item
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentType)) return;

	// Create an Equipped Slotted Item and add it to the Equipped Grid Slot
	const float TileSize = UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	UHoverItem* HoverItem = GetHoverItem();
	UEquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(HoverItem->GetInventoryItem(), EquipmentType, TileSize);
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
		
	// Clear the Hover Item
	Grid_Equippables->ClearHoverItem();
	
	// Inform the server that we've equipped an item (potentially unequipping an item as well)
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquippedSlottedItemClicked(HoverItem->GetInventoryItem(), nullptr);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
	
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UEquippedSlottedItem* EquippedSlottedItem)
{
	// Remove the Item Description
	UInv_InventoryStatics::ItemUnhovered(GetOwningPlayer());

	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;
	
	// Get Item to Equip
	UInv_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
	
	// Get Item to Unequip
	UInv_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();
		
	// Get the Equipped Grid Slot holding this item
	UEquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	
	// Clear the equipped grid slot of this item (set its inventory item to nullptr)
	ClearSlotOfItem(EquippedGridSlot);
	
	// Assign previously equipped item as the hover item
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	
	// Remove of the equipped slotted item from the equipped grid slot
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	
	// Make a new equipped slotted item (for the item we held in HoverItem)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	
	// Broadcast delegates for OnItemEquipped/OnItemUnequipped (from the IC)
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const
{
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquippedSlottedItemClicked(ItemToEquip, ItemToUnequip);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UEquippedSlottedItem* EquippedSlottedItem, UEquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;

	UEquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(ItemToEquip, EquippedSlottedItem->GetEquipmentType(), UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize());
	if (IsValid(SlottedItem)) SlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::UInv_SpatialInventory::EquippedSlottedItemClicked);
	EquippedGridSlot->SetEquippedSlotItem(SlottedItem);
}

void UInv_SpatialInventory::RemoveEquippedSlottedItem(UEquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UInv_SpatialInventory::ClearSlotOfItem(UEquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlotItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

UEquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem] (const UEquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == EquippedItem;
	});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

bool UInv_SpatialInventory::CanEquipHoverItem(UEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentType) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	UHoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	return HasHoverItem() && IsValid(HeldItem) && !HoverItem->IsStackable() &&
		HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable &&
			HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentType);
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
	const auto& Manifest = Item->GetItemManifest();
	UItemDescription* ItemDescriptionWidget = GetItemDescription();
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(ItemDescriptionTimer);

	FTimerDelegate ItemDescriptionDelegate;
	ItemDescriptionDelegate.BindLambda([this, &Manifest, ItemDescriptionWidget] ()
	{
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
		Manifest.AssimilateInventoryFragments(ItemDescriptionWidget);
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

UHoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
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
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}
