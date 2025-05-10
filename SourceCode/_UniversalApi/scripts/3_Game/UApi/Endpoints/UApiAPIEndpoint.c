class UApiAPIEndpoint extends UApiBaseEndpoint
{
	// Uses the QnA Endpoint to send requests returns QnAAnswer
	int QnA(string Question, string Key, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "QnA/" + Key;

		ref UApiQuestionRequest questionObj = new UApiQuestionRequest(Question);
		string jsonString = questionObj.ToJson();

		if (Question && jsonString && ReturnString)
		{
			Post(endpoint, jsonString, new UApiDBCallBack(cbInstance, cbFunction, cid, oid));
		}
		else if (Question && jsonString)
		{
			Post(endpoint, jsonString, new UApiDBNestedCallBack(new UApiCallback<QnAAnswer>(cbInstance, cbFunction, oid), cid));
		}
		else
		{
			Print("[UAPI] [Api] Error QnA K:" + Key + " Q: " + Question);
			cid = -1;
		}

		return cid;
	}

	// Helper function for returning the question to chat
	int ChatQnA(string Question, bool Slient)
	{
		if (Slient)
		{
			return QnA(Question, "", GetDayZGame(), "CBQnAChatMessageSilent");
		}
		return QnA(Question, "", GetDayZGame(), "CBQnAChatMessage");
	}

	// Sends request to get text translated returns a UApiTranslationResponse object
	int Translate(string Text, string To, string From, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Translate";

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiTranslationResponse>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiTranslationRequest translationReq = new UApiTranslationRequest(Text, {To}, From);

		if (translationReq && Text && To && DBCBX)
		{
			Post(endpoint, translationReq.ToJson(), DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Translate " + Text);
			cid = -1;
		}

		return cid;
	}

	int Translate(string Text, TStringArray To, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Translate";

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiTranslationResponse>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiTranslationRequest translationReq = new UApiTranslationRequest(Text, To);

		if (translationReq && Text && To && DBCBX)
		{
			Post(endpoint, translationReq.ToJson(), DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Translate " + Text);
			cid = -1;
		}

		return cid;
	}

	int Wit(string Text, string Key, Class cbInstance, string cbFunction, string oid = "")
	{
		int cid = UApi().CallId();
		string endpoint = "Wit/" + Key;

		RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = new UApiSilentCallBack;
		}

		UApiQuestionRequest questionreq = new UApiQuestionRequest(Text);
		string jsonString = questionreq.ToJson();

		if (jsonString && Text && Text != "" && Key && Key != "" && DBCBX)
		{
			Post(endpoint, jsonString, DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error Wit K:" + Key + " T:" + Text);
			cid = -1;
		}

		return cid;
	}

	int LUIS(string Text, string Key, Class cbInstance, string cbFunction, string oid = "")
	{
		int cid = UApi().CallId();
		string endpoint = "LUIS/" + Key;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = new UApiSilentCallBack;
		}

		ref UApiQuestionRequest questionreq = new UApiQuestionRequest(Text);
		string jsonString = questionreq.ToJson();

		if (jsonString && Text && Text != "" && Key && Key != "" && DBCBX)
		{
			Post(endpoint, jsonString, DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error LUIS K:" + Key + " T:" + Text);
			cid = -1;
		}

		return cid;
	}

	int SteamQuery(string ip, string queryPort, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "ServerQuery/Status/" + ip + "/" + queryPort;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiServerStatus>(cbInstance, cbFunction, oid), cid);
		}

		if (ip && ip != "" && queryPort && queryPort != "" && DBCBX)
		{
			Post(endpoint, "{}", DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error ServerQuery IP:" + ip + " Port:" + queryPort);
			cid = -1;
		}

		return cid;
	}

	int ServerQuery(string ip, string queryPort, Class cbInstance, string cbFunction, string oid = "")
	{
		int cid = UApi().CallId();
		string endpoint = "ServerQuery/Status/" + ip + "/" + queryPort;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else
		{
			DBCBX = new UApiSilentCallBack;
		}

		if (ip && ip != "" && queryPort && queryPort != "" && DBCBX)
		{
			Post(endpoint, "{}", DBCBX);
		}
		else
		{
			Print("[UAPI] [Api] Error ServerQuery IP:" + ip + " Port:" + queryPort);
			cid = -1;
		}

		return cid;
	}

	int ServerQueryObj(string ip, string queryPort, Class cbInstance, string cbFunction, string oid = "")
	{
		int cid = UApi().CallId();
		string endpoint = "ServerQuery/Status/" + ip + "/" + queryPort;

		if (ip && ip != "" && queryPort && queryPort != "")
		{
			Post(endpoint, "{}", new UApiDBNestedCallBack(new UApiCallback<UApiServerStatus>(cbInstance, cbFunction, oid), cid));
		}
		else
		{
			Print("[UAPI] [Api] Error ServerQuery IP:" + ip + " Port:" + queryPort);
			cid = -1;
		}

		return cid;
	}

	int Toxicity(string text, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Toxicity";

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiToxicityResponse>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiQuestionRequest questionreq = new UApiQuestionRequest(text);

		if (text && text != "" && questionreq && DBCBX)
		{
			Post(endpoint, questionreq.ToJson(), DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Toxicity", "Text:" + text + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int RandomNumbers(int count, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Random";

		if (count == -1)
		{
			count = 2048;
		}

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiRandomNumberResponse>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiRandomNumberRequest randomreq = new UApiRandomNumberRequest(count);

		if (count > 0 && count <= 2048 && randomreq && DBCBX)
		{
			Post(endpoint, randomreq.ToJson(), DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Random", "Count: " + count + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int RandomNumbersFull(int count, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Random/Full";

		if (count == -1)
		{
			count = 4096;
		}

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiRandomNumberResponse>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiRandomNumberRequest randomreq = new UApiRandomNumberRequest(count);

		if (count > 0 && count <= 4096 && randomreq && DBCBX)
		{
			Post(endpoint, randomreq.ToJson(), DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Random", "Count: " + count + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int CryptoPrice(string from, string to, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Crypto/Price/" + from + "/" + to;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiCryptoConvertResult>(cbInstance, cbFunction, oid), cid);
		}

		if (from && to && DBCBX)
		{
			Post(endpoint, "{}", DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Crypto Price", "From: " + from + " To: " + to + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int CryptoConvert(string from, string to, float value, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Crypto/Convert/" + from + "/" + to;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiCryptoConvertResult>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiCryptoConvertRequest req = new UApiCryptoConvertRequest(value);

		if (from && to && value > 0 && DBCBX)
		{
			Post(endpoint, req.ToJson(), DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Crypto Convert", "From: " + from + " To: " + to + " Value: " + value + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int Crypto(TStringArray from, string to, Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();
		string endpoint = "Crypto/" + to;

		ref RestCallback DBCBX;
		if (cbInstance && cbFunction != "" && ReturnString)
		{
			DBCBX = new UApiDBCallBack(cbInstance, cbFunction, cid, oid);
		}
		else if (cbInstance && cbFunction != "")
		{
			DBCBX = new UApiDBNestedCallBack(new UApiCallback<UApiCryptoResults>(cbInstance, cbFunction, oid), cid);
		}

		ref UApiCryptoRequest req = new UApiCryptoRequest(from);

		if (from && from.Count() > 0 && to && DBCBX)
		{
			Post(endpoint, req.ToJson(), DBCBX);
		}
		else
		{
			Error2("[UAPI] [Api] Error Crypto", "From: " + from.Count() + " To: " + to + " CID:" + cid);
			cid = -1;
		}

		return cid;
	}

	int Status(Class cbInstance, string cbFunction, string oid = "", bool ReturnString = false)
	{
		int cid = UApi().CallId();

		if (ReturnString)
		{
			Post("Status", "{}", new UApiDBCallBack(cbInstance, cbFunction, cid, oid));
		}
		else
		{
			Post("Status", "{}", new UApiDBNestedCallBack(new UApiCallback<UApiStatus>(cbInstance, cbFunction, oid), cid));
		}

		return cid;
	}
}