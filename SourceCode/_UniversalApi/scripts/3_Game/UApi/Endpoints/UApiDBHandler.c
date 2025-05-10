class UApiDBHandler<Class T> extends UApiDBHandlerBase
{
	override int Save(string oid, Class object)
	{
		string jsonString = "{}";
		T obj;
		if (Class.CastTo(obj, object) && UApiJSONHandler<T>.GetString(obj, jsonString))
		{
			return UApi().db(Database).Save(Mod, oid, jsonString);
		}
		Error2("[UAPI] DB HANDLER Save", "Error convertering to JSON or casting make sure you are passing the right class type");
		return -1;
	}

	override int Save(string oid, Class object, Class cbInstance, string cbFunction)
	{
		string jsonString = "{}";
		T obj;
		if (Class.CastTo(obj, object) && UApiJSONHandler<T>.GetString(obj, jsonString))
		{
			return UApi().db(Database).Save(Mod, oid, jsonString, new UApiCallback<T>(cbInstance, cbFunction));
		}
		Error2("[UAPI] DB HANDLER Save", "Error convertering to JSON or casting make sure you are passing the right class type");
		return -1;
	}

	override int Load(string oid, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Load(Mod, oid, new UApiCallback<T>(cbInstance, cbFunction), "{}");
	}

	override int Load(string oid, Class cbInstance, string cbFunction, string defaultJson)
	{
		return UApi().db(Database).Load(Mod, oid, new UApiCallback<T>(cbInstance, cbFunction), defaultJson);
	}

	override int Load(string oid, Class cbInstance, string cbFunction, Class inObject)
	{
		string jsonString = "{}";
		T obj;
		if (Class.CastTo(obj, inObject) && UApiJSONHandler<T>.GetString(obj, jsonString))
		{
			ref UApiCallbackLoader<T> cb = new UApiCallbackLoader<T>(cbInstance, cbFunction);
			cb.SetObject(obj);
			return UApi().db(Database).Load(Mod, oid, cb, jsonString);
		}
		Error2("[UAPI] DB HANDLER Load", "Error convertering to JSON or casting make sure you are passing the right class type");
		return -1;
	}

	override int Query(UApiQueryBase query, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Query(Mod, query, new UApiCallback<UApiQueryResult<T>>(cbInstance, cbFunction));
	}

	override int Query(string query, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Query(Mod, new UApiDBQuery(query), new UApiCallback<UApiQueryResult<T>>(cbInstance, cbFunction));
	}
}

class UApiDBHandlerBase extends Managed
{
	string Mod = "";
	int Database = PLAYER_DB;

	void UApiDBHandlerBase(string mod, int database = PLAYER_DB)
	{
		Mod = mod;
		Database = database;
	}

	int Save(string oid, Class object)
	{
		Error2("[UAPI] UApiDBHandlerBase SAVE", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int Save(string oid, Class object, Class cbInstance, string cbFunction)
	{
		Error2("[UAPI] UApiDBHandlerBase SAVE", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int Load(string oid, Class cbInstance, string cbFunction)
	{
		Error2("[UAPI] UApiDBHandlerBase LOAD", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int Load(string oid, Class cbInstance, string cbFunction, string defaultJson)
	{
		Error2("[UAPI] UApiDBHandlerBase LOAD", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int Load(string oid, Class cbInstance, string cbFunction, Class inObject)
	{
		Error2("[UAPI] UApiDBHandlerBase LOAD", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int LoadJson(string oid, Class cbInstance, string cbFunction, string defaultJson = "{}")
	{
		return UApi().db(Database).Load(Mod, oid, cbInstance, cbFunction, defaultJson);
	}

	int Increment(string oid, string element, float value = 1)
	{
		return Transaction(oid, element, value);
	}

	int Transaction(string oid, string element, float value)
	{
		return UApi().db(Database).Transaction(Mod, oid, element, value);
	}

	int Transaction(string oid, string element, float value, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Transaction(Mod, oid, element, value, new UApiCallback<UApiTransactionResponse>(cbInstance, cbFunction));
	}

	int Transaction(string oid, string element, float value, float min, float max, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Transaction(Mod, oid, element, value, min, max, new UApiCallback<UApiTransactionResponse>(cbInstance, cbFunction));
	}

	int Update(string oid, string element, string value, string operation = UpdateOpts.SET)
	{
		return UApi().db(Database).Update(Mod, oid, element, value, operation);
	}

	int Update(string oid, string element, string value, string operation, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).Update(Mod, oid, element, value, operation, new UApiCallback<UApiUpdateResponse>(cbInstance, cbFunction));
	}

	int QueryUpdate(UApiQueryBase query, string element, string value, string operation = UpdateOpts.SET)
	{
		return UApi().db(Database).QueryUpdate(query, Mod, element, value, operation);
	}

	int QueryUpdate(UApiQueryBase query, string element, string value, string operation, Class cbInstance, string cbFunction)
	{
		return UApi().db(Database).QueryUpdate(query, Mod, element, value, operation, new UApiCallback<UApiQueryUpdateResponse>(cbInstance, cbFunction));
	}

	int Query(UApiQueryBase query, Class cbInstance, string cbFunction)
	{
		Error2("[UAPI] UApiDBHandlerBase QUERY", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	int Query(string query, Class cbInstance, string cbFunction)
	{
		Error2("[UAPI] UApiDBHandlerBase QUERY", "Incorrect Ussage class is not type of UApiDBHandler<T>");
		return -1;
	}

	void Cancel(int cid)
	{
		UApi().RequestCallCancel(cid);
	}
}