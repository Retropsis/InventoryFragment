
#include "Item/Fragment/ItemFragment.h"

#include "EquipmentManagement/EquipActor/EquipActor.h"
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

void FStrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Strength increased by %f"), GetValue()));	
}

void FStrengthModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Strength decreased by %f"), GetValue()));	
}

void FArmorModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item equipped. Armor increased by: %f"), GetValue()));
}

void FArmorModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item unequipped. Armor decreased by: %f"), GetValue()));
}

void FDamageModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item equipped. Damage increased by: %f"), GetValue()));
}

void FDamageModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Item equipped. Damage increased by: %f"), GetValue()));
}

void FEquipmentFragment::Assimilate(UCompositeBase* Composite) const
{
	FInventoryItemFragment::Assimilate(Composite);
	for (const auto& Modifier : EquipmentModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FEquipmentFragment::Manifest()
{
	FInventoryItemFragment::Manifest();
	for (auto& Modifier : EquipmentModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FEquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	
	for (auto& Modifier : EquipmentModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}

void FEquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	
	for (auto& Modifier : EquipmentModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

AEquipActor* FEquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh)) return nullptr;

	AEquipActor* SpawnActor = AttachMesh->GetWorld()->SpawnActor<AEquipActor>(EquipActorClass);
	SpawnActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);
	
	return SpawnActor;
}

void FEquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FEquipmentFragment::SetEquippedActor(AEquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}
