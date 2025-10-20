// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Fragment/ItemFragment.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryGrid.generated.h"

struct FGridFragment;
struct FItemManifest;
class USlottedItem;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);
	FSlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }

private:
	void ConstructGrid();
	bool MatchesCategory(const UInv_InventoryItem* Item) const;
	FSlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* InventoryItem);
	FSlotAvailabilityResult HasRoomForItem(const FItemManifest& ItemManifest);
	void AddItemToIndices(const FSlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	void AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	USlottedItem* CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FGridFragment* GridFragment, const FImageFragment* ImageFragment, const int32 Index) const;
	void SetSlottedImage(const USlottedItem* SlottedItem, const FGridFragment* GridFragment, const FImageFragment* ImageFragment) const;
	FVector2D GetDrawSize(const FGridFragment* GridFragment) const;
	void AddSlottedItemToCanvas(const int32 Index, const FGridFragment* GridFragment, USlottedItem* SlottedItem) const;
	void UpdateGridSlots(UInv_InventoryItem* NewItem, int32 Index, const bool bStackable, const int32 StackAmount);

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	
	UPROPERTY(EditAnywhere, Category="Inventory", BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	EInv_ItemCategory ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;

	UPROPERTY()
	TMap<int32, TObjectPtr<USlottedItem>> SlottedItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<USlottedItem> SlottedItemClass;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize;
};
