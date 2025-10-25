
#include "Item/Fragment/ItemFragment.h"

void FHealthPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Healed for %f HP"), HealAmount));	
}

void FManaPotionFragment::OnConsume(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Refilled for %f MP"), ManaAmount));	
}
