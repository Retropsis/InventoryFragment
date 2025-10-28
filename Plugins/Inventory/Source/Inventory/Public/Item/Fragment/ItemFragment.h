#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "ItemFragment.generated.h"

class AEquipActor;
class UCompositeBase;
class APlayerController;

USTRUCT(BlueprintType)
struct FItemFragment
{
	GENERATED_BODY()

	FItemFragment() {}
	FItemFragment(const FItemFragment&) = default;
	FItemFragment& operator=(const FItemFragment&) = default;
	FItemFragment(FItemFragment&&) = default;
	FItemFragment& operator=(FItemFragment&&) = default;
	virtual ~FItemFragment() {}

	virtual void Manifest() {}
	
	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Fragment"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

/*
 * ItemFragment specifically for assimilation into a widget
 */
USTRUCT(BlueprintType)
struct FInventoryItemFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const;

protected:
	bool MatchesWidgetTag(const UCompositeBase* Composite) const;
};

USTRUCT(BlueprintType)
struct FGridFragment : public FItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FIntPoint GridSize{ 1, 1 };

	UPROPERTY(EditAnywhere, Category="Inventory")
	float GridPadding{ 0.f };
};

USTRUCT(BlueprintType)
struct FImageFragment : public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const override;
	UTexture2D* GetIcon() const { return Icon; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};

	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D Dimensions{ 44.f, 44.f };
};

USTRUCT(BlueprintType)
struct FTextFragment : public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const override;
	void SetText(const FText& Text) { FragmentText = Text; }
	FText GetText() const { return FragmentText; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FText FragmentText;
};

USTRUCT(BlueprintType)
struct FLabeledNumberFragment : public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const override;
	virtual void Manifest() override;
	
	void SetLabelText(const FText& Text) { Text_Label = Text; }
	FText GetLabelText() const { return Text_Label; }
	void SetValue(float NewValue) { Value = NewValue; }
	float GetValue() const { return Value; }

	bool bRandomizeOnManifest{true};

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	FText Text_Label;
	
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	float Value{0.f};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float Min{0.f};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float Max{0.f};

	UPROPERTY(EditAnywhere, Category="Inventory")
	bool bCollapseLabel{false};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	bool bCollapseValue{false};

	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 MinFractionalDigits{1};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 MaxFractionalDigits{1};
};

USTRUCT(BlueprintType)
struct FStackableFragment : public FItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 MaxStackSize{1};
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 StackCount{1};
};

/*
 * Consumables
 */
USTRUCT(BlueprintType)
struct FConsumeModifier : public FLabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}	
};

USTRUCT(BlueprintType)
struct FConsumableFragment : public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const override;
	virtual void OnConsume(APlayerController* PC);
	virtual void Manifest() override;

private:
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FConsumeModifier>> ConsumeModifiers;
};

USTRUCT(BlueprintType)
struct FHealthPotionFragment : public FConsumeModifier
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;
};

USTRUCT(BlueprintType)
struct FManaPotionFragment : public FConsumeModifier
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;
};

/*
 * Equippables
 */
USTRUCT()
struct FEquipmentModifier : public FLabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) {}
	virtual void OnUnequip(APlayerController* PC) {}
};

USTRUCT(BlueprintType)
struct FStrengthModifier : public FEquipmentModifier
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;
};

USTRUCT(BlueprintType)
struct FArmorModifier : public FEquipmentModifier
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;
};

USTRUCT(BlueprintType)
struct FDamageModifier : public FEquipmentModifier
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;
};

USTRUCT(BlueprintType)
struct FEquipmentFragment : public FInventoryItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UCompositeBase* Composite) const override;
	virtual void Manifest() override;
	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);
	AEquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;
	void DestroyAttachedActor() const;
	FGameplayTag GetEquipmentType() const { return EquipmentType; }
	void SetEquippedActor(AEquipActor* EquipActor);
	
	bool bEquipped{false};

private:
	UPROPERTY(EditAnywhere, Category="Inventory"/*, meta=(ExcludeBaseStruct)*/)
	TArray<TInstancedStruct<FEquipmentModifier>> EquipmentModifiers;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<AEquipActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AEquipActor> EquippedActor = nullptr;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FName SocketAttachPoint{ NAME_None };

	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag EquipmentType = FGameplayTag::EmptyTag;
};


