// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProxyMesh.generated.h"

class UEquipmentComponent;

UCLASS()
class INVENTORY_API AProxyMesh : public AActor
{
	GENERATED_BODY()

public:
	AProxyMesh();
	
	USkeletalMeshComponent* GetMesh() const { return Mesh; }

protected:
	virtual void BeginPlay() override;

private:
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEquipmentComponent> EquipmentComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh;

	FTimerHandle TimerForNextTick;
	void DelayedInitializedOwner();
	void DelayedInitialization();
	
};
