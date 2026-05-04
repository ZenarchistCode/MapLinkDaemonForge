modded class UApiEntityStore extends UApiObject_Base 
{
	override void SaveEntity(notnull EntityAI item, bool recursive = true)
	{
		m_Type = item.GetType();
		item.GetPersistentID(m_pid1, m_pid2, m_pid3, m_pid4); //Just for testing but maybe someone will find this usefull
		m_Health = item.GetHealth("", "");
		array<EntityAI> items = new array<EntityAI>;
		int i = 0;
		InventoryLocation il = new InventoryLocation;
		if (item.GetInventory().GetCurrentInventoryLocation(il))
		{
			m_Slot = il.GetSlot();
			m_Idx = il.GetIdx();
			m_Row = il.GetRow();
			m_Col = il.GetCol();
			m_Flip = il.GetFlip();
		}

		if (recursive)
		{
			item.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, items);
			if (items && items.Count() > 0)
			{
				//items.Debug();
				for (i = 0; i < items.Count(); i++)
				{
					EntityAI child_item = EntityAI.Cast(items.Get(i));
					if (!m_Cargo)
					{
						m_Cargo = new array<ref UApiEntityStore>;
					}

					if (child_item && (item.GetInventory().HasEntityInCargo(child_item) || item.GetInventory().HasAttachment(child_item)))
					{
						UApiEntityStore crg_itemstore = new UApiEntityStore(child_item);
						m_Cargo.Insert(crg_itemstore);
					} else 
					{
						break;
					}
				}
			}
		}

		PlayerBase HoldingPlayer;
		if (Class.CastTo(HoldingPlayer, item.GetHierarchyRootPlayer()))
		{
			m_IsInHands = (HoldingPlayer.GetHumanInventory().GetEntityInHands() == item);
			m_QuickBarSlot = HoldingPlayer.GetQuickBarEntityIndex(item);
		}

		m_IsMagazine = item.IsMagazine() && !item.IsAmmoPile();
		m_IsWeapon = item.IsWeapon();
		
		ItemBase itemB;
		if (Class.CastTo(itemB, item))
		{
			if (itemB.HasQuantity())
			{
				m_Quantity = itemB.GetQuantity();
			}

			m_Wet = itemB.GetWet();
			m_Tempature = itemB.GetTemperature();
			m_Energy = itemB.GetEnergy();
			if (itemB.GetCompEM())
			{
				m_IsOn = itemB.GetCompEM().IsSwitchedOn();
			}

			m_LiquidType = itemB.GetLiquidType();
			m_Agents = itemB.GetAgents();
			m_Cleanness = itemB.m_Cleanness;
			itemB.OnUApiSave(this);
		}

		Magazine_Base mag;
		float dmg;
		string cartType;
		if (m_IsMagazine && Class.CastTo(mag, item))
		{
			m_Quantity = mag.GetAmmoCount();
			for (i = 0; i < mag.GetAmmoCount(); i++)
			{
				dmg = -1;
				cartType = "";
				if (mag.GetCartridgeAtIndex(i, dmg, cartType) && cartType != "" && dmg >= 0)
				{
					if (!m_MagAmmo)
					{ 
						m_MagAmmo = new array<ref UApiAmmoData>;
					}

					float dmgSave = dmg;
					if (dmgSave < 0.000001)
						dmgSave = 0.001;

					m_MagAmmo.Insert(new UApiAmmoData(i, dmgSave, cartType));
				}
			}
		} else if (item.IsAmmoPile() && Class.CastTo(mag, item))
		{
			m_Quantity = mag.GetAmmoCount();
		}

		Weapon_Base weap;
		if (m_IsWeapon && Class.CastTo(weap, item))
		{
			int m_CurrentMuzzle = weap.GetCurrentMuzzle();
			m_Quantity = weap.GetTotalCartridgeCount(m_CurrentMuzzle);
			Write("m_IsJammed",  weap.IsJammed());
			Write("m_CurrentMuzzle", m_CurrentMuzzle);
			Write("m_Zeroing", weap.GetStepZeroing(weap.GetCurrentMuzzle()));
			Write("m_Zoom", weap.GetZoom());
			for (i = 0; i < weap.GetTotalCartridgeCount(m_CurrentMuzzle); i++)
			{
				dmg = -1;
				cartType = "";
				if (weap.GetInternalMagazineCartridgeInfo(m_CurrentMuzzle, i, dmg, cartType) && cartType != "" && dmg >= 0)
				{
					if (!m_MagAmmo)
					{ 
						m_MagAmmo = new array<ref UApiAmmoData>;
					}

					float dmgSaveW = dmg;
					if (dmgSaveW < 0.000001)
						dmgSaveW = 0.001;

					m_MagAmmo.Insert(new UApiAmmoData(i, dmgSaveW, cartType));
				}
			}

			if (!weap.IsChamberEmpty(m_CurrentMuzzle))
			{
				dmg = -1;
				cartType = "";
				if (weap.GetCartridgeInfo(m_CurrentMuzzle, dmg, cartType) && cartType != "" && dmg >= 0)
				{
					float dmgCh = dmg;
					if (dmgCh < 0.000001)
						dmgCh = 0.001;

					m_ChamberedRound = new UApiAmmoData(-1, dmgCh, cartType);
				}
			}

			if (!m_FireModes)
			{
				m_FireModes = new array<int>;
			}

			for (i = 0; i < weap.GetMuzzleCount(); ++i)
			{
				m_FireModes.Insert(weap.GetCurrentMode(i));
			}
		}
		
		// Damage System
		DamageZoneMap zones = new DamageZoneMap;
		DamageSystem.GetDamageZoneMap(item,zones);
		for(i = 0; i < zones.Count(); i++)
		{
			string zone = zones.GetKey(i);
			SaveZoneHealth(zone, item.GetHealth(zone, ""));
		}
		
		CarScript vehicle;
		if (Class.CastTo(vehicle,item))
		{
			m_IsVehicle = true;
			vehicle.OnUApiSave(this);
		}
	}
	
	override EntityAI Create(EntityAI parent = NULL, bool RestoreOrginalLocation = true)
	{
		EntityAI item;
		if (parent == NULL)
		{
			item = EntityAI.Cast(GetGame().CreateObject(m_Type, "0 0 0"));
		} 

		if (m_Slot == -1) 
		{
			item = EntityAI.Cast(parent.GetInventory().CreateEntityInCargoEx(m_Type, m_Idx, m_Row, m_Col, m_Flip));
		} else if (m_IsInHands)
		{
			PlayerBase player = PlayerBase.Cast(parent.GetHierarchyRootPlayer());
			if (player) 
			{
			 	item = EntityAI.Cast(player.GetHumanInventory().CreateInHands(m_Type));
			}		
		} else 
		{
			item = EntityAI.Cast(parent.GetInventory().CreateAttachmentEx(m_Type, m_Slot));
		}

		if (!item && parent)
		{
			item = EntityAI.Cast(GetGame().CreateObject(m_Type, parent.GetPosition()));
		} 

		if (!item)
		{
			Print("[UAPI] [ERROR] Couldn't create item " + m_Type);
			return NULL;
		} 

		LoadEntity(item);
		return item;
	}
	
	override EntityAI CreateAtPos(vector Pos, vector Ori = "0 0 0")
	{
		EntityAI item;
		item = EntityAI.Cast(GetGame().CreateObject(m_Type, Pos));
		if (!item)
		{
			Print("[UAPI] [UAPI] [ERROR] Couldn't create item " + m_Type);
			return NULL;
		} 

		item.SetPosition(Pos);
		item.SetOrientation(Ori);
		LoadEntity(item);
		return item;
	}
	
	override void LoadEntity(EntityAI item)
	{
		int i;
		item.SetHealth("", "", m_Health);
		ItemBase itemB;
		
		Weapon_Base weap;
		if (m_IsWeapon && Class.CastTo(weap, item))
		{
			int m_CurrentMuzzle = GetInt("m_CurrentMuzzle");
			if (m_CurrentMuzzle >= weap.GetMuzzleCount() || m_CurrentMuzzle < 0)
			{
				weap.SetCurrentMuzzle(m_CurrentMuzzle);
			}
		}

		if (m_Cargo && m_Cargo.Count() > 0)
		{
			for(i = 0; i < m_Cargo.Count(); i++)
			{
				if (m_Cargo.Get(i) && m_Cargo.Get(i).m_IsMagazine && m_IsWeapon && weap)
				{ 
					//Is a mag in a weapon
					Magazine_Base child_mag = Magazine_Base.Cast(m_Cargo.Get(i).Create(item));
					if (weap && child_mag)
					{
						weap.AttachMagazine(weap.GetCurrentMuzzle(), child_mag);
					}
				} else 
				{
					m_Cargo.Get(i).Create(item);				
				}
			}
		}

		if (Class.CastTo(itemB, item))
		{
			if (itemB.HasQuantity() && !itemB.IsMagazine())
			{
				itemB.SetQuantity(m_Quantity);
			}

			itemB.SetWet(m_Wet);
			itemB.SetTemperature(m_Tempature);
			itemB.SetLiquidType(m_LiquidType);
			if (itemB.GetCompEM())
			{
				itemB.GetCompEM().SetEnergy(m_Energy);
				if (m_IsOn)
				{
					itemB.GetCompEM().SwitchOn();
				}
			}

			itemB.RemoveAllAgents();//Removes any default agents then add the needed ones.
			itemB.TransferAgents(m_Agents);
			itemB.SetCleanness(m_Cleanness);
			itemB.OnUApiLoad(this);
		}

		PlayerBase HoldingPlayer;
		if (Class.CastTo(HoldingPlayer, item.GetHierarchyRootPlayer()))
		{
			if (m_QuickBarSlot >= 0)
			{
				Print("[UAPI] SetQuickBarEntityShortcut " + m_Type + " to " + m_QuickBarSlot);
				HoldingPlayer.SetQuickBarEntityShortcut(item, m_QuickBarSlot);
			}
		}

		Magazine_Base mag;
		float dmg;
		string cartType;
		int count;
		if (m_IsMagazine && Class.CastTo(mag, item))
		{
			UApi_ApplyMagazineCartridgesNow(mag);
			if (m_MagAmmo && m_MagAmmo.Count() > 0)
			{
				ItemBase ibMag = ItemBase.Cast(item);
				if (ibMag)
					ibMag.UApi_StoreMagazineCartridgeSnapshot(this);
			}
		} else if (item.IsAmmoPile() && Class.CastTo(mag, item))
		{
			count = m_Quantity;
			mag.ServerSetAmmoCount(count);
		}
		
		CarScript vehicle;
		if (m_IsVehicle && Class.CastTo(vehicle,item))
		{
			vehicle.OnUApiLoad(this);
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(vehicle.Synchronize);
		}
		
		// Damage System
		DamageZoneMap zones = new DamageZoneMap;
		DamageSystem.GetDamageZoneMap(item, zones);
		for(i = 0; i < zones.Count(); i++)
		{
			string zone = zones.GetKey(i);
			float health;
			if (ReadZoneHealth(zone, health))
			{
				item.SetHealth(zone, "", health);
			}
		}
		
		item.SetSynchDirty();
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(item.AfterStoreLoad);
	}

	void UApi_ApplyMagazineCartridgesNow(Magazine_Base mag)
	{
		if (!mag)
			return;

		UApi_MagazineStackRestore(mag, UApi_GetRestoreAmmoCount(), m_MagAmmo);
	}

	/** Rounds to restore: max of serialized count and per-cartridge rows (handles float m_Quantity vs JSON). */
	int UApi_GetRestoreAmmoCount()
	{
		int fromQty = Math.Floor(m_Quantity + 0.5);
		int fromArr = 0;
		if (m_MagAmmo)
			fromArr = m_MagAmmo.Count();

		int n = fromArr;
		if (fromQty > n)
			n = fromQty;

		if (n < 0)
			n = 0;

		return n;
	}

	/** Stable ascending copy by cartridge slot (used for magazine restore). */
	static array<ref UApiAmmoData> UApi_SortedMagAmmoSnapshot(array<ref UApiAmmoData> src)
	{
		array<ref UApiAmmoData> rem = new array<ref UApiAmmoData>;
		array<ref UApiAmmoData> dst = new array<ref UApiAmmoData>;
		if (!src)
			return dst;

		for (int si = 0; si < src.Count(); si++)
			rem.Insert(src.Get(si));

		while (rem.Count() > 0)
		{
			int bestI = -1;
			int bestIdx = int.MAX;
			for (int j = 0; j < rem.Count(); j++)
			{
				UApiAmmoData r = rem.Get(j);
				if (!r)
					continue;

				int ci = r.cartIndex();
				if (bestI < 0 || ci < bestIdx)
				{
					bestIdx = ci;
					bestI = j;
				}
			}

			if (bestI < 0)
				break;

			dst.Insert(rem.Get(bestI));
			rem.Remove(bestI);
		}

		return dst;
	}

	/**
	 * Refill using the same native path as vanilla CombineItems (ServerStoreCartridge),
	 * instead of SetCartridgeAtIndex which can remap or reject mod cartridge class names.
	 */
	static void UApi_MagazineStackRestore(Magazine_Base mag, int qtyHint, array<ref UApiAmmoData> rounds)
	{
		if (!mag)
			return;

		if (!rounds || rounds.Count() < 1)
		{
			int qc = qtyHint;
			if (qc < 0)
				qc = 0;

			mag.ServerSetAmmoCount(qc);
			mag.SetSynchDirty();
			return;
		}

		array<ref UApiAmmoData> sorted = UApi_SortedMagAmmoSnapshot(rounds);
		mag.ServerSetAmmoCount(0);

		for (int bi = 0; bi < sorted.Count(); bi++)
		{
			UApiAmmoData round = sorted.Get(bi);
			if (!round)
				continue;

			string ctype = round.cartTypeName();
			if (ctype == "")
				continue;

			float d = round.dmg();
			if (d < 0)
				continue;

			if (d < 0.000001)
				d = 0.001;

			if (!mag.ServerStoreCartridge(d, ctype))
				Print("[UAPI][WARN] UApi_MagazineStackRestore: ServerStoreCartridge failed type=" + ctype);
		}

		int want = qtyHint;
		if (want < sorted.Count())
			want = sorted.Count();

		UApiAmmoData pad = sorted.Get(sorted.Count() - 1);
		float padD = 0.001;
		string padT = "";
		if (pad)
		{
			padT = pad.cartTypeName();
			padD = pad.dmg();
			if (padD < 0.000001)
				padD = 0.001;
		}

		while (padT != "" && mag.GetAmmoCount() < want)
		{
			if (!mag.ServerStoreCartridge(padD, padT))
				break;
		}

		mag.SetSynchDirty();
	}
}