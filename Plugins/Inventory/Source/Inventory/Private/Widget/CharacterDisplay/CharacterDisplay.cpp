// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CharacterDisplay/CharacterDisplay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/ProxyMesh.h"
#include "Kismet/GameplayStatics.h"

void UCharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AProxyMesh::StaticClass(), Actors);

	if (!Actors.IsValidIndex(0)) return;

	AProxyMesh* ProxyMesh = Cast<AProxyMesh>(Actors[0]);
	if (!IsValid(ProxyMesh)) return;

	Mesh = ProxyMesh->GetMesh();
}

void UCharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsDragging) return;

	LastMousePosition = CurrentMousePosition;
	CurrentMousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	const float HorizontalDelta = LastMousePosition.X - CurrentMousePosition.X;

	if (!Mesh.IsValid()) return;
	Mesh->AddRelativeRotation(FRotator(0.f, HorizontalDelta, 0.f));
}

FReply UCharacterDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	CurrentMousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	LastMousePosition = CurrentMousePosition;
	bIsDragging = true;
	return FReply::Handled();
}

FReply UCharacterDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	return FReply::Handled();
}

void UCharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	bIsDragging = false;
}
