// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Composite/Composite.h"
#include "Blueprint/WidgetTree.h"

void UComposite::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this] (UWidget* Widget)
	{
		if (UCompositeBase* Composite = Cast<UCompositeBase>(Widget); IsValid(Composite))
		{
			Children.Add(Composite);
			Composite->Collapse();
		}
	});
}

void UComposite::ApplyFunction(FuncType Function)
{
	for (auto& Child : Children)
	{
		Child->ApplyFunction(Function);
	}
}

void UComposite::Collapse()
{
	for (auto& Child : Children)
	{
		Child->Collapse();
	}
}
