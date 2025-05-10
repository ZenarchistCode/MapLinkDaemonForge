// Zen's note: Absolutely no idea what all this is for, but I'm leaving it in because I assume Daemonforge had his reasons and it's probably important lol.
modded class ShelterKit extends ItemBase
{
	override void DisassembleKit(ItemBase item) 
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player && player.IsBeingTransfered())
		{
			return;
		}

		super.DisassembleKit(item);
	}
}

modded class FenceKit extends ItemBase
{

	override void DisassembleKit(ItemBase item) 
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player && player.IsBeingTransfered())
		{
			return;
		}

		super.DisassembleKit(item);
	}
}

modded class WatchtowerKit extends ItemBase
{
	override void DisassembleKit(ItemBase item) 
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player && player.IsBeingTransfered())
		{
			return;
		}

		super.DisassembleKit(item);
	}
}

modded class TerritoryFlagKit extends ItemBase
{
	override void DisassembleKit(ItemBase item) 
	{
		PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (player && player.IsBeingTransfered())
		{
			return;
		}

		super.DisassembleKit(item);
	}
}