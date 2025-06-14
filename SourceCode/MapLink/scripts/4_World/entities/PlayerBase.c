modded class PlayerBase extends ManBase
{
	protected ref Timer m_MapLink_UnderProtectionTimer;
	protected int m_LastMapTransferTimestamp = 0;
	protected int m_SecondsCooldownBetweenTransfer;
			
	int GetLastMapTransferTimestamp()
	{
		return m_LastMapTransferTimestamp;
	}

	void SetLastMapTransferTimestamp(int timestamp)
	{
		m_LastMapTransferTimestamp = timestamp;
	}

	int GetSecondsCooldownBetweenTransfer()
	{
		return m_SecondsCooldownBetweenTransfer;
	}

	void SyncLastMapTransferTimestamp()
	{
		int nowTimestamp = UUtil.GetUnixInt();
		int lastTravelTimestamp = GetLastMapTransferTimestamp();
		RPCSingleParam(MAPLINK_TRAVELTIMER, new Param2<int, int>(m_LastMapTransferTimestamp, GetMapLinkConfig().TravelCooldownSeconds), true, GetIdentity());
	}

	override void OnConnect()
	{
		super.OnConnect();

		if (!GetIdentity() || GetGame().IsClient())
			return;

		if (GetMapLinkConfig().TravelCooldownSeconds <= 0)
			return;

		UApi().db(PLAYER_DB).PublicLoad(
					"MapLinkLastTransferTime", 
					GetIdentity().GetId(), 
					this, 
					"ReceiveLastMapLinkTransferTimeFromDB");
	}

	void ReceiveLastMapLinkTransferTimeFromDB(int cid, int status, string oid, string data)
	{
		if (status == UAPI_SUCCESS || status == UAPI_EMPTY)
		{
			string dataFromJSON = SimpleValueStore.GetValue(data);
			SetLastMapTransferTimestamp(dataFromJSON.ToInt());
		}
	}

	protected void UpdateMapLinkProtectionClient(int time)
	{
		MLLog.Debug("UpdateMapLinkProtectionClient" + time);

		if (time > 0)
		{
			GetDayZGame().MapLinkStartCountDown(time);		
			GetInputController().OverrideRaise(true, false);
			GetInputController().OverrideMeleeEvade(true, false);
		}

		if (time <= 0)
		{
			GetDayZGame().MapLinkStopCountDown();		
			GetInputController().OverrideMeleeEvade(false, false);

			bool expansionSafeZone = false;

			#ifdef EXPANSIONMODCORE
			if (Expansion_IsInSafeZone())
			{
				// Expansion safe zone forces its own non-raise so don't override it.
				expansionSafeZone = true;
			}
			#endif

			if (!expansionSafeZone)
			{
				GetInputController().OverrideRaise(false, false);
			}
		}
	}
	
	void UpdateMapLinkProtection(int time = -1)
	{
		MLLog.Debug("UpdateMapLinkProtection" + time);

		if (!GetGame().IsDedicatedServer())
		{
			return;
		}

		RPCSingleParam(MAPLINK_UNDERPROTECTION, new Param1<int>(time), true, GetIdentity());
		if (m_MapLink_UnderProtection && time < 0)
		{
			GetInputController().OverrideMeleeEvade(false, false);
			GetInputController().OverrideRaise(false, false);

			if (m_MapLink_UnderProtectionTimer)
			{
				if (m_MapLink_UnderProtectionTimer.IsRunning())
				{
					m_MapLink_UnderProtectionTimer.Stop();
				}
			}

			RemoveProtectionSafe();
			return;
		}

		SetAllowDamage(false);
		m_MapLink_UnderProtection = true;
		GetInputController().OverrideMeleeEvade(true, false);
		GetInputController().OverrideRaise(true, false);
		
		SetSynchDirty();
		
		if (!m_MapLink_UnderProtectionTimer)
		{
			m_MapLink_UnderProtectionTimer = new Timer;
		}

		if (m_MapLink_UnderProtectionTimer.IsRunning())
		{
			m_MapLink_UnderProtectionTimer.Stop();
		}

		m_MapLink_UnderProtectionTimer.Run(time, this, "UpdateMapLinkProtection", new Param1<int>(-1), false);
	}

	protected void RemoveProtectionSafe()
	{
		bool PlayerHasGodMode = false;

		// Zen's note: GetGame().IsDedicatedServer() check unncessary as RemoveProtectionSafe is only called if GetGame().IsDedicatedServer() = true

		#ifdef JM_COT
			if (COTHasGodMode())
			{
				MLLog.Log("RemoveProtectionSafe COT ADMIN TOOLS ACTIVE");
				PlayerHasGodMode = true;
			}
		#endif
		#ifdef VPPADMINTOOLS
			if (GodModeStatus())
			{
				MLLog.Log("RemoveProtectionSafe VPP ADMIN TOOLS ACTIVE");
				PlayerHasGodMode = true;
			}
		#endif
		#ifdef ZOMBERRY_AT
			if (ZBGodMode)
			{
				MLLog.Log("RemoveProtectionSafe ZOMBERRY ADMIN TOOLS ACTIVE");
				PlayerHasGodMode = true;
			}
		#endif
		#ifdef TRADER 
			if (m_Trader_IsInSafezone)
			{
				MLLog.Log("RemoveProtectionSafe Player Is In Trader(DrJones) Safe Zone");
				PlayerHasGodMode = true;
			}
		#endif
		#ifdef TRADERPLUS
			if (IsInsideSZ && IsInsideSZ.SZStatut)
			{
				MLLog.Log("RemoveProtectionSafe Player Is In Trader+ Safe Zone");
				PlayerHasGodMode = true;
			}		
		#endif

		if (!PlayerHasGodMode)
		{
			SetAllowDamage(true);
		}

		m_MapLink_UnderProtection = false;
		SetSynchDirty();
	}
	
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
		StatUpdateByTime(AnalyticsManagerServer.STAT_PLAYTIME);
		//Making sure not to save freshspawns or dead people, dead people logic is handled in EEKilled
		if (!GetGame().IsClient() && GetHealth("","Health") > 0 && StatGet(AnalyticsManagerServer.STAT_PLAYTIME) >= MAPLINK_BODYCLEANUPTIME && !IsBeingTransfered() && !MapLinkShoudDelete())
		{
			this.SavePlayerToUApi();
		}
    }
	
	void SavePlayerToUApi()
	{
		if (m_MapLinkGUIDCache && m_MapLinkNameCache && GetGame().IsDedicatedServer())
		{
			MLLog.Debug("Saving Player to API " + m_MapLinkNameCache + "(" + m_MapLinkGUIDCache + ")" + " Health:  " + GetHealth("","Health") + " PlayTime: " +  StatGet(AnalyticsManagerServer.STAT_PLAYTIME) + " IsUnconscious: " + IsUnconscious() + " IsRestrained: " + IsRestrained());
			PlayerDataStore teststore = new PlayerDataStore(PlayerBase.Cast(this));
			UApi().db(PLAYER_DB).Save("MapLink", m_MapLinkGUIDCache, teststore.ToJson());

			if (IsAlive() && !IsUnconscious())
			{
				UApi().db(PLAYER_DB).PublicSave("MapLink", m_MapLinkGUIDCache, SimpleValueStore.StoreValue(UApiConfig().ServerID + "~" + m_TransferPoint),NULL,"");
			} else 
			{
				UApi().db(PLAYER_DB).PublicSave("MapLink", m_MapLinkGUIDCache, SimpleValueStore.StoreValue(""),NULL,"");
			}

			//NotificationSystem.SimpleNoticiation("Your Data has been saved to the API", "Notification","Notifications/gui/data/notifications.edds", -16843010, 10, this.GetIdentity());
		} else 
		{
			MLLog.Debug("Failed to save player to API");
		}
	}
	
	override void OnUApiSave(PlayerDataStore data)
	{
		super.OnUApiSave(data);

		int i = 0;
		for(i = 0; i < m_ModifiersManager.m_ModifierList.Count(); i++)
		{
            ModifierBase mdfr = ModifierBase.Cast(m_ModifiersManager.m_ModifierList.GetElement(i));
            if (mdfr && mdfr.IsActive() && mdfr.IsPersistent()) 
			{ 
				data.AddModifier(mdfr.GetModifierID(), mdfr.GetAttachedTime());
			}
		}

		for(i = 0; i < m_AgentPool.m_VirusPool.Count(); i++)
		{
			data.AddAgent(m_AgentPool.m_VirusPool.GetKey(i), m_AgentPool.m_VirusPool.GetElement(i));
		}

		data.m_TransferPoint = m_TransferPoint;
		data.m_BrokenLegState = m_BrokenLegState;
		data.m_BleedingBits = GetBleedingBits();

		if (GetBleedingManagerServer())
		{
			GetBleedingManagerServer().OnUApiSave(data);
		} else 
		{
			MLLog.Log("Bleeding Manager is NULL");
		}

		if (m_PlayerStomach)
		{
			for (i = 0; i < m_PlayerStomach.m_StomachContents.Count(); i++)
			{
				StomachItem stomachItem;
				if (Class.CastTo(stomachItem, m_PlayerStomach.m_StomachContents.Get(i)))
				{
					data.AddStomachItem(stomachItem.m_Amount, stomachItem.m_FoodStage, stomachItem.m_ClassName, stomachItem.m_Agents);
				}
			}
		}

		if (GetPlayerStats())
		{
			for (i = 0; i < GetPlayerStats().GetPCO().m_PlayerStats.Count(); i++)
			{
				PlayerStatBase TheStat = PlayerStatBase.Cast(GetPlayerStats().GetPCO().m_PlayerStats.Get(i));
				if (TheStat && TheStat.GetLabel() != "")
				{
					data.AddStat(TheStat.GetLabel(), TheStat.Get());
				}
			}
		}

		data.m_Camera3rdPerson = m_Camera3rdPerson;
	}
	
	override void OnUApiLoad(PlayerDataStore data)
	{
		super.OnUApiLoad(data);

		int i = 0;
		
		for (i = 0; i < GetPlayerStats().GetPCO().Get().Count(); i++)
		{
			PlayerStatBase TheStat;
			float statvalue;
			if (Class.CastTo(TheStat, GetPlayerStats().GetPCO().Get().Get(i)) && data.ReadStat(TheStat.GetLabel(), statvalue))
			{
				TheStat.SetByFloatEx(statvalue);
			} else if (TheStat) 
			{
				MLLog.Err("Failed to set stat for " + TheStat.GetLabel());
			}
		}
		
		for (i = 0; i < data.m_Modifiers.Count(); i++)
		{
			if (data.m_Modifiers.Get(i))
			{
				ModifierBase mdfr = m_ModifiersManager.GetModifier(data.m_Modifiers.Get(i).ID());
				if (mdfr.IsTrackAttachedTime() && data.m_Modifiers.Get(i).Value() >= 0)
				{
					mdfr.SetAttachedTime(data.m_Modifiers.Get(i).Value());
				}
				m_ModifiersManager.ActivateModifier(data.m_Modifiers.Get(i).ID(), EActivationType.TRIGGER_EVENT_ON_CONNECT);
			}
		}

		for(i = 0; i < data.m_Agents.Count(); i++)
		{
			m_AgentPool.SetAgentCount(data.m_Agents.Get(i).ID(), data.m_Agents.Get(i).Value());
		}

		data.m_TransferPoint = "";
		m_TransferPoint = "";
		m_BrokenLegState = data.m_BrokenLegState;

		//SetBleedingBits(data.m_BleedingBits);

		if (GetBleedingManagerServer())
		{	
			GetBleedingManagerServer().OnUApiLoad(data);
		} else 
		{
			MLLog.Log("Bleeding Manager is NULL");
		}

		if (m_PlayerStomach && data.m_Stomach)
		{
			for (i = 0; i < data.m_Stomach.Count(); i++)
			{
				UApiStomachItem stomachItem;
				if (Class.CastTo(stomachItem, data.m_Stomach.Get(i)))
				{
					m_PlayerStomach.AddToStomach(stomachItem.m_ClassName, stomachItem.m_Amount, stomachItem.m_FoodStage, stomachItem.m_Agents);
				}
			}
		}

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.SetSynchDirty);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.SendUApiAfterLoadClient, 200);
		m_Camera3rdPerson = data.m_Camera3rdPerson && !GetGame().GetWorld().Is3rdPersonDisabled();
	}
	
	void SendUApiAfterLoadClient()
	{
		RPCSingleParam(MAPLINK_AFTERLOADCLIENT, new Param2<bool, bool>(true, m_Camera3rdPerson), true, GetIdentity());
	}
	
	override void OnDisconnect()
	{
		StatUpdateByTime(AnalyticsManagerServer.STAT_PLAYTIME);
		//If the player has played less than 1 minutes just kill them so their data doesn't save to the local database
		if (StatGet(AnalyticsManagerServer.STAT_PLAYTIME) <= MAPLINK_BODYCLEANUPTIME)
		{ 
			if (GetIdentity())
			{
				MLLog.Info("OnDisconnect Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") they are fresh spawn PlayTime: " + StatGet(AnalyticsManagerServer.STAT_PLAYTIME));
			} else 
			{
				MLLog.Info("OnDisconnect Player: NULL (NULL) they are fresh spawn PlayTime: " + StatGet(AnalyticsManagerServer.STAT_PLAYTIME));
			}

			SetAllowDamage(true);
			SetHealth("","", 0); 
		}

		//If the player has played less than 1 minutes just kill them so their data doesn't save to the local database
		if (IsBeingTransfered())
		{ 
			if (GetIdentity())
			{
				MLLog.Info("OnDisconnect Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") Is Transfering");
			} else 
			{
				MLLog.Info("OnDisconnect Player: NULL (NULL)  Is Transfering");
			}

			SetAllowDamage(true);
			SetHealth("","", 0); 
		}

		super.OnDisconnect();
	}
	
	
	override void EEKilled(Object killer)
	{
		//Only save dead people who've been on the server for more than 1 minutes and who arn't tranfering
		StatUpdateByTime(AnalyticsManagerServer.STAT_PLAYTIME);
		if ((!IsBeingTransfered() && StatGet(AnalyticsManagerServer.STAT_PLAYTIME) > MAPLINK_BODYCLEANUPTIME) || (killer && killer != this))
		{
			this.SavePlayerToUApi();
		}

		//If they are transfering delete
		if (IsBeingTransfered()  && (!killer || killer == this))
		{
			if (GetIdentity())
			{
				MLLog.Info("Marking Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") for delete cause of transfer");
			} else 
			{
				MLLog.Info("Marking Player: NULL (NULL)  for delete cause of transfer");
			}

			//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Delete, 400, false);
			SetPosition(vector.Zero);
			m_MapLink_ShouldDelete = true;
		}
		
		//Fresh spawn just delete the body since I have to spawn players in to send notifications about player transfers
		if (!IsBeingTransfered() && StatGet(AnalyticsManagerServer.STAT_PLAYTIME) <= MAPLINK_BODYCLEANUPTIME && (!killer || killer == this))
		{
			if (GetIdentity())
			{
				MLLog.Info("Deleteing Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") cause they are fresh spawn PlayTime: " + StatGet(AnalyticsManagerServer.STAT_PLAYTIME));
			} else 
			{
				MLLog.Info("Deleteing Player: NULL (NULL) cause they are fresh spawn PlayTime: " + StatGet(AnalyticsManagerServer.STAT_PLAYTIME));
			}

			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.Delete, 400, false);
			SetPosition(vector.Zero);
			m_MapLink_ShouldDelete = true;
		}

		super.EEKilled(killer);
	}

	override void OnUnconsciousStart()
	{
		super.OnUnconsciousStart();
		SavePlayerToUApi();
	}
	
	override void OnUnconsciousStop(int pCurrentCommandID)
	{		
		super.OnUnconsciousStop(pCurrentCommandID);
		SavePlayerToUApi();
	}
	
	void MapLinkUpdateClientSettingsToServer()
	{
		if (GetGame().IsClient())
		{
			RPCSingleParam(MAPLINK_UPDATE3RDPERSON, new Param1<bool>(m_Camera3rdPerson), true, NULL);
		}
	}
	
	void UApiKillAndDeletePlayer()
	{
		if (GetIdentity())
		{
			MLLog.Info("Killing for transfering Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ")");
		} else
		{
			MLLog.Info("Killing for transfering Player: NULL (NULL)");
		}

		SetAllowDamage(true);
		SetHealth("","", 0);
	}
	
	void UApiRequestTravel(string arrivalPoint, string serverName)
	{
		if (GetGame().IsClient())
		{
			MLLog.Debug("Player: " + GetIdentity().GetId() + " is requesting to travel to " + arrivalPoint + " on Server: " + serverName);
			RPCSingleParam(MAPLINK_REQUESTTRAVEL, new Param2<string, string>(arrivalPoint,  serverName), true, NULL);
		}

		if (GetGame().IsDedicatedServer())
		{
			// Check time since last transfer 
			int nowTimestamp = UUtil.GetUnixInt();
			int lastTravelTimestamp = GetLastMapTransferTimestamp();
			MLLog.Debug("[MapLink Transfer] Timestamp=" + lastTravelTimestamp);

			if (GetMapLinkConfig().TravelCooldownSeconds > 0)
			{
				if (lastTravelTimestamp > 0)
				{
					int timePassed = nowTimestamp - lastTravelTimestamp;

					MLLog.Debug("[MapLink Transfer] Do not allow transfer for " + GetIdentity().GetId() + ": Time Passed Since Last Transfer=" + timePassed + "/" + GetMapLinkConfig().TravelCooldownSeconds + " seconds");

					if (timePassed < GetMapLinkConfig().TravelCooldownSeconds)
					{
						return;
					}
				}
			}

			MLLog.Debug("Player: " + GetIdentity().GetName() + "("+ GetIdentity().GetId() + ") is requesting to travel to " + arrivalPoint + " on Server: " + serverName);
			UApiDoTravel(arrivalPoint, serverName);
		}
	}
	
	bool UApiDoManualServerTravel(string arrivalPoint, UApiServerData serverData, int cost = 0, int id = 0)
	{
		string pid = "NULL";
		if (GetIdentity())
		{
			pid = GetIdentity().GetId();
		}

		if (!serverData)
		{
			MLLog.Err("Manual Travel of user " + pid + " to " + arrivalPoint + " failed NULL Server Data");
			return false;
		}

		if (serverData && GetIdentity() && (cost <= 0 || UGetPlayerBalance(GetMapLinkConfig().GetCurrencyKey(id)) >= cost))
		{
			URemoveMoney(GetMapLinkConfig().GetCurrencyKey(id),cost);
			this.UApiSaveTransferPoint(arrivalPoint);
			this.SavePlayerToUApi();
			MLLog.Info("Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") Sending to Server: " + serverData.Name + "(" + serverData.IP  + ":" + serverData.Port.ToString() + ") at ArrivalPoint: " + arrivalPoint);
			GetRPCManager().SendRPC("MapLink", "RPCRedirectedKicked", new Param1<UApiServerData>(serverData), true, GetIdentity());
			SetAllowDamage(false);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UApiKillAndDeletePlayer, 350, false);
			return true;
		}

		MLLog.Err("Manual Travel of user " + pid + " to " + arrivalPoint + " on " + serverData.Name + "(" + serverData.IP  + ":" + serverData.Port.ToString() + ") failed");
		return false;
	}
	
	protected bool UApiDoTravel(string arrivalPoint, string serverName)
	{
		UApiServerData serverData = UApiServerData.Cast(GetMapLinkConfig().GetServer(serverName));
		MapLinkDepaturePoint dpoint = MapLinkDepaturePoint.Cast(GetMapLinkConfig().GetDepaturePoint(GetPosition()));
		int cost;
		int id;

		if (dpoint && serverData && dpoint.GetArrivalPointData(arrivalPoint, id, cost)) 
		{
			MLLog.Debug("Working with Currency Key: " + GetMapLinkConfig().GetCurrencyKey(id));
			if (GetIdentity() && (cost <= 0 || UGetPlayerBalance(GetMapLinkConfig().GetCurrencyKey(id)) >= cost))
			{
				m_LastMapTransferTimestamp = UUtil.GetUnixInt();
				UApi().db(PLAYER_DB).PublicSave("MapLinkLastTransferTime", m_MapLinkGUIDCache, SimpleValueStore.StoreValue(m_LastMapTransferTimestamp.ToString()),NULL,"");
				URemoveMoney(GetMapLinkConfig().GetCurrencyKey(id),cost);
				this.UApiSaveTransferPoint(arrivalPoint);
				this.SavePlayerToUApi();
				MLLog.Info("Player: " + GetIdentity().GetName() + " (" + GetIdentity().GetId() +  ") Sending to Server: " + serverName  + " at ArrivalPoint: " + arrivalPoint);
				GetRPCManager().SendRPC("MapLink", "RPCRedirectedKicked", new Param1<UApiServerData>(serverData), true, GetIdentity());
				SetAllowDamage(false);
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UApiKillAndDeletePlayer, 350, false);
				return true;
			}
		}

		string pid = "NULL";
		if (GetIdentity())
		{
			pid = GetIdentity().GetId();
		}

		MLLog.Err("User " + pid + " Tried to travel to " + arrivalPoint + " on " + serverName + " but validation failed");
		return false;
	} 
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		/*
		if (rpc_type == MAPLINK_TRAVELTIMER && GetGame().IsClient()) 
		{
			Param2<int, int> timedata;

			if (ctx.Read(timedata))	
			{
				m_LastMapTransferTimestamp = timedata.param1;
				m_SecondsCooldownBetweenTransfer = timedata.param2;

				DeparturePointMenu departureMenu;

				if (GetGame().GetUIManager())
				{
					departureMenu = DeparturePointMenu.Cast(GetGame().GetUIManager().GetMenu());
					if (departureMenu)
					{
						departureMenu.UpdateCooldownTimer();
					}
				}
			}
		}
		*/
		
		if (rpc_type == MAPLINK_AFTERLOADCLIENT && GetGame().IsClient()) 
		{
			Param2<bool, bool> alcdata;
			if (ctx.Read(alcdata))	
			{
				if (alcdata.param1)
				{
					UApiAfterLoadClient();
					m_Camera3rdPerson = alcdata.param2 && !GetGame().GetWorld().Is3rdPersonDisabled();
				}
			}
		}

		if (rpc_type == MAPLINK_UNDERPROTECTION && GetGame().IsClient()) 
		{
			Param1<int> updata;
			if (ctx.Read(updata))	{
				UpdateMapLinkProtectionClient(updata.param1);
			}
		}
		
		if (rpc_type == MAPLINK_REQUESTTRAVEL && sender && GetIdentity() && GetGame().IsDedicatedServer())
		{
			Param2<string, string> rtdata;
			if (ctx.Read(rtdata) && GetIdentity().GetId() == sender.GetId())	
			{
				UApiDoTravel(rtdata.param1, rtdata.param2);
			}
		}
		
		if (rpc_type == MAPLINK_UPDATE3RDPERSON && sender && GetIdentity() && GetGame().IsDedicatedServer()) 
		{
			Param1<bool> trdpdata;
			if (ctx.Read(trdpdata) && GetIdentity().GetId() == sender.GetId()) 
			{
				m_Camera3rdPerson = trdpdata.param1;
			}
		}
	}
	
	void UApiAfterLoadClient()
	{
		this.UpdateInventoryMenu();
	}
	
	override void SetSuicide(bool state)
	{
		super.SetSuicide(state);

		if (state && IsUnderMapLinkProtection() && GetGame().IsDedicatedServer())
		{
			SetAllowDamage(true);
		}
	}

	override void SetActions(out TInputActionMap InputActionMap) 
	{
		super.SetActions(InputActionMap);

		AddAction(ActionMapLinkOpenOnAny, InputActionMap);
	}
	
	bool FindDepaturePointForEntity(EntityAI entity, out MapLinkDepaturePoint depaturePoint)
	{
		if (!entity)
			return false;
		
		if (!depaturePoint)
		{ 
			//So you can use super here and if the point is set don't set it.
			if (GetMapLinkConfig().IsDepaturePoint(entity.GetType(), entity.GetPosition()))
			{
				depaturePoint = MapLinkDepaturePoint.Cast(GetMapLinkConfig().GetDepaturePointAny(entity.GetType(), entity.GetPosition()));
				return true;
			}

			return false;
		}

		return true;
	}
	
	/*
	//Money Handling used from Hived Banking
	bool MLCanAccept(int ID, ItemBase item){
		return !item.IsRuined() || GetMapLinkConfig().GetCurrency(ID).CanUseRuinedBills;
	}
	
	
	int MLGetPlayerBalance(int ID){
		int PlayerBalance = 0;
		if (!GetMapLinkConfig().GetCurrency(ID) || GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count() < 1){
			MLLog.Err("Currency ID: " + ID + " is not configured");
			return 0;
		}
		array<EntityAI> inventory = new array<EntityAI>;
		this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventory);
		
		ItemBase item;
		for (int i = 0; i < inventory.Count(); i++){
			if (Class.CastTo(item, inventory.Get(i))){
				for (int j = 0; j < GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count(); j++){
					if (item.GetType() == GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Item && MLCanAccept(ID, item)){
						PlayerBalance += MLCurrentQuantity(item) * GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Value;
					}
				}
			}
		}
		return PlayerBalance;
	}
	
	
	int MLAddMoney(int ID, int Amount){
		if (Amount <= 0){
			return 2;
		}
		int Return = 0;
		int AmountToAdd = Amount;
		int LastIndex = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count() - 1;
		int SmallestCurrency = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		int PlayerBalance = MLGetPlayerBalance(ID);
		int OptimalPlayerBalance = PlayerBalance + AmountToAdd;
		
		MapLinkMoneyValue MoneyValue = GetMapLinkConfig().GetCurrency(ID).GetHighestDenomination(AmountToAdd);
		int MaxLoop = 3000;
		while (MoneyValue && AmountToAdd >= SmallestCurrency && NoError && MaxLoop > 0){
			MaxLoop--;
			int AmountToSpawn = GetMapLinkConfig().GetCurrency(ID).GetAmount(MoneyValue,AmountToAdd);
			if (AmountToSpawn == 0){
				NoError = false;
			}
			
			int AmountLeft = MLCreateMoneyInventory(MoneyValue.Item, AmountToSpawn);
			if (AmountLeft > 0){
				Return = 1;
				MLCreateMoneyGround(MoneyValue.Item, AmountLeft);
			}
			
			int AmmountAdded = MoneyValue.Value * AmountToSpawn;
			
			AmountToAdd = AmountToAdd - AmmountAdded;
			
			MapLinkMoneyValue NewMoneyValue = GetMapLinkConfig().GetCurrency(ID).GetHighestDenomination(AmountToAdd);
			if (NewMoneyValue && NewMoneyValue != MoneyValue){
				MoneyValue = NewMoneyValue;
			} else {
				NoError = false;
			}
		}
		return Return;
	}
	
	
	int MLRemoveMoney(int ID, int Amount){
		if (Amount <= 0){
			return 2;
		}
		int Return = 0;
		int AmountToRemove = Amount;
		int LastIndex = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count() - 1;
		int SmallestCurrency = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(LastIndex).Value;
		bool NoError = true;
		for (int i = 0; i < GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count(); i++){
			AmountToRemove =  MLRemoveMoneyInventory(ID, GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(i), AmountToRemove);
		}
		if (AmountToRemove >= SmallestCurrency){ // Now to delete a larger bill and make change
			for (int j = LastIndex; j >= 0; j--){
				//MLLog.Debug("Trying to remove " + GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Item);
				int NewAmountToRemove =  MLRemoveMoneyInventory(ID, GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j), GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Value);
				if (NewAmountToRemove == 0){
					int AmountToAddBack = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Value - AmountToRemove;
					//MLLog.Debug("A " + GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(j).Item + " removed trying to add back " + AmountToAddBack);
					Return = MLAddMoney(ID, AmountToAddBack);
				}
			}
		}
		return Return;
	}
	
	//Return how much left still to remove
	int MLRemoveMoneyInventory(int ID, MapLinkMoneyValue MoneyValue, float Amount){
		int AmountToRemove = GetMapLinkConfig().GetCurrency(ID).GetAmount(MoneyValue, Amount);
		int LastIndex = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Count() - 1;
		int SmallestCurrency = GetMapLinkConfig().GetCurrency(ID).MoneyValues.Get(LastIndex).Value;
		if (AmountToRemove > 0){
			array<EntityAI> itemsArray = new array<EntityAI>;
			this.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);
			for (int i = 0; i < itemsArray.Count(); i++){
				ItemBase item = ItemBase.Cast(itemsArray.Get(i));
				if (item && MLCanAccept(ID, item)){
					string ItemType = item.GetType();
					ItemType.ToLower();
					string MoneyType = MoneyValue.Item;
					MoneyType.ToLower();
					if (ItemType == MoneyType){
						int CurQuantity = item.GetQuantity();
						int AmountRemoved = 0;
						if (!item.HasQuantity()){
							CurQuantity = 1;
						} 
						if (AmountToRemove < CurQuantity){
							AmountRemoved = MoneyValue.Value * AmountToRemove;
							item.MLSetQuantity(CurQuantity - AmountToRemove);
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount - AmountRemoved;
						} else if (AmountToRemove == CurQuantity){
							AmountRemoved = MoneyValue.Value * AmountToRemove;
							GetGame().ObjectDelete(item);
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount - AmountRemoved;
						} else {
							AmountRemoved = MoneyValue.Value * CurQuantity;
							AmountToRemove = AmountToRemove - CurQuantity;
							GetGame().ObjectDelete(item);
							Amount = Amount - AmountRemoved;
						}
						if (AmountToRemove <= 0){
							this.UpdateInventoryMenu(); // RPC-Call needed?
							return Amount;
						}
					}
				}
			}
		}
		this.UpdateInventoryMenu(); // RPC-Call needed?
		return Amount;
	}*/
}

modded class DayZPlayerMeleeFightLogic_LightHeavy
{
    override bool HandleFightLogic(int pCurrentCommandID, HumanInputController pInputs, EntityAI pEntityInHands, HumanMovementState pMovementState, out bool pContinueAttack)
	{
        PlayerBase player = PlayerBase.Cast(m_DZPlayer);
        
        if (player)
        {
            if (player.IsUnderMapLinkProtection())
            return false;
        }

        return super.HandleFightLogic(pCurrentCommandID, pInputs, pEntityInHands, pMovementState, pContinueAttack);
    }
}

modded class ActionUnpin extends ActionSingleUseBase
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
        if (player.IsUnderMapLinkProtection())
            	return false;

		return super.ActionCondition(player, target, item);
	}
}