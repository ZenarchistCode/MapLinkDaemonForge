static Object SpawnObject(string type, vector position, vector orientation = "0 0 0", float scale = 1.0, bool replace = true, bool useScale = false)
{
    // Overwrite scale to 1.0
    if (!useScale)
        scale = 1.0;
	
	// Get objects within 1 meter of the spawn
    array<Object> objectsNearby = new array<Object>;
    GetGame().GetObjectsAtPosition3D(position, 0.1, objectsNearby, null);
	
	// Delete any existing objects
    foreach(Object z_obj : objectsNearby)
    {
        if (z_obj.GetType() == type && z_obj.GetPosition() == position && z_obj.GetOrientation() == orientation)
        {
            //if (!replace)
            //    return NULL;
            
            GetGame().ObjectDelete(z_obj);
			Error("DUPLICATE OBJECT FOUND SPAWNED BY INIT.C @ " + position + " - " + type);
        }
    }
    
    Object obj = GetGame().CreateObjectEx(type, position, ECE_SETUP | ECE_UPDATEPATHGRAPH | ECE_CREATEPHYSICS);
    if (!obj) 
    {
        Error("Failed to create object " + type);
        return NULL;
    }

    obj.SetPosition(position);
    obj.SetOrientation(orientation);
    obj.SetOrientation(obj.GetOrientation());
    obj.SetScale(scale);
    obj.Update();
    obj.SetAffectPathgraph(true, false);
    
    if (obj.CanAffectPathgraph()) 
    {
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetGame().UpdatePathgraphRegionByObject, 100, false, obj);
    }
    
    return obj;
}
