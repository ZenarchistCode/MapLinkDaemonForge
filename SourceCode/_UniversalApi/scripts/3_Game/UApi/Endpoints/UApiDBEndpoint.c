class UApiDBEndpoint extends UApiBaseEndpoint
{
	protected string m_Collection = "Object";

	void UApiDBEndpoint(string collection)
	{
		m_Collection = collection;
	}

	override protected string EndpointBaseUrl()
	{
		return UApiConfig().GetBaseURL() + m_Collection + "/";
	}

	int Save(string mod, string oid, string jsonString)
	{
		if (mod == "" || oid == "" || jsonString == "")
		{
			Error2("[UAPI] Error on DB Save", "OID, jsonString and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Save/" + oid + "/" + mod;

		Post(endpoint, jsonString, m_StaticUApiSilentCallBack);

		return cid;
	}

	int Save(string mod, string oid, string jsonString, Class cbInstance, string cbFunction)
	{
		if (mod == "" || oid == "" || jsonString == "")
		{
			Error2("[UAPI] Error on DB Save", "OID, jsonString and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Save/" + oid + "/" + mod;

		Post(endpoint, jsonString, new UApiDBCallBack(cbInstance, cbFunction, cid, oid));

		return cid;
	}

	int Save(string mod, string oid, string jsonString, UApiCallbackBase cb)
	{
		if (mod == "" || oid == "" || jsonString == "" || !cb)
		{
			Error2("[UAPI] Error on DB Save", "OID and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Save/" + oid + "/" + mod;

		cb.SetOID(oid);

		Post(endpoint, jsonString, new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Load(string mod, string oid, UApiCallbackBase cb, string jsonString = "{}")
	{
		if (mod == "" || oid == "" || jsonString == "" || !cb)
		{
			Error2("[UAPI] Error on DB Load", "OID and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Load/" + oid + "/" + mod;

		cb.SetOID(oid);

		Post(endpoint, jsonString, new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Load(string mod, string oid, Class cbInstance, string cbFunction, string jsonString = "{}")
	{
		if (mod == "" || oid == "" || jsonString == "")
		{
			Error2("[UAPI] Error on DB Load", "OID, jsonString and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Load/" + oid + "/" + mod;

		Post(endpoint, jsonString, new UApiDBCallBack(cbInstance, cbFunction, cid, oid));

		return cid;
	}

	int Query(string mod, UApiQueryBase query, UApiCallbackBase cb)
	{
		if (mod == "" || !query || !cb)
		{
			Error2("[UAPI] Error on DB Query", "Mod, query and callback must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Query/" + mod;

		cb.SetOID(mod);

		Post(endpoint, query.ToJson(), new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Query(string mod, UApiQueryBase query, Class cbInstance, string cbFunction)
	{
		if (mod == "" || !query)
		{
			Error2("[UAPI] Error on DB Query", "Mod and query must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Query/" + mod;

		Post(endpoint, query.ToJson(), new UApiDBCallBack(cbInstance, cbFunction, cid, ""));

		return cid;
	}

	int Increment(string mod, string oid, string element, float value = 1)
	{
		if (mod == "" || oid == "" || element == "")
		{
			Error2("[UAPI] Error on DB Incerment", "OID and Mod must be valid strings");
			return -1;
		}

		return Transaction(mod, oid, element, value);
	}

	int Transaction(string mod, string oid, string element, float value)
	{
		if (mod == "" || oid == "" || element == "")
		{
			Error2("[UAPI] Error on DB Transaction", "OID, element and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Transaction/" + oid + "/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		Post(endpoint, transaction.ToJson(), m_StaticUApiSilentCallBack);

		return cid;
	}

	int Transaction(string mod, string oid, string element, float value, UApiCallbackBase cb)
	{
		if (mod == "" || oid == "" || element == "" || !cb)
		{
			Error2("[UAPI] Error on DB Transaction", "OID, element, callback and Mod must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Transaction/" + oid + "/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		cb.SetOID(oid);

		Post(endpoint, transaction.ToJson(), new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Transaction(string mod, string oid, string element, float value, float min, float max, UApiCallbackBase cb)
	{
		if (mod == "" || oid == "" || element == "" || !cb)
		{
			Error2("[UAPI] Error on DB Transaction", "OID, element, callback and Mod must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Transaction/" + oid + "/" + mod;

		ref UApiValidatedTransaction transaction = new UApiValidatedTransaction(element, value, min, max);

		cb.SetOID(oid);

		Post(endpoint, transaction.ToJson(), new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Transaction(string mod, string oid, string element, float value, Class cbInstance, string cbFunction)
	{
		if (mod == "" || element == "" || oid == "")
		{
			Error2("[UAPI] Error on DB Transaction", "OID, element and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Transaction/" + oid + "/" + mod;

		ref UApiTransaction transaction = new UApiTransaction(element, value);

		Post(endpoint, transaction.ToJson(), new UApiDBCallBack(cbInstance, cbFunction, cid, oid));

		return cid;
	}

	int Transaction(string mod, string oid, string element, float value, float min, float max, Class cbInstance, string cbFunction)
	{
		if (mod == "" || oid == "" || element == "")
		{
			Error2("[UAPI] Error on DB Transaction", "OID, element, and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Transaction/" + oid + "/" + mod;

		ref UApiValidatedTransaction transaction = new UApiValidatedTransaction(element, value, min, max);

		Post(endpoint, transaction.ToJson(), new UApiDBCallBack(cbInstance, cbFunction, cid, oid));

		return cid;
	}

		int Update(string mod, string oid, string element, string value, string operation = UpdateOpts.SET)
	{
		if (mod == "" || oid == "" || element == "" || operation == "")
		{
			Error2("[UAPI] Error on DB Update", "OID, Element, operation, and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Update/" + oid + "/" + mod;

		ref UApiUpdateData updatedata = new UApiUpdateData(element, value, operation);

		Post(endpoint, updatedata.ToJson(), m_StaticUApiSilentCallBack);

		return cid;
	}

	int Update(string mod, string oid, string element, string value, string operation, UApiCallbackBase cb)
	{
		if (mod == "" || oid == "" || element == "" || operation == "" || !cb)
		{
			Error2("[UAPI] Error on DB Update", "OID, callback, operation, Element and Mod must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Update/" + oid + "/" + mod;

		ref UApiUpdateData updatedata = new UApiUpdateData(element, value, operation);

		cb.SetOID(oid);
		Post(endpoint, updatedata.ToJson(), new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int Update(string mod, string oid, string element, string value, string operation, Class cbInstance, string cbFunction)
	{
		if (mod == "" || oid == "" || element == "" || operation == "")
		{
			Error2("[UAPI] Error on DB Update", "OID, Element, operation, and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = m_StaticUApiSilentCallBack;
		}

		string endpoint = "Update/" + oid + "/" + mod;

		ref UApiUpdateData updatedata = new UApiUpdateData(element, value, operation);

		Post(endpoint, updatedata.ToJson(), DBCBX);

		return cid;
	}

	int QueryUpdate(UApiQueryBase query, string mod, string element, string value, string operation = UpdateOpts.SET)
	{
		if (!query || mod == "" || element == "" || operation == "")
		{
			Error2("[UAPI] Error on DB Update", "OID, Element, operation, and Mod must be valid strings");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Query/Update/" + mod;

		ref UApiDBQueryUpdate updatedata = new UApiDBQueryUpdate(query, element, value, operation);

		Post(endpoint, updatedata.ToJson(), m_StaticUApiSilentCallBack);

		return cid;
	}

	int QueryUpdate(UApiQueryBase query, string mod, string element, string value, string operation, UApiCallbackBase cb)
	{
		if (!query || mod == "" || element == "" || operation == "" || !cb)
		{
			Error2("[UAPI] Error on DB Update", "OID, callback, operation, Element and Mod must be valid");
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "Query/Update/" + mod;

		ref UApiUpdateData updatedata = new UApiUpdateData(element, value, operation);

		cb.SetOID(mod);
		Post(endpoint, updatedata.ToJson(), new UApiDBNestedCallBack(cb, cid));

		return cid;
	}

	int QueryUpdate(UApiQueryBase query, string mod, string element, string value, string operation, Class cbInstance, string cbFunction)
	{
		if (!query || mod == "" || element == "" || operation == "")
		{
			Error2("[UAPI] Error on DB Update", "OID, Element, operation, and Mod must be valid strings");
			return -1;
		}

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

		string endpoint = "Query/Update/" + mod;

		ref UApiDBQueryUpdate updatedata = new UApiDBQueryUpdate(query, element, value, operation);

		Post(endpoint, updatedata.ToJson(), DBCBX);

		return cid;
	}

	int PublicSave(string mod, string oid, string jsonString, Class cbInstance = NULL, string cbFunction = "")
	{
		if (m_Collection != "Player")
		{
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "PublicSave/" + oid + "/" + mod;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = m_StaticUApiSilentCallBack;
		}

		if (jsonString)
		{
			Post(endpoint, jsonString, DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Saving " + endpoint + " Data for " + mod);
			cid = -1;
		}

		return cid;
	}

	int PublicLoad(string mod, string oid, Class cbInstance, string cbFunction, string jsonString = "{}", string baseUrl = "")
	{
		if (m_Collection != "Player")
		{
			return -1;
		}

		int cid = UApi().CallId();
		string endpoint = "PublicLoad/" + oid + "/" + mod;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = m_StaticUApiSilentCallBack;
		}

		if (baseUrl != "" && DBCBX)
		{
			string url = baseUrl + m_Collection + "/" + endpoint;
			UApi().Post(url, jsonString, DBCBX);
		}
		else if (DBCBX)
		{
			Post(endpoint, jsonString, DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Loading Player Data for " + mod);
			cid = -1;
		}

		return cid;
	}
}