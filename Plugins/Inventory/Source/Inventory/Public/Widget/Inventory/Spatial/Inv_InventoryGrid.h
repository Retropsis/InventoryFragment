// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryGrid.generated.h"

class UItemPopUp;
enum class EGridSlotState : uint8;
struct FGameplayTag;
struct FGridFragment;
struct FImageFragment;
struct FItemManifest;
class UHoverItem;
class USlottedItem;
class UInv_ItemComponent;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void AddItem(UInv_InventoryItem* Item);

	UFUNCTION()
	void AddStacks(const FSlotAvailabilityResult& Result);

	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnPopUpMenuSplit(int32 SplitAmount, int32 Index);
	
	UFUNCTION()
	void OnPopUpMenuConsume(int32 Index);
	
	UFUNCTION()
	void OnPopUpMenuDrop(int32 Index);
	
	FSlotAvailabilityResult HasRoomForItem(const UInv_ItemComponent* ItemComponent);
	EInv_ItemCategory GetItemCategory() const { return ItemCategory; }
	void ShowCursor();
	void HideCursor();
	void SetOwningCanvas(UCanvasPanel* OwningCanvas);

private:
	void ConstructGrid();
	bool MatchesCategory(const UInv_InventoryItem* Item) const;
	FSlotAvailabilityResult HasRoomForItem(const UInv_InventoryItem* InventoryItem);
	FSlotAvailabilityResult HasRoomForItem(const FItemManifest& ItemManifest);
	FIntPoint GetItemDimensions(const FItemManifest& ItemManifest) const;
	void AddItemToIndices(const FSlotAvailabilityResult& Result, UInv_InventoryItem* NewItem);
	void AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount);
	USlottedItem* CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FGridFragment* GridFragment, const FImageFragment* ImageFragment, const int32 Index) const;
	void SetSlottedImage(const USlottedItem* SlottedItem, const FGridFragment* GridFragment, const FImageFragment* ImageFragment) const;
	FVector2D GetDrawSize(const FGridFragment* GridFragment) const;
	void AddSlottedItemToCanvas(const int32 Index, const FGridFragment* GridFragment, USlottedItem* SlottedItem) const;
	void UpdateGridSlots(UInv_InventoryItem* NewItem, int32 Index, const bool bStackable, const int32 StackAmount);
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, int32 Index) const;
	bool HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, int32 MaxStackSize);
	bool CheckSlotConstrains(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, int32 MaxStackSize) const;
	bool HasValidItem(const UInv_GridSlot* GridSlot) const;
	bool IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	bool IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const;
	int32 DetermineAmountToFillForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill,  const UInv_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UInv_GridSlot* GridSlot) const;
	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	void AssignHoverItem(UInv_InventoryItem* InventoryItem);
	void AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex);
	void RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex);
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	ETileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	void OnTileParametersUpdated(const FTileParameters& Parameters);
	FIntPoint CalculateStartingCoordinates(const FIntPoint& Coordinates, const FIntPoint& Dimensions, const ETileQuadrant Quadrant) const;
	FSpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
	bool CursorExitedCanvas(const FVector2D& BoundaryPosition, const FVector2D& BoundarySize, const FVector2D& Location);
	void HighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions);
	void ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EGridSlotState GridSlotState);
	void PutDownOnIndex(const int32 Index);
	void ClearHoverItem();
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	bool IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const;
	void SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex);
	bool ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const;
	void SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldConsumeHoverItemStacks(int32 RoomInClickedSlot, int32 HoveredStackCount) const;
	void ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index);
	bool ShouldFillInStack(int32 RoomInClickedSlot, int32 HoveredStackCount) const;
	void FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index);
	void CreateItemPopUp(const int32 GridIndex);

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<UCanvasPanel> OwningCanvasPanel;
	
	UPROPERTY(EditAnywhere, Category="Inventory", BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	EInv_ItemCategory ItemCategory;

	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlots;

	UPROPERTY()
	TMap<int32, TObjectPtr<USlottedItem>> SlottedItems;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<USlottedItem> SlottedItemClass;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UHoverItem> HoverItemClass;

	UPROPERTY()
	TObjectPtr<UHoverItem> HoverItem;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UItemPopUp> ItemPopUpClass;

	UPROPERTY()
	TObjectPtr<UItemPopUp> ItemPopUp;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;
	
	UPROPERTY()
	TObjectPtr<UUserWidget>  HiddenCursorWidget;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	float TileSize;

	UPROPERTY(EditAnywhere, Category="Inventory")
	FVector2D ItemPopUpOffset;

	FTileParameters TileParameters;
	FTileParameters LastTileParameters;
	int32 ItemDropIndex{INDEX_NONE};
	FSpaceQueryResult CurrentQueryResult;
	bool bMouseWithinCanvas;
	bool bLastMouseWithinCanvas;
	int32 LastHighlightedIndex;
	FIntPoint LastHighlightedDimensions;
};
