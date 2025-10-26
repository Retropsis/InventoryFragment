
#include "Item/Fragment/ItemFragment.h"
#include "Widget/Composite/CompositeBase.h"
#include "Widget/Composite/Leaf_Image.h"
#include "Widget/Composite/Leaf_LabeledValue.h"
#include "Widget/Composite/Leaf_Text.h"

void FInventoryItemFragment::Assimilate(UCompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FInventoryItemFragment::MatchesWidgetTag(const UCompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FImageFragment::Assimilate(UCompositeBase* Composite) const
{
	FInventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	ULeaf_Image* Image = Cast<ULeaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(Dimensions);
	Image->SetImageSize(Dimensions);
}

void FTextFragment::Assimilate(UCompositeBase* Composite) const
{
	FInventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	ULeaf_Text* LeafText = Cast<ULeaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(FragmentText);	
}

void FLabeledNumberFragment::Assimilate(UCompositeBase* Composite) const
{
	FInventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	ULeaf_LabeledValue* LabeledValue = Cast<ULeaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetLabelText(Text_Label, bCollapseLabel);
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	LabeledValue->SetValueText(FText::AsNumber(Value, &Options), bCollapseValue);	
}

void FLabeledNumberFragment::Manifest()
{
	FInventoryItemFragment::Manifest();

	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	bRandomizeOnManifest = false;
}

void FConsumableFragment::Assimilate(UCompositeBase* Composite) const
{
	FInventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : ConsumeModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnConsume(PC);
	}
}

void FConsumableFragment::Manifest()
{
	FInventoryItemFragment::Manifest();
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FHealthPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Healed for %f HP"), GetValue()));	
}

void FManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Refilled for %f MP"), GetValue()));	
}
