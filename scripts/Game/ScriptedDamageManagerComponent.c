modded class ScriptedDamageManagerComponent : BaseScriptedDamageManagerComponent
{
	private const float SPLATTER_INTERSECTION_DISTANCE = 3.0;
	
	private ResourceName m_decalResourceName = "{E1866216CEB08179}materials/BloodSplatter.emat";
	private IEntity m_owner = GetOwner();
	private World m_world = GetOwner().GetWorld();

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

		vector hitPosition = hitTransform[0];
		vector hitDirection = hitTransform[1];
		vector hitNormal = hitTransform[2];
		auto param = new TraceParam();
  		param.Start = hitPosition;
  		param.End = hitPosition + hitDirection * SPLATTER_INTERSECTION_DISTANCE;
  		param.Flags = TraceFlags.WORLD;
		param.Flags |= TraceFlags.ENTS;
  		param.Exclude = m_owner;
		float intersectionDistance = m_world.TraceMove(param, NULL) * SPLATTER_INTERSECTION_DISTANCE;
		if (param.TraceEnt)
		{
			Decal decal = m_world.CreateDecal(
				param.TraceEnt,
				param.Start,
				hitDirection,
				0,
				3,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				1,
				1,
				m_decalResourceName,
				-1,
				0xFFFFFFFF
			);
		}
	}
}
