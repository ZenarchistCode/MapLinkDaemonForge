class MapLinkConfig extends UApiConfigBase 
{
	string ConfigVersion = "0";
	static string CurrentVersion = "0";

	int TravelCooldownSeconds = 0;
	
	ref array<ref UApiServerData> Servers = new array<ref UApiServerData>;
	ref array<ref MapLinkArrivalPoint> ArrivalPoints = new array<ref MapLinkArrivalPoint>;
	ref array<ref MapLinkDepaturePoint> DepaturePoints = new array<ref MapLinkDepaturePoint>;
	ref array<ref MapLinkCurrency> Currencies = new array<ref MapLinkCurrency>;
	
	int LogLevel_File = 3;
	int LogLevel_API = 2;
	
	static float MAX_DEPATUREPOINT_DISTANCE = 20;
	static float MAX_DEPATUREPOINTANY_DISTANCE = 0.7;
	
	override void SetDefaults()
	{
		/*
			This is to set the defaults for the mod before requesting a load so that way 
			if it doesn't exsit the API will create the file
		*/

		Servers.Insert(new UApiServerData(""));
		DepaturePoints.Insert(new MapLinkDepaturePoint("Demo000"));
		ArrivalPoints.Insert(new MapLinkArrivalPoint("Demo000"));
		Currencies.Insert(new MapLinkCurrency(-1));
		Currencies.Insert(new MapLinkCurrency(-2));
	}

	override void OnDataReceive()
	{
		SetDataReceived();

		if (!ConfigVersion || ConfigVersion != CurrentVersion)
		{
			ConfigVersion = CurrentVersion;
			LogLevel_File = 3;
			LogLevel_API = 2;
			Save(); //Resave the upgrade Version Back to the server
		}

		MLLog.SetLogLevels(LogLevel_File, LogLevel_API);
		if (Currencies && Currencies.Count())
		{
			for (int i = 0; i < Currencies.Count(); i++)
			{
				TStringIntMap currenciesMap = new TStringIntMap;
				for (int j = 0; j < Currencies.Get(i).MoneyValues.Count(); j++)
				{
					currenciesMap.Insert(Currencies.Get(i).MoneyValues.Get(j).Item, Currencies.Get(i).MoneyValues.Get(j).Value);
				}

				UCurrency.Register(Currencies.Get(i).Name, currenciesMap);
			}
		}

		Valiate();
	}
	
	void Valiate()
	{
	}
	
	override void Load()
	{
		if (!m_DataReceived)
		{
			SetDefaults();
		}

		m_DataReceived = false;
		//Set the Defaults so that way, when you load if this its the server Requesting the data it will create it based on the defaults
		UApi().Rest().GlobalsLoad("MapLink", this, this.ToJson());
	}
	
	override void Save()
	{
		if (GetGame().IsDedicatedServer())
		{
			UApi().Rest().GlobalsSave("MapLink", this.ToJson());
		}
	}

	override string ToJson()
	{
		return UApiJSONHandler<MapLinkConfig>.ToString(this);
	}
	
	// This is called by the API System on the successfull response from the API
	override void OnSuccess(string data, int dataSize) 
	{
		if (UApiJSONHandler<MapLinkConfig>.FromString(data, this))
		{
			OnDataReceive();
		} else 
		{
			MLLog.Err("CallBack Failed errorCode: Invalid Data");
		}
	}
	
	MapLinkDepaturePoint GetDepaturePoint(vector Pos)
	{
		float closestPointDistance = MAX_DEPATUREPOINT_DISTANCE;
		int closestPointIndex = -1;
		for (int i = 0; i < DepaturePoints.Count(); i++)
		{
			float curPointDistance = vector.Distance(DepaturePoints.Get(i).Position, Pos);
			if (curPointDistance < closestPointDistance)
			{
				closestPointIndex = i;
				closestPointDistance = curPointDistance;
			}
		}

		if (closestPointIndex >= 0)
		{
			return MapLinkDepaturePoint.Cast(DepaturePoints.Get(closestPointIndex));
		}

		MLLog.Err("COULD NOT FIND A MAP LINK DEPATURE POINT at " + Pos);
		return NULL;
	}
	
	bool IsDepaturePoint(string type, vector pos)
	{
		for (int i = 0; i < DepaturePoints.Count(); i++)
		{
			if (DepaturePoints.Get(i).TerminalType == type && vector.Distance(DepaturePoints.Get(i).Position, pos) < MAX_DEPATUREPOINTANY_DISTANCE)
			{
				return true;
			}
		}

		return false;
	}
	
	
	MapLinkDepaturePoint GetDepaturePointAny(string type, vector pos)
	{
		for (int i = 0; i < DepaturePoints.Count(); i++)
		{
			if (DepaturePoints.Get(i).TerminalType == type && vector.Distance(DepaturePoints.Get(i).Position, pos) <= MAX_DEPATUREPOINTANY_DISTANCE)
			{
				return MapLinkDepaturePoint.Cast(DepaturePoints.Get(i));
			}
		}

		return NULL;
	}
	
	
	MapLinkSpawnPointPos SpawnPointPos(string arrivalPoint)
	{
		return GetSpawnPointPos(UApiConfig().ServerID, arrivalPoint);
	}
	
	int GetProtectionTime(string arrivalPoint)
	{
		for (int i = 0; i < ArrivalPoints.Count(); i++)
		{
			//Print("GetProtectionTime - Is: " + arrivalPoint + " -> " + ArrivalPoints.Get(i).Name);
			if (ArrivalPoints.Get(i).Name == arrivalPoint)
			{
				return ArrivalPoints.Get(i).ProtectionTime(UApiConfig().ServerID);
			}
		}

		MLLog.Err("GetProtectionTime - Failed to Get Protection Time for " + arrivalPoint);
		return -1;
	}
	
	MapLinkSpawnPointPos GetSpawnPointPos(string serverName, string arrivalPoint)
	{
		for (int i = 0; i< ArrivalPoints.Count(); i++)
		{
			if (ArrivalPoints.Get(i).Name == arrivalPoint)
			{
				return MapLinkSpawnPointPos.Cast(ArrivalPoints.Get(i).GetSpawnPos(serverName));
			}
		}

		MLLog.Err("GetSpawnPointPos - Failed to Get SpawnPoint for " + arrivalPoint + " on " + serverName);
		return NULL;
	}
	
	UApiServerData GetServer(string serverName)
	{
		for(int i = 0; i < Servers.Count(); i++)
		{
			if (Servers.Get(i).Name == serverName)
			{
				return UApiServerData.Cast(Servers.Get(i));
			}
		}

		MLLog.Err("GetServer - Failed to Get Server Data for " + serverName);
		return NULL;
	}
	
	MapLinkArrivalPoint GetArrivalPoint(string arrivalPoint)
	{
		for (int i = 0; i< ArrivalPoints.Count(); i++)
		{
			if(ArrivalPoints.Get(i).Name == arrivalPoint)
			{
				return MapLinkArrivalPoint.Cast(ArrivalPoints.Get(i));
			}
		}

		MLLog.Err("GetArrivalPoint - Failed to Get Arrival Point Data for " + arrivalPoint);
		return NULL;
	}
	
	string GetNiceMapName(string worldName)
	{
		string LowerWorldName = worldName;
		LowerWorldName.ToLower();

		switch (LowerWorldName)
		{
			case "enoch":
				return "Livonia";
			case "enochgloom":
				return "Livonia";
			case "chernarusplus":
				return "Chernarus";
			case "chernarusplusgloom":
				return "Chernarus";
			case "deerisle":
				return "Deer Isle";
			case "chiemsee":
				return "Chiemsee";
			case "rostow":
				return "Rostow";
			case "namalsk":
				return "Namalsk";
			case "esseker":
				return "Esseker";
		};

		string FirstLeter = worldName.Substring(0,1);
		FirstLeter.ToUpper();
		worldName.Set(0, FirstLeter);
		return worldName;
	}
	
	string GetCostIcon(int id)
	{
		for (int i = 0; i < Currencies.Count(); i++)
		{
			if (Currencies.Get(i).ID == id)
			{
				string icon = Currencies.Get(i).Icon;
				if (icon.Contains(".paa") || icon.Contains("set:") || icon.Contains(".edds"))
				{
					return icon;
				}

				return "set:maplink_money image:"+icon;
			}
		}

		return "set:maplink_money image:dollar";
	}
	
	MapLinkCurrency GetCurrency(int id)
	{
		for (int i = 0; i < Currencies.Count(); i++)
		{
			if (Currencies.Get(i).ID == id)
			{
				return Currencies.Get(i);
			}
		}

		return NULL;
	}
	
	string GetCurrencyKey(int id)
	{
		MapLinkCurrency currency = GetCurrency(id);
		if (currency)
		{
			return currency.Name;
		}

		return "";
	}
}

static ref MapLinkConfig m_MapLinkConfig;

static MapLinkConfig GetMapLinkConfig()
{
	if (!m_MapLinkConfig)
	{
		m_MapLinkConfig = new MapLinkConfig;
		m_MapLinkConfig.Load();
	}

	return m_MapLinkConfig;
}