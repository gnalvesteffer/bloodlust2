class BL_CharacterBleedBehavior
{
	private static const float BLEEDS_PER_DAMAGE_SCALAR = 0.1;
	private static const float MIN_BLEED_INTERVAL_MS = 2 * 1000;
	private static const float MAX_BLEED_INTERVAL_MS = 7 * 1000;
	private static const float MIN_DECAL_SIZE = 0.125;
	private static const float MAX_DECAL_SIZE = 0.375;
	private static const ref array<string> DROPLET_MATERIAL_PATHS =
	{
		"{39924245A51C37C7}materials/droplets/1.emat",
		"{F39040D0F13312C3}materials/droplets/2.emat",
		"{07980748FCEE7AB7}materials/droplets/3.emat",
		"{4E4012578E280EDD}materials/droplets/4.emat",
	};

	private IEntity m_owner;
	private World m_world;
	private SCR_CharacterDamageManagerComponent m_damageManagerComponent;
	private HitZone m_bloodHitZone;
	private int m_bleedsRemaining = 0;
	private float m_nextBleedTime = 0;
	private float m_lastHealth = 0;

	void OnInit(
		IEntity owner,
		World world,
		SCR_CharacterDamageManagerComponent damageManagerComponent
	)
	{
		m_owner = owner;
		m_world = world;
		m_damageManagerComponent = damageManagerComponent;
		m_bloodHitZone = m_damageManagerComponent.GetBloodHitZone();
		m_lastHealth = m_bloodHitZone.GetHealth();
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
		m_bleedsRemaining += damage * BLEEDS_PER_DAMAGE_SCALAR;
		m_nextBleedTime = CalculateNextBleedTime();
	}

	void OnFrame()
	{
		float time = m_world.GetWorldTime();
		float health = m_bloodHitZone.GetHealth();
		if (health > m_lastHealth) // unit must've been healed
		{
			m_bleedsRemaining = 0;
		}
		if (m_bleedsRemaining > 0 && time >= m_nextBleedTime)
		{
			TryCreateBleedSplatter();
			m_nextBleedTime = CalculateNextBleedTime();
			--m_bleedsRemaining;
		}
		m_lastHealth = health;
	}

	private void TryCreateBleedSplatter()
	{
		// TODO: make bleed originate from hit positions, and not the character origin.
		vector intersectionPosition;
		auto groundTraceParam = BL_Utilities.GetSurfaceIntersection(
			m_owner,
			m_world,
			m_owner.GetOrigin() + Vector(0, 1, 0),
			Vector(Math.RandomFloat(-0.25, 0.25), -1, Math.RandomFloat(-0.25, 0.25)),
			BL_Constants.GROUND_SPLATTER_INTERSECTION_DISTANCE,
			intersectionPosition
		);
		if (groundTraceParam.TraceEnt)
		{
			m_world.CreateDecal(
				groundTraceParam.TraceEnt,
				m_owner.GetOrigin() + Vector(0, BL_Constants.DECAL_FAR_PLANE / 4, 0),
				-groundTraceParam.TraceNorm,
				0,
				BL_Constants.DECAL_FAR_PLANE,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(MIN_DECAL_SIZE, MAX_DECAL_SIZE),
				1,
				GetRandomDropletMaterialPath(),
				BL_Constants.DECAL_LIFETIME_SECS,
				BL_Utilities.CalculateBloodColorFromDamage(m_bloodHitZone.GetHealthScaled() * 100)
			);
		}
	}

	private float CalculateNextBleedTime()
	{
		return m_world.GetWorldTime() + Math.RandomFloat(MIN_BLEED_INTERVAL_MS, MAX_BLEED_INTERVAL_MS);
	}

	string GetRandomDropletMaterialPath()
	{
		return DROPLET_MATERIAL_PATHS.GetRandomElement();
	}
}