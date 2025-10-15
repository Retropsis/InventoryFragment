// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragmentGameMode.h"
#include "InventoryFragmentCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInventoryFragmentGameMode::AInventoryFragmentGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
