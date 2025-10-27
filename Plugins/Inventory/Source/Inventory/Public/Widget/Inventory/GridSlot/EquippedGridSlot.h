// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_GridSlot.h"
#include "NativeGameplayTags.h"
#include "EquippedGridSlot.generated.h"

class UOverlay;
class UEquippedSlottedItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquippedGridSlotClicked, UEquippedGridSlot*, GridSlot, const FGameplayTag&, EquipmentType);

UCLASS()
class INVENTORY_API UEquippedGridSlot : public UInv_GridSlot
{
	GENERATED_BODY()

public:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	UEquippedSlottedItem* OnItemEquipped(UInv_InventoryItem* Item, const FGameplayTag& Type, float TileSize);
	void SetEquippedSlotItem(UEquippedSlottedItem* Item) { EquippedSlottedItem = Item; }

	FEquippedGridSlotClicked EquippedGridSlotClicked;

	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="GameItem.Equipment"))
	FGameplayTag EquipmentType;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UEquippedSlottedItem> EquippedSlottedItemClass;

	UPROPERTY()
	TObjectPtr<UEquippedSlottedItem> EquippedSlottedItem;
};
