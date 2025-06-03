class CfgPatches
{
	class MapLink
	{
		requiredVersion=0.1;
		requiredAddons[]={
			"UniversalApi",
			"MapLinkBase"
		};
	};
};

class CfgMods
{
	class MapLink
	{
		dir="MapLink";
        name="MapLink";
        credits="DaemonForge, Iceblade, Dumpgrah";
        author="DaemonForge";
        authorID="0";
        version="0.1";
        extra=0;
        type="mod";
		inputs = "MapLink/inputs/inputs.xml";
	    dependencies[]={ "Game", "World", "Mission"};
	    class defs
	    {
			
			class imageSets
			{
				files[]=
				{
					"MapLink/gui/maplink_icons.imageset",
					"MapLink/gui/maplink_money.imageset"
				};
			};
			class gameScriptModule
            {
				value = "";
                files[]={
					"MapLink/scripts/3_Game"
				};
            };
			class worldScriptModule
            {
                value="";
                files[]={ 
					"MapLink/scripts/4_World"
				};
            };
			
	        class missionScriptModule
            {
                value="";
                files[]={
					"MapLink/scripts/5_Mission"
				};
            };
        };
    };
};

class CfgVehicles
{
	class HouseNoDestruct;
	class MapLink_boat_small9 : HouseNoDestruct
	{
		scope = 1;
		model="DZ\structures_sakhal\wrecks\boat_small9.p3d";
	};
	class MapLink_TrailMap_Enoch : HouseNoDestruct
	{
		scope = 1;
		model = "\DZ\structures_bliss\signs\Tourist\TrailMap_Enoch.p3d";
	}; 
	class MapLink_TrailMap_Chernarus : HouseNoDestruct
	{
		scope = 1;
		model = "\DZ\structures\signs\Tourist\trailmap_noarrow.p3d";
	}; 
};