#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ItemFragment.generated.h"

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

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(Categories="Fragment"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
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
struct FImageFragment : public FItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }

private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};

	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D Dimensions{ 44.f, 44.f };
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

USTRUCT(BlueprintType)
struct FConsumableFragment : public FItemFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}	
};

USTRUCT(BlueprintType)
struct FHealthPotionFragment : public FConsumableFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float HealAmount{ 20.f };
};

USTRUCT(BlueprintType)
struct FManaPotionFragment : public FConsumableFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) override;

	UPROPERTY(EditAnywhere, Category="Inventory")
	float ManaAmount{ 10.f };
};