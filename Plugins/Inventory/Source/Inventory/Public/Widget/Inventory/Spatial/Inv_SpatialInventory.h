// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Button.h"
#include "Item/Inv_InventoryItem.h"
#include "Widget/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Inv_SpatialInventory.generated.h"

class UEquippedSlottedItem;
struct FGameplayTag;
class UEquippedGridSlot;
class UItemDescription;
class UCanvasPanel;
class UButton;
class UWidgetSwitcher;
class UInv_InventoryGrid;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FSlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) override;
	virtual void OnItemHovered(UInv_InventoryItem* Item) override;
	virtual void OnItemUnhovered() override;
	virtual bool HasHoverItem() const override;
	virtual UHoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equippables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Craftables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Equippables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Consumables;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Craftables;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UItemDescription> ItemDescription;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UItemDescription> EquippedItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UItemDescription> EquippedItemDescription;

	FTimerHandle ItemDescriptionTimer;
	FTimerHandle EquippedItemDescriptionTimer;

	UPROPERTY()
	TArray<TObjectPtr<UEquippedGridSlot>> EquippedGridSlots;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float ItemDescriptionDelay{ .5f };

	UPROPERTY(EditAnywhere, Category="Inventory")
	float EquippedItemDescriptionDelay{ .5f };

	UFUNCTION()
	void ShowEquippedItemDescription(UInv_InventoryItem* Item);

	UFUNCTION()
	void ShowEquippables();
	
	UFUNCTION()
	void ShowConsumables();
	
	UFUNCTION()
	void ShowCraftables();

	UFUNCTION()
	void EquippedGridSlotClicked(UEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentType);
	
	UFUNCTION()
	void EquippedSlottedItemClicked(UEquippedSlottedItem* EquippedSlottedItem);
	
	void DisableButton(UButton* Button);
	void SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button);
	void SetItemDescriptionSizeAndPosition(UItemDescription* Description, UCanvasPanel* Canvas) const;
	void SetEquippedItemDescriptionSizeAndPosition(UItemDescription* Description, UItemDescription* EquippedDescription, UCanvasPanel* Canvas) const;
	UItemDescription* GetItemDescription();
	UItemDescription* GetEquippedItemDescription();
	bool CanEquipHoverItem(UEquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentType) const;
	UEquippedGridSlot* FindSlotWithEquippedItem(UInv_InventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UEquippedGridSlot* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UEquippedSlottedItem* EquippedSlottedItem);
	void MakeEquippedSlottedItem(UEquippedSlottedItem* EquippedSlottedItem, UEquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip);
	void BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const;

	TWeakObjectPtr<UInv_InventoryGrid> ActiveGrid;
};
