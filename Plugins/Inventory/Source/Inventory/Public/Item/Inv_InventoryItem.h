// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Item/Manifest/Inv_ItemManifest.h"
#include "Inv_InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryItem : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	
	void SetItemManifest(const FItemManifest& Manifest);
	const FItemManifest& GetItemManifest() const { return ItemManifest.Get<FItemManifest>(); }
	FItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FItemManifest>(); }

private:
	UPROPERTY(VisibleAnywhere, meta=(BaseStruct="/Script/Inventory.Inv_ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;
};
