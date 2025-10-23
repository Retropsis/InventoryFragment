// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/Inventory/Spatial/Inv_InventoryGrid.h"

#include "Inventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Component/Inv_InventoryComponent.h"
#include "InventoryManagement/Utilities/Inv_InventoryStatics.h"
#include "Item/Inv_InventoryItem.h"
#include "Item/Fragment/FragmentTags.h"
#include "Item/Fragment/ItemFragment.h"
#include "Item/Manifest/Inv_ItemManifest.h"
#include "Widget/Inventory/GridSlot/Inv_GridSlot.h"
#include "Widget/Inventory/HoverItem/HoverItem.h"
#include "Widget/Inventory/SlottedItem/SlottedItem.h"
#include "Widget/Utiliies/Inv_WidgetUtiliies.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::UInv_InventoryGrid::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::UInv_InventoryGrid::AddStacks);
}

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPosition = UInv_WidgetUtiliies::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitedCanvas(CanvasPosition, UInv_WidgetUtiliies::GetWidgetSize(CanvasPanel), MousePosition))
	{
		return;
	}
	
	UpdateTileParameters(CanvasPosition, MousePosition);
}

bool UInv_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPosition, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;

	bMouseWithinCanvas = UInv_WidgetUtiliies::IsWithinBounds(BoundaryPosition, BoundarySize, Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;

	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [] (UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

void UInv_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [] (UInv_GridSlot* GridSlot)
	{
		if (GridSlot->IsAvailable())
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas) return;
	
	// Calculate the Tile Quadrant, Index and Coordinates
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	LastTileParameters = TileParameters;
	TileParameters.Coordinates = HoveredTileCoordinates;
	TileParameters.Index = UInv_WidgetUtiliies::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.Quadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);
	OnTileParametersUpdated(TileParameters);
}

void UInv_InventoryGrid::OnTileParametersUpdated(const FTileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	
	// Get Hover Item's dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	
	// calculate the starting coordinate for highlighting
	FIntPoint StartingCoordinates = CalculateStartingCoordinates(Parameters.Coordinates, Dimensions, Parameters.Quadrant);
	ItemDropIndex = UInv_WidgetUtiliies::GetIndexFromPosition(StartingCoordinates, Columns);
	CurrentQueryResult = CheckHoverPosition(StartingCoordinates, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FGridFragment* GridFragment = GetFragment<FGridFragment>(CurrentQueryResult.ValidItem.Get(), Fragment::Grid);
		if (!GridFragment) return;
		
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EGridSlotState::GrayedOut);
	}
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EGridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [State = GridSlotState] (UInv_GridSlot* GridSlot)
	{
		switch (State)
		{
		case EGridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EGridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EGridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		case EGridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		}
	});
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

FSpaceQueryResult UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FSpaceQueryResult Result;
	
	// in the grid bounds?
	if (!IsInGridBounds(UInv_WidgetUtiliies::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;
	// If more than one of the indices is occupied with the same item, do they have the same upper left index?
	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlots, UInv_WidgetUtiliies::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&] (const UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	// if so, is there only one item in the way? (can we swap?)
	if (OccupiedUpperLeftIndices.Num() == 1)
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}
	return Result;
}

FIntPoint UInv_InventoryGrid::CalculateStartingCoordinates(const FIntPoint& Coordinates, const FIntPoint& Dimensions, const ETileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;
	FIntPoint StartingCoordinates;
	switch (Quadrant)
	{
	case ETileQuadrant::TopLeft:
		StartingCoordinates.X = Coordinates.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoordinates.Y = Coordinates.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case ETileQuadrant::TopRight:
		StartingCoordinates.X = Coordinates.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoordinates.Y = Coordinates.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case ETileQuadrant::BottomLeft:
		StartingCoordinates.X = Coordinates.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoordinates.Y = Coordinates.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case ETileQuadrant::BottomRight:
		StartingCoordinates.X = Coordinates.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoordinates.Y = Coordinates.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant."));
		return FIntPoint{-1, -1};
	}
	return StartingCoordinates;
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

ETileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	// Calculate relative position within the tile
	const float TileLocalX = FMath::Fmod((MousePosition.X - CanvasPosition.X), TileSize);
	const float TileLocalY = FMath::Fmod((MousePosition.Y - CanvasPosition.Y), TileSize);

	// Determine which quadrant cursor is in
	const bool bIsTop = TileLocalY < TileSize / 2.f;
	const bool bIsLeft = TileLocalX < TileSize / 2.f;

	ETileQuadrant HoveredTileQuadrant{ETileQuadrant::None};
	if (bIsTop && bIsLeft) HoveredTileQuadrant = ETileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = ETileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = ETileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = ETileQuadrant::BottomRight;

	return HoveredTileQuadrant;
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;

	FSlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UInv_InventoryGrid::AddStacks(const FSlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;

	for (const FSlotAvailability& SlotAvailability : Result.SlotAvailabilities)
	{
		if (SlotAvailability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[SlotAvailability.Index];
			const auto&  SlottedItem = SlottedItems.FindChecked(SlotAvailability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + SlotAvailability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + SlotAvailability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), SlotAvailability.Index, Result.bStackable, SlotAvailability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), SlotAvailability.Index, Result.bStackable, SlotAvailability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::AddItemToIndices(const FSlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	for (const FSlotAvailability& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(Item, Fragment::Grid);
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(Item, Fragment::Icon);
	if (!GridFragment || !ImageFragment) return;

	USlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

USlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable, const int32 StackAmount, const FGridFragment* GridFragment, const FImageFragment* ImageFragment, const int32 Index) const
{
	USlottedItem* SlottedItem = CreateWidget<USlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SlottedItem->SetGridIndex(Index);
	SetSlottedImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	return SlottedItem;
}

void UInv_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlots.IsValidIndex(GridIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
	}
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);

	HoverItem->SetPreviousGrisIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(InventoryItem, Fragment::Grid);
	if (!GridFragment) return;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&] (UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
	});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<USlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UHoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FGridFragment* GridFragment = GetFragment<FGridFragment>(InventoryItem, Fragment::Grid);
	const FImageFragment* ImageFragment = GetFragment<FImageFragment>(InventoryItem, Fragment::Icon);
	if (!GridFragment || !ImageFragment) return;

	const FVector2D DrawSize = GetDrawSize(GridFragment);
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UInv_InventoryGrid::SetSlottedImage(const USlottedItem* SlottedItem, const FGridFragment* GridFragment, const FImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

FVector2D UInv_InventoryGrid::GetDrawSize(const FGridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	return  GridFragment->GetGridSize() * IconTileWidth;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FGridFragment* GridFragment, USlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPosition = UInv_WidgetUtiliies::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPositionWithPadding = DrawPosition + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPositionWithPadding);
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackable)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}
	
	const FGridFragment* GridFragment = GetFragment<FGridFragment>(NewItem, Fragment::Grid);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);

	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&] (UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);
	});
}

FSlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FSlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* InventoryItem)
{
	return HasRoomForItem(InventoryItem->GetItemManifest());
}

FSlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FItemManifest& ItemManifest)
{
	FSlotAvailabilityResult Result;

	// Determine if the item is stackable.
	const FStackableFragment* StackableFragment = ItemManifest.GetFragmentOfType<FStackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Determine how many stacks to add.
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;

	TSet<int32> CheckedIndices;
	// For each Grid Slot:
	for (const TObjectPtr<UInv_GridSlot>& GridSlot : GridSlots)
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill == 0) break;
	
		// Is this index claimed yet?
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetTileIndex())) continue;

		// Is Item Within Grid Bounds ?
		if (!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(ItemManifest))) continue;
	
		// Can the item fit here? (i.e. is it out of grid bounds?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(ItemManifest), CheckedIndices, TentativelyClaimed, ItemManifest.GetItemType(), MaxStackSize)) continue;
		
		// How much to fill?
		const int32 AmountToFillInSlot = DetermineAmountToFillForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0) continue;

		CheckedIndices.Append(TentativelyClaimed);
		
		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FSlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);
		
		// How much is the Remainder?
		AmountToFill -= AmountToFillInSlot;
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0) return Result;
	}
	return Result;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FItemManifest& ItemManifest) const
{
	const FGridFragment* GridFragment = ItemManifest.GetFragmentOfType<FGridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const  int32 MaxStackSize)
{
	// Is there room at this index? (i.e. are there other items in the way?)
	bool bHasRoomAtIndex = true;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns, [&] (const UInv_GridSlot* SubGridSlot)
	{
		if (CheckSlotConstrains(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});
	
	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstrains(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{
	// Check any other important conditions - ForEach2D over a 2D range
 		// Index claimed?
		if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetTileIndex())) return false;
	
 		// Has valid item?
		if (!HasValidItem(SubGridSlot))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
			return true;
		}

		// Is This Slot Upper Left Slot?
		if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	
 		// If so, is this a stackable item?
		const UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
		if (!SubItem->IsStackable()) return false;
	
 		// Is this item the same type as the item we're trying to add?
		if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	
 		// If stackable, is this slot at the max stack size already?
		if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	return true;
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetTileIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& Dimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlots.Num()) return false;
	const int32 EndColumn = (StartIndex % Columns) + Dimensions.X;
	const int32 EndRow = (StartIndex / Columns) + Dimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::DetermineAmountToFillForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	// If we are at a slot that doesn't hold the stack count, we must get the actual stack count.
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

bool UInv_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int j = 0; j < Rows; j++)
	{
		for (int i = 0; i < Columns; i++)
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);

			// int32 Index = i + j * Columns;
			const FIntPoint TilePosition = FIntPoint(i, j);
			GridSlot->SetTileIndex(UInv_WidgetUtiliies::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(TilePosition * TileSize);
			
			GridSlots.Add(GridSlot);
		}
	}
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}
