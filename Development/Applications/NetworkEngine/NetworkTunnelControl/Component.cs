using System.ComponentModel;

namespace NetworkTunnelControl
{
	[TypeConverter(typeof(Component))]
	public class Component : ExpandableObjectConverter
	{
		public enum Type
		{
			Transform,
			Light,

		}

		public Type type { get; set; }
	}
}