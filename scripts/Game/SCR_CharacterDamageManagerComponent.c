modded class SCR_CharacterDamageManagerComponent : ScriptedDamageManagerComponent
{
	private const float DAMAGE_THRESHOLD = 20.0;

	private IEntity m_owner;
	private World m_world;
	private ref BL_BleedingBehavior m_bleedingBehavior;

	override void OnInit(IEntity owner)
	{
		super.OnInit(owner);

		m_owner = GetOwner();
		m_world = m_owner.GetWorld();
		m_bleedingBehavior = new BL_BleedingBehavior;
		m_bleedingBehavior.OnInit(m_owner, m_world, this);
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

		if (damage >= DAMAGE_THRESHOLD)
		{
			vector hitPosition = hitTransform[0];
			vector hitDirection = hitTransform[1];
			//vector hitNormal = hitTransform[2];
			SpawnBloodSplatterFromHit(hitPosition, hitDirection, damage);
			m_bleedingBehavior.OnDamage(type, damage, pHitZone, instigator, hitTransform, speed, colliderID, nodeID);
		}
	}

	override void OnFrame(IEntity owner, float timeSlice)
	{
		super.OnFrame(owner, timeSlice);
		m_bleedingBehavior.OnFrame();
	}

	private void SpawnBloodSplatterFromHit(vector hitPosition, vector hitDirection, float damage)
	{
		vector intersectionPosition;
		int materialColor = BL_Utilities.CalculateBloodColorFromDamage(damage);

		// attempt ground splatter
		auto groundTraceParam = BL_Utilities.GetSurfaceIntersection(
			m_owner,
			m_world,
			hitPosition,
			Vector(0, -1, 0),
			BL_Constants.GROUND_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (groundTraceParam.TraceEnt) // spawn splatter below character
		{
			m_world.CreateDecal(
				groundTraceParam.TraceEnt,
				m_owner.GetOrigin() + Vector(0, BL_Constants.DECAL_FAR_PLANE / 4, 0),
				vector.Lerp(-groundTraceParam.TraceNorm, hitDirection, 0.5),
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(0.75, 1.5),
				1,
				GetRandomSplatterMaterialPath(),
				-1,
				materialColor
			);
		}

		// attempt surface splatter
		auto surfaceTraceParam = BL_Utilities.GetSurfaceIntersection(
			m_owner,
			m_world,
			hitPosition,
			hitDirection,
			BL_Constants.SURFACE_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (surfaceTraceParam.TraceEnt) // spawn splatter on surface
		{
			m_world.CreateDecal(
				surfaceTraceParam.TraceEnt,
				intersectionPosition - hitDirection * (BL_Constants.DECAL_FAR_PLANE / 4),
				hitDirection,
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(0.5, 1.0),
				1,
				GetRandomSplatterMaterialPath(),
				-1,
				materialColor
			);
		}
	}

	string GetRandomSplatterMaterialPath()
	{
		const string SPLATTER_MATERIAL_DIRECTORY = "{E1866216CEB08179}materials/splatters";
		const int TOTAL_SPLATTER_MATERIALS = 7;

		return string.Format("%1/%2.emat", SPLATTER_MATERIAL_DIRECTORY, Math.RandomInt(1, TOTAL_SPLATTER_MATERIALS)));
	}
}
