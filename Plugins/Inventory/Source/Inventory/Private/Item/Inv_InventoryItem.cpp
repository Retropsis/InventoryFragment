// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Inv_InventoryItem.h"
#include "Net/UnrealNetwork.h"

void UInv_InventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
}

void UInv_InventoryItem::SetItemManifest(const FItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FItemManifest>(Manifest);
}
