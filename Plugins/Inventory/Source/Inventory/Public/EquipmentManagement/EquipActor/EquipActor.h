// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "EquipActor.generated.h"

UCLASS()
class INVENTORY_API AEquipActor : public AActor
{
	GENERATED_BODY()

public:
	AEquipActor();
	FGameplayTag GetEquipmentType() const { return EquipmentType; }
	void SetEquipmentType(FGameplayTag Type) { EquipmentType = Type; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag EquipmentType;
};
