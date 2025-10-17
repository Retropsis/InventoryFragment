
#include "Item/ItemTags.h"


namespace GameItems
{
	namespace Equipment
	{
		namespace Weapons
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Axe, "GameItems.Equipment.Weapons.Axe", "Axe");
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sword, "GameItems.Equipment.Weapons.Sword", "Sword");
		}
		
		namespace Cloaks
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(RedCloak, "GameItems.Equipment.Cloaks.RedCloak", "RedCloak");
		}
		
		namespace Masks
		{
			UE_DEFINE_GAMEPLAY_TAG_COMMENT(SteelMask, "GameItems.Equipment.Masks.SteelMask", "SteelMask");
		}
	}

	namespace Consumable
	{
		namespace Potions
		{
			namespace Red
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Small, "GameItems.Consumable.Potions.Red.Small", "Small");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Large, "GameItems.Consumable.Potions.Red.Large", "Large");
			}
			
			namespace Blue
			{
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Small, "GameItems.Consumable.Potions.Blue.Small", "Small");
				UE_DEFINE_GAMEPLAY_TAG_COMMENT(Large, "GameItems.Consumable.Potions.Blue.Large", "Large");
			}
		}
	}
	
	namespace Craftable
	{
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireFernFruit, "GameItems.Craftable.FireFernFruit", "FireFernFruit");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(LuminDaisy, "GameItems.Craftable.LuminDaisy", "LuminDaisy");
		UE_DEFINE_GAMEPLAY_TAG_COMMENT(ScorchPetalBlossom, "GameItems.Craftable.ScorchPetalBlossom", "ScorchPetalBlossom");
	}
}

