
#include "Item/Manifest/Inv_ItemManifest.h"
#include "Item/Inv_InventoryItem.h"

UInv_InventoryItem* FItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* Item = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);

	return Item;
}
