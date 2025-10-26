// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Leaf.h"
#include "Leaf_LabeledValue.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORY_API ULeaf_LabeledValue : public ULeaf
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	void SetLabelText(const FText& Text, bool bCollapse) const;
	void SetValueText(const FText& Text, bool bCollapse) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Label;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Value;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 FontSize_Label{12};

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 FontSize_Value{18};
};
