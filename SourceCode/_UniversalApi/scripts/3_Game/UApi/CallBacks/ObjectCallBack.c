// This method has to be used for the template class to work,
// you can't have a template class that extends RestCallback

class UApiCallback<Class T> extends UApiCallbackBase
{
	override void OnError(int errorCode, int cid)
	{
		if (GetInstance() && Function != "")
		{
			GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, T>(cid, errorCode, OID, NULL));
		}
	}

	override void OnSuccess(string jsonData, int cid)
	{
		if (GetInstance() && Function != "")
		{
			ref T obj;
			if (UApiJSONHandler<T>.FromString(jsonData, obj))
			{
				int rstatus = UAPI_SUCCESS;
				StatusObject sobj;
				if (Class.CastTo(sobj, obj))
				{
					switch (sobj.Status)
					{
						case "NotFound":
							rstatus = UAPI_NOTFOUND;
							break;
						case "NoResults":
							rstatus = UAPI_EMPTY;
							break;
						case "Error":
							rstatus = UAPI_ERROR;
							break;
						case "NoPerms":
						case "NoAuth":
						case "InvalidAuth":
							rstatus = UAPI_UNAUTHORIZED;
							break;
						case "NotSetup":
							rstatus = UAPI_NOTSETUP;
							break;
					}
				}
				GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, T>(cid, rstatus, OID, obj));
			}
			else
			{
				GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, T>(cid, UAPI_JSONERROR, OID, NULL));
			}
		}
	}
}


class UApiCallbackLoader<Class T> extends UApiCallbackBase
{
	ref T obj;

	void SetObject(T object)
	{
		obj = object;
	}

	override void OnError(int errorCode, int cid)
	{
		if (GetInstance() && Function != "")
		{
			GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, T>(cid, errorCode, OID, NULL));
		}
	}

	override void OnSuccess(string jsonData, int cid)
	{
		int rstatus = UAPI_JSONERROR;

		if (UApiJSONHandler<T>.FromString(jsonData, obj))
		{
			rstatus = UAPI_SUCCESS;
			StatusObject sobj;
			if (Class.CastTo(sobj, obj))
			{
				switch (sobj.Status)
				{
					case "NotFound":
						rstatus = UAPI_NOTFOUND;
						break;
					case "NoResults":
						rstatus = UAPI_EMPTY;
						break;
					case "Error":
						rstatus = UAPI_ERROR;
						break;
					case "NoPerms":
					case "NoAuth":
					case "InvalidAuth":
						rstatus = UAPI_UNAUTHORIZED;
						break;
					case "NotSetup":
						rstatus = UAPI_NOTSETUP;
						break;
				}
			}
		}

		if (GetInstance() && Function != "")
		{
			GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, T>(cid, rstatus, OID, obj));
		}
	}
}


class UApiJSONCallback extends UApiCallbackBase
{
	override void OnError(int errorCode, int cid)
	{
		if (GetInstance() && Function != "")
		{
			GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, string>(cid, errorCode, OID, "{}"));
		}
	}

	override void OnSuccess(string jsonData, int cid)
	{
		if (GetInstance() && Function != "")
		{
			GetGame().GameScript.CallFunctionParams(GetInstance(), Function, NULL, new Param4<int, int, string, string>(cid, UAPI_SUCCESS, OID, jsonData));
		}
	}
}


class UApiCallbackBase extends Managed
{
	protected Class Instance;
	protected string Function;
	protected string OID;

	protected Class GetInstance()
	{
		return Instance;
	}

	void UApiCallbackBase(Class instance, string function, string oid = "")
	{
		Instance = instance;
		Function = function;
		OID = oid;
	}

	// So I can set it automatically to save on coding for other devs
	void SetOID(string oid)
	{
		if (OID == "" && oid != "")
		{
			OID = oid;
		}
	}

	void OnError(int errorCode, int cid)
	{
		Error2("[UAPI] Callback Error", "Error calling back OnError, not set up correctly CallId: " + cid);
	}

	void OnSuccess(string jsonData, int cid)
	{
		Error2("[UAPI] Callback Error", "Error calling back OnSuccess, not set up correctly CallId: " + cid);
	}
}


class UApiDBNestedCallBack extends RestCallback
{
	protected int CallId;
	protected ref UApiCallbackBase m_CB;

	protected UApiCallbackBase GetCB()
	{
		return m_CB;
	}

	void UApiDBNestedCallBack(UApiCallbackBase cb, int callId)
	{
		m_CB = cb;
		CallId = callId;
	}

	override void OnError(int errorCode)
	{
		if (UApi().IsCallCanceled(CallId))
		{
			Print("[UAPI] Call " + CallId + " not called as it was requested to be canceled - OnError " + UApi().ErrorToString(errorCode));
			return;
		}

		int rstatus = UAPI_SERVERERROR;
		if (errorCode == ERestResultState.EREST_ERROR_CLIENTERROR)
		{
			rstatus = UAPI_CLIENTERROR;
		}

		GetCB().OnError(rstatus, CallId);
	}

	override void OnTimeout()
	{
		if (UApi().IsCallCanceled(CallId))
		{
			Print("[UAPI] Call " + CallId + " not called as it was requested to be canceled - OnTimeout");
			return;
		}

		GetCB().OnError(UAPI_TIMEOUT, CallId);
	}

	override void OnSuccess(string data, int dataSize)
	{
		if (UApi().IsCallCanceled(CallId))
		{
			Print("[UAPI] Call " + CallId + " not called as it was requested to be canceled - OnSuccess");
			return;
		}

		if (dataSize <= 0 || data == "{}" || data == "" || data == "{ }")
		{
			GetCB().OnError(UAPI_EMPTY, CallId);
			return;
		}

		GetCB().OnSuccess(data, CallId);
	}
}
