#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

struct FItemFragment;

/*
 * Item Manifest contains all necessary data to create a new item
 */
USTRUCT(BlueprintType)
struct INVENTORY_API FItemManifest
{
	GENERATED_BODY()

public:
	UInv_InventoryItem* Manifest(UObject* NewOuter);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }

	template<typename T> requires std::derived_from<T, FItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& Tag) const;

	template<typename T> requires std::derived_from<T, FItemFragment>
	const T* GetFragmentOfType() const;

	template<typename T> requires std::derived_from<T, FItemFragment>
	T* GetFragmentOfTypeMutable();

private:
	UPROPERTY(EditAnywhere, Category="Inventory", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FItemFragment>> Fragments;
		
	UPROPERTY(EditAnywhere, Category="Inventory")
	EInv_ItemCategory ItemCategory{EInv_ItemCategory::None};

	UPROPERTY(EditAnywhere, Category="Inventory")
	FGameplayTag ItemType;
};

template<typename T> requires std::derived_from<T, FItemFragment>
const T* FItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& Tag) const
{
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(Tag)) continue;
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename T> requires std::derived_from<T, FItemFragment>
const T* FItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename T> requires std::derived_from<T, FItemFragment>
T* FItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FItemFragment>& Fragment : Fragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}
