modded class ScriptedDamageManagerComponent : BaseScriptedDamageManagerComponent
{
	private const float SPLATTER_INTERSECTION_DISTANCE = 1.5;
	private const float FAR_PLANE = 3.0;
	
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
		//vector hitNormal = hitTransform[2];
		auto character = ChimeraCharacter.Cast(m_owner);
		if (character)
		{
			SpawnBloodSplatterFromHit(hitPosition, hitDirection);
		}
	}
	
	private void SpawnBloodSplatterFromHit(vector hitPosition, vector hitDirection)
	{
		vector intersectionPosition;
				
		auto groundTraceParam = GetSurfaceIntersection(hitPosition, Vector(0, -1, 0), SPLATTER_INTERSECTION_DISTANCE, intersectionPosition);
		if (groundTraceParam.TraceEnt) // spawn splatter below character
		{
			m_world.CreateDecal(
				groundTraceParam.TraceEnt,
				m_owner.GetOrigin() + Vector(0, 1, 0),
				-groundTraceParam.TraceNorm,
				0,
				FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(1, 2),
				1,
				m_decalResourceName,
				-1,
				0xFFFFFFFF
			);
		}
		
		auto surfaceTraceParam = GetSurfaceIntersection(hitPosition, hitDirection, SPLATTER_INTERSECTION_DISTANCE, intersectionPosition);
		if (surfaceTraceParam.TraceEnt) // spawn splatter on surface
		{
			m_world.CreateDecal(
				surfaceTraceParam.TraceEnt,
				intersectionPosition - hitDirection * 0.25,
				hitDirection,
				0,
				FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(0.75, 1.0),
				1,
				m_decalResourceName,
				-1,
				0xFFFFFFFF
			);
		}
	}
	
	private TraceParam GetSurfaceIntersection(vector origin, vector direction, float distance, out vector intersectionPosition)
	{
		auto param = new TraceParam();
  		param.Start = origin;
  		param.End = origin + direction * distance;
  		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  		param.Exclude = m_owner;
		float intersectionDistance = m_world.TraceMove(param, NULL) * distance;
		intersectionPosition = origin + (direction * intersectionDistance);
		return param;
	}
}
