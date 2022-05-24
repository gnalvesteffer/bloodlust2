modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{
	private ref BL_CharacterSplatterBehavior _splatterBehavior;
	private ref BL_CharacterBleedBehavior _bleedBehavior;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);

		auto world = owner.GetWorld();

		_splatterBehavior = new BL_CharacterSplatterBehavior;
		_splatterBehavior.OnInit(owner, world, this);

		_bleedBehavior = new BL_CharacterBleedBehavior;
		_bleedBehavior.OnInit(owner, world, this);
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
			_splatterBehavior.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
			_bleedBehavior.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
		}
	}

	override void OnFrame(IEntity owner, float timeSlice)
	{
		super.OnFrame(owner, timeSlice);
		_bleedBehavior.OnFrame();
	}
}
