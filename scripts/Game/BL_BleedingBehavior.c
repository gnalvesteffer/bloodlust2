class BL_BleedingBehavior
{
	private const float BLEEDS_PER_DAMAGE_SCALAR = 0.05;
	private const float MIN_BLEED_INTERVAL_MS = 2 * 1000;
	private const float MAX_BLEED_INTERVAL_MS = 7 * 1000;
	private const float MIN_SPLATTER_SIZE = 0.5;
	private const float MAX_SPLATTER_SIZE = 0.75;

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
				Math.RandomFloat(MIN_SPLATTER_SIZE, MAX_SPLATTER_SIZE),
				1,
				GetRandomDropletMaterialPath(),
				-1,
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
		const string DROPLET_MATERIAL_DIRECTORY = "{E1866216CEB08179}materials/droplets";
		const int TOTAL_DROPLET_MATERIALS = 3;

		return string.Format("%1/%2.emat", DROPLET_MATERIAL_DIRECTORY, Math.RandomInt(1, TOTAL_DROPLET_MATERIALS)));
	}
}