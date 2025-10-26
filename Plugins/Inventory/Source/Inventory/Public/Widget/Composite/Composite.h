// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CompositeBase.h"
#include "Composite.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UComposite : public UCompositeBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void ApplyFunction(FuncType Function) override;
	virtual void Collapse() override;

private:
	UPROPERTY()
	TArray<TObjectPtr<UCompositeBase>> Children;
};
