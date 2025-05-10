class UApiDBGlobalEndpoint extends UApiBaseEndpoint
{
	override protected string EndpointBaseUrl()
	{
		return UApiConfig().GetBaseURL() + "Globals/";
	}

	int Save(string mod, string jsonString)
	{
		int cid = UApi().CallId();
		string endpoint = "/Save/" + mod;

		if (mod && jsonString)
		{
			Post(endpoint, jsonString, m_StaticUApiSilentCallBack);
		}
		else
		{
			Print("[UAPI] [Api] Error Saving " + endpoint + " Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int Save(string mod, string jsonString, Class cbInstance, string cbFunction)
	{
		int cid = UApi().CallId();
		string endpoint = "/Save/" + mod;

		if (mod && jsonString)
		{
			Post(endpoint, jsonString, new UApiDBCallBack(cbInstance, cbFunction, cid, mod));
		}
		else
		{
			Print("[UAPI] [Api] Error Saving " + endpoint + " Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int Save(string mod, string jsonString, UApiCallbackBase cb)
	{
		int cid = UApi().CallId();
		string endpoint = "/Save/" + mod;

		if (mod && jsonString && cb)
		{
			cb.SetOID(mod);
			Post(endpoint, jsonString, new UApiDBNestedCallBack(cb, cid));
		}
		else
		{
			Print("[UAPI] [Api] Error Saving " + endpoint + " Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int Load(string mod, Class cbInstance, string cbFunction, string jsonString = "{}")
	{
		int cid = UApi().CallId();
		string endpoint = "/Load/" + mod;

		if (mod && jsonString)
		{
			Post(endpoint, jsonString, new UApiDBCallBack(cbInstance, cbFunction, cid, mod));
		}
		else
		{
			Print("[UAPI] [Api] Error Loading Player Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int Load(string mod, UApiCallbackBase cb, string jsonString = "{}")
	{
		int cid = UApi().CallId();
		string endpoint = "/Load/" + mod;

		if (mod && cb && jsonString)
		{
			cb.SetOID(mod);
			Post(endpoint, jsonString, new UApiDBNestedCallBack(cb, cid));
		}
		else
		{
			Print("[UAPI] [Api] Error Loading Player Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int Increment(string mod, string element, float value = 1)
	{
		return Transaction(mod, element, value);
	}

	int Transaction(string mod, string element, float value)
	{
		int cid = UApi().CallId();
		string endpoint = "/Transaction/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		if (element && transaction && mod)
		{
			Post(endpoint, transaction.ToJson(), m_StaticUApiSilentCallBack);
		}
		else
		{
			Print("[UAPI] [Api] Error Transaction " + mod);
			cid = -1;
		}

		return cid;
	}

	int Transaction(string mod, string element, float value, Class cbInstance, string cbFunction)
	{
		int cid = UApi().CallId();
		string endpoint = "/Transaction/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		if (element && transaction && mod)
		{
			Post(endpoint, transaction.ToJson(), new UApiDBCallBack(cbInstance, cbFunction, cid, mod));
		}
		else
		{
			Print("[UAPI] [Api] Error Transaction " + mod);
			cid = -1;
		}

		return cid;
	}

	int Transaction(string mod, string element, float value, UApiCallbackBase cb)
	{
		int cid = UApi().CallId();
		string endpoint = "/Transaction/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		if (element && transaction && mod)
		{
			cb.SetOID(mod);
			Post(endpoint, transaction.ToJson(), new UApiDBNestedCallBack(cb, cid));
		}
		else
		{
			Print("[UAPI] [Api] Error Transaction " + mod);
			cid = -1;
		}

		return cid;
	}

	int Update(string mod, string element, string value, string operation = UpdateOpts.SET, Class cbInstance = NULL, string cbFunction = "")
	{
		int cid = UApi().CallId();
		ref RestCallback DBCBX;

		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, mod);
		}
		else
		{
			DBCBX = m_StaticUApiSilentCallBack;
		}

		string endpoint = "/Update/" + mod;

		ref UApiUpdateData updatedata = new UApiUpdateData(element, value, operation);

		if (element && updatedata && DBCBX)
		{
			Post(endpoint, updatedata.ToJson(), DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Transaction " + mod);
			cid = -1;
		}

		return cid;
	}
}