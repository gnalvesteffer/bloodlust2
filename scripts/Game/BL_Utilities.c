class BL_Utilities
{
	static TraceParam GetSurfaceIntersection(
		IEntity owner,
		World world,
		vector origin,
		vector direction, 
		float distance, 
		out vector intersectionPosition
	)
	{
		auto param = new TraceParam();
  		param.Start = origin;
  		param.End = origin + direction * distance;
  		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  		param.Exclude = owner;
		float intersectionDistance = world.TraceMove(param, NULL) * distance;
		intersectionPosition = origin + (direction * intersectionDistance);
		return param;
	}
	
	static int CalculateBloodColorFromDamage(float damage)
	{
		const float DAMAGE_OPACITY_SCALAR = 4.0;
		const int MINIMUM_SPLATTER_OPACITY = 255 / 3;

		return Color.FromRGBA(255, 255, 255, Math.ClampInt(damage * DAMAGE_OPACITY_SCALAR, MINIMUM_SPLATTER_OPACITY, 255)).PackToInt();
	}
}