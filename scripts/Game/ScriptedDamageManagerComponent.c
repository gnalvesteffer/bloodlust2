modded class ScriptedDamageManagerComponent : BaseScriptedDamageManagerComponent
{
	private const float GROUND_SPLATTER_INTERSECTION_DISTANCE = 3;
	private const float SURFACE_SPLATTER_INTERSECTION_DISTANCE = 2;
	private const float FAR_PLANE = 3.0;
	private const float DAMAGE_THRESHOLD = 20.0;
	private const float DAMAGE_OPACITY_SCALAR = 3.0;
	private const string SPLATTER_MATERIAL_DIRECTORY = "{E1866216CEB08179}materials/splatters";
	private const int TOTAL_SPLATTER_MATERIALS = 7;
	
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

		if (damage >= DAMAGE_THRESHOLD)
		{
			vector hitPosition = hitTransform[0];
			vector hitDirection = hitTransform[1];
			//vector hitNormal = hitTransform[2];
			auto character = ChimeraCharacter.Cast(m_owner);
			if (character)
			{
				SpawnBloodSplatterFromHit(hitPosition, hitDirection, damage);
			}
		}
	}
	
	private void SpawnBloodSplatterFromHit(vector hitPosition, vector hitDirection, float damage)
	{
		vector intersectionPosition;
		int materialColor = Color.FromRGBA(255, 255, 255, Math.ClampInt(damage * DAMAGE_OPACITY_SCALAR, 0, 255)).PackToInt();
				
		auto groundTraceParam = GetSurfaceIntersection(hitPosition, Vector(0, -1, 0), GROUND_SPLATTER_INTERSECTION_DISTANCE, intersectionPosition);
		if (groundTraceParam.TraceEnt) // spawn splatter below character
		{
			m_world.CreateDecal(
				groundTraceParam.TraceEnt,
				m_owner.GetOrigin() + Vector(0, 1, 0),
				vector.Lerp(-groundTraceParam.TraceNorm, hitDirection, 0.5),
				0,
				FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(1, 2),
				1,
				GetRandomSplatterResourceName(),
				-1,
				materialColor
			);
		}
		
		auto surfaceTraceParam = GetSurfaceIntersection(hitPosition, hitDirection, SURFACE_SPLATTER_INTERSECTION_DISTANCE, intersectionPosition);
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
				GetRandomSplatterResourceName(),
				-1,
				materialColor
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
	
	private ResourceName GetRandomSplatterResourceName()
	{
		return new ResourceName(string.Format("%1/%2.emat", SPLATTER_MATERIAL_DIRECTORY, Math.RandomInt(1, TOTAL_SPLATTER_MATERIALS)));
	}
}
