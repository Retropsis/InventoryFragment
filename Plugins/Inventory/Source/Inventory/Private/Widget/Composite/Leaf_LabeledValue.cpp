// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Composite/Leaf_LabeledValue.h"
#include "Components/TextBlock.h"

void ULeaf_LabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfo_Label = Text_Label->GetFont();
	FontInfo_Label.Size = FontSize_Label;
	Text_Label->SetFont(FontInfo_Label);
	
	FSlateFontInfo FontInfo_Value = Text_Value->GetFont();
	FontInfo_Value.Size = FontSize_Value;
	Text_Value->SetFont(FontInfo_Value);
	
}

void ULeaf_LabeledValue::SetLabelText(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Label->SetText(Text);
}

void ULeaf_LabeledValue::SetValueText(const FText& Text, bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Value->SetText(Text);
}
