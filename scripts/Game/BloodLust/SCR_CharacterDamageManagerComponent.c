modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{
	private ref BL_CharacterSplatterBehavior m_splatterBehavior;
	private ref BL_CharacterBleedBehavior m_bleedBehavior;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);

		auto world = owner.GetWorld();

		m_splatterBehavior = new BL_CharacterSplatterBehavior;
		m_splatterBehavior.OnInit(owner, world, this);

		m_bleedBehavior = new BL_CharacterBleedBehavior;
		m_bleedBehavior.OnInit(owner, world, this);
	}

	override void OnDamage(
			EDamageType type,
			float damage,
			HitZone pHitZone,
			IEntity instigator,
			inout vector hitTransform[3],
			float speed,
			int colliderID,
			int nodeID
	)
	{
		super.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);

		if (damage >= BL_Constants.DAMAGE_THRESHOLD)
		{
			m_splatterBehavior.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
			m_bleedBehavior.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
		}
	}

	override void OnFrame(IEntity owner, float timeSlice)
	{
		super.OnFrame(owner, timeSlice);
		m_bleedBehavior.OnFrame();
	}
}
