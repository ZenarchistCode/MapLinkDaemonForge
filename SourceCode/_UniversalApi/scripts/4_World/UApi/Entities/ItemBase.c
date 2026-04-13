modded class ItemBase 
{
	protected ref array<ref UApiAmmoData> m_UApiDeferredMagAmmo;
	protected int m_UApiDeferredMagCount;

	override void OnUApiSave(UApiEntityStore data)
	{
		super.OnUApiSave(data);
	}
	
	override void OnUApiLoad(UApiEntityStore data)
	{
		super.OnUApiLoad(data);
	}

	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
	}

	/** Snapshot per-cartridge data from UApiEntityStore (call during LoadEntity). AfterStoreLoad + delayed pass reapply. */
	void UApi_StoreMagazineCartridgeSnapshot(UApiEntityStore store)
	{
		if (!store || !store.m_MagAmmo || store.m_MagAmmo.Count() < 1)
			return;

		if (!IsMagazine() || IsAmmoPile())
			return;

		m_UApiDeferredMagCount = store.UApi_GetRestoreAmmoCount();
		if (!m_UApiDeferredMagAmmo)
			m_UApiDeferredMagAmmo = new array<ref UApiAmmoData>;
		else
			m_UApiDeferredMagAmmo.Clear();

		for (int c = 0; c < store.m_MagAmmo.Count(); c++)
		{
			UApiAmmoData r = store.m_MagAmmo.Get(c);
			if (!r)
				continue;

			float rd = r.dmg();
			if (rd >= 0 && rd < 0.000001)
				rd = 0.001;

			m_UApiDeferredMagAmmo.Insert(new UApiAmmoData(r.cartIndex(), rd, r.cartTypeName()));
		}

		if (m_UApiDeferredMagAmmo.Count() < 1)
			return;

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.UApi_DelayedMagazineCartridgePass, 1200, false);
	}

	void UApi_DelayedMagazineCartridgePass()
	{
		UApi_ApplyStoredMagazineCartridges();
		if (m_UApiDeferredMagAmmo)
			m_UApiDeferredMagAmmo.Clear();
	}

	void UApi_ApplyStoredMagazineCartridges()
	{
		if (!m_UApiDeferredMagAmmo || m_UApiDeferredMagAmmo.Count() < 1)
			return;

		Magazine_Base mag = Magazine_Base.Cast(this);
		if (!mag || !IsMagazine() || IsAmmoPile())
			return;

		int ammoCount = m_UApiDeferredMagCount;
		if (ammoCount < 0)
			ammoCount = 0;

		UApiEntityStore.UApi_MagazineStackRestore(mag, ammoCount, m_UApiDeferredMagAmmo);
	}
}
