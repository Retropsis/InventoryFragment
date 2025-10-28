// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentManagement/Component/EquipmentComponent.h"

#include "EquipmentManagement/EquipActor/EquipActor.h"
#include "GameFramework/Character.h"
#include "InventoryManagement/Component/Inv_InventoryComponent.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"
#include "Item/Inv_InventoryItem.h"
#include "Item/Fragment/ItemFragment.h"

void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializePlayerController();
}

void UEquipmentComponent::InitializeOwner(APlayerController* PlayerController)
{
	if (IsValid(PlayerController))
	{
		OwningPlayerController = PlayerController;
	}
	InitializeInventoryComponent();
}

void UEquipmentComponent::InitializePlayerController()
{
	if (OwningPlayerController = Cast<APlayerController>(GetOwner()); OwningPlayerController.IsValid())
	{
		if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwningCharacter))
		{
			OnPossessedPawnChanged(nullptr, OwningCharacter);
		}
		else
		{
			OwningPlayerController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
		}
	}
}

void UEquipmentComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (ACharacter* OwningCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwningCharacter))
	{
		OwningSkeletalMesh = OwningCharacter->GetMesh();
	}
	InitializeInventoryComponent();
}

void UEquipmentComponent::InitializeInventoryComponent()
{
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid()) return;

	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}
}

AEquipActor* UEquipmentComponent::SpawnEquippedActor(FEquipmentFragment* EquipmentFragment, const FItemManifest& ItemManifest, USkeletalMeshComponent* AttachMesh)
{
	AEquipActor* SpawnedEquipActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	SpawnedEquipActor->SetEquipmentType(EquipmentFragment->GetEquipmentType());
	SpawnedEquipActor->SetOwner(GetOwner());
	EquipmentFragment->SetEquippedActor(SpawnedEquipActor);
	return SpawnedEquipActor;
}

void UEquipmentComponent::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FEquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FEquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsProxy)
	{
		EquipmentFragment->OnEquip(OwningPlayerController.Get());
	}

	if (!OwningSkeletalMesh.IsValid()) return;
	AEquipActor* SpawnedEquipActor = SpawnEquippedActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());
	
	EquippedActors.Add(SpawnedEquipActor);
}

void UEquipmentComponent::OnItemUnequipped(UInv_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;
	
	FItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FEquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FEquipmentFragment>();
	if (!EquipmentFragment) return;

	if (!bIsProxy)
	{
		EquipmentFragment->OnUnequip(OwningPlayerController.Get());
	}
	
	RemoveEquippedActor(EquipmentFragment->GetEquipmentType());
}

AEquipActor* UEquipmentComponent::FindEquippedActor(const FGameplayTag& Type)
{
	auto FoundActor = EquippedActors.FindByPredicate([&Type] (const AEquipActor* EquippedActor)
	{
		return EquippedActor->GetEquipmentType().MatchesTagExact(Type);
	});
	return FoundActor ? *FoundActor : nullptr;
}

void UEquipmentComponent::RemoveEquippedActor(const FGameplayTag& Type)
{
	if (AEquipActor* EquippedActor = FindEquippedActor(Type); IsValid(EquippedActor))
	{
		EquippedActors.Remove(EquippedActor);
		EquippedActor->Destroy();
	}
}