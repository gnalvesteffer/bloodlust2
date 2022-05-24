class BL_CharacterSplatterBehavior
{
	private static const float MIN_DECAL_SIZE = 1;
	private static const float MAX_DECAL_SIZE = 1.25;
	private static const ref array<string> SPLATTER_MATERIAL_PATHS =
	{
		"{E1866216CEB08179}materials/splatters/1.emat",
		"{8E3E81D1390C727B}materials/splatters/2.emat",
		"{C7505DF437AA8B21}materials/splatters/3.emat",
		"{8E8848EB456CFF4B}materials/splatters/4.emat",
		"{7A800F7348B1973F}materials/splatters/5.emat",
		"{24682630F73C1930}materials/splatters/6.emat",
		"{D06061A8FAE17144}materials/splatters/7.emat",
	};

	private IEntity _owner;
	private World _world;
	private SCR_CharacterDamageManagerComponent _damageManagerComponent;

	void OnInit(
		IEntity owner,
		World world,
		SCR_CharacterDamageManagerComponent damageManagerComponent
	)
	{
		_owner = owner;
		_world = world;
		_damageManagerComponent = damageManagerComponent;
	}

	void OnDamage(
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
		vector hitPosition = hitTransform[0];
		vector hitDirection = hitTransform[1];
		//vector hitNormal = hitTransform[2];
		SpawnBloodSplatterFromHit(hitPosition, hitDirection, damage);
	}

	void OnFrame(IEntity owner, float timeSlice)
	{
	}

	private void SpawnBloodSplatterFromHit(vector hitPosition, vector hitDirection, float damage)
	{
		vector intersectionPosition;
		int materialColor = BL_Utilities.CalculateBloodColorFromDamage(damage);

		// attempt ground splatter
		auto groundTraceParam = BL_Utilities.GetSurfaceIntersection(
			_owner,
			_world,
			hitPosition,
			Vector(0, -1, 0),
			BL_Constants.GROUND_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (groundTraceParam.TraceEnt) // spawn splatter below character
		{
			_world.CreateDecal(
				groundTraceParam.TraceEnt,
				_owner.GetOrigin() + Vector(0, BL_Constants.DECAL_FAR_PLANE / 4, 0),
				vector.Lerp(-groundTraceParam.TraceNorm, hitDirection, 0.5),
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(MIN_DECAL_SIZE, MAX_DECAL_SIZE),
				1,
				GetRandomSplatterMaterialPath(),
				BL_Constants.DECAL_LIFETIME_SECS,
				materialColor
			);
		}

		// attempt surface splatter
		auto surfaceTraceParam = BL_Utilities.GetSurfaceIntersection(
			_owner,
			_world,
			hitPosition,
			hitDirection,
			BL_Constants.SURFACE_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (surfaceTraceParam.TraceEnt) // spawn splatter on surface
		{
			_world.CreateDecal(
				surfaceTraceParam.TraceEnt,
				intersectionPosition - hitDirection * (BL_Constants.DECAL_FAR_PLANE / 4),
				hitDirection,
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(MIN_DECAL_SIZE, MAX_DECAL_SIZE),
				1,
				GetRandomSplatterMaterialPath(),
				BL_Constants.DECAL_LIFETIME_SECS,
				materialColor
			);
		}
	}

	string GetRandomSplatterMaterialPath()
	{
		return SPLATTER_MATERIAL_PATHS.GetRandomElement();
	}
}