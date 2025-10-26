// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/Composite/Composite.h"
#include "ItemDescription.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class INVENTORY_API UItemDescription : public UComposite
{
	GENERATED_BODY()

public:
	FVector2D GetBoxSize() const;
	
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
