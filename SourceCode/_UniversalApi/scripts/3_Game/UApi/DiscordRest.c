class UniversalDiscordRest extends Managed
{	
	protected static RestApi Api()
	{
		RestApi clCore = GetRestApi();
		if (!clCore)
		{
			clCore = CreateRestApi();
			clCore.SetOption(ERestOption.ERESTOPTION_READOPERATION, 15);
		}

		return clCore;
	}
	
	static string Link(string PlainId = "")
	{
		if (PlainId == "" && GetGame().IsClient())
		{
			return BaseUrl() + "Discord/" +  GetDayZGame().GetSteamId();
		}

		return BaseUrl() + "Discord/" + PlainId;
	}
	
	protected static void Post(string url, string jsonString = "{}", RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}

		RestContext ctx =  Api().GetRestContext(url);
		ctx.SetHeader(UApi().GetAuthToken());
		ctx.POST(UCBX , "", jsonString);
	}
	
	protected static void Get(string url, RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}

		RestContext ctx =  Api().GetRestContext(url);
		ctx.GET(UCBX , "");
	}
	
	protected static string PostNow(string url, string jsonString = "{}")
	{
		RestContext ctx =  Api().GetRestContext(url);
		ctx.SetHeader(UApi().GetAuthToken());
		return ctx.POST_now("", jsonString);
	}
	
	protected static string BaseUrl()
	{
		return UApiConfig().ServerURL;
	}
	
	static void AddRole(string GUID, string RoleId, RestCallback UCBX = NULL) 
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}

		string url = BaseUrl() + "Discord/AddRole/" + GUID;
		
		UApiDiscordRoleReq roleReq = new UApiDiscordRoleReq(RoleId);
		
		string jsonString = roleReq.ToJson();
		if (jsonString)
		{
			Post(url,jsonString,UCBX);
		} 
		else 
		{
			Print("[UPAI] [Discord] Error Adding Role (" + RoleId + ") To " + GUID);
		}
	}
	
	static void RemoveRole(string GUID, string RoleId, RestCallback UCBX = NULL) 
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}
		
		string url = BaseUrl() + "Discord/RemoveRole/" + GUID;
		
		UApiDiscordRoleReq roleReq = new UApiDiscordRoleReq(RoleId);
		
		string jsonString = roleReq.ToJson();
		
		if (jsonString)
		{
			Post(url,jsonString,UCBX);
		} 
		else 
		{
			Print("[UPAI] [Discord] Error Removing Role (" + RoleId + ") To " + GUID);
		}
	}
	
	
	static void GetUser(string GUID, RestCallback UCBX) 
	{
		string url = BaseUrl() + "Discord/Get/" + GUID;
		
		Post(url,"{}",UCBX);
	}
	
	static void GetUserWithPlainId(string plainId, RestCallback UCBX) 
	{
		string url = BaseUrl() + "Discord/GetWithPlainId/" + plainId;
		
		Post(url,"{}",UCBX);
	}
	
	
	static void CheckDiscord(string PlainId, RestCallback UCBX,  string baseUrl = "")	
	{		
		if (baseUrl == "")
		{
			baseUrl = BaseUrl();
		}

		string url = baseUrl + "Discord/Check/" + PlainId;
		
		Post(url,"{}",UCBX);
	}
	
	static void ChannelCreate(string Name, RestCallback UCBX, UApiChannelOptions Options = NULL) 
	{
		
		UApiCreateChannelObject obj = new UApiCreateChannelObject(Name, UApiChannelCreateOptions.Cast(Options));
		
		if (obj)
		{
			string url = BaseUrl() + "Discord/Channel/Create";
			
			Post(url,obj.ToJson(),UCBX);		
		}
	}
	
	static void ChannelDelete(string id, string reason,  RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}
		
		UApiUpdateChannelObject obj = new UApiUpdateChannelObject(reason, NULL);
		
		if (obj)
		{
			string url = BaseUrl() + "Discord/Channel/Delete/" + id;
			
			Post(url,obj.ToJson(),UCBX);		
		}
	}
	
	
	static void ChannelEdit(string id, string reason, UApiChannelUpdateOptions options, RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}
		
		UApiUpdateChannelObject obj = new UApiUpdateChannelObject(reason, UApiChannelUpdateOptions.Cast(options));
		
		if (obj)
		{
			string url = BaseUrl() + "Discord/Channel/Edit/" + id;
			
			Post(url,obj.ToJson(),UCBX);		
		}
	}
	
	static void ChannelSend(string id, string message, RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}
		
		UApiDiscordBasicMessage obj = new UApiDiscordBasicMessage(message);
		
		if (obj)
		{
			string url = BaseUrl() + "Discord/Channel/Send/" + id;
			
			Post(url,obj.ToJson(),UCBX);		
		}
	}
	
	static void ChannelSendEmbed(string id, UApiDiscordEmbed message, RestCallback UCBX = NULL)
	{
		if (!UCBX)
		{
			UCBX = m_StaticUApiSilentCallBack;
		}
				
		if (message)
		{
			string url = BaseUrl() + "Discord/Channel/Send/" + id;
			
			Post(url,message.ToJson(),UCBX);		
		}
	}
	
	static void ChannelMessages(string id, RestCallback UCBX, UApiDiscordChannelFilter filter = NULL,  string auth = "")
	{
		if (!filter)
		{
			filter = m_StaticDiscordFilter;
		}
		
		string url = BaseUrl() + "Discord/Channel/Messages/" + id;
		
		if (filter && UCBX)
		{
			Post(url, filter.ToJson(), UCBX);	
		} 
		else if (UCBX) 
		{
			Post(url, "{}", UCBX);
		}
	}
	
	// !!!!!WARNING!!!!! 
	
	// ALL OF THE FOLLOWING FUCNTIONS ARE THREAD BLOCKING ONLY RUN in Secondary Thread!
	
	
	
	
	
	
	
	
	
	
	// !!!!!WARNING!!!!!
	// THE FOLLOWING FUCNTION IS THREAD BLOCKING ONLY RUN in secondary Thread!
	static UApiDiscordUser GetUserNow(string GUID, bool ReturnError = false) 
	{
		string url = BaseUrl() + "Discord/Get/" + GUID;
		
		string Result = PostNow(url,"{}");
		
		JsonSerializer js = new JsonSerializer();
		string error;
		
		UApiDiscordUser user;
		
		js.ReadFromString(user, Result, error);
		
		if (error != "")
		{
			Print("[UPAI] [GetUserNow] Error: " + error);
		}
		if (user && (user.Status == "Success" || ReturnError))
		{
			return user;
		} 
		else if (!user && ReturnError)
		{
			user = new UApiDiscordUser;
			user.Status = "Error";
			user.Error = "Error Fetching Data";
			return user;
		}
		
		return NULL;
	}
	
	// !!!!!WARNING!!!!!
	// THE FOLLOWING FUCNTION IS THREAD BLOCKING ONLY RUN in secondary Thread!
	static UApiDiscordUser GetUserWithPlainIdNow(string plainId, bool ReturnError = false) 
	{
		string url = BaseUrl() + "Discord/GetWithPlainId/" + plainId;
		
		string Result = PostNow(url,"{}");
		Print(Result);
		JsonSerializer js = new JsonSerializer();
		string error;
		
		UApiDiscordUser user;
		
		Print("[UPAI] [GetUserWithPlainIdNow] Read from Sting");
		js.ReadFromString(user, Result, error);
		
		if (error != "")
		{
			Print("[UPAI] [GetUserWithPlainIdNow] Error: " + error);
		}
		if (user && (user.Status == "Success" || ReturnError))
		{
			Print("[UPAI] [GetUserWithPlainIdNow] Returning User");
			return user;
		} 
		else if (!user && ReturnError)
		{
			Print("[UPAI] [GetUserWithPlainIdNow] Returning Error");
			user = new UApiDiscordUser;
			user.Status = "Error";
			user.Error = "Error Fetching Data";
			return user;
		}

		Print("[UPAI] [GetUserWithPlainIdNow] Returning NULL");
		
		return NULL;		
	}	
}