class UApiQueryResult<Class T> : StatusObject 
{
	ref array<ref T> Results;
	int Count;
	
	static UApiQueryResult<T> CreateFrom(string stringData) 
	{
		UApiQueryResult<T> returnval;
		if (UApiJSONHandler<ref UApiQueryResult<T>>.FromString(stringData, returnval)) 
		{
			return returnval;
		} 
		Error("[UAPI] Failed to create Query Results");
		return NULL;
	}
	
	bool FromJson(string stringData) 
	{
		return UApiJSONHandler<ref UApiQueryResult<T>>.FromString(stringData, this);
	}

	array<ref T> GetResults() 
	{
		return Results;
	}
}