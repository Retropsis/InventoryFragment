// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Utiliies/Inv_WidgetUtiliies.h"

int32 UInv_WidgetUtiliies::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtiliies::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}
