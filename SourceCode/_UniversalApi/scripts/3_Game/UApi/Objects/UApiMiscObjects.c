class UApiTextObject extends UApiObject_Base 
{
	string Text = "";
	
	void UApiTextObject(string text) 
	{
		Text = text;
	}
	
	override string ToJson() 
	{
		string jsonString = JsonFileLoader<ref UApiTextObject>.JsonMakeData(this);
		return jsonString;
	}
}

class UApiToxicityResponse extends StatusObject 
{
	float IdentityAttack;
	float Insult;
	float Obscene;
	float SevereToxicity;
	float SexualExplicit;
	float Threat;
	float Toxicity;
}

class UApiRandomNumberResponse extends StatusObject 
{
	ref TIntArray Numbers;
}