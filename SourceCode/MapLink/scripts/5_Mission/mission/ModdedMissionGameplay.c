modded class MissionGameplay
{
	void MissionGameplay()
    {   
		GetRPCManager().AddRPC("MapLink", "RPCRedirectedKicked", this, SingeplayerExecutionType.Both);
    }
	
	override void UniversalApiReadyTokenReceived()
	{
		if (m_UApi_Initialized)
		{ 
			//So we don't load 3 times!
			GetMapLinkConfig().Load(); //  Reload config everytime the Auth key is renewed.
		}
		
		super.UniversalApiReadyTokenReceived();
	}

	void RPCRedirectedKicked(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target) 
	{
		Param1< UApiServerData > data; 
		if (!ctx.Read(data)) return;
		MLLog.Info("Kicked from Game");
		UApiServerData serverData = UApiServerData.Cast(data.param1);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(GetGame().DisconnectSessionForce);
		GetDayZGame().HiveSetReconnectTo(serverData);
	}	
}

modded class MissionMainMenu 
{
	static bool MAPLINK_DISABLECUTSCENE_ONTRANSFER = true;
	
	override void OnInit() 
	{
		if (GetDayZGame().IsToTransfer() && MAPLINK_DISABLECUTSCENE_ONTRANSFER)
		{
			m_NoCutscene = true;
		}

		super.OnInit();
	}
	
	override void OnMissionStart()
	{
		super.OnMissionStart();

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.DoReconnect);
	}

	void DoReconnect() 
	{ 
		GetDayZGame().HiveDoReconnect(m_mainmenu);
	}
}