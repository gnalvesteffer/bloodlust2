class BL_CharacterBleedBehavior
{
	private static const float BLEEDS_PER_DAMAGE_SCALAR = 0.5;
	private static const float MIN_BLEED_INTERVAL_MS = 0.1 * 1000;
	private static const float MAX_BLEED_INTERVAL_MS = 7 * 1000;
	private static const float MIN_DECAL_SIZE = 0.25;
	private static const float MAX_DECAL_SIZE = 0.5;
	private static const ref array<string> DROPLET_MATERIAL_PATHS =
	{
		"{39924245A51C37C7}materials/droplets/1.emat",
		"{F39040D0F13312C3}materials/droplets/2.emat",
		"{07980748FCEE7AB7}materials/droplets/3.emat",
		"{4E4012578E280EDD}materials/droplets/4.emat",
	};

	private IEntity _owner;
	private World _world;
	private SCR_CharacterDamageManagerComponent _damageManagerComponent;
	private HitZone _bloodHitZone;
	private int _bleedsRemaining = 0;
	private float _nextBleedTime = 0;
	private float _lastHealth = 0;

	void OnInit(
		IEntity owner,
		World world,
		SCR_CharacterDamageManagerComponent damageManagerComponent
	)
	{
		_owner = owner;
		_world = world;
		_damageManagerComponent = damageManagerComponent;
		_bloodHitZone = _damageManagerComponent.GetBloodHitZone();
		_lastHealth = _bloodHitZone.GetHealth();
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
		_bleedsRemaining += damage * BLEEDS_PER_DAMAGE_SCALAR;
		_nextBleedTime = CalculateNextBleedTime();
	}

	void OnFrame()
	{
		float time = _world.GetWorldTime();
		float health = _bloodHitZone.GetHealth();
		if (health > _lastHealth) // unit must've been healed
		{
			_bleedsRemaining = 0;
		}
		if (_bleedsRemaining > 0 && time >= _nextBleedTime)
		{
			TryCreateBleedSplatter();
			_nextBleedTime = CalculateNextBleedTime();
			--_bleedsRemaining;
		}
		_lastHealth = health;
	}

	private void TryCreateBleedSplatter()
	{
		// TODO: make bleed originate from hit positions, and not the character origin.
		vector intersectionPosition;
		auto groundTraceParam = BL_Utilities.GetSurfaceIntersection(
			_owner,
			_world,
			_owner.GetOrigin() + Vector(0, 1, 0),
			Vector(Math.RandomFloat(-1, 1), -1, Math.RandomFloat(-1, 1)),
			BL_Constants.GROUND_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (groundTraceParam.TraceEnt)
		{
			_world.CreateDecal(
				groundTraceParam.TraceEnt,
				_owner.GetOrigin() + Vector(0, BL_Constants.DECAL_FAR_PLANE / 4, 0),
				-groundTraceParam.TraceNorm,
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(MIN_DECAL_SIZE, MAX_DECAL_SIZE),
				1,
				GetRandomDropletMaterialPath(),
				BL_Constants.DECAL_LIFETIME_SECS,
				BL_Utilities.CalculateBloodColorFromDamage(_bloodHitZone.GetHealthScaled() * 100)
			);
		}
	}

	private float CalculateNextBleedTime()
	{
		return _world.GetWorldTime() + Math.RandomFloat(MIN_BLEED_INTERVAL_MS, MAX_BLEED_INTERVAL_MS);
	}

	string GetRandomDropletMaterialPath()
	{
		return DROPLET_MATERIAL_PATHS.GetRandomElement();
	}
}