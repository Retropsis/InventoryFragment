// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Composite/Leaf.h"

void ULeaf::ApplyFunction(FuncType Function)
{
	Function(this);
}
