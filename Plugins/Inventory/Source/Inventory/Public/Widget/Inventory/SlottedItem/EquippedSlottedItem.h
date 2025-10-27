// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SlottedItem.h"
#include "EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, UEquippedSlottedItem*, EquippedSlottedItem);

UCLASS()
class INVENTORY_API UEquippedSlottedItem : public USlottedItem
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	void SetEquipmentType(const FGameplayTag& Type) { EquipmentType = Type; }
	FGameplayTag GetEquipmentType() const { return EquipmentType; }

	FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;

private:
	UPROPERTY()
	FGameplayTag EquipmentType;
};
