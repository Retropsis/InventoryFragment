// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_ItemComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	FItemManifest GetItemManifest() const { return ItemManifest; }
	FString GetPickupMessage() const { return PickupMessage; }

private:

	UPROPERTY(Replicated, EditAnywhere, Category="Inventory")
	FItemManifest ItemManifest;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	FString PickupMessage;
};
