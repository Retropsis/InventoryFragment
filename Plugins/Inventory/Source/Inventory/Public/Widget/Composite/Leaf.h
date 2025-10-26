// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CompositeBase.h"
#include "Leaf.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API ULeaf : public UCompositeBase
{
	GENERATED_BODY()

public:
	virtual void ApplyFunction(FuncType Function) override;
};
