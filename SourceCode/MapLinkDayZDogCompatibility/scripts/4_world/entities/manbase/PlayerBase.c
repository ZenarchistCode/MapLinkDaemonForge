#ifdef MAPLINK
modded class PlayerBase
{
	override void SendUApiAfterLoadClient()
	{
		super.SendUApiAfterLoadClient();

		if (HaveDog())
		{
			GetGame().ObjectDelete(GetMyDog());
			InitialDogSpawn(ModelToWorld("-0.5 0 1.5"));
		}
	}
}

modded class PlayerDataStore
{
	override void SavePlayer(PlayerBase player)
	{
		if (GetGame().IsServer() && player && player.HaveDog() && player.GetDogSlot())
		{
			player.StoreDogInventory();
			player.StoreDogHealth();
		}

		super.SavePlayer(player);

		if (GetGame().IsServer() && player && player.HaveDog())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Call(player.RestoreDogInventory);
		}
	}
}
#endif