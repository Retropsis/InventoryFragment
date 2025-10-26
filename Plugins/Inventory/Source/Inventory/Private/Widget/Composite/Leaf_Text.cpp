// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Composite/Leaf_Text.h"
#include "Components/TextBlock.h"

void ULeaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo = Text_LeafText->GetFont();
	FontInfo.Size = FontSize;
	Text_LeafText->SetFont(FontInfo);
}

void ULeaf_Text::SetText(const FText& Text) const
{
	Text_LeafText->SetText(Text);
}
