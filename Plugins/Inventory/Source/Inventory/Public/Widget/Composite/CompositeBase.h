// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "CompositeBase.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UCompositeBase : public UUserWidget
{
	GENERATED_BODY()

public:
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag& Tag) { FragmentTag = Tag; }
	virtual void Collapse();
	void Expand();

	using FuncType = TFunction<void(UCompositeBase*)>;
	virtual void ApplyFunction(FuncType Function) {}
	
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag;
};
