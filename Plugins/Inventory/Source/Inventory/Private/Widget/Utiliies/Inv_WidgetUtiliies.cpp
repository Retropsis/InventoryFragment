// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Utiliies/Inv_WidgetUtiliies.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

int32 UInv_WidgetUtiliies::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtiliies::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}

bool UInv_WidgetUtiliies::IsWithinBounds(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize, const FVector2D& MousePosition)
{
	return MousePosition.X >= BoundaryPosition.X && MousePosition.X <= (BoundaryPosition.X + WidgetSize.X) &&
		MousePosition.Y >= BoundaryPosition.Y && MousePosition.Y <= (BoundaryPosition.Y + WidgetSize.Y);
}

FVector2D UInv_WidgetUtiliies::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

FVector2D UInv_WidgetUtiliies::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	FVector2D PixelPosition;
	FVector2D ViewportPosition;
	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewportPosition);
	return ViewportPosition;
}