#pragma once

#include "Inv_GridTypes.generated.h"

class UInv_InventoryItem;

UENUM(BlueprintType)
enum class EInv_ItemCategory : uint8
{
	Equippable,
	Consumable,
	Craftable,
	None
};

USTRUCT()
struct FSlotAvailability
{
	GENERATED_BODY()

	FSlotAvailability() {}
	FSlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}

	int32 Index{INDEX_NONE};
	int32 AmountToFill{0};
	bool bItemAtIndex{false};
};

USTRUCT()
struct FSlotAvailabilityResult
{
	GENERATED_BODY()

	FSlotAvailabilityResult() {}

	TWeakObjectPtr<UInv_InventoryItem> Item;

	int32 TotalRoomToFill{0};
	int32 Remainder{0};
	bool bStackable{false};
	TArray<FSlotAvailability> SlotAvailabilities;
};
