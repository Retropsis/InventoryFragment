// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Leaf.h"
#include "Leaf_Text.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORY_API ULeaf_Text : public ULeaf
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	void SetText(const FText& Text) const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_LeafText;

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 FontSize{12};
};
