class CheckIfHasDiscord extends UApiDiscordCallBack
{
	override void OnDiscordUserReceived(UApiDiscordUser user)
	{
		// Do stuff here
		Print("[UPAI] [CheckIfHasDiscord] Success: " + user.id);
	}

	override void OnDiscordUserNotFound(UApiDiscordUser user)
	{
		// Do stuff here
		Print("[UPAI] [CheckIfHasDiscord] User not found");

		if (GetGame().IsClient())
		{
			GetGame().OpenURL(UApi().Discord().Link());
		}
	}
}
