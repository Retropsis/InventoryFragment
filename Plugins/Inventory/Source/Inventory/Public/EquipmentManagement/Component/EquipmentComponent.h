// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

struct FGameplayTag;
struct FItemManifest;
struct FEquipmentFragment;
class AEquipActor;
class UInv_InventoryItem;
class UInv_InventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetOwningSkeletalMesh(USkeletalMeshComponent* OwningMesh) { OwningSkeletalMesh = OwningMesh; }
	void SetIsProxy(bool bProxy) { bIsProxy = bProxy; }
	void InitializeOwner(APlayerController* PlayerController);

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

	UPROPERTY()
	TArray<TObjectPtr<AEquipActor>> EquippedActors;

	bool bIsProxy{false};

	UFUNCTION()
	void OnItemEquipped(UInv_InventoryItem* EquippedItem);

	UFUNCTION()
	void OnItemUnequipped(UInv_InventoryItem* UnequippedItem);

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void InitializePlayerController();
	void InitializeInventoryComponent();
	AEquipActor* SpawnEquippedActor(FEquipmentFragment* EquipmentFragment, const FItemManifest& ItemManifest, USkeletalMeshComponent* AttachMesh);
	AEquipActor* FindEquippedActor(const FGameplayTag& Type);
	void RemoveEquippedActor(const FGameplayTag& Type);
};
