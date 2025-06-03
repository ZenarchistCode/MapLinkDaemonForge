modded class MissionServer extends MissionBase
{
	void MissionServer()
	{
		UApi();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.UniversalApiReady);
	}	
	
	override void OnClientPrepareEvent(PlayerIdentity identity, out bool useDB, out vector pos, out float yaw, out int preloadTimeout)
	{
		if (!UApi().IsOnline())
		{
			Error("[UniversalAPI] API is not connected! Kicking player " + identity.GetId() + " to avoid database sync issues. Please start the Universal API Manager.");
			GetGame().DisconnectPlayer(identity);
		}

		if (identity)
		{
			//Print("[UAPI] On Prepare - GUID: " + identity.GetId());
			UApi().PreparePlayerAuth(identity.GetId());
		}

		super.OnClientPrepareEvent(identity, useDB, pos, yaw, preloadTimeout);
	}
	
	override void UniversalApiReady()
	{
		//You requests for after the AuthToken Is received for server side code
		super.UniversalApiReady();
	}
}